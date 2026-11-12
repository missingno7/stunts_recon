"""integ33: reviewed L7 readable names adopted program-wide, the far-data `dw seg`
binder (fontmat owns fontdefseg + word_30602) and owned DGROUP word-alignment
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
        cls.recipe = read_json(ROOT / 'recipes/fontmat.json')
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


    def test_fill_needs_its_odd_predecessor_and_a_word_aligned_follower(self):
        from link_fill import checked_fill
        manifest = copy.deepcopy(self.manifest)
        row = self.rows(manifest)[0]
        row['object'] = 'race:_DATA'
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
