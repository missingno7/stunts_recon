"""integ38: whole seg012 modules owning their DGROUP data (asm012_131254,
asm012_154486), the in-image prefix of the first _BSS contribution
(BSS_IN_IMAGE) with the word fill before it, DGROUP-framed external addends
and the FONTHDR font-header STRUC."""
import copy
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402


class Oracle:
    image = None
    relocations = None

    @classmethod
    def load(cls):
        if cls.image is None:
            from oracle import verify
            from mz import MZ
            result = verify(write=False)
            cls.image = MZ.parse(result[1]).load_image(result[1])
            cls.relocations = result[2]['unpacked_mz']['relocations']
        return cls


def owners():
    return read_json(ROOT / 'layout/manifest.json')['owners']


class ModuleDataTests(unittest.TestCase):

    def test_timer_module_owns_its_data(self):
        rows = {o['id']: o for o in owners()}
        module = rows['asm012_131254']
        self.assertEqual((module['start'], module['end'], module['kind']), (131254, 132056, 'MATCHING_ASM'))
        data = rows['asm012_131254:_DATA']
        self.assertEqual((data['start'], data['end'], data['kind']), (194664, 194778, 'MATCHING_ASM_DATA'))
        recipe = read_json(ROOT / 'recipes/asm012_131254.json')
        self.assertEqual(recipe['module_proof']['kind'], 'asm-module-extent-v1')
        # no object references the reference-label aliases of the dword high words
        for owner in owners():
            if owner.get('recipe'):
                text = (ROOT / read_json(ROOT / owner['recipe'])['source']).read_text(errors='replace')
                self.assertNotRegex(text, r'_word_2F87[6A]|_word_2F892', owner['id'])

    def test_sub_35E08_module_owns_word_54AA(self):
        rows = {o['id']: o for o in owners()}
        module = rows['asm012_154486']
        self.assertEqual((module['start'], module['end']), (154486, 155464))
        data = rows['asm012_154486:_DATA']
        self.assertEqual((data['start'], data['end']), (199706, 199708))
        recipe = read_json(ROOT / 'recipes/asm012_154486.json')
        # the four ss: operands of sub_35E08 are DGROUP-framed FIXUPPs to the own _DATA
        own = [f for f in recipe['expected_fixups'] if f['target'] == '_DATA']
        self.assertEqual([(f['offset'] + 154486, f['displacement'], f['frame']) for f in own],
                         [(155383, 0, 'DGROUP'), (155427, 0, 'DGROUP'), (155432, 1, 'DGROUP'),
                          (155448, 1, 'DGROUP')])
        o = Oracle.load()
        for f in own:
            at = f['offset'] + 154486
            self.assertEqual(int.from_bytes(o.image[at:at + 2], 'little'), 199706 - 178032 + f['displacement'])
        text = (ROOT / recipe['source']).read_text()
        self.assertNotIn('54AAh', text)
        self.assertNotIn('54ABh', text)

    def test_small_dgroup_tail_rows(self):
        rows = {(o['start'], o['end']): o for o in owners()}
        self.assertEqual(rows[(199705, 199706)]['kind'], 'LINK_FILL')
        self.assertEqual(rows[(199705, 199706)]['object'], 'seg034_shape2d_group:_DATA')
        self.assertEqual(rows[(199993, 199994)]['kind'], 'LINK_FILL')
        self.assertEqual(rows[(199993, 199994)]['object'], 'library_dos_nmsghdr_119038:EPAD')
        last = owners()[-1]
        self.assertEqual(last, {'id': 'obj_seg000:_BSS@image', 'kind': 'BSS_IN_IMAGE', 'start': 199994,
                                'end': 200000, 'bss_owner': 'obj_seg000:_BSS'})
        # [199484,199488) stays raw: no operand anywhere addresses DGROUP 53CCh..53CFh
        self.assertEqual(rows[(199484, 199488)]['kind'], 'UNRESOLVED_RAW')


