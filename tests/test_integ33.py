"""integ33: reviewed L7 readable names adopted program-wide, the far-data `dw seg`
binder (asm012_141362 owns fontdefseg + word_30602) and owned DGROUP word-alignment
fill (link_fill.WORD_BASIS)."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402


def _oracle():
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    return MZ.parse(result[1]).load_image(result[1]), result[2]['unpacked_mz']['relocations']


class RegistryTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.registry = read_json(ROOT / 'layout/names-registry.json')['names']

    def test_reviewed_l7_rows(self):
        l7 = [r for r in self.registry.values() if r['basis'].startswith('integ33')]
        # integ34 replaced five of them by seg000's compiler-hash-constrained static names,
        # integ35 24 more by the seg008/009/027/028/032 file statics (module-private rows),
        # integ37 29 by the seg005/seg006 static sets and retired two interior elements
        # (200300/200301 are camera_buttons_pressed[2]/[3])
        self.assertEqual(len(l7), 145)
        # rejected: a folded addend label and three interior element addresses
        for address in ('180476', '180390', '180398', '180406'):
            self.assertNotIn(address, self.registry)
        # review corrections: fence list names follow update_frame's nfences use
        self.assertEqual(self.registry['180468']['name'], 'fence_off_3')
        self.assertEqual(self.registry['180472']['name'], 'fence_off_4')
        self.assertEqual(self.registry['36192']['name'], 'track_edge_points')
        self.assertIs(self.registry['196482'].get('object_declared'), False)

    def test_no_accepted_object_declares_a_superseded_data_spelling(self):
        aliases = {}
        for name, symbol in read_json(ROOT / 'layout/data-symbols.json')['symbols'].items():
            aliases.setdefault(symbol['load_address'], set()).add(name)
        declared = set()
        for owner in read_json(ROOT / 'layout/manifest.json')['owners']:
            if owner.get('recipe') and owner['kind'] in ('MATCHING_C', 'MATCHING_ASM', 'MATCHING_C_DATA'):
                recipe = read_json(ROOT / owner['recipe'])
                decl = recipe.get('object_declarations') or recipe.get('binding', {}).get('declarations') or {}
                declared |= {p['name'] for p in decl.get('publics', [])} | set(decl.get('externals', []))
        for address, row in self.registry.items():
            if row['kind'] != 'data':
                continue
            stale = (aliases.get(int(address), set()) - {'_' + row['name']}) & declared
            self.assertFalse(stale, row)


class FarDataSegmentWordTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        from assembler import assemble_source
        cls.image, cls.relocations = _oracle()
        cls.recipe = read_json(ROOT / 'recipes/asm012_141362.json')
        cls.obj, _ = assemble_source((ROOT / cls.recipe['source']).read_bytes(), cls.recipe['profile'])

    def bind(self, recipe=None, obj=None):
        from secondary_contribution import _data_fixups
        obj = obj or self.obj
        recipe = recipe or self.recipe
        return _data_fixups(obj, recipe, '_DATA', obj.segment_bytes('_DATA'),
                            recipe['secondary_dgroup_segments'], self.image, self.relocations)

    def test_dw_seg_binds_to_the_far_data_paragraph_with_its_relocation(self):
        payload, rows, generated = self.bind()
        self.assertEqual(payload, self.image[198144:198148])
        self.assertEqual(payload[:2], (176624 // 16).to_bytes(2, 'little'))
        self.assertEqual(rows, [{'segment': '_DATA', 'offset': 0, 'loc': 'base16',
                                 'target': '_fontdef_default', 'linked_value': 0x2B1F}])
        self.assertEqual(generated, [{'segment': 12288, 'offset': 1536, 'load_offset': 198144}])

    def test_far_target_needs_its_declared_partition(self):
        recipe = copy.deepcopy(self.recipe)
        recipe['secondary_external_targets'] = {'data': ['_fontdef_default']}
        with self.assertRaises(ValueError):
            self.bind(recipe)

    def test_far_word_needs_the_external_frame_and_the_anchored_alias(self):
        obj = copy.copy(self.obj)
        fix = dict(next(f for f in self.obj.linker_fixups if f['segment'] == '_DATA'))
        fix.update(frame_method=5, frame_kind='target', frame_index=0)
        obj.linker_fixups = [fix if f['segment'] == '_DATA' else f for f in self.obj.linker_fixups]
        with self.assertRaisesRegex(ValueError, 'far-data segment word'):
            self.bind(obj=obj)
        obj = copy.copy(self.obj)
        obj.publics = [p if p['name'] != '_fontdefseg' else {**p, 'name': '_fontdefseg2'}
                       for p in self.obj.publics]
        with self.assertRaisesRegex(ValueError, 'far-data segment word'):
            self.bind(obj=obj)

    def test_unknown_far_public_is_refused(self):
        from data_only import far_data_segment_targets
        with self.assertRaisesRegex(ValueError, 'Unknown far-data'):
            far_data_segment_targets({'_no_such_far'}, self.image, self.relocations)


class DgroupWordFillTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, _ = _oracle()
        cls.manifest = read_json(ROOT / 'layout/manifest.json')

    def rows(self, manifest=None):
        from link_fill import WORD_BASIS
        return [o for o in (manifest or self.manifest)['owners'] if o.get('basis') == WORD_BASIS]

    def test_owned_fill_bytes(self):
        from link_fill import checked_fill
        rows = self.rows()
        self.assertEqual([(o['start'], o['object']) for o in rows],
                         [(190343, 'obj_seg004:_DATA'), (191519, 'obj_seg008:_DATA'),
                          (191859, 'obj_seg009:_DATA'),        # integ36: before runtime CRT0 _DATA
                          (192365, 'library_output_120254:_DATA'),  # integ36: runtime OUTPUT -> NMALLOC
                          (192387, 'library_amalloc_123070:_DATA'),  # integ37: AMALLOC -> data-only CMISCDAT
                          (192671, 'library_ctype_192414:_DATA'),    # integ37: data-only CTYPE -> RAND
                          (196097, 'asm012_133660:_DATA'), (199481, 'obj_seg031:_DATA'),
                          (199705, 'seg034_shape2d_group:_DATA'),       # integ38: -> asm012_154486 _DATA
                          (199993, 'library_dos_nmsghdr_119038:EPAD')])  # integ38: -> seg000 _BSS
        for row in rows:
            receipt = checked_fill(row, self.manifest, self.image)
            self.assertEqual(receipt['next_segment']['alignment'], 'word')

    def test_fill_needs_its_odd_predecessor_and_a_word_aligned_follower(self):
        from link_fill import checked_fill
        manifest = copy.deepcopy(self.manifest)
        row = self.rows(manifest)[0]
        row['object'] = 'obj_seg005:_DATA'
        with self.assertRaisesRegex(ValueError, 'does not follow'):
            checked_fill(row, manifest, self.image)
        manifest = copy.deepcopy(self.manifest)
        row = self.rows(manifest)[2]
        image = bytearray(self.image)
        image[row['start']] = 1
        with self.assertRaisesRegex(ValueError, 'one zero byte'):
            checked_fill(row, manifest, bytes(image))
        manifest = copy.deepcopy(self.manifest)
        row = self.rows(manifest)[1]
        at = manifest['owners'].index(row)
        manifest['owners'][at + 1] = {**manifest['owners'][at + 1], 'kind': 'UNRESOLVED_RAW'}
        with self.assertRaisesRegex(ValueError, 'not followed by an accepted'):
            checked_fill(row, manifest, self.image)


if __name__ == '__main__':
    unittest.main()
