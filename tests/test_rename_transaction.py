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


def external_fixture(externals, target_index):
    raw = b''.join(record(k, b''.join(name(n) + b'\0' for n in externals) if k == 0x8C else
                         b[:-1] + bytes([target_index]) if k == 0x9C else b)
                   for k, b in OmfReader.records(object_fixture()[1]))
    return read_object(raw), raw


def local_fixture(symbol='_old_function', local='_private', offset=0):
    raw = bytearray()
    for kind, body in OmfReader.records(object_fixture(symbol)[1]):
        raw.extend(record(kind, body))
        if kind == 0x90:
            raw.extend(record(0xB6, b'\x00\x01' + name(local) + struct.pack('<H', offset) + b'\x00'))
    raw = bytes(raw)
    return read_object(raw), raw


def communal_fixture(order):
    raw = b''.join(record(k, b) + (b''.join(record(0xB0, name(n) + b'\0\x62' + bytes([size]))
                                          for n, size in order) if k == 0x8C else b'')
                   for k, b in OmfReader.records(object_fixture()[1]))
    return read_object(raw, communals=True), raw


def fixture_files():
    return {
        'game/owner.c': b'int old_function(void) { return 1; }\n',
        'lib/caller.ASM': b'extrn _old_function:far\n',
        'recipes/owner.json': json_bytes({'id': 'owner', 'source': 'game/owner.c', 'profile': 'fixture',
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
        files['game/owner.c'] += b'int _old_function;\nint old_function_suffix;\n'
        changes = rename.make_changes(files, [ROW])
        self.assertIn(b'int _old_function;', changes['game/owner.c'])
        self.assertIn(b'int old_function_suffix;', changes['game/owner.c'])
        self.assertIn(b'int reviewed_function(void)', changes['game/owner.c'])
        self.assertIn(b'_reviewed_function', changes['lib/caller.ASM'])

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
        files['lib/long.ASM'] = ('public _' + 'a' * 30 + 'first\n').encode()
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
        recipe.update(kind='asm', source='lib/private.ASM', start=16, end=32)
        recipe.pop('public')
        recipe.pop('members')
        files['recipes/owner.json'] = json_bytes(recipe)
        files['lib/private.ASM'] = b'old_function proc far\n retf\nold_function endp\n'
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
        recipe.update(kind='asm', source='lib/private.ASM', start=16, end=32)
        recipe.pop('public')
        recipe.pop('members')
        files['recipes/owner.json'] = json_bytes(recipe)
        files['lib/private.ASM'] = b'old_function proc far\n retf\nold_function endp\n'
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
            rename.check_applied([ROW], {**applied, 'game/owner.c': files['game/owner.c']})

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

    def test_port_occurrences_are_reported_without_edits(self):
        files = fixture_files()
        files['port/service.c'] = b'int old_function(void);\n'
        files['tools/porting/adapter.py'] = b'pattern = "old_function"\n'
        files['docs/porting/notes.md'] = b'Use old_function.\n'
        changes = rename.make_changes(files, [ROW])
        self.assertFalse(set(changes) & {'port/service.c', 'tools/porting/adapter.py', 'docs/porting/notes.md'})
        with tempfile.TemporaryDirectory() as work:
            root = Path(work)
            for key, raw in files.items():
                path = root / key
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(raw)
            hits = rename.port_occurrences(root, [ROW])
        self.assertEqual({h['file'] for h in hits}, {'port/service.c', 'tools/porting/adapter.py', 'docs/porting/notes.md'})
        self.assertTrue(all(h['line'] == 1 and 'old_function' in h['text'] for h in hits))

    def test_native_signature_string_does_not_refuse(self):
        files = fixture_files()
        files['port/build.py'] = b'pattern = r"\\bold_function\\b"\n'
        good, bad = rename.preflight([ROW], files)
        self.assertEqual(good, [ROW])
        self.assertFalse(bad)

    def test_scoped_field_declarations_accesses_and_shadowing(self):
        text = ('struct A { int old; }; struct B { int old; };\n'
                'struct A a; struct B b; int f(void) { int old; return a.old + b.old + old; }\n'
                'int g(void) { struct B a; return a.old; }\n')
        row = {'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'descriptive'}
        result = rename.replace_fields(text, [row])
        self.assertIn('struct A { int descriptive; }', result)
        self.assertIn('a.descriptive + b.old + old', result)
        self.assertIn('struct B a; return a.old', result)

    def test_pointer_array_typedef_and_nested_field_access(self):
        text = ('typedef struct A { int *old; } A;\n'
                'struct Outer { A items[2]; };\n'
                'int f(struct Outer *p) { return *p->items[1].old; }\n')
        result = rename.replace_fields(text, [{'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'cursor'}])
        self.assertIn('int *cursor;', result)
        self.assertIn('p->items[1].cursor', result)

    def test_field_strings_and_comments_are_opaque(self):
        text = 'struct A { int old; }; /* old */ char *s="old";\r\n'
        result = rename.replace_fields(text, [{'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'cursor'}])
        self.assertEqual(result, text.replace('int old;', 'int cursor;'))

    def test_assembly_field_uses_matching_scoped_spelling(self):
        text = 'A struc\n old dw ?\nA ends\nB struc\n old dw ?\nB ends\n mov ax,A.old\n db \"A.old\" ; narrative A.old\n'
        result = rename.replace_fields(text, [{'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'cursor'}], asm=True)
        self.assertIn(' cursor dw ?', result)
        self.assertIn('B struc\n old dw ?', result)
        self.assertIn('A.cursor', result)
        self.assertIn('db \"A.old\" ; narrative A.old', result)

    def test_unresolved_field_receiver_refuses_instead_of_guessing(self):
        text = 'struct A { int old; }; int f(void) { return unknown->old; }'
        with self.assertRaisesRegex(ValueError, 'Unresolved'):
            rename.replace_fields(text, [{'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'cursor'}])

    def test_global_declaration_only_row_without_owner_or_address(self):
        files = fixture_files()
        row = {'old': 'old_function', 'new': 'new_global', 'kind': 'global', 'address': None}
        good, bad = rename.preflight([row], files)
        self.assertFalse(bad)
        self.assertEqual(good[0]['grounding'], 'complete-declarations-and-historical-link')

    def test_field_rows_use_structure_identity(self):
        self.assertNotEqual(rename.row_id({'kind': 'field', 'struct': 'A', 'old': 'unk'}),
                            rename.row_id({'kind': 'field', 'struct': 'B', 'old': 'unk'}))

    def test_crlf_msc_primitive_macros_keep_field_coordinates(self):
        text = '#include "stunts_types.h"\r\nstruct A { U8 *old; };\r\nunsigned const I8 *p;\r\n'
        result = rename.replace_fields(text, [{'kind': 'field', 'struct': 'A', 'old': 'old', 'new': 'cursor'}])
        self.assertEqual(result, text.replace('*old;', '*cursor;'))

    def test_reserved_namespace_is_only_reconstruction_code(self):
        files = fixture_files()
        files['port/service.c'] = b'int reviewed_function(void);\n'
        self.assertFalse(rename.preflight([ROW], files)[1])

    def test_reviewed_code_offset_requires_supported_inventory_spelling(self):
        files = fixture_files()
        recipe = json.loads(files['recipes/owner.json'])
        files['recipes/caller.json'] = json_bytes({'id': 'caller', 'source': 'lib/caller.ASM',
                                                  'reviewed_code_offsets': ['_old_function']})
        manifest = json.loads(files['layout/manifest.json'])
        manifest['owners'].append({'id': 'caller', 'kind': 'MATCHING_ASM', 'recipe': 'recipes/caller.json'})
        files['layout/manifest.json'] = json_bytes(manifest)
        _, bad = rename.preflight([ROW], files)
        self.assertIn('near-entry proof requires the original inventory spelling', bad[0]['reason'])

    def test_internal_near_public_does_not_require_external_alias_resolver(self):
        files = fixture_files()
        recipe = json.loads(files['recipes/owner.json'])
        recipe.update(reviewed_code_offsets=['_old_function'],
                      object_declarations={'publics': [{'name': '_old_function', 'segment': 'CODE', 'offset': 0}]})
        files['recipes/owner.json'] = json_bytes(recipe)
        good, bad = rename.preflight([ROW], files)
        self.assertEqual(good, [ROW])
        self.assertFalse(bad)

    def test_binding_refusal_is_distinct_from_byte_change_and_infrastructure(self):
        from probe_module import ProbeFailure
        self.assertEqual(rename.proof_failure_category(ProbeFailure('unknown alias', {'category': 'BINDING_REVIEW_REQUIRED'})), 'refused')
        self.assertEqual(rename.proof_failure_category(ProbeFailure('different payload', {'category': 'BYTE_MISMATCH'})), 'name-sensitive')
        self.assertEqual(rename.proof_failure_category(OSError('missing tool')), 'proof-error')

    def test_communal_current_source_declaration_is_renamed_with_symbol(self):
        files = fixture_files()
        files['game/owner.c'] += b'int old_global[2];\n'
        manifest = json.loads(files['layout/manifest.json'])
        manifest['bss_owners'] = [{'id': 'c_common', 'kind': 'LINK_COMMUNAL', 'communals': [
            {'name': '_old_global', 'size': 4, 'declarers': ['owner'], 'size_basis': {
                'kind': 'declared-type', 'declaration': 'int old_global[2];'}}]}]
        files['layout/manifest.json'] = json_bytes(manifest)
        row = {'kind': 'global', 'old': 'old_global', 'new': 'reviewed_global'}
        changes = rename.make_changes(files, [row])
        item = json.loads(changes['layout/manifest.json'])['bss_owners'][0]['communals'][0]
        self.assertEqual(item['name'], '_reviewed_global')
        self.assertEqual(item['size_basis']['declaration'], 'int reviewed_global[2];')
        self.assertEqual(item['size'], 4)
        self.assertIn(item['size_basis']['declaration'].encode(), changes['game/owner.c'])

    def test_source_only_global_alias_does_not_invent_a_registry_public(self):
        files = fixture_files()
        files['game/owner.c'] += b'extern int old_global;\n'
        files['layout/data-symbols.json'] = json_bytes({'symbols': {'_old_global': {'load_address': 32}}})
        row = {'kind': 'global', 'old': 'old_global', 'new': 'reviewed_global', 'address': 32}
        changes = rename.make_changes(files, [row])
        registry = json.loads(changes.get('layout/names-registry.json', files['layout/names-registry.json']))
        self.assertNotIn('32', registry['names'])
        self.assertIn('_reviewed_global', json.loads(changes['layout/data-symbols.json'])['symbols'])
        rename.check_applied([row], {**files, **changes})

    def test_file_move_reports_port_basename_in_split_case_variant_path(self):
        with tempfile.TemporaryDirectory() as work:
            root = Path(work)
            path = root / 'port/build.py'
            path.parent.mkdir()
            original = b"source = ROOT / 'game' / 'OWNER.C'\n"
            path.write_bytes(original)
            rows = [{'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}]
            hits = rename.port_occurrences(root, rows)
            self.assertEqual(hits[0]['names'], ['owner.c'])
            self.assertEqual(hits[0]['line'], 1)
            self.assertEqual(path.read_bytes(), original)

    def test_file_move_updates_case_variant_path_references(self):
        files = fixture_files()
        files['tools/ref.py'] = b"p = ROOT / 'game' / 'OWNER.C'\nq = 'GAME/OWNER.C'\n"
        rows = [{'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}]
        changes = rename.make_changes(files, rows)
        self.assertIn(b"'readable.c'", changes['tools/ref.py'])
        self.assertIn(b"'game/readable.c'", changes['tools/ref.py'])

    def test_comment_mentions_do_not_collide(self):
        files = fixture_files()
        files['lib/caller.ASM'] += b'; reviewed_function is a proposed contract name\n'
        self.assertFalse(rename.preflight([ROW], files)[1])


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

    def test_external_order_and_indexes_may_change(self):
        before = external_fixture(['_target', '_unused'], target_index=1)
        after = external_fixture(['_unused', '_target'], target_index=2)
        self.assertIsNone(rename.compare_objects(before, after, {}))

    def test_fixup_target_change_with_identical_record_encoding_is_refused(self):
        before = external_fixture(['_target', '_unused'], target_index=1)
        after = external_fixture(['_unused', '_target'], target_index=1)
        diff = rename.compare_objects(before, after, {})
        self.assertIn('linker_fixups', diff['semantic']['record'])

    def test_public_order_may_change(self):
        before = local_fixture()
        after = local_fixture()
        after[0].publics.reverse()
        self.assertIsNone(rename.compare_objects(before, after, {}))

    def test_extra_unused_external_is_refused(self):
        before = external_fixture(['_target'], 1)
        after = external_fixture(['_target', '_unused'], 1)
        self.assertIsNotNone(rename.compare_objects(before, after, {}))

    def test_declaration_type_indexes_are_preserved(self):
        before = external_fixture(['_target'], 1)
        raw = b''.join(record(k, b[:-1] + b'\x01' if k == 0x8C else b)
                       for k, b in OmfReader.records(before[1]))
        after = read_object(raw), raw
        self.assertIn('declarations', rename.compare_objects(before, after, {})['semantic']['record'])

    def test_communal_order_requires_whole_image_but_sizes_are_exact(self):
        before = communal_fixture([('_one', 2), ('_two', 4)])
        after = communal_fixture([('_two', 4), ('_one', 2)])
        self.assertIsNone(rename.compare_objects(before, after, {}))
        changed = communal_fixture([('_two', 4), ('_one', 3)])
        self.assertIn('communals', rename.compare_objects(before, changed, {})['semantic']['record'])

    def test_external_index_reorder_through_rename_map(self):
        before = external_fixture(['_target', '_unused'], 1)
        after = external_fixture(['_unused', '_renamed_target'], 2)
        self.assertIsNone(rename.compare_objects(before, after, {'_target': '_renamed_target'}))

    def test_refresh_records_new_orders_and_fixup_indexes(self):
        after = external_fixture(['_unused', '_target'], 2)[0]
        recipe = {'object_declarations': {}, 'expected_fixups': [], 'binding': {'declarations': {}}}
        updated = rename.refresh_expectations(recipe, after)
        self.assertEqual(updated['object_declarations']['externals'], ['_unused', '_target'])
        self.assertEqual(updated['expected_fixups'][0]['target_index'], 2)
        self.assertEqual(updated['binding']['declarations'], updated['object_declarations'])
        self.assertEqual(recipe['expected_fixups'], [])


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
        for path in ('layout/oracle.lock.json', 'game/../layout/oracle.lock.json', '../outside',
                     'include/../../outside', '.git/index'):
            with self.subTest(path=path), self.assertRaises(ValueError):
                rename.publication_path(path)

    def test_port_is_untouched_on_successful_publication(self):
        path = self.root / 'port/service.c'
        path.parent.mkdir()
        path.write_bytes(b'int old_function(void);\n')
        self.files['port/service.c'] = path.read_bytes()
        rename.publish(rename.make_changes(self.files, [ROW]), self.files, lambda: None)
        self.assertEqual(path.read_bytes(), b'int old_function(void);\n')

    def test_historical_link_is_mandatory_and_loaded_set_is_checked(self):
        from types import SimpleNamespace
        baseline = {'status': 'BUILT_EXACT', 'loaded_library_modules': [('GAME.LIB', 1, 'UNIT')],
                    'explicit_objects': 1}
        candidate = {**baseline, 'loaded_library_modules': [('GAME.LIB', 2, 'UNIT')]}
        calls = []
        def historical():
            calls.append((self.root / 'game/owner.c').read_bytes())
            return baseline if len(calls) == 1 else candidate
        engine = rename.ProofEngine(self.root, self.files,
                                    compiler=lambda recipe, source: object_fixture(recipe['public']))
        with patch('oracle.verify'), patch('build_exact.build', return_value={'status': 'HYBRID_EXACT', 'relocation_count': 1}), \
             patch.object(rename, 'exact_bss_gate', return_value={'status': 'PLACED'}), \
             patch.object(rename, 'historical_proof', side_effect=historical):
            result = engine.trial([ROW], whole=True)
        self.assertEqual(len(calls), 2)
        self.assertIn(b'old_function', calls[0])
        self.assertIn(b'reviewed_function', calls[1])
        self.assertEqual(result['failures'][0]['category'], 'name-sensitive')
        self.assertIn('different set of modules', result['failures'][0]['error'])

    def test_historical_member_identities_survive_worker_json(self):
        import histbuild
        link = self.root / 'build/historical/reallink/link-library'
        link.mkdir(parents=True)
        (link / 'RESULT.MAP').write_text(' 0000:0000       _old_function\n')
        (link / 'GAME.LIB').write_bytes(b'game')
        (link / 'LIBH.LIB').write_bytes(b'helpers')
        (link / 'objects.json').write_text('[{}]')
        report = {'status': 'BUILT_EXACT', 'report': 'build/historical/report.json',
                  'link': {'library_report': {'explicit_objects': 1}}}
        def library(raw):
            return [('UNIT', object_fixture()[1])] if raw == b'game' else []
        with patch.object(histbuild, 'ROOT', self.root), \
             patch.object(histbuild, 'build', return_value=report), \
             patch.object(OmfReader, 'split_library', side_effect=library):
            fresh = rename.historical_proof()
        from_worker = json.loads(json_bytes(fresh))
        self.assertEqual(fresh['loaded_library_modules'], [['GAME.LIB', 0, 'UNIT']])
        self.assertEqual(fresh['loaded_library_modules'], from_worker['loaded_library_modules'])
        changed = copy.deepcopy(from_worker)
        changed['loaded_library_modules'][0][1] = 1
        self.assertNotEqual(fresh['loaded_library_modules'], changed['loaded_library_modules'])

    def test_external_reorder_is_clean_and_recipe_refresh_is_transactional(self):
        recipe = json.loads(self.files['recipes/owner.json'])
        recipe.update(object_declarations={}, expected_fixups=[])
        self.files['recipes/owner.json'] = json_bytes(recipe)
        (self.root / 'recipes/owner.json').write_bytes(self.files['recipes/owner.json'])
        def compiler(recipe, source):
            renamed = b'reviewed_function' in source
            obj, raw = external_fixture(['_unused', '_target'] if renamed else ['_target', '_unused'],
                                        2 if renamed else 1)
            obj.publics[0]['name'] = recipe['public']
            # Rebuild the actual PUBDEF too; semantic and record views agree.
            raw = b''.join(record(k, b'\0\1' + name(recipe['public']) + b'\0\0\0' if k == 0x90 else b)
                           for k, b in OmfReader.records(raw))
            return read_object(raw), raw
        engine = rename.ProofEngine(self.root, self.files, compiler, image=lambda: {'status': 'HYBRID_EXACT'})
        result = engine.trial([ROW], whole=True)
        self.assertFalse(result['failures'])
        updated = json.loads(engine.current['recipes/owner.json'])
        self.assertEqual(updated['object_declarations']['externals'], ['_unused', '_target'])
        self.assertEqual(updated['expected_fixups'][0]['target_index'], 2)
        engine.stage({})
        rename.publish(rename.make_changes(self.files, [ROW]), self.files, lambda: None)

    def test_split_path_references_and_applied_move_check(self):
        files = {**self.files, 'tools/ref.py': b"p = ROOT / 'game' / 'owner.c'\n"}
        row = {'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}
        changes = rename.make_changes(files, [row])
        self.assertIn(b"'readable.c'", changes['tools/ref.py'])
        applied = {k: v for k, v in {**files, **changes}.items() if v is not None}
        rename.check_applied([row], applied)

    def test_identical_whole_trials_reuse_frozen_proof_and_restore_files(self):
        calls = []
        def image():
            calls.append(1)
            return {'status': 'HYBRID_EXACT'}
        engine = rename.ProofEngine(self.root, self.files,
                                    compiler=lambda recipe, source: object_fixture(recipe['public']), image=image)
        first = engine.trial([ROW], whole=True)
        engine.stage({})
        second = engine.trial([ROW], whole=True)
        self.assertFalse(first['failures'])
        self.assertFalse(second['failures'])
        self.assertEqual(second['reused_from'], 1)
        self.assertEqual(calls, [1])
        self.assertIn(b'reviewed_function', (self.root / 'game/owner.c').read_bytes())

    def test_failed_unrenamed_baseline_is_a_proof_error(self):
        engine = rename.ProofEngine(self.root, self.files,
                                    compiler=lambda recipe, source: object_fixture(recipe['public']))
        with patch('oracle.verify'), patch.object(rename, 'historical_proof', side_effect=rename.ExactnessFailure('baseline differs')):
            result = engine.trial([ROW], whole=True)
        self.assertEqual(result['failures'][0]['category'], 'proof-error')
        self.assertIn('Unrenamed historical baseline failed', result['failures'][0]['error'])

    def test_assembly_owner_move_updates_child_parent_without_inventory_rewrite(self):
        recipe = json.loads(self.files['recipes/owner.json'])
        recipe.update(kind='asm', source='lib/caller.ASM')
        self.files['recipes/owner.json'] = json_bytes(recipe)
        manifest = json.loads(self.files['layout/manifest.json'])
        manifest['owners'].append({'id': 'owner:_BSS', 'kind': 'MATCHING_ASM_DATA', 'parent': 'owner'})
        self.files['layout/manifest.json'] = json_bytes(manifest)
        row = {'kind': 'asm-module', 'old': 'owner', 'new': 'readable_module',
               'source': 'lib/caller.ASM', 'new_source': 'lib/readable.ASM'}
        changes = rename.make_changes(self.files, [row])
        child = json.loads(changes['layout/manifest.json'])['owners'][1]
        self.assertEqual(child['parent'], 'readable_module')
        self.assertEqual(child['id'], 'readable_module:_BSS')

    def test_file_move_updates_paths_and_rolls_back(self):
        for key in ('tools/ref.py', 'tests/ref.py', 'layout/ref.json'):
            self.files[key] = b'"game/owner.c"\n'
            path = self.root / key
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(self.files[key])
        row = {'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}
        good, bad = rename.preflight([row], self.files)
        self.assertEqual(good, [row])
        self.assertFalse(bad)
        changes = rename.make_changes(self.files, [row])
        self.assertIsNone(changes['game/owner.c'])
        self.assertEqual(changes['game/readable.c'], self.files['game/owner.c'])
        for key in ('tools/ref.py', 'tests/ref.py', 'layout/ref.json'):
            self.assertIn(b'game/readable.c', changes[key])
        self.assertEqual(json.loads(changes['recipes/owner.json'])['source'], 'game/readable.c')
        def fail():
            self.assertFalse((self.root / row['old']).exists())
            self.assertTrue((self.root / row['new']).exists())
            raise rename.ExactnessFailure('packed executable differs')
        with self.assertRaisesRegex(ValueError, 'packed executable'):
            rename.publish(changes, self.files, fail)
        self.assertEqual(rename.canonical_files(self.root), self.files)
        self.assertFalse((self.root / row['new']).exists())

    def test_assembly_owner_and_source_move_together(self):
        recipe = json.loads(self.files['recipes/owner.json'])
        recipe.update(kind='asm', source='lib/caller.ASM')
        self.files['recipes/owner.json'] = json_bytes(recipe)
        row = {'kind': 'asm-module', 'old': 'owner', 'new': 'readable_module',
               'source': 'lib/caller.ASM', 'new_source': 'lib/readable.ASM'}
        good, bad = rename.preflight([row], self.files)
        self.assertFalse(bad)
        changes = rename.make_changes(self.files, good)
        self.assertEqual(json.loads(changes['recipes/readable_module.json'])['id'], 'readable_module')
        self.assertEqual(json.loads(changes['layout/manifest.json'])['owners'][0]['id'], 'readable_module')
        self.assertIsNone(changes['lib/caller.ASM'])
        self.assertIn('lib/readable.ASM', changes)

    def test_cross_directory_moves_update_every_current_reference_and_prove(self):
        for source, destination in (('game/owner.c', 'lib/owner.c'),
                                    ('lib/caller.ASM', 'game/caller.ASM')):
            with self.subTest(source=source):
                files = dict(self.files)
                recipe = json.loads(files['recipes/owner.json'])
                recipe['source'] = source
                files['recipes/owner.json'] = json_bytes(recipe)
                for path in ('port/ref.py', 'tools/porting/ref.py', 'docs/porting/ref.md',
                             'README.md', 'AGENTS.md', 'Makefile'):
                    files[path] = (source + "\nROOT / '" + str(Path(source).parent) +
                                   "' / '" + Path(source).name + "'\n").encode()
                files['evidence/ref.md'] = source.encode()
                row = dict(kind='file', old=source, new=destination)
                good, bad = rename.preflight([row], files)
                self.assertFalse(bad)
                changes = rename.make_changes(files, good)
                self.assertIsNone(changes[source])
                self.assertEqual(changes[destination], files[source])
                self.assertEqual(json.loads(changes['recipes/owner.json'])['source'], destination)
                self.assertNotIn('evidence/ref.md', changes)
                for path in ('port/ref.py', 'tools/porting/ref.py', 'docs/porting/ref.md',
                             'README.md', 'AGENTS.md', 'Makefile'):
                    self.assertIn(destination.encode(), changes[path])
                    self.assertIn(("'" + str(Path(destination).parent) + "'").encode(), changes[path])
                engine = rename.ProofEngine(self.root, files,
                    compiler=lambda recipe, source: object_fixture(),
                    image=lambda: {'status': 'HYBRID_EXACT'})
                self.assertFalse(engine.trial([row], whole=True)['failures'])
                engine.stage({})

    def test_owner_only_renames_are_complete_for_both_source_kinds(self):
        for kind, source in (('c-unit', 'game/owner.c'), ('asm-module', 'lib/caller.ASM')):
            with self.subTest(kind=kind):
                files = dict(self.files)
                recipe = json.loads(files['recipes/owner.json'])
                recipe.update(source=source, kind='asm' if kind == 'asm-module' else 'c')
                files['recipes/owner.json'] = json_bytes(recipe)
                manifest = json.loads(files['layout/manifest.json'])
                manifest['owners'][0]['name'] = 'old_inventory_function'
                manifest['owners'].append({'id': 'owner:_DATA', 'kind': 'MATCHING_C_DATA',
                                           'parent': 'owner'})
                manifest['bss_owners'] = [{'id': 'owner:_BSS', 'parent': 'owner',
                                          'declarers': ['owner']}]
                files['layout/manifest.json'] = json_bytes(manifest)
                files['evidence/old.json'] = b'{"owner": "owner"}'
                files['port/ref.py'] = b'owner_id = "owner"\n'
                row = dict(kind=kind, old='owner', new='readable', source=source, new_source=source)
                good, bad = rename.preflight([row], files)
                self.assertFalse(bad)
                changes = rename.make_changes(files, good)
                self.assertIsNone(changes['recipes/owner.json'])
                self.assertNotIn(source, changes)
                updated = json.loads(changes['layout/manifest.json'])
                self.assertEqual(updated['owners'][0]['name'], 'readable')
                self.assertEqual(updated['owners'][0]['recipe'], 'recipes/readable.json')
                self.assertEqual(updated['owners'][1]['id'], 'readable:_DATA')
                self.assertEqual(updated['bss_owners'][0]['declarers'], ['readable'])
                self.assertNotIn('evidence/old.json', changes)
                self.assertIn(b'"readable"', changes['port/ref.py'])
                engine = rename.ProofEngine(self.root, files,
                    compiler=lambda recipe, source: object_fixture(),
                    image=lambda: {'status': 'HYBRID_EXACT'})
                result = engine.trial([row], whole=True)
                self.assertFalse(result['failures'])
                engine.stage({})

    def test_file_move_passes_same_object_and_image_proof(self):
        row = {'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}
        sources = []
        def compiler(recipe, source):
            sources.append(recipe['source'])
            return object_fixture()
        engine = rename.ProofEngine(self.root, self.files, compiler, image=lambda: {'status': 'HYBRID_EXACT'})
        result = engine.trial([row], whole=True)
        self.assertFalse(result['failures'])
        self.assertEqual(sources, ['game/owner.c', 'game/readable.c'])
        engine.stage({})
        self.assertEqual(rename.canonical_files(self.root), self.files)

    def test_move_recovery_restores_source_and_removes_destination(self):
        row = {'kind': 'c-unit', 'old': 'game/owner.c', 'new': 'game/readable.c'}
        with transaction.exclusive():
            transaction.apply(rename.prepare_publication(rename.make_changes(self.files, [row])))
        rename.rollback_publication()
        self.assertEqual(rename.canonical_files(self.root), self.files)

    def test_changed_bytes_fixups_or_placement_roll_back(self):
        for reason in ('code byte changed', 'fixup target changed', 'communal placement moved'):
            with self.subTest(reason=reason):
                def fail():
                    raise rename.ExactnessFailure(reason)
                with self.assertRaises(rename.ExactnessFailure):
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
            (self.root / 'game/owner.c').write_bytes(b'user edit')
            raise ValueError('image failed')
        with self.assertRaisesRegex(ValueError, 'conflicting edit'):
            rename.publish(rename.make_changes(self.files, [ROW]), self.files, concurrent)
        self.assertEqual((self.root / 'game/owner.c').read_bytes(), b'user edit')
        self.assertTrue(transaction.journal_path().exists())

    def test_stale_inputs_refuse_before_writes(self):
        (self.root / 'game/owner.c').write_bytes(b'user edit')
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

    def test_communal_placement_failure_is_name_sensitive(self):
        def compiler(recipe, source):
            return object_fixture(recipe['public'])
        def unsupported():
            raise rename.ExactnessFailure('communal moved from original address')
        engine = rename.ProofEngine(self.root, self.files, compiler, image=unsupported)
        clean, failed = engine.compatible([ROW], whole=True)
        self.assertFalse(clean)
        self.assertEqual(failed[0]['status'], 'name-sensitive')
        self.assertEqual(failed[0]['failures'][0]['phase'], 'image/BSS')

    def test_bisection_retains_good_name_and_proves_union(self):
        # Tiny injected compiler: one name changes a real object payload byte.
        files = {**self.files, 'game/owner.c': self.files['game/owner.c'] + b'int second_function(void);\n'}
        (self.root / 'game/owner.c').write_bytes(files['game/owner.c'])
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
        files = {**self.files, 'game/owner.c': self.files['game/owner.c'] + b'int second_function(void);\n'}
        (self.root / 'game/owner.c').write_bytes(files['game/owner.c'])
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