class BssInImageTests(unittest.TestCase):

    def setUp(self):
        from build_exact import validate_layout
        self.validate = validate_layout
        self.manifest = read_json(ROOT / 'layout/manifest.json')
        self.size = len(Oracle.load().image)

    def test_canonical_layout(self):
        self.validate(self.manifest, self.size)

    def test_row_must_name_first_accepted_bss_owner(self):
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['bss_owner'] = 'obj_seg001_complete:_BSS'
        bad['owners'][-1]['id'] = 'obj_seg001_complete:_BSS@image'
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_raw_bss_placeholder_cannot_own_image_bytes(self):
        bad = copy.deepcopy(self.manifest)
        row = bad['bss_owners'][0]
        bad['bss_owners'][0] = {'id': row['id'], 'kind': 'UNRESOLVED_RAW', 'start': row['start'],
                                'end': row['end'], 'segment': '_BSS'}
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_row_must_end_the_image(self):
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['extra'] = True
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_word_fill_before_bss(self):
        from link_fill import checked_word_fill
        image = Oracle.load().image
        fill = next(o for o in self.manifest['owners'] if o['start'] == 199993)
        receipt = checked_word_fill(fill, self.manifest, image)
        self.assertEqual(receipt['next_segment']['owner'], 'obj_seg000:_BSS')
        self.assertEqual(receipt['next_segment']['alignment'], 'word')
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['bss_owner'] = 'obj_seg005:_BSS'
        fill = next(o for o in bad['owners'] if o['start'] == 199993)
        with self.assertRaisesRegex(ValueError, 'first _BSS contribution'):
            checked_word_fill(fill, bad, image)

    def test_promote_runtime_candidate_splits_the_raw_tail(self):
        import promote_runtime
        staged = copy.deepcopy(self.manifest)
        # the state before integ38: one raw row [199993,200000)
        staged['owners'][-2:] = [{'id': 'raw_30d39_30d40', 'kind': 'UNRESOLVED_RAW',
                                  'start': 199993, 'end': 200000}]
        staged = promote_runtime.replace_raw_library(staged, {'kind': 'BSS_IN_IMAGE',
                                                              'bss_owner': 'obj_seg000:_BSS'})
        self.assertEqual([(o['kind'], o['start'], o['end']) for o in staged['owners'][-2:]],
                         [('UNRESOLVED_RAW', 199993, 199994), ('BSS_IN_IMAGE', 199994, 200000)])
        self.validate(staged, self.size)
        with self.assertRaisesRegex(ValueError, 'Unsupported in-image BSS candidate form'):
            promote_runtime.replace_raw_library(staged, {'kind': 'BSS_IN_IMAGE', 'bss_owner': 1})


class DgroupAddendTests(unittest.TestCase):

    def test_external_high_word_addend_has_object_extent(self):
        from code_symbols import resolve_recipe_symbols
        o = Oracle.load()
        recipe = read_json(ROOT / 'recipes/timer_get_counter.json')
        fix = [f for f in recipe['expected_fixups'] if f['displacement']]
        self.assertEqual([(f['target'], f['displacement']) for f in fix], [('_timer_callback_counter', 2)])
        symbols = resolve_recipe_symbols(recipe, o.image, o.relocations)
        self.assertEqual(symbols['_timer_callback_counter']['width'], 4)


class FontHeaderTests(unittest.TestCase):

    def test_fonthdr_struc_names_the_header_fields(self):
        for name in ('font_draw_text', 'asm012_144064_late'):
            text = (ROOT / ('asm/%s.ASM' % name)).read_text()
            self.assertIn('FONTHDR struc', text)
            self.assertRegex(text, r'fh_glyphs dw 256 dup \(\?\)')
            # no numeric font-header offsets remain after `mov ds, _fontdefseg`
            body = text[text.index('FONTHDR ends'):]
            self.assertNotRegex(body, r'\bds:\[?(0|2|4|8|0Ah|0Ch|0Eh|10h|12h|14h)\]?(?![\w\]])')
            self.assertIn('add', body)
            self.assertRegex(body, r'add\s+bx,\s*fh_glyphs')


if __name__ == '__main__':
    unittest.main()
