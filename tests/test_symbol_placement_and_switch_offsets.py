"""integ22: pinned procedure-tied data aliases, placed reference widths, new code aliases."""
import copy
import sys
import unittest
from contextlib import contextmanager
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
from code_symbols import resolve_code_symbols
from data_symbols import resolve_symbols
from mz import MZ
from oracle import verify


@contextmanager
def layout_override(module, filename, layout):
    original = read_json
    with patch(module + '.read_json',
               side_effect=lambda path: layout if str(path).endswith(filename) else original(path)):
        yield


class ProcedureTiedDataAliases(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']
        cls.layout = read_json(ROOT/'layout/data-symbols.json')
        cls.frame = cls.layout['frame_load_address']

    def resolve_with(self, name, mutate):
        bad = copy.deepcopy(self.layout)
        mutate(bad['symbols'][name])
        with layout_override('data_symbols', 'data-symbols.json', bad):
            return resolve_symbols([name], self.image, self.relocations)


    def test_use_line_outside_procedure_rejects(self):
        # seg001:9582 genuinely uses gnam_string, but inside setup_aero_trackdata.
        with self.assertRaisesRegex(ValueError, 'outside its pinned procedure'):
            self.resolve_with('_gnam_string', lambda s: s.update(
                reference_use_path='src/restunts/asmorig/seg001.asm', reference_use_line=9582))

    def test_anchor_outside_named_procedure_rejects(self):
        # A genuine 0x81E2 immediate is not enough when it lies outside the
        # verified procedure that holds the pinned use line.
        with self.assertRaisesRegex(ValueError, 'verified procedure|procedure'):
            self.resolve_with('_mat_z_rot', lambda s: s.update(reference_use_proc='mat_rot_z'))
        def moved(symbol):
            symbol['references'] = [{'start': 159704, 'hex': '55', 'operand_offset': 0}]
        with self.assertRaisesRegex(ValueError, 'outside its verified procedure'):
            self.resolve_with('_mat_z_rot', moved)

    def test_unpinned_use_file_rejects(self):
        for path in ('src/restunts/asmorig/dseg.asm', 'src/restunts/asmorig/seg999.asm',
                     'src/restunts/c/math.c'):
            with self.assertRaisesRegex(ValueError, 'pinned reference segment'):
                self.resolve_with('_mat_z_rot', lambda s, p=path: s.update(reference_use_path=p))

    def test_declaration_placement_must_equal_alias_offset(self):
        with self.assertRaisesRegex(ValueError, 'declaration placement differs'):
            self.resolve_with('_mat_z_rot', lambda s: s.update(load_address=s['load_address']+2))
        # The legacy seg003 form is held to the same placement rule.
        with self.assertRaisesRegex(ValueError, 'declaration placement differs'):
            self.resolve_with('_bravshape', lambda s: s.update(load_address=s['load_address']+2))

    def test_anchor_operand_must_be_decoded_displacement_or_immediate(self):
        def shifted(symbol):
            symbol['references'] = [dict(symbol['references'][0], operand_offset=0)]
        with self.assertRaisesRegex(ValueError, '16-bit displacement or immediate'):
            self.resolve_with('_mat_z_rot', shifted)

    def test_use_path_requires_procedure(self):
        with self.assertRaisesRegex(ValueError, 'pinned segment and procedure'):
            self.resolve_with('_mat_z_rot', lambda s: s.pop('reference_use_proc'))


class PlacedReferenceWidths(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']
        cls.layout = read_json(ROOT/'layout/data-symbols.json')

    def test_span_measured_at_another_label_placement_rejects(self):
        # simd_player's 776-byte label span lies at DGROUP 0xA6EA; granting it
        # at 0x9260 (framespersec) would allow addends into unrelated objects.
        bad = copy.deepcopy(self.layout)
        bad['symbols']['_framespersec'].update(width=776,
            width_provenance='reference-label-span: dseg.asm:40300-41076')
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'not placed at the alias address'):
                resolve_symbols(['_framespersec'], self.image, self.relocations)
            self.assertEqual(resolve_symbols(['_word_9260'], self.image, self.relocations)
                             ['_word_9260']['allowed_addends'], [0])


    def test_placed_span_grants_width_to_differently_spelled_alias(self):
        import data_symbols
        resolved = resolve_symbols(['_word_32CC0', '_carshapevecs'], self.image, self.relocations)
        self.assertEqual(resolved['_word_32CC0']['allowed_addends'], list(range(6)))
        self.assertEqual(resolved['_carshapevecs']['allowed_addends'], list(range(36)))
        # Without a frame (no placement proof) the spelling rule still applies.
        symbol = self.layout['symbols']['_word_32CC0']
        with self.assertRaisesRegex(ValueError, 'label span differs'):
            data_symbols._check_reference_width('_word_32CC0', symbol)
        # A differently spelled alias cannot borrow a span placed elsewhere.
        moved = dict(symbol, load_address=symbol['load_address'] + 2)
        with self.assertRaisesRegex(ValueError, 'not placed at the alias address'):
            data_symbols._check_reference_width('_word_32CC2', moved, self.layout['frame_load_address'])


class Integ22CodeAliases(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']
        cls.layout = read_json(ROOT/'layout/code-symbols.json')


    def test_truncated_external_needs_explicit_review(self):
        bad = copy.deepcopy(self.layout)
        bad['symbols']['_file_load_shape2d_res_nofatal_t'].pop('reviewed_alias')
        with layout_override('code_symbols', 'code-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'inventory or explicit review'):
                resolve_code_symbols(['_file_load_shape2d_res_nofatal_t'], self.image, self.relocations)

    def test_anchor_must_reach_the_mapped_entry(self):
        bad = copy.deepcopy(self.layout)
        anchor = bad['symbols']['_sub_39088']['anchors'][0]
        bad['symbols']['_sub_39088']['frame_load_address'] += 16
        with layout_override('code_symbols', 'code-symbols.json', bad):
            with self.assertRaises(ValueError):
                resolve_code_symbols(['_sub_39088'], self.image, self.relocations)
        self.assertEqual(anchor['caller_task'], 'load_16fa3')


class CLocalCodeTableOffsets(unittest.TestCase):
    """MSC switch-table words: encoded in-segment offset, zero FIXUPP displacement."""
    START, FRAME, LENGTH = 90618, 85344, 8

    def fix(self, **changes):
        row = {'segment': 'UNIT_TEXT', 'offset': 4, 'width': 2, 'loc': 'offset16',
               'target': 'UNIT_TEXT', 'target_kind': 'segment', 'target_method': 0,
               'target_index': 1, 'frame_method': 0, 'frame_kind': 'segment',
               'frame': 'UNIT_TEXT', 'frame_index': 1, 'displacement': 0,
               'encoded_addend': '0200', 'self_relative': False}
        row.update(changes)
        return row

    def bind(self, fixes, kind='c', payload=None, **recipe_changes):
        from types import SimpleNamespace
        from binder import bind_contribution
        payload = payload or bytes([0x90]*4) + bytes.fromhex(fixes[0]['encoded_addend']) + bytes([0x90]*2)
        segments = [{'index': 1, 'name': 'UNIT_TEXT', 'class': 'CODE', 'length': self.LENGTH}]
        publics = [{'name': '_f', 'segment': 'UNIT_TEXT', 'offset': 0}]
        obj = SimpleNamespace(linker_fixups=fixes, segment_defs=segments, groups=[], publics=publics,
                              externals=[], segment_lengths={'UNIT_TEXT': self.LENGTH},
                              segment_length=lambda name: self.LENGTH if name == 'UNIT_TEXT' else 0,
                              segment_bytes=lambda name: payload, local_publics=[], local_externals=[])
        recipe = {'kind': kind, 'object_segment': 'UNIT_TEXT', 'public': '_f', 'start': self.START,
                  'end': self.START + self.LENGTH, 'expected_fixups': fixes, 'expected_relocations': [],
                  'original_frame_load_address': self.FRAME,
                  'binding': {'mode': 'external-far-call-dgroup-offset16-v1' if kind == 'c'
                              else 'asm-external-dgroup-offset16-v1',
                              'declarations': {'segments': segments, 'groups': [], 'publics': publics,
                                               'externals': []}}}
        recipe.update(recipe_changes)
        symbols = {'UNIT_TEXT': {'kind': 'local-text', 'frame_load_address': self.FRAME}}
        return bind_contribution(obj, recipe, symbols)

    def test_encoded_offset_binds_to_own_segment(self):
        import struct
        payload, receipt = self.bind([self.fix()])
        self.assertEqual(struct.unpack_from('<H', payload, 4)[0], self.START - self.FRAME + 2)
        payload, _ = self.bind([self.fix(frame_method=5, frame_kind='target', frame_index=0)])
        self.assertEqual(struct.unpack_from('<H', payload, 4)[0], self.START - self.FRAME + 2)

    def test_offset_outside_module_rejects(self):
        with self.assertRaisesRegex(ValueError, 'local code target differs'):
            self.bind([self.fix(encoded_addend='0800')])

    def test_nonzero_displacement_or_relative_or_group_rejects(self):
        with self.assertRaisesRegex(ValueError, 'declarations/FIXUPPs differ'):
            self.bind([self.fix(displacement=2, encoded_addend='0000')])
        with self.assertRaisesRegex(ValueError, 'declarations/FIXUPPs differ'):
            self.bind([self.fix(self_relative=True)])
        with self.assertRaisesRegex(ValueError, 'declarations/FIXUPPs differ'):
            self.bind([self.fix()], members=[{'name': 'f'}])

    def test_asm_keeps_zero_encoded_rule_and_f0_frame(self):
        with self.assertRaisesRegex(ValueError, 'local code target differs'):
            self.bind([self.fix()], kind='asm')
        with self.assertRaisesRegex(ValueError, 'local code target differs'):
            self.bind([self.fix(frame_method=5, frame_kind='target', frame_index=0,
                                encoded_addend='0000', displacement=2)], kind='asm')


if __name__ == '__main__':
    unittest.main()
