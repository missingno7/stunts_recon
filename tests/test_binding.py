"""Independent linker differentials and negative production binding checks."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from binder import bind_data_offsets
from common import read_json
from compiler import compile_source
from data_symbols import resolve_symbols
from linker_probe import experiment
from mz import MZ
from oracle import verify


class BindingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.obj, _ = compile_source(b'extern unsigned char flags[]; unsigned int lookup(int i) { return flags[i+3]; }\n', 'msc510-medium')
        cls.declarations = {'segments': cls.obj.segment_defs, 'groups': cls.obj.groups,
                            'publics': cls.obj.publics, 'externals': cls.obj.externals}
        cls.symbols = {'_flags': {'group': 'DGROUP', 'frame_load_address': 0x2b770, 'load_address': 0x34d06}}

    def bind(self, obj=None, expected=None, declarations=None, symbols=None, length=14):
        return bind_data_offsets(obj or self.obj, 'UNIT_TEXT', '_lookup', length,
            self.obj.linker_fixups if expected is None else expected,
            self.declarations if declarations is None else declarations,
            self.symbols if symbols is None else symbols)

    def test_original_addend_and_target_frame(self):
        payload, report = self.bind()
        self.assertEqual(payload.hex(), '558bec8b5e068a8799952ae45dcb')
        self.assertEqual(self.obj.segment_bytes('UNIT_TEXT')[8:10], b'\x03\x00')
        self.assertEqual(report['generated_relocations'], [])

    def test_historical_link_differential(self):
        for profile in ['msc500-medium', 'msc510-medium']:
            for paragraph in [False, True]:
                with self.subTest(profile=profile, paragraph=paragraph):
                    _, _, row = experiment(0x123, 3, profile, paragraph)
                    self.assertTrue(row['historical_link_equal'])
                    self.assertEqual(row['binding']['fixups'][0]['frame_load_address'], 16 if paragraph else 0)
                    self.assertEqual(row['binding']['fixups'][0]['linked_value'], 0x126 if paragraph else 0x134)

    def test_obligations_must_be_complete(self):
        with self.assertRaises(ValueError): self.bind(expected=[])
        fix = copy.deepcopy(self.obj.linker_fixups); fix[0]['encoded_addend'] = '0000'
        with self.assertRaises(ValueError): self.bind(expected=fix)

    def test_declarations_cannot_drift(self):
        dec = copy.deepcopy(self.declarations); dec['externals'].append('_unchecked')
        with self.assertRaises(ValueError): self.bind(declarations=dec)

    def test_missing_or_unused_external(self):
        with self.assertRaises(ValueError): self.bind(symbols={})
        with self.assertRaises(ValueError): self.bind(symbols={**self.symbols, '_extra': self.symbols['_flags']})

    def test_unproven_modes_rejected(self):
        for key, value in [('frame_method', 1), ('loc', 'pointer32'), ('self_relative', True),
                           ('displacement', 1), ('target_kind', 'segment')]:
            obj = copy.deepcopy(self.obj); obj.linker_fixups[0][key] = value
            with self.subTest(key=key), self.assertRaises(ValueError):
                self.bind(obj=obj, expected=obj.linker_fixups)

    def test_bad_frame_or_overflow(self):
        for changes in [{'frame_load_address': 1}, {'load_address': 0}, {'group': 'OTHER'},
                        {'load_address': 0x2b770 + 65535}]:
            symbols = copy.deepcopy(self.symbols); symbols['_flags'].update(changes)
            with self.subTest(changes=changes), self.assertRaises(ValueError): self.bind(symbols=symbols)

    def test_no_trimming_or_hidden_storage(self):
        with self.assertRaises(ValueError): self.bind(length=13)
        obj = copy.deepcopy(self.obj); obj.segment_lengths['_BSS'] = 4
        with self.assertRaises(ValueError): self.bind(obj=obj)

    def test_encoded_addend_must_equal_object(self):
        obj = copy.deepcopy(self.obj); obj.linker_fixups[0]['encoded_addend'] = '0200'
        with self.assertRaises(ValueError): self.bind(obj=obj, expected=obj.linker_fixups)

    def test_real_far_call_remains_blocked(self):
        obj, _ = compile_source(b'extern int helper(int); int lookup(int i) { return helper(i); }\n', 'msc510-medium')
        dec = {'segments': obj.segment_defs, 'groups': obj.groups, 'publics': obj.publics, 'externals': obj.externals}
        with self.assertRaises(ValueError):
            self.bind(obj=obj, expected=obj.linker_fixups, declarations=dec,
                      symbols={'_helper': self.symbols['_flags']}, length=obj.segment_length('UNIT_TEXT'))

    def test_symbol_evidence_and_bss(self):
        result = verify(write=False); image = MZ.parse(result[1]).load_image(result[1])
        relocs = result[2]['unpacked_mz']['relocations']
        symbols = resolve_symbols(['_byte_44D06'], image, relocs)
        self.assertGreater(symbols['_byte_44D06']['load_address'], len(image))
        layout = read_json(ROOT/'layout/data-symbols.json')
        with self.assertRaises(ValueError): resolve_symbols(['_byte_44D06'], image, [])
        bad = copy.deepcopy(layout); bad['symbols']['_byte_44D06']['load_address'] += 1
        with patch('data_symbols.read_json', return_value=bad), self.assertRaises(ValueError):
            resolve_symbols(['_byte_44D06'], image, relocs)

    def test_reviewed_struct_field_mutation_rejected(self):
        result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1]);relocs=result[2]['unpacked_mz']['relocations']
        self.assertEqual(resolve_symbols(['_clip'],image,relocs)['_clip']['load_address'],0x30eb0)
        bad=read_json(ROOT/'layout/data-symbols.json');bad['symbols']['_clip']['fields'][1]['offset']+=2
        with patch('data_symbols.read_json',return_value=bad),self.assertRaises(ValueError):
            resolve_symbols(['_clip'],image,relocs)


if __name__ == '__main__': unittest.main()
