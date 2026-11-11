"""Self-contained rename policy, complete-object proof and real journal tests.

No ignored inputs, DOS runner, or worker scratch is required. Synthetic OMF
fixtures exercise the strict production decoder; only compiler/image execution
is injected in the isolation test. The dry-run command exercises pinned tools.
"""
import copy
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import rename
import transaction
from common import json_bytes
from object_probe import read_object
from omf import OmfReader

FIXTURE = Path(__file__).parent / 'fixtures/rename'


def record(kind, body):
    raw = bytes([kind]) + struct.pack('<H', len(body) + 1) + body
    return raw + bytes([-sum(raw) & 255])


def name(value):
    raw = value.encode('ascii')
    return bytes([len(raw)]) + raw


def object_fixture(symbol='_old_function', external='_target', payload=b'\x9a\0\0\0\0\xcb'):
    raw = b''.join([
        record(0x80, name('UNIT')),
        record(0x96, name('CODE') + name('CODE')),
        record(0x98, b'\x28' + struct.pack('<H', len(payload)) + b'\x01\x02\x00'),
        record(0x8C, name(external) + b'\x00'),
        record(0x90, b'\x00\x01' + name(symbol) + b'\x00\x00\x00'),
        record(0xA0, b'\x01\x00\x00' + payload),
        record(0x9C, b'\xcc\x01\x56\x01'),
        record(0x8A, b'\x00')])
    return read_object(raw), raw


def local_fixture(symbol='_old_function', local='_private', offset=0):
    raw = bytearray()
    for kind, body in OmfReader.records(object_fixture(symbol)[1]):
        raw.extend(record(kind, body))
        if kind == 0x90:
            raw.extend(record(0xB6, b'\x00\x01' + name(local) + struct.pack('<H', offset) + b'\x00'))
    raw = bytes(raw)
    return read_object(raw), raw


def fixture_files():
    return {
        'src/owner.c': b'int old_function(void) { return 1; }\n',
        'asm/caller.ASM': b'extrn _old_function:far\n',
        'recipes/owner.json': json_bytes({'id': 'owner', 'source': 'src/owner.c', 'profile': 'fixture',
                                          'public': '_old_function', 'members': [
                                              {'name': 'old_function', 'public': '_old_function'}]}),
        'layout/manifest.json': json_bytes({'owners': [
            {'id': 'owner', 'kind': 'MATCHING_C', 'recipe': 'recipes/owner.json'}]}),
        'layout/names-registry.json': json_bytes({'names': {'16': {
            'name': 'old_function', 'inventory_name': 'original_label', 'kind': 'code'}}}),
        'layout/code-symbols.json': json_bytes({'symbols': {'_old_function': {
            'mapped_target': {'name': 'old_function', 'start': 16}}}}),
        'layout/oracle.lock.json': b'{"immutable": true}\n'}


ROW = {'old': 'old_function', 'new': 'reviewed_function', 'owner': 'owner',
       'address': '0x10', 'kind': 'c', 'confidence': 'high'}


