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


if __name__ == '__main__':
    unittest.main()
