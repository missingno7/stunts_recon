"""integ44: source-only cosmetic republication guarded by complete OBJ identity."""
import contextlib
import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import batch_publish  # noqa: E402
import promote  # noqa: E402
from common import identity, sha  # noqa: E402
import communal_unit  # noqa: E402
from common import read_json  # noqa: E402


class CosmeticPublicationTests(unittest.TestCase):
    def make_accepted(self, root):
        (root/'asm').mkdir(parents=True)
        (root/'recipes').mkdir()
        (root/'layout').mkdir()
        (root/'build/workers/integ44').mkdir(parents=True)
        accepted = b'PUBLIC _foo\nfoo PROC\n  ret\nfoo ENDP\n'
        candidate = b'PUBLIC _foo\nfoo PROC\nreadable_join:\n  ret\nfoo ENDP\n'
        (root/'asm/foo.ASM').write_bytes(accepted)
        (root/'build/workers/integ44/foo.ASM').write_bytes(candidate)
        (root/'recipes/foo.json').write_text(json.dumps({
            'id': 'foo', 'start': 0, 'end': 1, 'profile': 'masm510-game',
            'kind': 'asm', 'source': 'asm/foo.ASM'}), encoding='utf-8')
        (root/'layout/manifest.json').write_text(json.dumps({'owners': [{
            'id': 'foo', 'name': 'foo', 'recipe': 'recipes/foo.json',
            'kind': 'MATCHING_ASM', 'start': 0, 'end': 1}]}), encoding='utf-8')
        return accepted, candidate, root/'build/workers/integ44/foo.ASM'

    def test_full_object_hash_is_the_cosmetic_gate(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            old, new, candidate = self.make_accepted(root)
            with mock.patch.object(promote, 'ROOT', root), \
                 mock.patch.object(promote, 'inputs', return_value={'snapshot': 'same'}), \
                 mock.patch.object(promote, '_cosmetic_compile', return_value=b'complete OMF'):
                result = promote.check_cosmetic('foo', candidate, new, {'snapshot': 'same'})
                self.assertEqual(result['object'], identity(b'complete OMF'))
                self.assertEqual(result['old_source'], identity(old))
                self.assertEqual(result['new_source'], identity(new))

            def changed(recipe, source):
                return b'old OMF' if source == old else b'changed PUBDEF spelling'

            with mock.patch.object(promote, 'ROOT', root), \
                 mock.patch.object(promote, 'inputs', return_value={'snapshot': 'same'}), \
                 mock.patch.object(promote, '_cosmetic_compile', side_effect=changed):
                with self.assertRaisesRegex(ValueError, 'full pinned-tool OBJ identity differs'):
                    promote.check_cosmetic('foo', candidate, new, {'snapshot': 'same'})

    def test_single_publication_changes_only_source_and_keeps_provenance(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            old, new, candidate = self.make_accepted(root)
            recipe_before = (root/'recipes/foo.json').read_bytes()
            manifest_before = (root/'layout/manifest.json').read_bytes()
            state = lambda: {'asm/foo.ASM': sha((root/'asm/foo.ASM').read_bytes())}

            def fake_check(name, path, source, before):
                return {'function': name, 'destination': 'asm/foo.ASM', 'recipe': 'recipes/foo.json',
                        'kind': 'asm', 'profile': 'masm510-game', 'old_source': identity(old),
                        'new_source': identity(source), 'object': identity(b'full OMF'),
                        'candidate': str(path), 'bytes': len(b'full OMF')}

            def prepare(changes):
                return [{'path': path, 'before': (root/path).read_bytes(), 'after': data}
                        for path, data in changes.items()]

            def apply(rows):
                for row in rows:
                    (root/row['path']).write_bytes(row['after'])

            with mock.patch.object(promote, 'ROOT', root), \
                 mock.patch.object(promote, 'inputs', side_effect=state), \
                 mock.patch.object(promote, 'check_cosmetic', side_effect=fake_check), \
                 mock.patch.object(promote, 'exclusive', return_value=contextlib.nullcontext()), \
                 mock.patch.object(promote, 'publishing', return_value=contextlib.nullcontext()), \
                 mock.patch.object(promote, 'ensure_consistent'), \
                 mock.patch.object(promote, 'prepare', side_effect=prepare), \
                 mock.patch.object(promote, 'apply', side_effect=apply), \
                 mock.patch.object(promote, 'invalidate_receipts'), \
                 mock.patch.object(promote, 'finish'), \
                 mock.patch.object(promote, 'rollback'):
                result = promote.cosmetic_promote('foo', candidate)

            self.assertEqual(result['status'], 'COSMETIC_PUBLISHED')
            self.assertEqual((root/'asm/foo.ASM').read_bytes(), new)
            self.assertEqual((root/'recipes/foo.json').read_bytes(), recipe_before)
            self.assertEqual((root/'layout/manifest.json').read_bytes(), manifest_before)
            self.assertEqual(result['old_source'], identity(old))
            self.assertEqual(result['new_source'], identity(new))
            self.assertEqual(result['object'], identity(b'full OMF'))

    def test_batch_records_each_complete_object_identity(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root/'build/workers/integ44').mkdir(parents=True)
            a, b = root/'build/workers/integ44/a.ASM', root/'build/workers/integ44/b.ASM'
            a.write_bytes(b'a'); b.write_bytes(b'b')
            entries = [{'name': 'a', 'candidate': str(a)}, {'name': 'b', 'candidate': str(b)}]

            def check(name, candidate, source, before):
                return {'function': name, 'destination': 'asm/'+name+'.ASM',
                        'old_source': identity(b'old '+name.encode()), 'new_source': identity(source),
                        'object': identity(b'OBJ '+name.encode()), 'bytes': 5}

            with mock.patch.object(batch_publish, 'ROOT', root), \
                 mock.patch.object(batch_publish.P, 'check_cosmetic', side_effect=check), \
                 mock.patch.object(batch_publish.P, 'inputs', return_value={'snapshot': 'same'}), \
                 mock.patch.object(batch_publish, 'lock_free_snapshot',
                                   side_effect=lambda action, capture: action(capture())):
                summary = batch_publish.cosmetic_batch(entries, verify_only=True)

            self.assertEqual(summary['status'], 'COSMETIC_VERIFIED_ONLY')
            self.assertEqual([row['function'] for row in summary['candidates']], ['a', 'b'])
            for row in summary['candidates']:
                self.assertEqual(row['object']['sha256'], sha(b'OBJ '+row['function'].encode()))
                saved = json.loads((root/'build/acceptance/_cosmetic'/f"{row['function']}.json").read_text())
                self.assertEqual(saved['old_source'], row['old_source'])
                self.assertEqual(saved['new_source'], row['new_source'])

    def test_cosmetic_recipe_changes_are_limited_to_resolved_closures(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root/'recipes').mkdir()
            (root/'recipes/foo.json').write_text(json.dumps({
                'id':'foo', 'kind':'asm', 'include_closure':[]}), encoding='utf-8')
            closure = [{'path':'include/platform_hw.inc', 'size':12, 'sha256':'a'*64}]
            rows = [{'recipe_closure_updates': {
                'recipes/foo.json': {'include_closure':closure}}}]
            with mock.patch.object(promote, 'ROOT', root):
                changes = promote.cosmetic_recipe_changes(rows)
            updated = json.loads(changes['recipes/foo.json'].decode('utf-8'))
            self.assertEqual(updated['include_closure'], closure)
            self.assertEqual(set(updated), {'id','kind','include_closure'})

    def test_communal_inventory_can_refresh_only_same_accepted_owner_extent(self):
        manifest = copy.deepcopy(read_json(ROOT/'layout/manifest.json'))
        accepted = communal_unit.accepted_row(manifest)
        self.assertIsNotNone(accepted)
        with tempfile.TemporaryDirectory() as temporary:
            candidate_path = Path(temporary)/'communal_candidate.json'
            candidate_path.write_text(json.dumps({
                'schema': accepted['schema'], 'start': accepted['start'],
                'end': accepted['end'], 'communals': accepted['communals']}), encoding='utf-8')
            candidate = communal_unit.load_candidate(candidate_path)
        # Model the previous complete inventory without changing canonical
        # state; this accepted owner has the same identity and extent.
        before_row = communal_unit.accepted_row(manifest)
        before_row['communals'].extend(copy.deepcopy(before_row['communals'][:9]))
        before = communal_unit.accepted_row(manifest)
        updated = communal_unit.attach(manifest, candidate)
        after = communal_unit.accepted_row(updated)
        self.assertEqual((after['id'], after['start'], after['end']),
                         (before['id'], before['start'], before['end']))
        self.assertEqual(len(before['communals']), 312)
        self.assertEqual(len(after['communals']), 303)
        self.assertEqual(communal_unit.accepted_row(manifest)['communals'], before['communals'])

        wrong_owner = dict(candidate, id='other_communal_unit')
        with self.assertRaisesRegex(ValueError, 'Communal candidate does not match the accepted unit owner and extent'):
            communal_unit.attach(manifest, wrong_owner)
        wrong_extent = dict(candidate, end=candidate['end']-1)
        with self.assertRaises(ValueError):
            communal_unit.attach(manifest, wrong_extent)


if __name__ == '__main__':
    unittest.main()