class RenamePolicyTests(unittest.TestCase):
    def test_clean_c_and_asm_pair(self):
        for suffix, asm in (('c', False), ('ASM', True)):
            with self.subTest(suffix=suffix):
                before = (FIXTURE / ('before.' + suffix)).read_bytes().decode('ascii')
                after = (FIXTURE / ('after.' + suffix)).read_bytes().decode('ascii')
                self.assertEqual(rename.replace_tokens(before, rename.symbol_map([ROW]), asm), after)

    def test_c_underscore_identifier_is_distinct_from_assembly_decoration(self):
        files = fixture_files()
        files['src/owner.c'] += b'int _old_function;\nint old_function_suffix;\n'
        changes = rename.make_changes(files, [ROW])
        self.assertIn(b'int _old_function;', changes['src/owner.c'])
        self.assertIn(b'int old_function_suffix;', changes['src/owner.c'])
        self.assertIn(b'int reviewed_function(void)', changes['src/owner.c'])
        self.assertIn(b'_reviewed_function', changes['asm/caller.ASM'])

    def test_multiline_comments_literals_and_crlf(self):
        text = '/* prose\r\nold_function */\r\n"old_function"; old_function(); \'x\';\r\n'
        self.assertEqual(rename.replace_tokens(text, rename.symbol_map([ROW])),
                         text.replace('old_function();', 'reviewed_function();'))

    def test_preprocessor_numbers_are_not_identifiers(self):
        text = '0old_function 123old_function old_function();'
        self.assertEqual(rename.replace_tokens(text, rename.symbol_map([ROW])),
                         '0old_function 123old_function reviewed_function();')

    def test_header_names_are_opaque(self):
        text = '#include <old_function.h>\n#include "old_function.h"\nold_function();\n'
        self.assertEqual(rename.replace_tokens(text, rename.symbol_map([ROW])),
                         text.replace('old_function();', 'reviewed_function();'))

    def test_collision_anywhere(self):
        good, bad = rename.preflight([ROW], fixture_files(), {'reviewed_function'})
        self.assertFalse(good)
        self.assertIn('already exists', bad[0]['reason'])

    def test_truncation_collision(self):
        files = fixture_files()
        files['asm/long.ASM'] = ('public _' + 'a' * 30 + 'first\n').encode()
        row = {**ROW, 'new': 'a' * 30 + 'second'}
        good, bad = rename.preflight([row], files)
        self.assertFalse(good)
        self.assertIn('truncation collision', bad[0]['reason'])

    def test_reserved_names(self):
        for word in ('while', 'MOV', 'segment', 'ax', '_cdecl', 'fsubrp', 'fnstcw'):
            with self.subTest(word=word):
                good, bad = rename.preflight([{**ROW, 'new': word}], fixture_files())
                self.assertFalse(good)
                self.assertIn('reserved', bad[0]['reason'])

    def test_clean_preflight(self):
        good, bad = rename.preflight([ROW], fixture_files())
        self.assertEqual(good, [ROW])
        self.assertFalse(bad)

    def test_runtime_is_refused(self):
        _, bad = rename.preflight([{**ROW, 'owner': 'pinned_runtime'}], fixture_files())
        self.assertIn('no rebuildable', bad[0]['reason'])

    def test_wrong_address_is_refused(self):
        _, bad = rename.preflight([{**ROW, 'address': '0x20'}], fixture_files())
        self.assertIn('not grounded', bad[0]['reason'])

    def test_private_asm_entry_requires_listing_grounding(self):
        files = fixture_files()
        recipe = json.loads(files['recipes/owner.json'])
        recipe.update(kind='asm', source='asm/private.ASM', start=16, end=32)
        recipe.pop('public')
        recipe.pop('members')
        files['recipes/owner.json'] = json_bytes(recipe)
        files['asm/private.ASM'] = b'old_function proc far\n retf\nold_function endp\n'
        files['layout/code-symbols.json'] = b'{"symbols": {}}'
        registry = json.loads(files['layout/names-registry.json'])
        registry['names']['16']['object_declared'] = False
        files['layout/names-registry.json'] = json_bytes(registry)
        good, bad = rename.preflight([ROW], files)
        self.assertFalse(bad)
        self.assertEqual(good[0]['grounding'], 'pinned-MASM-listing-required')

    def test_unregistered_private_label_does_not_invent_inventory_facts(self):
        files = fixture_files()
        recipe = json.loads(files['recipes/owner.json'])
        recipe.update(kind='asm', source='asm/private.ASM', start=16, end=32)
        recipe.pop('public')
        recipe.pop('members')
        files['recipes/owner.json'] = json_bytes(recipe)
        files['asm/private.ASM'] = b'old_function proc far\n retf\nold_function endp\n'
        files['layout/code-symbols.json'] = b'{"symbols": {}}'
        files['layout/names-registry.json'] = b'{"names": {}}'
        good, bad = rename.preflight([ROW], files)
        self.assertFalse(good)
        self.assertIn('lacks a grounded registry inventory target', bad[0]['reason'])

    def test_batch_duplicate_destination(self):
        _, bad = rename.preflight([ROW, {**ROW, 'old': 'other_function'}], fixture_files())
        self.assertEqual(len(bad), 2)

    def test_json_bindings_registry_and_evidence(self):
        files = fixture_files()
        changes = rename.make_changes(files, [ROW])
        self.assertNotIn('layout/oracle.lock.json', changes)
        recipe = json.loads(changes['recipes/owner.json'])
        self.assertEqual(recipe['public'], '_reviewed_function')
        self.assertEqual(recipe['members'][0]['name'], 'old_function')
        symbols = json.loads(changes['layout/code-symbols.json'])['symbols']
        self.assertEqual(symbols['_reviewed_function']['mapped_target']['name'], 'old_function')
        self.assertEqual(symbols['_reviewed_function']['reviewed_alias'], '_reviewed_function')
        registry = json.loads(changes['layout/names-registry.json'])['names']['16']
        self.assertEqual(registry['inventory_name'], 'original_label')
        self.assertEqual(registry['name'], 'reviewed_function')
        applied = {**files, **changes}
        rename.check_applied([ROW], applied)
        with self.assertRaisesRegex(ValueError, 'partially applied'):
            rename.check_applied([ROW], {**applied, 'src/owner.c': files['src/owner.c']})

    def test_key_collision_refuses(self):
        with self.assertRaisesRegex(ValueError, 'key collision'):
            rename.renamed({'a': 1, 'b': 2}, {'a': 'b'})

    def test_fixed_partial_entry_alias_is_refused(self):
        files = fixture_files()
        doc = json.loads(files['layout/code-symbols.json'])
        doc['symbols']['_old_function']['module_entry'] = {'owner': 'owner'}
        files['layout/code-symbols.json'] = json_bytes(doc)
        _, failed = rename.preflight([ROW], files)
        self.assertIn('requires the original emitted spelling', failed[0]['reason'])

    def test_native_c_references_are_renamed(self):
        files = fixture_files()
        files['port/service.c'] = b'int old_function(void);\nint native_caller(void) { return old_function(); }\n'
        changes = rename.make_changes(files, [ROW])
        self.assertIn(b'return reviewed_function()', changes['port/service.c'])

    def test_native_signature_string_requires_explicit_review(self):
        files = fixture_files()
        files['port/build.py'] = b'pattern = r"\\bold_function\\b"\n'
        good, bad = rename.preflight([ROW], files)
        self.assertFalse(good)
        self.assertIn('port/build.py:1', bad[0]['reason'])


