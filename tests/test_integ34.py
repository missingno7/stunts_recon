"""integ34: seg000's complete static set (first game BSS ownership) placed by an
image-derived processing-order proof, and owned CODE word-alignment fill
(link_fill.CODE_WORD_BASIS)."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
import bss_link  # noqa: E402

# integ36 adds the two runtime-internal fills 118055 (crt0 -> crt0dat) and
# 124475 (raise -> rand) once their neighbours were accepted.
CODE_FILLS = [118055, 123425, 123685, 123779, 123807, 123835, 123915, 124295, 124475, 124903, 124987,
              125207, 125313, 125447, 139609, 171213]


def _image():
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    return MZ.parse(result[1]).load_image(result[1])


def _module(mid, code, bss=0, owners=(), nonempty=None):
    return {'id': mid, 'code': list(code), 'code_nonempty': list(code if nonempty is None else nonempty),
            'bss': bss, 'bss_align': 'word', 'owners': list(owners)}


class IndependentStaticOrderTests(unittest.TestCase):
    EDATA = 199994
    SEGS = ['S000_TEXT', 'S001_TEXT', 'S002_TEXT', 'S012_TEXT', '_TEXT']

    def order(self, modules, start=EDATA, rid='o:_BSS'):
        row = {'id': rid, 'start': start, 'end': start + 12}
        return bss_link.independent_static_order([row], modules, self.SEGS, self.EDATA)[rid]

    def test_first_code_segment_owner_needs_no_link_order(self):
        result = self.order([_module('seg000', ['S000_TEXT'], 12, ['o:_BSS']),
                             _module('rt', ['_TEXT'], 38), _module('raw', ['S012_TEXT'])])
        self.assertEqual(result['problems'], [])
        self.assertEqual((result['predicted_start'], result['after'], result['before']), (self.EDATA, 1, []))

    def test_codeless_or_shared_segment_bss_module_is_ambiguous(self):
        codeless = self.order([_module('o', ['S001_TEXT'], 12, ['o:_BSS']), _module('d', [], 4)])
        self.assertIn('d', codeless['ambiguous'])
        self.assertTrue(codeless['problems'])
        shared = self.order([_module('o', ['S002_TEXT'], 12, ['o:_BSS']), _module('a', ['S012_TEXT'], 2),
                             _module('b', ['S001_TEXT', 'S012_TEXT'], 2), _module('c', ['S001_TEXT'])])
        self.assertIn('b', shared['ambiguous'])

    def test_earlier_sole_declarers_shift_the_start(self):
        modules = [_module('seg000', ['S000_TEXT'], 12, ['x:_BSS']), _module('o', ['S001_TEXT'], 36, ['o:_BSS'])]
        ok = self.order(modules, self.EDATA + 12)
        self.assertEqual((ok['problems'], ok['before']), ([], ['seg000']))
        wrong = self.order(modules, self.EDATA)
        self.assertTrue(any('image-derived order places' in p for p in wrong['problems']))

    def test_owner_must_introduce_its_code_segment(self):
        result = self.order([_module('o', ['S012_TEXT'], 12, ['o:_BSS']), _module('p', ['S012_TEXT'])])
        self.assertTrue(any('also declared' in p for p in result['problems']))
        unplaced = self.order([_module('o', ['S099_TEXT'], 12, ['o:_BSS'])])
        self.assertTrue(unplaced['problems'])

    def test_check_static_requires_the_order_evidence(self):
        row = {'id': 'o:_BSS', 'start': self.EDATA, 'end': self.EDATA + 4, 'placement': bss_link.STATIC}

        class Obj:
            segment_lengths = {'UNIT_TEXT': 16, '_BSS': 4}
            linker_fixups = [{'target_kind': 'segment', 'target': '_BSS', 'segment': 'UNIT_TEXT',
                              'loc': 'offset16', 'self_relative': False, 'offset': 4,
                              'encoded_addend': '0000', 'displacement': 0}]

            def segment_length(self, name):
                return self.segment_lengths[name]
        img = bytearray(16)
        img[4:6] = (self.EDATA - 178032).to_bytes(2, 'little')
        args = ([row], {row['id']: (Obj(), 'UNIT_TEXT', 0)}, bytes(img), [{'name': '_BSS', 'start': self.EDATA}],
                self.EDATA, 178032)
        self.assertFalse(bss_link.check_static(*args)[0])
        order = {row['id']: {'problems': [], 'basis': bss_link.ORDER_BASIS}}
        self.assertTrue(bss_link.check_static(*args, order)[0])
        order[row['id']]['problems'] = ['x']
        self.assertFalse(bss_link.check_static(*args, order)[0])


class Seg000StaticsTests(unittest.TestCase):

    def test_published_complete_static_set(self):
        manifest = read_json(ROOT / 'layout/manifest.json')
        rows = [(o['id'], o['start'], o['end'], o.get('placement'), o['kind']) for o in manifest['bss_owners']]
        self.assertEqual(rows[0], ('obj_seg000:_BSS', 199994, 200006, bss_link.STATIC, 'MATCHING_C_DATA'))
        self.assertEqual(rows[1][1], 200006)
        recipe = read_json(ROOT / 'recipes/obj_seg000.json')
        self.assertEqual(recipe['secondary_dgroup_segments']['_BSS']['end'], 200006)
        self.assertTrue(any('compiler-hash-constrained' in n for n in recipe['review_notes']))
        source = (ROOT / 'src/obj_seg000.c').read_text()
        names = ['hiscore_rank_prev', 'hiscore_old_entry', 'hiscore_opponent_earlier',
                 'hiscore_current_place', 'end_hiscore_random', 'hiscore_opponent_live']
        for name in names:
            self.assertIn('static short %s;' % name, source)
            self.assertNotIn('_' + name, recipe['object_declarations']['externals'])
        registry = read_json(ROOT / 'layout/names-registry.json')['names']
        for address, name in zip(range(199994, 200006, 2), names):
            row = registry[str(address)]
            self.assertEqual((row['name'], row['object_declared']), (name, False))
            self.assertIn('compiler-hash-constrained', row['basis'])


class CodeWordFillTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image = _image()
        cls.manifest = read_json(ROOT / 'layout/manifest.json')

    def rows(self, manifest=None):
        from link_fill import CODE_WORD_BASIS
        return [o for o in (manifest or self.manifest)['owners'] if o.get('basis') == CODE_WORD_BASIS]

    def test_published_fills_rederive(self):
        from link_fill import checked_fill
        self.assertEqual([r['start'] for r in self.rows()], CODE_FILLS)
        for row in self.rows():
            receipt = checked_fill(row, self.manifest, self.image)
            self.assertEqual(receipt['next_segment']['alignment'], 'word')

    def test_refusals(self):
        from link_fill import checked_fill
        row = self.rows()[-1]      # obj_seg029 -> audio_make_filename
        image = bytearray(self.image)
        image[row['start']] = 0x90
        with self.assertRaises(ValueError):
            checked_fill(row, self.manifest, bytes(image))
        wrong = copy.deepcopy(self.manifest)
        target = next(o for o in wrong['owners'] if o['id'] == row['id'])
        target['object'] = 'load_29cce'
        with self.assertRaises(ValueError):
            checked_fill(target, wrong, self.image)
        import link_fill
        original = link_fill._code_segdef
        try:
            link_fill._code_segdef = lambda r: {'name': 'X', 'class': 'CODE', 'alignment': 'paragraph'}
            with self.assertRaisesRegex(ValueError, 'WORD'):
                checked_fill(row, self.manifest, self.image)
        finally:
            link_fill._code_segdef = original
        incomplete = {**next(o for o in self.manifest['owners'] if o['id'] == 'obj_seg029'), 'end': 171212}
        with self.assertRaisesRegex(ValueError, 'complete'):
            link_fill._code_segdef(incomplete)

    def test_runtime_path_replaces_exactly_one_raw_byte(self):
        import promote_runtime
        manifest = copy.deepcopy(self.manifest)
        at = next(i for i, o in enumerate(manifest['owners']) if o['start'] == 139609)
        manifest['owners'][at] = {'id': 'raw_22159_2215a', 'kind': 'UNRESOLVED_RAW',
                                  'classification': 'UNRESOLVED_MIXED', 'start': 139609, 'end': 139610}
        cand = {'kind': 'LINK_FILL', 'basis': 'link-code-word-alignment-v1', 'start': 139609,
                'object': 'asm012_137138'}
        staged = promote_runtime.replace_raw_library(manifest, cand)
        self.assertEqual(staged['owners'][at], self.manifest['owners'][at])
        with self.assertRaises(ValueError):
            promote_runtime.replace_raw_library(staged, cand)
        with self.assertRaises(ValueError):
            promote_runtime.replace_raw_library(manifest, {**cand, 'end': 139610})


if __name__ == '__main__':
    unittest.main()
