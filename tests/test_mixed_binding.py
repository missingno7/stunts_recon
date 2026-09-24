"""Historical LINK differentials and fail-closed mixed far/data binding checks."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from binder import bind_contribution, bind_mixed_far_data
from compiler import compile_source
from linker_probe import mixed_far_data_experiment


SOURCE = (b'extern unsigned char flags[]; '
          b'long product(long a,long b) { flags[1]=3; flags[2]=4; return a*b; }\n')


class MixedBindingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.obj, _ = compile_source(SOURCE, 'msc510-medium')
        cls.start = 0x15000
        cls.symbols = {
            '__aFlmul': {'kind': 'far-code', 'frame_load_address': 0x1cc50,
                         'load_address': 0x1e8d8},
            '_flags': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                       'load_address': 0x34d06},
        }
        cls.relocations = [
            {'segment': 0x1000, 'offset': 0x5000 + f['offset'] + 2,
             'load_offset': cls.start + f['offset'] + 2}
            for f in cls.obj.linker_fixups if f['loc'] == 'pointer32'
        ]

    def bind(self, obj=None, expected=None, declarations=None, symbols=None,
             relocations=None, length=None):
        obj = obj or self.obj
        declarations = declarations or {
            'segments': obj.segment_defs, 'groups': obj.groups,
            'publics': obj.publics, 'externals': obj.externals,
        }
        return bind_mixed_far_data(
            obj, 'UNIT_TEXT', '_product',
            obj.segment_length('UNIT_TEXT') if length is None else length,
            obj.linker_fixups if expected is None else expected,
            declarations, self.symbols if symbols is None else symbols,
            self.start, self.relocations if relocations is None else relocations)

    def test_historical_link_both_compilers_and_segment_orders(self):
        for profile in ('msc510-medium', 'msc500-medium'):
            for first, offset in ((False, 0x123), (True, 0x2a)):
                with self.subTest(profile=profile, library_first=first):
                    obj, symbols, row = mixed_far_data_experiment(profile, first, offset)
                    self.assertTrue(row['historical_link_equal'])
                    self.assertEqual([f['loc'] for f in obj.linker_fixups],
                                     ['pointer32', 'offset16', 'offset16'])
                    self.assertEqual(len(row['relocations']), 1)
                    self.assertEqual(set(symbols), {'__aFlmul', '_flags'})

    def test_complete_mixed_binding_and_recipe_dispatch(self):
        payload, report = self.bind()
        self.assertEqual(report['mode'], 'external-far-call-dgroup-offset16-v1')
        self.assertEqual(report['generated_relocations'], self.relocations)
        self.assertEqual([f['kind'] for f in report['fixups']],
                         ['far-call', 'dgroup-offset16', 'dgroup-offset16'])
        self.assertEqual(payload[26:30], bytes.fromhex('881cc51c'))
        self.assertEqual(payload[10:12], bytes.fromhex('9895'))
        self.assertEqual(payload[5:7], bytes.fromhex('9795'))
        recipe = {'object_segment': 'UNIT_TEXT', 'public': '_product',
                  'start': self.start, 'end': self.start + len(payload),
                  'expected_fixups': self.obj.linker_fixups,
                  'expected_relocations': self.relocations,
                  'binding': {'mode': report['mode'], 'declarations': {
                      'segments': self.obj.segment_defs, 'groups': self.obj.groups,
                      'publics': self.obj.publics, 'externals': self.obj.externals}}}
        self.assertEqual(bind_contribution(self.obj, recipe, self.symbols), (payload, report))

    def test_complete_fixups_declarations_and_ownership_required(self):
        with self.assertRaises(ValueError):
            self.bind(expected=self.obj.linker_fixups[:-1])
        with self.assertRaises(ValueError):
            self.bind(expected=list(reversed(self.obj.linker_fixups)))
        with self.assertRaises(ValueError):
            self.bind(length=self.obj.segment_length('UNIT_TEXT') - 1)
        obj = copy.deepcopy(self.obj)
        obj.segment_lengths['_BSS'] = 1
        with self.assertRaises(ValueError):
            self.bind(obj=obj)
        obj = copy.deepcopy(self.obj)
        obj.publics.append({'name': '_hidden', 'segment': 'UNIT_TEXT', 'offset': 1})
        with self.assertRaises(ValueError):
            self.bind(obj=obj)
        declarations = {'segments': self.obj.segment_defs, 'groups': self.obj.groups,
                        'publics': self.obj.publics,
                        'externals': self.obj.externals + ['_unchecked']}
        with self.assertRaises(ValueError):
            self.bind(declarations=declarations)

    def test_unproven_fixup_modes_and_frames_rejected(self):
        for index, key, value in [
            (0, 'frame_method', 1), (0, 'self_relative', True),
            (0, 'displacement', 1), (0, 'encoded_addend', '01000000'),
            (0, 'offset', 0), (0, 'target_index', 3),
            (1, 'frame_kind', 'group'), (1, 'target_method', 1),
            (1, 'loc', 'base16'), (1, 'displacement', 1),
            (1, 'encoded_addend', '0000'), (1, 'offset', 26),
        ]:
            obj = copy.deepcopy(self.obj)
            obj.linker_fixups[index][key] = value
            with self.subTest(index=index, key=key), self.assertRaises(ValueError):
                self.bind(obj=obj)

    def test_symbol_and_ordered_relocation_controls(self):
        original, _ = self.bind()
        changed = copy.deepcopy(self.symbols)
        changed['__aFlmul']['load_address'] += 1
        altered, _ = self.bind(symbols=changed)
        self.assertNotEqual(original, altered)
        for bad in ({}, {**self.symbols, '_extra': self.symbols['_flags']}):
            with self.assertRaises(ValueError):
                self.bind(symbols=bad)
        for name, change in [('__aFlmul', {'frame_load_address': 1}),
                             ('__aFlmul', {'kind': 'near-code'}),
                             ('_flags', {'group': 'OTHER'}),
                             ('_flags', {'load_address': 0x2b770 + 65535})]:
            symbols = copy.deepcopy(self.symbols)
            symbols[name].update(change)
            with self.subTest(name=name, change=change), self.assertRaises(ValueError):
                self.bind(symbols=symbols)
        with self.assertRaises(ValueError):
            self.bind(relocations=[])
        wrong = copy.deepcopy(self.relocations)
        wrong[0]['load_offset'] += 1
        with self.assertRaises(ValueError):
            self.bind(relocations=wrong)
        wrong = copy.deepcopy(self.relocations)
        wrong[0]['segment'] += 1
        with self.assertRaises(ValueError):
            self.bind(relocations=wrong)


if __name__ == '__main__':
    unittest.main()
