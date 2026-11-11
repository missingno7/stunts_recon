"""integ28: session-scoped evidence reuse and strict batch publication."""
import sys
import tempfile
import unittest
from contextlib import ExitStack
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import batch_publish as B
import build_exact
import memo
import transaction


class MemoTests(unittest.TestCase):
    def test_no_reuse_outside_a_session(self):
        calls = []
        for _ in range(2):
            memo.cached('t', 1, lambda: calls.append(1) or len(calls))
        self.assertEqual(len(calls), 2)

    def test_session_reuse_returns_independent_copies(self):
        calls = []
        with memo.session():
            first = memo.cached('t', 1, lambda: calls.append(1) or {'v': [1]})
            first['v'].append(2)
            second = memo.cached('t', 1, lambda: calls.append(1) or {'v': [1]})
            second['v'].append(3)
            third = memo.cached('t', 1, lambda: calls.append(1) or {'v': [1]})
        self.assertEqual(len(calls), 1)
        self.assertEqual(third, {'v': [1]})

    def test_store_is_dropped_when_the_session_ends(self):
        calls = []
        with memo.session():
            memo.cached('t', 2, lambda: calls.append(1))
        with memo.session():
            memo.cached('t', 2, lambda: calls.append(1))
        self.assertEqual(len(calls), 2)

    def test_recheck_detects_change_during_session(self):
        state = {'value': 'a'}
        with self.assertRaisesRegex(ValueError, 'changed during verification'):
            with memo.session():
                memo.cached('toolchain', 'p', lambda: state['value'], recheck=True)
                state['value'] = 'b'
        with memo.session():
            memo.cached('toolchain', 'p', lambda: state['value'], recheck=True)

    def test_exception_inside_session_propagates_unchanged(self):
        state = {'value': 'a'}
        with self.assertRaisesRegex(KeyError, 'original'):
            with memo.session():
                memo.cached('toolchain', 'q', lambda: state['value'], recheck=True)
                state['value'] = 'b'
                raise KeyError('original')

    def test_per_name_items_compute_only_missing_names(self):
        seen = []

        def compute(names):
            seen.append(list(names))
            return {n: n.upper() for n in names}
        with memo.session():
            self.assertEqual(memo.cached_items('n', 0, ['a', 'b'], compute), {'a': 'A', 'b': 'B'})
            self.assertEqual(memo.cached_items('n', 0, ['b', 'c'], compute), {'b': 'B', 'c': 'C'})
            self.assertEqual(memo.cached_items('n', 1, ['a'], compute), {'a': 'A'})
        self.assertEqual(seen, [['a', 'b'], ['c'], ['a']])

    def test_compute_error_is_not_cached(self):
        calls = []

        def bad():
            calls.append(1)
            raise ValueError('bad')
        with memo.session():
            for _ in range(2):
                with self.assertRaises(ValueError):
                    memo.cached('e', 0, bad)
        self.assertEqual(len(calls), 2)


class BuildAttributionTests(unittest.TestCase):
    def test_failing_owner_is_attributed_in_manifest_order(self):
        manifest = {'owners': [
            {'id': 'a', 'kind': 'MATCHING_C', 'recipe': 'recipes/a.json', 'start': 0, 'end': 2},
            {'id': 'b', 'kind': 'MATCHING_C', 'recipe': 'recipes/b.json', 'start': 2, 'end': 4},
            {'id': 'c', 'kind': 'MATCHING_C', 'recipe': 'recipes/c.json', 'start': 4, 'end': 6}]}
        recipes = {o['recipe']: {'start': o['start'], 'end': o['end'], 'source': 'src/'+o['id']+'.c'}
                   for o in manifest['owners']}

        def probe(recipe, oracle, override):
            if recipe['start'] >= 2:
                raise ValueError('bytes differ at ' + recipe['source'])
            return b'xx', {'binding': {}}
        with patch.object(build_exact, 'probe', side_effect=probe):
            with self.assertRaisesRegex(ValueError, 'src/b.c') as caught:
                build_exact._compile_owners(manifest, recipes, {}, None)
        self.assertEqual((caught.exception.owner_id, caught.exception.owner_recipe),
                         ('b', 'recipes/b.json'))


