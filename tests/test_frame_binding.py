"""Exact callback object binding and negative controls; no toolchain required."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from binder import bind_contribution, bind_frame_callback


class FrameObject:
    def __init__(self):
        self.segment_defs = [
            {'index': index, 'name': name, 'class': segment_class, 'length': length,
             'alignment_code': 2, 'alignment': 'word', 'combine_code': 2,
             'combine': 'public', 'big': False, 'use_32bit_offset': False,
             'frame': None, 'offset': None, 'overlay_index': 1, 'acbp': 72}
            for index, name, segment_class, length in (
                (1, 'UNIT_TEXT', 'CODE', 28), (2, '_DATA', 'DATA', 0),
                (3, 'CONST', 'CONST', 0), (4, '_BSS', 'BSS', 0))
        ]
        self.segment_lengths = {row['name']: row['length'] for row in self.segment_defs}
        self.groups = [{'index': 1, 'name': 'DGROUP', 'segment_indices': [3, 4, 2],
                        'segments': ['CONST', '_BSS', '_DATA']}]
        self.publics = [{'name': '_set_frame_callback', 'segment': 'UNIT_TEXT', 'offset': 0}]
        self.externals = ['__acrtused', '_frame_callback', '_timer_reg_callback',
                          '_word_46468', '_byte_442E4', '_set_frame_callback']
        self.payload = bytes.fromhex(
            'c70600000000b80000ba000052509a0000000083c404c606000000cb')
        self.linker_fixups = []
        for at, loc, width, name, index in [
            (24, 'offset16', 2, '_byte_442E4', 5),
            (15, 'pointer32', 4, '_timer_reg_callback', 3),
            (10, 'base16', 2, '_frame_callback', 2),
            (7, 'loader-offset16', 2, '_frame_callback', 2),
            (2, 'offset16', 2, '_word_46468', 4),
        ]:
            self.linker_fixups.append({
                'segment': 'UNIT_TEXT', 'offset': at, 'width': width, 'loc': loc,
                'self_relative': False, 'target_kind': 'external', 'target': name,
                'displacement': 0, 'frame_method': 5, 'frame_index': 0,
                'target_method': 2, 'target_index': index, 'frame_kind': 'target',
                'frame': name, 'encoded_addend': '00' * width,
            })

    def segment_length(self, name):
        return self.segment_lengths[name]

    def segment_bytes(self, name):
        return self.payload if name == 'UNIT_TEXT' else b''


class FrameBindingTests(unittest.TestCase):
    def setUp(self):
        self.obj = FrameObject()
        self.declarations = {
            'segments': copy.deepcopy(self.obj.segment_defs),
            'groups': copy.deepcopy(self.obj.groups),
            'publics': copy.deepcopy(self.obj.publics),
            'externals': copy.deepcopy(self.obj.externals),
        }
        self.symbols = {
            '_frame_callback': {'kind': 'far-code', 'frame_load_address': 0x11b70,
                                'load_address': 0x12596},
            '_timer_reg_callback': {'kind': 'far-code', 'frame_load_address': 0x1ea20,
                                    'load_address': 0x202aa},
            '_word_46468': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                            'load_address': 0x36468},
            '_byte_442E4': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                            'load_address': 0x342e4},
        }
        self.relocations = [
            {'segment': 4096, 'offset': 9579, 'load_offset': 75115},
            {'segment': 4096, 'offset': 9572, 'load_offset': 75108},
        ]

    def bind(self, obj=None, expected=None, declarations=None, symbols=None,
             relocations=None, segment='UNIT_TEXT', public='_set_frame_callback',
             length=28, start=75098):
        obj = self.obj if obj is None else obj
        return bind_frame_callback(
            obj, segment, public, length,
            obj.linker_fixups if expected is None else expected,
            self.declarations if declarations is None else declarations,
            self.symbols if symbols is None else symbols,
            start, self.relocations if relocations is None else relocations)

    def test_full_payload_and_ordered_relocations(self):
        payload, receipt = self.bind()
        self.assertEqual(payload.hex(),
            'c706f8ac0000b8260abab71152509a8a18a21e83c404c606748b00cb')
        self.assertEqual(receipt['generated_relocations'], self.relocations)
        self.assertEqual([r['loc'] for r in receipt['fixups']],
                         ['offset16', 'pointer32', 'base16', 'loader-offset16', 'offset16'])
        recipe = {'object_segment': 'UNIT_TEXT', 'public': '_set_frame_callback',
                  'start': 75098, 'end': 75126, 'expected_fixups': self.obj.linker_fixups,
                  'expected_relocations': self.relocations,
                  'binding': {'mode': 'external-frame-callback-v1',
                              'declarations': self.declarations}}
        self.assertEqual(bind_contribution(self.obj, recipe, self.symbols), (payload, receipt))

    def test_complete_obligations_and_declarations(self):
        for expected in (self.obj.linker_fixups[:-1],
                         list(reversed(self.obj.linker_fixups))):
            with self.subTest(expected=expected), self.assertRaises(ValueError):
                self.bind(expected=expected)
        for mutate in (
            lambda o: o.linker_fixups.append(copy.deepcopy(o.linker_fixups[-1])),
            lambda o: o.publics.append({'name': '_hidden', 'segment': 'UNIT_TEXT', 'offset': 1}),
            lambda o: o.externals.append('_unchecked'),
            lambda o: o.segment_lengths.__setitem__('_BSS', 2),
            lambda o: o.segment_defs[0].__setitem__('class', 'DATA'),
            lambda o: o.segment_defs[0].__setitem__('alignment_code', 3),
            lambda o: o.segment_defs[0].__setitem__('combine_code', 0),
            lambda o: o.segment_defs[0].__setitem__('overlay_index', 2),
            lambda o: o.segment_defs[0].__setitem__('acbp', 73),
            lambda o: o.groups[0]['segments'].reverse(),
        ):
            obj = copy.deepcopy(self.obj)
            mutate(obj)
            with self.subTest(obj=obj), self.assertRaises(ValueError):
                self.bind(obj=obj)
        for kwargs in ({'length': 27}, {'start': 75099}, {'public': '_other'},
                       {'segment': '_DATA'}):
            with self.subTest(kwargs=kwargs), self.assertRaises(ValueError):
                self.bind(**kwargs)

    def test_fixup_fields_and_object_skeleton(self):
        for index, key, value in [
            (0, 'offset', 23), (0, 'width', 4), (0, 'loc', 'base16'),
            (0, 'target', '_word_46468'), (0, 'target_index', 4),
            (1, 'self_relative', True), (1, 'target_kind', 'group'),
            (1, 'target_method', 1), (1, 'frame_method', 1),
            (1, 'frame_kind', 'group'), (1, 'frame_index', 1),
            (2, 'frame', '_timer_reg_callback'), (3, 'displacement', 1),
            (4, 'encoded_addend', '0100'),
        ]:
            obj = copy.deepcopy(self.obj)
            obj.linker_fixups[index][key] = value
            with self.subTest(index=index, key=key), self.assertRaises(ValueError):
                self.bind(obj=obj)
        for at in (0, 6, 9, 14, 22, 27):
            obj = copy.deepcopy(self.obj)
            obj.payload = obj.payload[:at] + bytes([obj.payload[at] ^ 1]) + obj.payload[at + 1:]
            with self.subTest(at=at), self.assertRaises(ValueError):
                self.bind(obj=obj)

    def test_symbol_evidence_and_relocation_coordinates(self):
        for name, key, value in [
            ('_frame_callback', 'kind', 'near-code'),
            ('_frame_callback', 'frame_load_address', 0x11b80),
            ('_frame_callback', 'load_address', 0x12597),
            ('_timer_reg_callback', 'load_address', 0x202ab),
            ('_word_46468', 'group', 'OTHER'),
            ('_word_46468', 'load_address', 0x36466),
            ('_byte_442E4', 'frame_load_address', 0x2b780),
        ]:
            symbols = copy.deepcopy(self.symbols)
            symbols[name][key] = value
            with self.subTest(name=name, key=key), self.assertRaises(ValueError):
                self.bind(symbols=symbols)
        with self.assertRaises(ValueError):
            self.bind(symbols={k: v for k, v in self.symbols.items()
                               if k != '_word_46468'})
        for relocations in ([], list(reversed(self.relocations)),
                            [{**self.relocations[0], 'offset': 9580}, self.relocations[1]],
                            [{**self.relocations[0], 'segment': 4097}, self.relocations[1]]):
            with self.subTest(relocations=relocations), self.assertRaises(ValueError):
                self.bind(relocations=relocations)


if __name__ == '__main__':
    unittest.main()
