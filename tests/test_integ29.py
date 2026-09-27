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
extrn _kb_checking:far
public _kb_shift_checking1
_kb_shift_checking1 proc far
 mov ax,40h
 mov es,ax
 or byte ptr es:[17h],20h
 call far ptr _kb_checking
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
        self.assertEqual([x[2] for x in in_segment_far_externs(KB_INSIDE.decode())], ['_kb_checking'])
        self.assertEqual(in_segment_far_externs(_outside(KB_INSIDE).decode()), [])
        self.assertEqual([x[2] for x in lint_source(KB_INSIDE.decode(), self.recipe)], ['_kb_checking'])

    def test_cross_frame_f0_far_call_is_refused(self):
        from link_frames import check_asm_object
        self.assertEqual(self.inside.linker_fixups[0]['frame_method'], 0)
        with self.assertRaisesRegex(ValueError, 'declare `extrn _kb_checking:far` outside'):
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

    def test_every_accepted_asm_module_is_lint_clean(self):
        from link_frames import lint_source
        manifest = read_json(ROOT / 'layout/manifest.json')
        found = []
        for owner in manifest['owners']:
            if owner['kind'] == 'MATCHING_ASM':
                recipe = read_json(ROOT / owner['recipe'])
                text = (ROOT / recipe['source']).read_bytes().decode('latin1')
                found += lint_source(text, recipe)
                self.assertIn(recipe['object_segment'], ('S002_TEXT', 'S012_TEXT', 'S018_TEXT'))
        self.assertEqual(found, [])


