"""Focused checks for symbol, extent, and source-emission evidence."""
import hashlib
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from code_symbols import resolve_callback_pointer, resolve_code_symbols
from common import read_json
from function_evidence import reviewed_functions
from data_symbols import resolve_symbols
from mz import MZ
from oracle import verify


class EvidenceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.result = verify(write=False)
        cls.image = MZ.parse(cls.result[1]).load_image(cls.result[1])
        cls.relocations = cls.result[2]['unpacked_mz']['relocations']

    def test_verified_function_extents_match_the_pristine_image(self):
        evidence = read_json(ROOT / 'evidence/functions.json')
        self.assertEqual(evidence['load_sha256'], hashlib.sha256(self.image).hexdigest())
        verified = {'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                    'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED'}
        for function in evidence['functions']:
            if function['status'] not in verified:
                continue
            with self.subTest(name=function['name']):
                start, end = function['start'], function['end']
                payload = self.image[start:end]
                self.assertEqual(end - start, function['size'])
                self.assertEqual(len(payload), function['size'])
                self.assertEqual(hashlib.sha256(payload).hexdigest(), function['sha256'])
                if function['bytes_hex'] is not None:
                    self.assertEqual(payload, bytes.fromhex(function['bytes_hex']))

    def test_sprite_shared_return_padding_has_a_bounded_flow_proof(self):
        reviewed = reviewed_functions(self.image)
        self.assertEqual((reviewed['sprite_set_1_from_argptr']['start'],
                          reviewed['sprite_set_1_from_argptr']['end']), (154358, 154388))
        self.assertEqual((reviewed['sprite_clear_1_color']['start'],
                          reviewed['sprite_clear_1_color']['end']), (144064, 144176))
        import copy
        original = read_json(ROOT / 'layout/function-evidence.json')
        for mutation in ('target', 'hex', 'pad'):
            changed = copy.deepcopy(original)
            clear = next(row for row in changed['functions'] if row['name']=='sprite_clear_1_color')
            if mutation == 'target':
                clear['terminal_shared_jump']['target'] += 1
            elif mutation == 'hex':
                clear['terminal_shared_jump']['hex'] = 'ebdd'
            else:
                clear['alignment_padding'][0]['provenance']['source_text'] = 'db 0'
            with self.subTest(mutation=mutation), patch('function_evidence.read_json', return_value=changed):
                with self.assertRaises(ValueError):
                    reviewed_functions(self.image)

    def test_reviewed_dispatch_tables_reject_changed_bounds_and_targets(self):
        reviewed = reviewed_functions(self.image)
        self.assertEqual((reviewed['polarAngle']['start'], reviewed['polarAngle']['end']),
                         (125518,125652))
        self.assertEqual((reviewed['file_load_resource']['start'],
                          reviewed['file_load_resource']['end']), (104906,105094))
        self.assertEqual((reviewed['sub_35C4E']['start'],
                          reviewed['sub_35C4E']['end']), (154702,155080))
        import copy
        original = read_json(ROOT/'layout/function-evidence.json')
        for change in ('index', 'target', 'island'):
            altered = copy.deepcopy(original)
            row = next(f for f in altered['functions'] if f['name']=='polarAngle')
            if change == 'index':
                row['dispatch_tables'][0]['byte_index_values'][-1] = 16
            elif change == 'target':
                row['dispatch_tables'][0]['targets'][0] += 1
            else:
                row['data_islands'][0]['hex'] = '00' + row['data_islands'][0]['hex'][2:]
            with self.subTest(change=change), patch('function_evidence.read_json', return_value=altered):
                with self.assertRaises(ValueError):
                    reviewed_functions(self.image)


    def test_code_aliases_require_independent_relocated_anchors(self):
        symbol = resolve_code_symbols({'_kb_call_readchar_callback'},
                                      self.image, self.relocations)
        self.assertEqual(symbol['_kb_call_readchar_callback'],
                         {'kind': 'far-code', 'frame_load_address': 125472,
                          'load_address': 133660})
        without_second = [row for row in self.relocations if row['load_offset'] != 133754]
        with self.assertRaises(ValueError):
            resolve_code_symbols({'_kb_call_readchar_callback'}, self.image, without_second)

    def test_reviewed_code_aliases_resolve_from_current_evidence(self):
        layout = read_json(ROOT / 'layout/code-symbols.json')
        far_names = {name for name, symbol in layout['symbols'].items()
                     if symbol.get('anchors') or symbol.get('pointer_anchors') or
                     symbol.get('table_anchors')}
        resolved = resolve_code_symbols(far_names, self.image, self.relocations)
        resolved['_frame_callback'] = resolve_callback_pointer(self.image, self.relocations)
        self.assertEqual(set(resolved), set(layout['symbols']))

        raw_name = next(name for name, symbol in layout['symbols'].items()
                        if 'mapped_target' in symbol)
        altered = read_json(ROOT / 'layout/code-symbols.json')
        altered['symbols'][raw_name]['mapped_target']['sha256'] = '0' * 64
        original_read = read_json

        def read_with_tampered_alias(path):
            return altered if path == ROOT / 'layout/code-symbols.json' else original_read(path)

        with patch('code_symbols.read_json', side_effect=read_with_tampered_alias):
            with self.assertRaises(ValueError):
                resolve_code_symbols({raw_name}, self.image, self.relocations)

        promoted_name = '_unload_resource'
        manifest = read_json(ROOT / 'layout/manifest.json')
        # Remove whichever owner (the function or a whole object subsuming it)
        # covers the alias's mapped entry.
        entry = layout['symbols'][promoted_name]['mapped_target']['start']
        missing_owner = {**manifest, 'owners': [owner for owner in manifest['owners']
                                                 if not owner['start'] <= entry < owner['end']]}
        self.assertEqual(len(missing_owner['owners']), len(manifest['owners']) - 1)

        def read_without_promoted_owner(path):
            return missing_owner if path == ROOT / 'layout/manifest.json' else original_read(path)

        with patch('code_symbols.read_json', side_effect=read_without_promoted_owner):
            with self.assertRaises(ValueError):
                resolve_code_symbols({promoted_name}, self.image, self.relocations)

    def test_callback_pointer_alias_requires_its_mapped_relocated_pair(self):
        self.assertEqual(resolve_callback_pointer(self.image, self.relocations),
                         {'kind': 'far-code', 'frame_load_address': 0x11b70,
                          'load_address': 0x12596})
        without_pointer = [row for row in self.relocations if row['load_offset'] != 75144]
        with self.assertRaises(ValueError):
            resolve_callback_pointer(self.image, without_pointer)

        changed = read_json(ROOT / 'layout/code-symbols.json')
        changed['symbols']['_frame_callback']['mapped_target']['sha256'] = '0' * 64

        def read_with_bad_callback(path):
            return changed if path == ROOT / 'layout/code-symbols.json' else read_json(path)

        with patch('code_symbols.read_json', side_effect=read_with_bad_callback):
            with self.assertRaises(ValueError):
                resolve_callback_pointer(self.image, self.relocations)

    def test_independently_anchored_data_symbols(self):
        names = {'_sdgame2ptr', '_textresprefix', '_word_46468', '_byte_442E4'}
        symbols = resolve_symbols(names, self.image, self.relocations)
        self.assertEqual(symbols['_sdgame2ptr']['load_address'] + 2, 0x363da)
        self.assertEqual(symbols['_textresprefix']['load_address'], 0x3645e)
        self.assertEqual(symbols['_word_46468']['load_address'], 0x36468)
        self.assertEqual(symbols['_byte_442E4']['load_address'], 0x342e4)

        corrupted = read_json(ROOT / 'layout/data-symbols.json')
        corrupted['symbols']['_sdgame2ptr']['references'][0]['start'] += 1
        with patch('data_symbols.read_json', return_value=corrupted):
            with self.assertRaises(ValueError):
                resolve_symbols(names, self.image, self.relocations)

        corrupted = read_json(ROOT / 'layout/data-symbols.json')
        corrupted['symbols']['_word_46468']['width'] = 64
        with patch('data_symbols.read_json', return_value=corrupted):
            with self.assertRaises(ValueError):
                resolve_symbols(names, self.image, self.relocations)

    def test_intro_data_aliases_have_reference_names_and_object_bounds(self):
        removed_interiors = {'_rect_unk2', '_rect_unk6'}
        names = {'_bravshape', '_cliprect_unk', '_intro_cliprect',
                 '_intro_colorvalue', '_logo2shape', '_logoshape',
                 '_mat_temp', '_rect_unk3'}
        resolved = resolve_symbols(names, self.image, self.relocations)
        self.assertEqual(set(resolved), names)
        self.assertEqual(len(resolved['_intro_cliprect']['allowed_addends']), 8)
        # integ26: the 15 x 8 rectangle table has a counted-stride extent.
        table = resolve_symbols(['_rect_unk'], self.image, self.relocations)['_rect_unk']
        self.assertEqual(len(table['allowed_addends']), 120)
        layout = read_json(ROOT / 'layout/data-symbols.json')
        field_map = read_json(ROOT / 'layout/aggregate-field-map.json')
        fields = {entry['alias']: entry for entry in field_map['entries']}
        for alias in removed_interiors:
            with self.subTest(alias=alias):
                self.assertNotIn(alias, layout['symbols'])
                entry = fields[alias]
                container = layout['symbols'][entry['container']]
                self.assertEqual(container['load_address'], table['load_address'])
                self.assertEqual(entry['address'], table['load_address'] + entry['offset'])
                self.assertIn(entry['offset'], table['allowed_addends'])
        changed = read_json(ROOT/'layout/data-symbols.json')
        changed['symbols']['_logo2shape']['reference_declaration_line'] += 1
        with patch('data_symbols.read_json', side_effect=lambda path:
                   changed if path == ROOT/'layout/data-symbols.json' else read_json(path)):
            with self.assertRaisesRegex(ValueError, 'label/use'):
                resolve_symbols(['_logo2shape'], self.image, self.relocations)


    def test_bss_symbols_need_relocation_evidence(self):
        symbols = resolve_symbols(['_byte_44D06'], self.image, self.relocations)
        self.assertGreater(symbols['_byte_44D06']['load_address'], len(self.image))
        with self.assertRaises(ValueError):
            resolve_symbols(['_byte_44D06'], self.image, [])


if __name__ == '__main__':
    unittest.main()
