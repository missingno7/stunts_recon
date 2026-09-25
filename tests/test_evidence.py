"""Focused checks for symbol, extent, and source-emission evidence."""
import hashlib
import re
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
from source_emission import numeric_literal_bytes
from x86_16_encoding import (bare_string_opcode_matches_source,
                             conversion_matches_source,
                             reviewed_nop_literal)


def emitted_run(path, label):
    lines = path.read_text(encoding='latin1').splitlines()
    pattern = re.compile(r'^\s*' + re.escape(label) + r'\s+(?:db|dw)\b', re.I)
    start = next(i for i, line in enumerate(lines) if pattern.match(line.split(';', 1)[0]))
    output = []
    for line in lines[start:]:
        payload = numeric_literal_bytes(line.split(';', 1)[0])
        if payload is None:
            break
        output.append(payload)
    return b''.join(output)


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

    def test_numeric_source_emissions_match_the_original_image(self):
        evidence = read_json(ROOT / 'evidence/functions.json')
        functions = {row['name']: row for row in evidence['functions']}
        examples = [('init_div0', 'byte_19F07', True),
                    ('parse_shape2d_helper2', 'word_2F354', True),
                    ('sub_39088', 'byte_3930E', False)]
        for function_name, label, inside in examples:
            with self.subTest(label=label):
                function = functions[function_name]
                emitted = emitted_run(ROOT / function['source'], label)
                address = int(label.rsplit('_', 1)[1], 16) - 0x10000
                self.assertEqual(emitted, self.image[address:address + len(emitted)])
                self.assertGreater(len(emitted), 0)
                self.assertLessEqual(function['start'], address)
                if inside:
                    self.assertLessEqual(address + len(emitted), function['end'])
                else:
                    self.assertGreaterEqual(address, function['end'])

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
                     if symbol.get('anchors') or symbol.get('pointer_anchors')}
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
        missing_owner = {**manifest, 'owners': [owner for owner in manifest['owners']
                                                 if owner.get('name') != 'unload_resource']}

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

    def test_bss_symbols_need_relocation_evidence(self):
        symbols = resolve_symbols(['_byte_44D06'], self.image, self.relocations)
        self.assertGreater(symbols['_byte_44D06']['load_address'], len(self.image))
        with self.assertRaises(ValueError):
            resolve_symbols(['_byte_44D06'], self.image, [])

    def test_literal_parser_accepts_only_one_unsigned_db_or_dw_value(self):
        self.assertEqual(numeric_literal_bytes('byte_19F07 db 30'), b'\x1e')
        self.assertEqual(numeric_literal_bytes('incnums db 0FFh'), b'\xff')
        self.assertEqual(numeric_literal_bytes('word_2ecf8 dw 1234h'), b'\x34\x12')
        self.assertEqual(numeric_literal_bytes('dw 0'), b'\x00\x00')
        for source in ('db 256', 'dw 65536', 'db -1', 'db 1,2',
                       'dw offset loc_12345', 'db symbol', 'db 1+2', 'dd 1',
                       'db 0 dup (?)', 'db ?', 'word_2ecf8 dw 0,0'):
            with self.subTest(source=source):
                self.assertIsNone(numeric_literal_bytes(source))

    def test_x86_source_aliases_reject_prefixes_and_extra_padding(self):
        self.assertTrue(conversion_matches_source('cbw', b'\x98'))
        self.assertTrue(conversion_matches_source('cwd', b'\x99'))
        self.assertFalse(conversion_matches_source('cbw', b'\x66\x98'))
        self.assertFalse(conversion_matches_source('cwd', b'\x66\x99'))

        self.assertEqual(reviewed_nop_literal('db 144'), b'\x90')
        self.assertIsNone(reviewed_nop_literal('db 0'))
        self.assertIsNone(reviewed_nop_literal('db 144, 0'))

        self.assertTrue(bare_string_opcode_matches_source('movsw', b'\xa5'))
        self.assertTrue(bare_string_opcode_matches_source('lodsb', b'\xac'))
        self.assertFalse(bare_string_opcode_matches_source('movsw', b'\xf3\xa5'))
        self.assertFalse(bare_string_opcode_matches_source('movsw', b'\xa4'))


if __name__ == '__main__':
    unittest.main()