class CompleteObjectTests(unittest.TestCase):
    def test_derived_local_record_offset_follows_name_length(self):
        before = local_fixture()
        after = local_fixture('_reviewed_function')
        self.assertNotEqual(before[0].local_symbol_records[0]['record_offset'],
                            after[0].local_symbol_records[0]['record_offset'])
        self.assertIsNone(rename.compare_objects(before, after, rename.symbol_map([ROW])))

    def test_local_body_hash_follows_permitted_symbol_spelling(self):
        before = local_fixture()
        after = local_fixture(local='_descriptive_private')
        self.assertNotEqual(before[0].local_symbol_records[0]['body_sha256'],
                            after[0].local_symbol_records[0]['body_sha256'])
        self.assertIsNone(rename.compare_objects(before, after, {'_private': '_descriptive_private'}))

    def test_real_local_public_offset_change_is_still_rejected(self):
        diff = rename.compare_objects(local_fixture(), local_fixture(offset=1), {})
        self.assertIsNotNone(diff['semantic'])
        self.assertIsNotNone(diff['omf'])

    def test_spelling_only_full_object(self):
        before = object_fixture()
        after = object_fixture('_reviewed_function')
        self.assertIsNone(rename.compare_objects(before, after, rename.symbol_map([ROW])))

    def test_payload_change_fails_with_offset(self):
        before = object_fixture()
        after = object_fixture('_reviewed_function', payload=b'\x9a\0\0\0\0\xc3')
        diff = rename.compare_objects(before, after, rename.symbol_map([ROW]))
        self.assertEqual(diff['semantic']['record'], 'object.segments.CODE[5]')
        self.assertIsNotNone(diff['omf'])

    def test_fixup_order_is_not_sorted(self):
        before = object_fixture()
        after = object_fixture('_reviewed_function')
        before[0].linker_fixups.append({**before[0].linker_fixups[0], 'offset': 3})
        after[0].linker_fixups = list(reversed(copy.deepcopy(before[0].linker_fixups)))
        diff = rename.compare_objects(before, after, rename.symbol_map([ROW]))
        self.assertIn('linker_fixups[0]', diff['semantic']['record'])

    def test_no_symbol_replacement_inside_machine_bytes(self):
        before = object_fixture(payload=b'_old_function\0')
        after = object_fixture('_reviewed_function', payload=b'_reviewed_function\0')
        self.assertIsNotNone(rename.compare_objects(before, after, rename.symbol_map([ROW])))

    def test_unowned_segment_extents_are_compared(self):
        before, after = object_fixture(), object_fixture('_reviewed_function')
        after[0].segment_lengths['_BSS'] = 12
        self.assertIn('segment_lengths', rename.compare_objects(before, after,
                      rename.symbol_map([ROW]))['semantic']['record'])

    def test_ordered_declarations_are_compared(self):
        before, after = object_fixture(), object_fixture('_reviewed_function')
        before[0].externals = ['_a', '_b']
        after[0].externals = ['_b', '_a']
        self.assertIn('externals[0]', rename.compare_objects(before, after,
                      rename.symbol_map([ROW]))['semantic']['record'])


class PublicationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.files = fixture_files()
        for key, raw in self.files.items():
            path = self.root / key
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(raw)
        self.patches = [patch.object(rename, 'ROOT', self.root), patch.object(transaction, 'ROOT', self.root)]
        for p in self.patches:
            p.start()
            self.addCleanup(p.stop)

    def test_clean_transaction(self):
        changes = rename.make_changes(self.files, [ROW])
        observed = []
        rename.publish(changes, self.files, lambda: observed.append(rename.canonical_files(self.root)))
        self.assertEqual(observed, [{**self.files, **changes}])
        self.assertFalse(transaction.journal_path().exists())
        self.assertEqual(transaction.read_generation() % 2, 0)

    def test_failure_rolls_back_every_file(self):
        changes = rename.make_changes(self.files, [ROW])
        def fail():
            self.assertEqual(rename.canonical_files(self.root), {**self.files, **changes})
            raise ValueError('owner: ordered FIXUPP[2] changed')
        with self.assertRaisesRegex(ValueError, 'FIXUPP'):
            rename.publish(changes, self.files, fail)
        self.assertEqual(rename.canonical_files(self.root), self.files)
        self.assertFalse(transaction.journal_path().exists())

    def test_header_is_in_the_same_transaction(self):
        header = 'include/functions.h'
        self.files[header] = b'int old_function(void);\n'
        path = self.root / header
        path.parent.mkdir()
        path.write_bytes(self.files[header])
        changes = rename.make_changes(self.files, [ROW])
        observed = []
        rename.publish(changes, self.files, lambda: observed.append(path.read_bytes()))
        self.assertEqual(observed, [b'int reviewed_function(void);\n'])
        self.assertFalse(transaction.journal_path().exists())

    def test_header_failure_rolls_back_all_files(self):
        header = 'include/functions.h'
        self.files[header] = b'int old_function(void);\n'
        path = self.root / header
        path.parent.mkdir()
        path.write_bytes(self.files[header])
        def fail():
            self.assertIn(b'reviewed_function', path.read_bytes())
            raise ValueError('image failed')
        with self.assertRaisesRegex(ValueError, 'image failed'):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, fail)
        self.assertEqual(rename.canonical_files(self.root), self.files)
        self.assertFalse(transaction.journal_path().exists())

    def test_marked_header_journal_recovers_after_interruption(self):
        header = 'include/functions.h'
        self.files[header] = b'int old_function(void);\n'
        path = self.root / header
        path.parent.mkdir()
        path.write_bytes(self.files[header])
        changes = rename.make_changes(self.files, [ROW])
        with transaction.exclusive():
            transaction.apply(rename.prepare_publication(changes))
        with self.assertRaisesRegex(ValueError, 'Interrupted publication'):
            transaction.ensure_consistent()
        with transaction.exclusive():
            rename.rollback_publication()
        self.assertEqual(rename.canonical_files(self.root), self.files)

    def test_header_journal_cannot_write_an_oracle_or_escape(self):
        for path in ('layout/oracle.lock.json', 'src/../layout/oracle.lock.json', '../outside',
                     'include/../../outside', '.git/index'):
            with self.subTest(path=path), self.assertRaises(ValueError):
                rename.publication_path(path)

    def test_native_sources_share_the_atomic_rollback(self):
        key = 'port/service.c'
        self.files[key] = b'int old_function(void);\n'
        path = self.root / key
        path.parent.mkdir()
        path.write_bytes(self.files[key])
        def fail():
            self.assertIn(b'reviewed_function', path.read_bytes())
            raise ValueError('native build failed')
        with self.assertRaisesRegex(ValueError, 'native build failed'):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, fail)
        self.assertEqual(rename.canonical_files(self.root), self.files)
        self.assertFalse(transaction.journal_path().exists())

    def test_interruption_also_rolls_back(self):
        def interrupt():
            raise KeyboardInterrupt()
        with self.assertRaises(KeyboardInterrupt):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, interrupt)
        self.assertEqual(rename.canonical_files(self.root), self.files)

    def test_partial_write_rolls_back(self):
        original = transaction.atomic_bytes
        count = [0]
        def write(path, raw):
            if str(path).endswith('.c'):
                count[0] += 1
                if count[0] == 1:
                    raise OSError('injected write failure')
            original(path, raw)
        with patch.object(transaction, 'atomic_bytes', write), self.assertRaises(OSError):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, lambda: None)
        self.assertEqual(rename.canonical_files(self.root), self.files)

    def test_concurrent_edit_is_preserved_for_recovery(self):
        def concurrent():
            (self.root / 'src/owner.c').write_bytes(b'user edit')
            raise ValueError('image failed')
        with self.assertRaisesRegex(ValueError, 'conflicting edit'):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, concurrent)
        self.assertEqual((self.root / 'src/owner.c').read_bytes(), b'user edit')
        self.assertTrue(transaction.journal_path().exists())

    def test_stale_inputs_refuse_before_writes(self):
        (self.root / 'src/owner.c').write_bytes(b'user edit')
        with self.assertRaisesRegex(ValueError, 'Canonical files changed'):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, lambda: None)
        self.assertFalse(transaction.journal_path().exists())

    def test_listing_must_ground_private_address_before_and_after(self):
        work = self.root / 'listing'
        work.mkdir()
        item = {'owners': ['owner'], 'recipe': {'start': 16}}
        compiled = (None, None, {'work_directory': str(work)})
        row = {**ROW, 'grounding': 'pinned-MASM-listing-required'}
        (work / 'UNIT.LST').write_text(' 0000\told_function proc far\n', encoding='ascii')
        rename.check_private_listing([row], item, compiled)
        with self.assertRaisesRegex(ValueError, 'address differs'):
            rename.check_private_listing([{**row, 'address': '0x11'}], item, compiled)
        (work / 'UNIT.LST').write_text(' 0000\treviewed_function proc far\n', encoding='ascii')
        rename.check_private_listing([row], item, compiled, after=True)

    def test_baseline_compile_failure_is_attributed(self):
        def missing(recipe, source):
            raise OSError('missing pinned compiler')
        engine = rename.ProofEngine(self.root, self.files, missing)
        clean, failed = engine.isolate([ROW])
        self.assertFalse(clean)
        self.assertEqual(failed[0]['status'], 'proof-error')
        self.assertEqual(failed[0]['failures'][0]['owner'], ['owner'])
        self.assertEqual(failed[0]['failures'][0]['record'], 'before-compile')

    def test_native_adapter_failure_is_refused(self):
        def compiler(recipe, source):
            return object_fixture(recipe['public'])
        def unsupported():
            raise rename.NativeBuildFailure('unsupported native adapter')
        engine = rename.ProofEngine(self.root, self.files, compiler, image=unsupported)
        clean, failed = engine.compatible([ROW], whole=True)
        self.assertFalse(clean)
        self.assertEqual(failed[0]['status'], 'refused')
        self.assertEqual(failed[0]['failures'][0]['phase'], 'native-build')

    def test_bisection_retains_good_name_and_proves_union(self):
        # Tiny injected compiler: one name changes a real object payload byte.
        files = {**self.files, 'src/owner.c': self.files['src/owner.c'] + b'int second_function(void);\n'}
        (self.root / 'src/owner.c').write_bytes(files['src/owner.c'])
        def compiler(recipe, source):
            public = recipe['public']
            return object_fixture(public, payload=(b'\x9a\0\0\0\0\xc3' if b'sensitive_function' in source
                                                   else b'\x9a\0\0\0\0\xcb'))
        engine = rename.ProofEngine(self.root, files, compiler, image=lambda: {'status': 'HYBRID_EXACT'})
        bad = {**ROW, 'old': 'second_function', 'new': 'sensitive_function'}
        good, failed = engine.isolate([ROW, bad])
        self.assertEqual(good, [ROW])
        self.assertEqual(failed[0]['status'], 'name-sensitive')
        self.assertEqual(failed[0]['old'], 'second_function')
        final = engine.trial(good, whole=True)
        self.assertFalse(final['failures'])
        self.assertEqual(final['whole_image']['status'], 'HYBRID_EXACT')

    def test_interacting_names_do_not_block_good_subset(self):
        files = {**self.files, 'src/owner.c': self.files['src/owner.c'] + b'int second_function(void);\n'}
        (self.root / 'src/owner.c').write_bytes(files['src/owner.c'])
        def compiler(recipe, source):
            sensitive = b'sensitive_function' in source and b'reviewed_function' in source
            return object_fixture(recipe['public'], payload=(b'\x9a\0\0\0\0\xc3' if sensitive
                                                            else b'\x9a\0\0\0\0\xcb'))
        engine = rename.ProofEngine(self.root, files, compiler, image=lambda: {'status': 'HYBRID_EXACT'})
        second = {**ROW, 'old': 'second_function', 'new': 'sensitive_function'}
        individually_clean, failures = engine.isolate([ROW, second])
        self.assertEqual(individually_clean, [ROW, second])
        self.assertFalse(failures)
        self.assertTrue(engine.trial(individually_clean)['failures'])
        compatible, failed = engine.compatible(individually_clean)
        self.assertEqual(compatible, [ROW])
        self.assertEqual(failed[0]['with_names'], ['old_function'])
        self.assertEqual(failed[0]['status'], 'name-sensitive')
        self.assertFalse(engine.trial(compatible, whole=True)['failures'])


if __name__ == '__main__':
    unittest.main()
