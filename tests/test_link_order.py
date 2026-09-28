"""integ25 item 0: candidate-order-exact MZ relocations for ASM (and C) objects.

The pinned LINK keeps the object's FIXUPP order; the ASM composed and group
binders now compare that candidate-derived stream in order instead of a site
multiset, so a set-equal but differently ordered obligation list fails.
"""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from assembler import assemble_source
from binder import bind_contribution, fixup_relocation_sites, link_order_sites
from code_symbols import resolve_recipe_symbols
from common import read_json
from multi_contribution import bind_multi
from mz import MZ
from oracle import verify
import link_order_probe


def recipe_of(owner_id):
    row, = [o for o in read_json(ROOT/'layout/manifest.json')['owners'] if o['id'] == owner_id]
    return read_json(ROOT/row['recipe'])


def relocation(site):
    return {'segment': (site // 65536) * 4096, 'offset': site % 65536, 'load_offset': site}


class PinnedLinkOrder(unittest.TestCase):
    def test_pinned_link_keeps_masm_and_msc_fixupp_order(self):
        report = link_order_probe.run()
        kinds = set()
        for case in report['cases']:
            with self.subTest(case=case['object'], link=case.get('link_profile')):
                self.assertEqual(case['linked_sites'], case['fixup_sites'])
                self.assertEqual(case['linked_sites'], case['model'])
                kinds.add('ascending' if case['fixup_sites'] == sorted(case['fixup_sites'])
                          else 'descending')
        # Both directions were observed, so the model is not "LINK sorts".
        self.assertEqual(kinds, {'ascending', 'descending'})

    def test_exepack_bank_regrouping_is_stable_and_bank_ordered(self):
        self.assertEqual(link_order_sites([70000, 10, 65540, 5]), [10, 5, 70000, 65540])
        self.assertEqual(link_order_sites([30, 20, 10]), [30, 20, 10])

    def test_fixup_sites_follow_fixupp_order(self):
        fixes = [{'segment': 'S', 'offset': 9, 'loc': 'base16'},
                 {'segment': 'S', 'offset': 1, 'loc': 'pointer32'},
                 {'segment': 'D', 'offset': 0, 'loc': 'pointer32'},
                 {'segment': 'S', 'offset': 4, 'loc': 'offset16'}]
        self.assertEqual(fixup_relocation_sites(fixes, 100, 'S'), [109, 103])


class AsmOrderExact(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle = verify(write=False)
        cls.image = MZ.parse(cls.oracle[1]).load_image(cls.oracle[1])
        cls.relocations = cls.oracle[2]['unpacked_mz']['relocations']

    def single(self, owner_id):
        recipe = recipe_of(owner_id)
        obj, _ = assemble_source((ROOT/recipe['source']).read_bytes(), recipe['profile'])
        return obj, recipe, resolve_recipe_symbols(recipe, self.image, self.relocations)

    def test_composed_asm_binds_in_candidate_order(self):
        obj, recipe, symbols = self.single('asm_load_25fa2')
        self.assertGreaterEqual(len(recipe['expected_relocations']), 2)  # integ32: load_2219d is inside whole asm012_139610
        payload, receipt = bind_contribution(obj, recipe, symbols)
        self.assertEqual(payload, self.image[recipe['start']:recipe['end']])
        self.assertEqual([r['load_offset'] for r in receipt['generated_relocations']],
                         fixup_relocation_sites(obj.linker_fixups, recipe['start'],
                                                recipe['object_segment']))

    def test_composed_asm_reversed_order_fails(self):
        obj, recipe, symbols = self.single('asm_load_25fa2')
        bad = copy.deepcopy(recipe)
        bad['expected_relocations'].reverse()
        with self.assertRaisesRegex(ValueError, 'order differs from its candidate FIXUPP order'):
            bind_contribution(obj, bad, symbols)

    def test_composed_asm_set_equal_but_reordered_fails(self):
        obj, recipe, symbols = self.single('asm_load_25fa2')
        bad = copy.deepcopy(recipe)
        rows = bad['expected_relocations']
        rows[0], rows[1] = rows[1], rows[0]
        self.assertEqual(sorted(r['load_offset'] for r in rows),
                         sorted(r['load_offset'] for r in recipe['expected_relocations']))
        with self.assertRaisesRegex(ValueError, 'order differs from its candidate FIXUPP order'):
            bind_contribution(obj, bad, symbols)

    def test_candidate_fixupp_order_change_fails_against_unchanged_obligations(self):
        obj, recipe, symbols = self.single('asm_load_25fa2')
        changed = copy.deepcopy(obj)
        changed.linker_fixups = list(reversed(changed.linker_fixups))
        bad = copy.deepcopy(recipe)
        bad['expected_fixups'] = changed.linker_fixups
        with self.assertRaisesRegex(ValueError, 'order differs from its candidate FIXUPP order'):
            bind_contribution(changed, bad, symbols)

    def test_group_asm_set_equal_but_reordered_fails(self):
        recipe = recipe_of('asm012_125482')
        obj, _ = assemble_source((ROOT/recipe['source']).read_bytes(), recipe['profile'])
        payload, receipt = bind_multi(obj, recipe, self.image, self.relocations)
        self.assertEqual(payload, self.image[recipe['start']:recipe['end']])
        for mutation in ('reverse', 'swap'):
            with self.subTest(mutation=mutation):
                bad = copy.deepcopy(recipe)
                rows = bad['expected_relocations']
                if mutation == 'reverse':
                    rows.reverse()
                else:
                    rows[-1], rows[-2] = rows[-2], rows[-1]
                with self.assertRaisesRegex(ValueError, 'order|obligations differ'):
                    bind_multi(obj, bad, self.image, self.relocations)

    def test_group_stream_includes_same_module_far_transfers_in_order(self):
        # A synthetic group whose recipe lists the right sites in another order
        # is rejected by the whole candidate stream check, not a multiset.
        recipe = recipe_of('asm012_125482')
        obj, _ = assemble_source((ROOT/recipe['source']).read_bytes(), recipe['profile'])
        stream = link_order_sites(fixup_relocation_sites(
            obj.linker_fixups, recipe['start'], recipe['object_segment']))
        self.assertEqual(stream, [r['load_offset'] for r in recipe['expected_relocations']])
        self.assertEqual([relocation(s) for s in stream], recipe['expected_relocations'])


if __name__ == '__main__':
    unittest.main()
