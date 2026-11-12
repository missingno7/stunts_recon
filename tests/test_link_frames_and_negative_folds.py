"""integ29: link-fidelity corrections, link-faithful ASM frames/segments,
ownership-correction transactions, the strlen negative fold and the real-link
diagnostic helpers."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json


KB_INSIDE = b""".8086
S018_TEXT segment word public 'CODE'
assume cs:S018_TEXT
extrn _nextkey:far
public _kb_shift_checking1
_kb_shift_checking1 proc far
 mov ax,40h
 mov es,ax
 or byte ptr es:[17h],20h
 call far ptr _nextkey
 ret
_kb_shift_checking1 endp
S018_TEXT ends
end
"""


def _outside(text):
    lines = text.split(b'\n')
    moved = [l for l in lines if l.startswith(b'extrn')]
    rest = [l for l in lines if not l.startswith(b'extrn')]
    return b'\n'.join(rest[:1] + moved + rest[1:])


class LinkFaithfulAsmTests(unittest.TestCase):
    """An in-segment `extrn X:far` for a cross-frame target is refused; the
    code segment must be the frame's CODE-class output segment."""

    @classmethod
    def setUpClass(cls):
        from assembler import assemble_source
        cls.recipe = read_json(ROOT / 'recipes/kb_shift_checking1.json')
        cls.inside, _ = assemble_source(KB_INSIDE.replace(b'\n', b'\r\n'), 'masm510-game')
        cls.outside, _ = assemble_source(_outside(KB_INSIDE).replace(b'\n', b'\r\n'), 'masm510-game')
        cls.text_segment, _ = assemble_source(
            _outside(KB_INSIDE).replace(b'S018_TEXT', b'_TEXT').replace(b'\n', b'\r\n'), 'masm510-game')

    def test_source_lint_finds_in_segment_far_extern(self):
        from link_frames import in_segment_far_externs, lint_source
        self.assertEqual([x[2] for x in in_segment_far_externs(KB_INSIDE.decode())], ['_nextkey'])
        self.assertEqual(in_segment_far_externs(_outside(KB_INSIDE).decode()), [])
        self.assertEqual([x[2] for x in lint_source(KB_INSIDE.decode(), self.recipe)], ['_nextkey'])

    def test_cross_frame_f0_far_call_is_refused(self):
        from link_frames import check_asm_object
        self.assertEqual(self.inside.linker_fixups[0]['frame_method'], 0)
        with self.assertRaisesRegex(ValueError, 'declare `extrn _nextkey:far` outside'):
            check_asm_object(self.inside, self.recipe)

    def test_external_frame_far_call_is_link_faithful(self):
        from link_frames import check_asm_object
        self.assertEqual(self.outside.linker_fixups[0]['frame_method'], 2)
        check_asm_object(self.outside, self.recipe)
        self.assertEqual(self.outside.linker_fixups, self.recipe['expected_fixups'])

    def test_runtime_text_segment_is_refused(self):
        from link_frames import check_asm_object
        with self.assertRaisesRegex(ValueError, 'output segment'):
            check_asm_object(self.text_segment, {**self.recipe, 'object_segment': '_TEXT'})

    def test_odd_start_needs_byte_alignment(self):
        from link_frames import require_link_faithful_segment
        with self.assertRaisesRegex(ValueError, 'BYTE-aligned'):
            require_link_faithful_segment(self.outside, {**self.recipe, 'start': self.recipe['start'] + 1})


