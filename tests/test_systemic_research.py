"""Proof boundaries for research-only context and census helpers."""
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))

from blocker_census import classify, primary_category
from code_symbols import resolve_code_symbols
from candidate_omf_census import compare_nonfixup, effective_output_sha
from data_symbols import resolve_symbols
from compiler import CompileFailure, compile_source
from common import ROOT, read_json
from mz import MZ
from oracle import verify
from prepare_far_call_candidate import straight_line_overlay
from tu_context import _variant_source, public_window, protected_window, protected_component
from x86_16_encoding import conversion_matches_source, reviewed_nop_literal, bare_string_opcode_matches_source


class FakeObject:
    publics = [{'name': '_first', 'segment': 'UNIT_TEXT', 'offset': 0},
               {'name': '_target', 'segment': 'UNIT_TEXT', 'offset': 3},
               {'name': '_later', 'segment': 'UNIT_TEXT', 'offset': 8}]
    linker_fixups = [{'segment': 'UNIT_TEXT', 'offset': 5, 'width': 2, 'target': '_x'},
                     {'segment': 'UNIT_TEXT', 'offset': 9, 'width': 2, 'target': '_y'}]

    def segment_bytes(self, name):
        assert name == 'UNIT_TEXT'
        return bytes(range(12))


class SystemicResearchTests(unittest.TestCase):
    def test_exact_raw_emission_mapping_remains_supervisor_until_cfg_review(self):
        inventory = read_json(ROOT/'recovery/restunts-inventory.json')['functions']
        queue = read_json(ROOT/'recovery/queue.json')['tasks']
        mixed = {f['name'] for f in inventory
                 if f['status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED'}
        self.assertIn('fatal_error', mixed)
        self.assertTrue(all(t['tier'] == 'SUPERVISOR' for t in queue if t['name'] in mixed))
        self.assertEqual(classify({'boundary_status': 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED',
                                   'capability_blockers': []},
                                  {'state': 'EMISSION_BYTES_VERIFIED_CFG_UNREVIEWED'}),
                         ['EMISSION_BYTES_CFG_REVIEW'])

    def test_nonfixup_census_never_masks_opcode_or_other_bytes(self):
        class Candidate:
            segment_lengths = {'UNIT_TEXT': 6, '_DATA': 0}
            publics = [{'name': '_target', 'segment': 'UNIT_TEXT', 'offset': 0}]
            linker_fixups = [{'segment': 'UNIT_TEXT', 'offset': 1, 'width': 4}]

            def segment_bytes(self, name):
                return self.code

        candidate = Candidate()
        candidate.code = bytes.fromhex('9a00000000cb')
        function = {'name': 'target', 'status': 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                    'start': 0, 'end': 6}
        original = bytes.fromhex('9a34127856cb')
        result = compare_nonfixup(candidate, function, original)
        self.assertEqual((result['nonfixup_equal'], result['nonfixup_total_bytes']), (True, 2))
        candidate.code = bytes.fromhex('e800000000cb')
        self.assertFalse(compare_nonfixup(candidate, function, original)['nonfixup_equal'])

    def test_effective_omf_collapse_preserves_fixup_and_public_differences(self):
        base = dict(segment_defs=[{'name': 'UNIT_TEXT', 'length': 6}],
                    segment_lengths={'UNIT_TEXT': 6},
                    segments={'UNIT_TEXT': bytes.fromhex('9a00000000cb')},
                    publics=[{'name': '_target', 'segment': 'UNIT_TEXT', 'offset': 0}],
                    externals=['_helper'],
                    linker_fixups=[{'segment': 'UNIT_TEXT', 'offset': 1,
                                    'width': 4, 'target': '_helper'}])
        first = SimpleNamespace(**base, module_name='FIRST')
        second = SimpleNamespace(**base, module_name='SECOND')
        self.assertEqual(effective_output_sha(first), effective_output_sha(second))
        changed_fixup = SimpleNamespace(**{**base, 'linker_fixups': [
            {**base['linker_fixups'][0], 'target': '_other'}]})
        changed_public = SimpleNamespace(**{**base, 'publics': [
            {**base['publics'][0], 'offset': 1}]})
        self.assertNotEqual(effective_output_sha(first), effective_output_sha(changed_fixup))
        self.assertNotEqual(effective_output_sha(first), effective_output_sha(changed_public))

    def test_public_window_keeps_only_its_fixups_with_relative_offset(self):
        window = public_window(FakeObject(), '_target')
        self.assertEqual(window['bytes'], bytes(range(3, 8)))
        self.assertEqual(window['fixups'],
                         [{'segment': 'UNIT_TEXT', 'offset': 2, 'width': 2, 'target': '_x'}])

    def test_protected_neighbor_requires_complete_literal_fixup_free_window(self):
        self.assertEqual(protected_window(FakeObject(), '_first', bytes(range(3)))['state'],
                         'EXACT_LITERAL_WINDOW')
        self.assertEqual(protected_window(FakeObject(), '_target', bytes(range(3, 8)))['state'],
                         'DIFFERS')
        self.assertEqual(protected_window(FakeObject(), '_absent', b'abc')['state'],
                         'MISSING_PUBLIC')

    def test_protected_component_keeps_complete_public_and_fixup_topology(self):
        class Candidate(FakeObject):
            segment_lengths = {'UNIT_TEXT': 12}
        candidate = Candidate()
        lock = {'segment': 'UNIT_TEXT', 'publics': candidate.publics,
                'fixups': candidate.linker_fixups}
        self.assertEqual(protected_component(candidate, lock, bytes(range(12)))['state'],
                         'EXACT_LITERAL_COMPONENT')
        changed = {**lock, 'publics': [*candidate.publics[:-1],
                   {**candidate.publics[-1], 'offset': 9}]}
        self.assertEqual(protected_component(candidate, changed, bytes(range(12)))['state'],
                         'DIFFERS')
        changed = {**lock, 'fixups': candidate.linker_fixups[:-1]}
        self.assertEqual(protected_component(candidate, changed, bytes(range(12)))['state'],
                         'DIFFERS')

    def test_context_composition_preserves_frozen_target_bytes(self):
        target = b'int target(void) { return 7; }'
        snippets = {'neighbor': b'int neighbor(void) { return 2; }'}
        before = _variant_source(target, {'before': ['neighbor']}, snippets)
        after = _variant_source(target, {'after': ['neighbor']}, snippets)
        self.assertEqual(before.count(target), 1)
        self.assertEqual(after.count(target), 1)
        self.assertNotEqual(before, after)

    def test_relocation_category_remains_unknown_without_object(self):
        card = {'boundary_status': 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                'capability_blockers': [], 'disassembly': []}
        observations = {'state': 'LINEAR_DECODE_COMPLETE_RESEARCH_ONLY',
                        'card_instruction_evidence': False, 'relocation_sites': 1,
                        'calls': {'direct_far_9a': 1}}
        labels = classify(card, observations)
        self.assertIn('BINDING_OR_FIXUP_MODE_UNKNOWN', labels)
        self.assertIn('INSTRUCTION_EVIDENCE_MISSING', labels)
        self.assertEqual(primary_category(labels)[0], 'BINDING_OR_FIXUP_MODE_UNKNOWN')
        self.assertEqual(primary_category(['DATA_OR_GLOBAL_OWNERSHIP',
                                           'TU_OR_OBJECT_CONTEXT_CANDIDATE'])[0], 'UNKNOWN')

    def test_local_omf_records_are_research_only(self):
        source = (b'static int helper(int value) { return value + 1; }\n'
                  b'int target(int value) { return helper(value); }\n')
        with self.assertRaises(CompileFailure) as rejected:
            compile_source(source, 'msc510-medium')
        self.assertEqual(rejected.exception.category, 'UNSUPPORTED_OBJECT')
        obj, receipt = compile_source(source, 'msc510-medium', research_local_symbols=True)
        self.assertTrue(obj.local_symbol_records)
        self.assertTrue(any(f['self_relative'] for f in obj.linker_fixups))
        self.assertTrue(receipt['research_local_symbols'])

    def test_raw_code_alias_needs_two_independent_relocated_anchors(self):
        _, unpacked, report, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        relocations = report['unpacked_mz']['relocations']
        symbol = resolve_code_symbols({'_kb_call_readchar_callback'}, image, relocations)
        self.assertEqual(symbol['_kb_call_readchar_callback'],
                         {'kind': 'far-code', 'frame_load_address': 125472,
                          'load_address': 133660})
        without_second = [r for r in relocations if r['load_offset'] != 133754]
        with self.assertRaises(ValueError):
            resolve_code_symbols({'_kb_call_readchar_callback'}, image, without_second)

    def test_every_reviewed_code_alias_resolves_from_current_evidence(self):
        _, unpacked, report, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        relocations = report['unpacked_mz']['relocations']
        layout = read_json(ROOT/'layout/code-symbols.json')
        resolved = resolve_code_symbols(set(layout['symbols']), image, relocations)
        self.assertEqual(set(resolved), set(layout['symbols']))
        raw_name = next(name for name, symbol in layout['symbols'].items()
                        if 'mapped_target' in symbol)
        altered = read_json(ROOT/'layout/code-symbols.json')
        altered['symbols'][raw_name]['mapped_target']['sha256'] = '0'*64
        original_read = read_json
        def read_with_tampered_alias(path):
            if path == ROOT/'layout/code-symbols.json':
                return altered
            return original_read(path)
        with patch('code_symbols.read_json', side_effect=read_with_tampered_alias):
            with self.assertRaises(ValueError):
                resolve_code_symbols({raw_name}, image, relocations)
        promoted_name = '_unload_resource'
        manifest = read_json(ROOT/'layout/manifest.json')
        missing_owner = {**manifest, 'owners': [owner for owner in manifest['owners']
                         if owner.get('name') != 'unload_resource']}
        def read_without_promoted_owner(path):
            if path == ROOT/'layout/manifest.json':
                return missing_owner
            return original_read(path)
        with patch('code_symbols.read_json', side_effect=read_without_promoted_owner):
            with self.assertRaises(ValueError):
                resolve_code_symbols({promoted_name}, image, relocations)

    def test_far_call_preparation_requires_original_call_sites(self):
        _, unpacked, _, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        inventory = read_json(ROOT/'recovery/restunts-inventory.json')
        wrapper = next(f for f in inventory['functions'] if f['name'] == 'unload_resource')
        fixups = read_json(ROOT/'recipes/unload_resource.json')['expected_fixups']
        overlay = straight_line_overlay(wrapper, image, fixups)
        self.assertEqual(overlay['padding_offsets'], [wrapper['end']-1])
        wrong = [{**fixups[0], 'offset': fixups[0]['offset']+1}]
        with self.assertRaises(ValueError):
            straight_line_overlay(wrapper, image, wrong)
        near = next(f for f in inventory['functions'] if f['name'] == 'file_load_shape2d_nofatal')
        with self.assertRaises(ValueError):
            straight_line_overlay(near, image, [])

    def test_far_call_preparation_accepts_direct_loop_and_mz_fixup_order(self):
        _, unpacked, _, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        inventory = read_json(ROOT/'recovery/restunts-inventory.json')['functions']
        loop = next(f for f in inventory if f['name'] == 'locate_many_resources')
        loop_fixups = read_json(ROOT/'recipes/locate_many_resources.json')['expected_fixups']
        loop_overlay = straight_line_overlay(loop, image, loop_fixups)
        self.assertEqual(loop_overlay['padding_offsets'], [loop['start']+5, loop['end']-1])
        self.assertTrue(any(row['instruction'].startswith('j') for row in loop_overlay['disassembly']))
        two_calls = next(f for f in inventory if f['name'] == 'file_combine_and_find')
        reverse_fixups = read_json(ROOT/'recipes/file_combine_and_find.json')['expected_fixups']
        self.assertGreater(reverse_fixups[0]['offset'], reverse_fixups[1]['offset'])
        two_call_overlay = straight_line_overlay(two_calls, image, reverse_fixups)
        self.assertEqual(len(two_call_overlay['reachable_instruction_offsets']),
                         len(two_call_overlay['disassembly']))

    def test_independently_anchored_data_addresses(self):
        _, unpacked, oracle_report, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        relocations = oracle_report['unpacked_mz']['relocations']
        names = {'_sdgame2ptr', '_textresprefix'}
        symbols = resolve_symbols(names, image, relocations)
        self.assertEqual(symbols['_sdgame2ptr']['load_address']+2, 0x363da)
        self.assertEqual(symbols['_textresprefix']['load_address'], 0x3645e)
        corrupted = read_json(ROOT/'layout/data-symbols.json')
        corrupted['symbols']['_sdgame2ptr']['references'][0]['start'] += 1
        with patch('data_symbols.read_json', return_value=corrupted), self.assertRaises(ValueError):
            resolve_symbols(names, image, relocations)

    def test_16_bit_conversion_alias_requires_unprefixed_opcode(self):
        self.assertTrue(conversion_matches_source('cbw', b'\x98'))
        self.assertTrue(conversion_matches_source('cwd', b'\x99'))
        self.assertFalse(conversion_matches_source('cbw', b'\x66\x98'))
        self.assertFalse(conversion_matches_source('cwd', b'\x66\x99'))

    def test_explicit_padding_admits_only_one_exact_nop_literal(self):
        self.assertEqual(reviewed_nop_literal('db 144'), b'\x90')
        self.assertIsNone(reviewed_nop_literal('db 0'))
        self.assertIsNone(reviewed_nop_literal('db 144, 0'))

    def test_implicit_string_operands_use_exact_unprefixed_opcode(self):
        self.assertTrue(bare_string_opcode_matches_source('movsw', b'\xa5'))
        self.assertTrue(bare_string_opcode_matches_source('lodsb', b'\xac'))
        self.assertFalse(bare_string_opcode_matches_source('movsw', b'\xf3\xa5'))
        self.assertFalse(bare_string_opcode_matches_source('movsw', b'\xa4'))


if __name__ == '__main__':
    unittest.main()