class BatchStageTests(unittest.TestCase):
    """Stage logic with the strict gates replaced by recorded fakes."""

    def run_stage(self, names, fail_individual=(), fail_union=None, fail_independent=(),
                  inputs_sequence=None):
        entries = [{'name': n, 'source': n.encode(), 'recipe_data': None} for n in names]
        builds = []

        def check(name, source, recipe_data, manifest, oracle, image):
            if name in fail_individual:
                raise ValueError('individual failure ' + name)
            return ({'id': name}, 'src/'+name+'.c', b'P'*4, {'fast': name}, manifest)

        def ownership(manifest, name, recipe, oracle, image):
            return {'owners': manifest['owners'] + [name]}

        def builder(union, recipes, publish, source_overrides):
            builds.append(sorted(recipes))
            if fail_union:
                fail_union(sorted(r.split('/')[1][:-5] for r in recipes))
            return {'inputs': 'I', 'executable': 'E', 'relocation_count': 0}

        def independent(recipe, source, oracle, image):
            if recipe['id'] in fail_independent:
                raise ValueError('dosbox differs ' + recipe['id'])
            return {'exact': True}
        inputs = iter(inputs_sequence) if inputs_sequence else None
        with ExitStack() as stack:
            stack.enter_context(patch.object(B, 'read_json', return_value={'owners': []}))
            stack.enter_context(patch.object(B.P, 'check_candidate', side_effect=check))
            stack.enter_context(patch.object(B.P, 'apply_ownership', side_effect=ownership))
            stack.enter_context(patch.object(B.P, 'inputs',
                                              side_effect=(lambda: next(inputs)) if inputs else (lambda: 'I')))
            stack.enter_context(patch('oracle.verify', return_value=(b'', b'', {})))
            stack.enter_context(patch('mz.MZ.parse', return_value=type(
                'M', (), {'load_image': staticmethod(lambda data: b'')})))
            survivors, union, staged, dropped = B.stage_batch(
                entries, 'I', independent=True, log=lambda *_: None, builder=builder,
                independent_check=independent)
        return [e['name'] for e in survivors], union, staged, dropped, builds

    def test_all_pass_one_union_build(self):
        names, union, staged, dropped, builds = self.run_stage(['a', 'b', 'c'])
        self.assertEqual(names, ['a', 'b', 'c'])
        self.assertEqual(union['owners'], ['a', 'b', 'c'])
        self.assertEqual(len(builds), 1)
        self.assertEqual(dropped, {})

    def test_individual_and_independent_failures_are_dropped_before_the_union(self):
        names, union, _, dropped, builds = self.run_stage(
            ['a', 'b', 'c', 'd'], fail_individual=('b',), fail_independent=('d',))
        self.assertEqual(names, ['a', 'c'])
        self.assertEqual(builds, [['recipes/a.json', 'recipes/c.json']])
        self.assertEqual(dropped['b']['stage'], 'individual')
        self.assertEqual(dropped['d']['stage'], 'individual')

    def test_attributed_union_failure_drops_only_that_candidate_and_rebuilds(self):
        def fail(members):
            if 'b' in members:
                error = ValueError('Candidate bytes differ')
                error.owner_recipe = 'recipes/b.json'
                raise error
        names, _, staged, dropped, builds = self.run_stage(['a', 'b', 'c'], fail_union=fail)
        self.assertEqual(names, ['a', 'c'])
        self.assertEqual(dropped['b']['stage'], 'union')
        # A fresh whole-image build follows the drop; nothing is reused.
        self.assertEqual(builds, [['recipes/a.json', 'recipes/b.json', 'recipes/c.json'],
                                  ['recipes/a.json', 'recipes/c.json']])
        self.assertIsNotNone(staged)

    def test_unattributed_union_failure_bisects_to_the_breaking_candidate(self):
        def fail(members):
            if 'c' in members:
                raise ValueError('Relocation order/pairs differ')
        names, _, _, dropped, builds = self.run_stage(['a', 'b', 'c', 'd', 'e'], fail_union=fail)
        self.assertEqual(names, ['a', 'b', 'd', 'e'])
        self.assertEqual(dropped['c']['stage'], 'union-bisect')
        self.assertLess(len(builds), 6)

    def test_every_candidate_failing_publishes_nothing(self):
        def fail(members):
            raise ValueError('Full image mismatch')
        names, _, staged, dropped, _ = self.run_stage(['a', 'b'], fail_union=fail)
        self.assertEqual(names, [])
        self.assertIsNone(staged)
        self.assertEqual(set(dropped), {'a', 'b'})

    def test_concurrent_input_change_is_not_a_candidate_failure(self):
        def fail(members):
            raise ValueError('Inputs changed during fresh construction')
        with self.assertRaises(transaction.SnapshotChanged):
            self.run_stage(['a', 'b'], fail_union=fail, inputs_sequence=['X'] * 10)


class BatchPublicationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        for folder in ('build', 'src', 'recipes', 'layout'):
            (self.root / folder).mkdir(parents=True, exist_ok=True)
        (self.root / 'layout/manifest.json').write_bytes(b'old manifest')
        self.candidates = []
        for name in ('one', 'two'):
            path = self.root / (name + '.c')
            path.write_bytes(b'int ' + name.encode() + b';\n')
            self.candidates.append({'name': name, 'candidate': str(path), 'recipe': None})

    def tearDown(self):
        self.temp.cleanup()

    def publish(self, canonical_build):
        def stage(entries, before, **kwargs):
            for e in entries:
                e.update(recipe={'id': e['name']}, destination='src/'+e['name']+'.c',
                         payload=b'PP', fast={}, independent={'exact': True})
            return entries, {'owners': ['new']}, {'executable': 'E', 'relocation_count': 1}, {}
        state = {'before': {'layout/manifest.json': 'old'}}

        def inputs():
            return {p.relative_to(self.root).as_posix(): B.sha(p.read_bytes())
                    for p in self.root.rglob('*') if p.is_file() and
                    p.parts[len(self.root.parts)] in ('src', 'recipes', 'layout')}
        with ExitStack() as stack:
            for module in (B, transaction):
                stack.enter_context(patch.object(module, 'ROOT', self.root))
            stack.enter_context(patch.object(B, 'stage_batch', side_effect=stage))
            stack.enter_context(patch.object(B.P, 'inputs', side_effect=inputs))
            stack.enter_context(patch.object(B.P, 'build', side_effect=canonical_build))
            return B.publish_batch(self.candidates, log=lambda *_: None)

    def test_batch_publishes_every_survivor_under_one_journal(self):
        def canonical(**kwargs):
            self.assertTrue((self.root / 'build/publication.json').exists())
            kwargs['artifact']['executable'] = b'MZ'
            return {'inputs': B.P.inputs(), 'executable': 'E', 'relocation_count': 1,
                    'matching_c_bytes': 1, 'matching_asm_bytes': 0, 'raw_initialized_bytes': 0}
        summary = self.publish(canonical)
        self.assertEqual(summary['status'], 'PROMOTED')
        self.assertEqual((self.root / 'src/one.c').read_bytes(), b'int one;\n')
        self.assertEqual((self.root / 'src/two.c').read_bytes(), b'int two;\n')
        self.assertFalse((self.root / 'build/publication.json').exists())
        self.assertIn(b'"new"', (self.root / 'layout/manifest.json').read_bytes())

    def test_canonical_failure_rolls_back_the_whole_batch(self):
        def canonical(**kwargs):
            raise ValueError('Full image mismatch')
        with self.assertRaisesRegex(ValueError, 'Full image mismatch'):
            self.publish(canonical)
        self.assertFalse((self.root / 'src/one.c').exists())
        self.assertFalse((self.root / 'src/two.c').exists())
        self.assertEqual((self.root / 'layout/manifest.json').read_bytes(), b'old manifest')
        self.assertFalse((self.root / 'build/publication.json').exists())

    def test_duplicate_names_are_rejected(self):
        with self.assertRaisesRegex(ValueError, 'Duplicate batch candidate'):
            B._freeze(self.candidates + self.candidates[:1])

    def test_batch_file_syntax(self):
        path = self.root / 'batch.txt'
        path.write_text('# comment\none a.c\ntwo b.c r.json  # tail\n', encoding='utf-8')
        self.assertEqual(B.read_batch(path), [{'name': 'one', 'candidate': 'a.c', 'recipe': None},
                                             {'name': 'two', 'candidate': 'b.c', 'recipe': 'r.json'}])
        path.write_text('one\n', encoding='utf-8')
        with self.assertRaises(ValueError):
            B.read_batch(path)


if __name__ == '__main__':
    unittest.main()