class StrlenNegativeFoldTests(unittest.TestCase):
    """Extended ruling: audio_filetemp[strlen(audio_filetemp) - 4] binds as
    _audio_filetemp-4 only with the inline strlen of the same array."""

    @classmethod
    def setUpClass(cls):
        from compiler import compile_source
        from object_flags import recipe_flags
        from communal_unit import recipe_declarations, check_object_communals
        from oracle import verify
        from mz import MZ
        cls.recipe = read_json(ROOT / 'recipes/audio_make_filename.json')
        source = (ROOT / cls.recipe['source']).read_bytes()
        declarations = recipe_declarations(cls.recipe)
        communals = None if declarations is None else [name for name, _ in declarations]
        cls.obj, _ = compile_source(source, cls.recipe['profile'], recipe_flags(cls.recipe),
                                    communals=communals)
        check_object_communals(cls.obj, cls.recipe)
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']

    def bind(self, recipe):
        # integ31: seg030 owns its _DATA; the code binds through the single
        # secondary path, which hands the CODE view to the same binders.
        from secondary_contribution import bind_single_secondary
        return bind_single_secondary(self.obj, recipe, self.image, self.relocations)

    def variant(self, **change):
        recipe = copy.deepcopy(self.recipe)
        recipe['negative_folded_index_bindings'][0].update(change)
        return recipe


    def test_unreviewed_addend_is_refused(self):
        recipe = copy.deepcopy(self.recipe)
        del recipe['negative_folded_index_bindings']
        with self.assertRaisesRegex(ValueError, 'leaves independently grounded object'):
            self.bind(recipe)

    def test_k_must_match_the_displacement(self):
        with self.assertRaisesRegex(ValueError, 'form differs'):
            self.bind(self.variant(k=3))

    def test_witness_must_be_the_inline_strlen(self):
        with self.assertRaisesRegex(ValueError, 'witness'):
            self.bind(self.variant(strlen_site=171435))

    def test_word_arrays_keep_the_old_limit(self):
        with self.assertRaisesRegex(ValueError, 'form differs'):
            self.bind(self.variant(element_size=2))


class GapFrameAnchorTests(unittest.TestCase):
    """nopsub_36AF2 lies in the 0x26AF frame (relocated pointer 26AF:0002)."""

    def test_inventory_frame_and_anchor(self):
        from oracle import verify
        from mz import MZ
        from function_evidence import current_inventory, _checked_gap_frame_anchor
        oracle = verify(write=False)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
        rows = {f['name']: f for f in current_inventory(image)['functions']}
        row = rows['nopsub_36AF2']
        self.assertEqual((row['segment'], row['segment_paragraph']), ('seg018', 0x26AF))
        anchor = dict(row['gap_entry_proof']['frame_anchor'])
        _checked_gap_frame_anchor(anchor, row, rows, image)
        with self.assertRaisesRegex(ValueError, 'frame anchor differs'):
            _checked_gap_frame_anchor({**anchor, 'hex': 'b80200ba8a26'}, row, rows, image)
        with self.assertRaisesRegex(ValueError, 'frame anchor differs'):
            _checked_gap_frame_anchor(anchor, {**row, 'segment_paragraph': 0x268A}, rows, image)


class RealLinkHelperTests(unittest.TestCase):
    """Diagnostic real-link helpers: alignment fill and the OMF writer."""

    def test_single_zero_before_word_data_is_link_fill(self):
        import reallink
        c = type('C', (), {})()
        c.image = bytes(20)
        fill = reallink.Unit('rawdata_5', 'raw-data', '_DATA', 5, 6)
        keep = reallink.Unit('rawdata_7', 'raw-data', '_DATA', 7, 8)
        obj = reallink.Unit('c_obj', 'c', 'S001_TEXT', 100, 110)
        obj.data = [('_DATA', 6, 7)]
        units, fills = reallink.drop_alignment_fill(c, [fill, obj, keep])
        self.assertEqual(fills, [5])
        self.assertEqual([u.id for u in units], ['c_obj', 'rawdata_7'])

    def test_raw_object_records_have_valid_checksums(self):
        import reallink
        from omf import OmfReader
        raw = reallink.RawObject('m', 'S012_TEXT', 'CODE', 'byte', b'\x9a\0\0\0\0\xcb')
        raw.fixups.append((3, '$$F'))
        raw.publics.append(('_p', 0))
        obj = OmfReader().read(raw.build())
        self.assertEqual([p['name'] for p in obj.publics], ['_p'])
        self.assertEqual(obj.segment_defs[0]['alignment'], 'byte')


if __name__ == '__main__':
    unittest.main()
