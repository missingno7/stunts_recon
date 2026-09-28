"""integ31: whole seg028 (object tail after a member's own pad, reviewed
inventory rows), TU/module data attached to its owner (secondary _DATA
growth, own-data MASM displacement form, a module's own far callback in its
_DATA), and the real-link fill/order helpers."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json


def _oracle():
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    return MZ.parse(result[1]).load_image(result[1]), result[2]['unpacked_mz']['relocations']


class Seg028Tests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = _oracle()
        cls.recipe = read_json(ROOT / 'recipes/obj_seg028.json')

    def test_object_tail_after_the_members_own_pad(self):
        from function_evidence import current_inventory
        from multi_contribution import _checked_object_tail
        inventory = current_inventory(self.image)
        members = self.recipe['members']
        self.assertEqual(_checked_object_tail(self.recipe, self.image, inventory, members), 170708)
        # The 90 before the tail must be the last member's reviewed pad.
        bad = copy.deepcopy(inventory)
        for f in bad['functions']:
            if f.get('name') == 'audio_driver_func1E':
                f['padding_offsets'] = []
        with self.assertRaisesRegex(ValueError, 'Object tail lacks'):
            _checked_object_tail(self.recipe, self.image, bad, members)

    def test_reviewed_seg028_rows(self):
        from function_evidence import current_inventory
        rows = {f['name']: f for f in current_inventory(self.image)['functions'] if f.get('name')}
        self.assertEqual((rows['sub_38702']['start'], rows['sub_38702']['end']), (165634, 166568))
        self.assertTrue(rows['set_chunk_channel']['local_symbol'])
        self.assertEqual(rows['loc_390C8']['gap_entry_proof']['kind'], 'verified-neighbours-v1')
        for name in ('sub_38702', 'set_chunk_channel', 'loc_390C8'):
            self.assertEqual(rows[name]['status'], 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')

    def test_flag_segments_use_the_reviewed_overlay_extent(self):
        from object_flags import recipe_segments
        self.assertEqual(recipe_segments(self.recipe), {'seg028'})


class OwnedDataTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = _oracle()

    def test_own_data_displacement_form_is_asm_only(self):
        from secondary_contribution import _own_addend
        fix = {'displacement': 12}
        self.assertEqual(_own_addend(fix, bytes(2), {'kind': 'asm'}), 12)
        with self.assertRaisesRegex(ValueError, 'displacement form differs'):
            _own_addend(fix, bytes(2), {'kind': 'c'})
        with self.assertRaisesRegex(ValueError, 'displacement form differs'):
            _own_addend(fix, b'\x01\x00', {'kind': 'asm'})

    def _asm(self, rid):
        from assembler import assemble_source
        recipe = read_json(ROOT / 'recipes' / (rid + '.json'))
        obj, _ = assemble_source((ROOT / recipe['source']).read_bytes(), recipe['profile'])
        return recipe, obj

    def test_module_far_callback_in_its_own_data(self):
        from multi_contribution import bind_multi
        recipe, obj = self._asm('keyboard_input_callbacks')
        payload, receipt = bind_multi(obj, recipe, self.image, self.relocations)
        self.assertEqual(payload, self.image[recipe['start']:recipe['end']])
        # The callback's segment word is an ordered data relocation obligation.
        self.assertEqual(recipe['secondary_dgroup_segments']['_DATA']['expected_relocations'],
                         [r for r in self.relocations if r['load_offset'] == 196094])
        bad = copy.deepcopy(recipe)
        del bad['module_proof']
        with self.assertRaisesRegex(ValueError, 'own-code data fixup'):
            bind_multi(obj, bad, self.image, self.relocations)

    def test_asm_module_data_publics_are_placed(self):
        from multi_contribution import bind_multi
        recipe, obj = self._asm('keyboard_interrupt_runtime')
        payload, _ = bind_multi(obj, recipe, self.image, self.relocations)
        self.assertEqual(payload, self.image[recipe['start']:recipe['end']])
        self.assertIn({'name': '_kbinput', 'segment': '_DATA', 'offset': 0x2FBDA - 0x2FB48}, obj.publics)

    def test_secondary_growth_keeps_segments_and_exact_bytes(self):
        from promote import apply_ownership
        manifest = read_json(ROOT / 'layout/manifest.json')
        recipe = read_json(ROOT / 'recipes/obj_seg009.json')
        self.assertIs(apply_ownership(manifest, 'obj_seg009', recipe, None, self.image), manifest)
        short = copy.deepcopy(recipe)
        spec = short['secondary_dgroup_segments']['_DATA']
        spec['end'] -= 2
        with self.assertRaises(ValueError):
            apply_ownership(manifest, 'obj_seg009', short, None, self.image)

    def test_loadds_reload_stays_with_the_composed_binder(self):
        from multi_contribution import bind_multi
        from compiler import compile_source
        from object_flags import recipe_flags
        from object_probe import recipe_sparse_zero
        from communal_unit import recipe_declarations, check_object_communals
        recipe = read_json(ROOT / 'recipes/obj_seg028.json')
        declarations = recipe_declarations(recipe)
        communals = None if declarations is None else [name for name, _ in declarations]
        obj, _ = compile_source((ROOT / recipe['source']).read_bytes(), recipe['profile'],
                                recipe_flags(recipe), sparse_zero=recipe_sparse_zero(recipe),
                                communals=communals)
        check_object_communals(obj, recipe)
        self.assertTrue(any(f['loc'] == 'base16' and f['target'] == '_DATA' for f in obj.linker_fixups))
        payload, _ = bind_multi(obj, recipe, self.image, self.relocations)
        self.assertEqual(payload, self.image[recipe['start']:recipe['end']])


class RealLinkHelperTests(unittest.TestCase):

    def test_fill_after_an_odd_length_module_data(self):
        import reallink as R
        c = R.load_inputs()
        owner = R.Unit('asm_x', 'asm', 'S012_TEXT', 133660, 133840)
        owner.data = [('_DATA', 196092, 196097)]
        raw = R.Unit('rawdata_196097', 'raw-data', '_DATA', 196097, 196200)
        units, fills = R.drop_alignment_fill(c, [owner, raw])
        self.assertEqual(fills, [196097])
        self.assertEqual((units[1].start, units[1].after_fill), (196098, True))


if __name__ == '__main__':
    unittest.main()