class OwnershipCorrectionTests(unittest.TestCase):
    """Correction transactions demote exactly the named owner, with a
    recorded reason and one reviewed basis, before the strict republication."""

    def setUp(self):
        self.manifest = {'owners': [
            {'id': 'raw_00000_0000a', 'kind': 'UNRESOLVED_RAW', 'classification': 'UNRESOLVED_MIXED',
             'start': 0, 'end': 10},
            {'id': 'asm_a', 'name': 'a', 'kind': 'MATCHING_ASM', 'start': 10, 'end': 21,
             'recipe': 'recipes/a.json'},
            {'id': 'load_x', 'name': 'x', 'kind': 'MATCHING_C', 'start': 21, 'end': 30,
             'recipe': 'recipes/x.json',
             'data_intervals': [{'segment': '_DATA', 'start': 41, 'end': 50, 'target': {'size': 9}}]},
            {'id': 'raw_0001e_00029', 'kind': 'UNRESOLVED_RAW', 'classification': 'UNRESOLVED_MIXED',
             'start': 30, 'end': 41},
            {'id': 'load_x:_DATA', 'kind': 'MATCHING_C_DATA', 'parent': 'load_x', 'segment': '_DATA',
             'start': 41, 'end': 50, 'target': {'size': 9}}]}
        self.image = bytes([0xcb] * 64)
        self.previous = {'kind': 'MATCHING_C', 'start': 21, 'end': 30,
                         'data_intervals': self.manifest['owners'][2]['data_intervals']}

    def recipe(self, **spec):
        base = {'schema': 'ownership-correction-v1', 'owner': 'load_x', 'previous': self.previous,
                'reason': 'reason long enough to be a recorded review reason', 'basis': 'link-data-alignment'}
        base.update(spec)
        return {'id': 'x', 'kind': 'c', 'start': 21, 'end': 30, 'ownership_correction': base}

    def test_correction_demotes_owner_and_its_data(self):
        from promote import correct_ownership
        result, record = correct_ownership(self.manifest, 'x', self.recipe(), self.image)
        kinds = [(o['kind'], o['start'], o['end']) for o in result['owners']]
        self.assertEqual(kinds, [('UNRESOLVED_RAW', 0, 10), ('MATCHING_ASM', 10, 21),
                                 ('UNRESOLVED_RAW', 21, 50)])
        self.assertEqual(record['previous'], self.previous)
        self.assertEqual(record['basis'], 'link-data-alignment')

    def test_stale_previous_extent_is_refused(self):
        from promote import correct_ownership
        with self.assertRaisesRegex(ValueError, 'current owner extent'):
            correct_ownership(self.manifest, 'x', self.recipe(previous={**self.previous, 'end': 31}), self.image)

    def test_unknown_basis_or_short_reason_is_refused(self):
        from promote import correct_ownership
        for spec in ({'basis': 'convenience'}, {'reason': 'short'}):
            with self.assertRaisesRegex(ValueError, 'shape differs'):
                correct_ownership(self.manifest, 'x', self.recipe(**spec), self.image)

    def test_kind_change_needs_odd_start_continuation(self):
        from promote import correct_ownership
        recipe = {**self.recipe(), 'kind': 'asm'}
        with self.assertRaisesRegex(ValueError, 'cannot change the contribution kind'):
            correct_ownership(self.manifest, 'x', recipe, self.image)
        manifest = copy.deepcopy(self.manifest)
        manifest['owners'][2]['data_intervals'] = []
        del manifest['owners'][4]
        manifest['owners'][3]['end'] = 50
        previous = {**self.previous, 'data_intervals': []}
        odd = {**self.recipe(basis='odd-start-asm-continuation', previous=previous), 'kind': 'asm'}
        result, record = correct_ownership(manifest, 'x', odd, self.image)
        self.assertEqual(record['reclassified_from']['kind'], 'MATCHING_C')
        padded = bytearray(self.image); padded[20] = 0x90
        with self.assertRaisesRegex(ValueError, 'continuation proof'):
            correct_ownership(manifest, 'x', odd, bytes(padded))

    def test_published_corrections_are_recorded(self):
        manifest = read_json(ROOT / 'layout/manifest.json')
        rows = {o['id']: o for o in manifest['owners']}
        expected = {'obj_seg003': 'link-data-alignment', 'obj_seg031': 'link-data-alignment',
                    'seg017_mouse_whole': 'link-frame-reassignment',
                    'load_2117b': 'odd-start-asm-continuation', 'asm_load_24b7c': 'owner-identity-repair'}
        for owner, basis in expected.items():
            self.assertEqual([r['basis'] for r in rows[owner]['ownership_corrections']], [basis])
        self.assertEqual((rows['obj_seg003:_DATA']['start'], rows['obj_seg031:_DATA']['start']),
                         (180578, 199442))
        self.assertEqual(rows['load_2117b']['kind'], 'MATCHING_ASM')
        self.assertEqual((rows['seg017_mouse_whole']['end'], rows['load_26af2']['kind']),
                         (158450, 'MATCHING_C'))
        self.assertTrue(all(o['start'] % 2 == 0 for o in manifest['owners']
                            if o['kind'] == 'MATCHING_C_DATA' and o.get('segment') == '_DATA'))


class StrlenNegativeFoldTests(unittest.TestCase):
    """Extended ruling: audio_filetemp[strlen(audio_filetemp) - 4] binds as
    _audio_filetemp-4 only with the inline strlen of the same array."""

    @classmethod
    def setUpClass(cls):
        from compiler import compile_source
        from object_flags import recipe_flags
        from oracle import verify
        from mz import MZ
        cls.recipe = read_json(ROOT / 'recipes/audio_make_filename.json')
        source = (ROOT / cls.recipe['source']).read_bytes()
        cls.obj, _ = compile_source(source, cls.recipe['profile'], recipe_flags(cls.recipe))
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']

    def bind(self, recipe):
        from code_symbols import resolve_recipe_symbols
        from binder import bind_contribution
        return bind_contribution(self.obj, recipe, resolve_recipe_symbols(recipe, self.image, self.relocations))

    def variant(self, **change):
        recipe = copy.deepcopy(self.recipe)
        recipe['negative_folded_index_bindings'][0].update(change)
        return recipe

    def test_reviewed_strlen_fold_binds(self):
        payload, _ = self.bind(self.recipe)
        self.assertEqual(payload, self.image[self.recipe['start']:self.recipe['end']])

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
