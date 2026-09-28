"""integ32: BSS ownership placed by the real link (static _BSS in module order,
near communals by the LINK 3.65 symbol-table walk), the corrected COMDEF
parser, the /ST:8000 + pinned CRT0 STACK link recipe, and the published
seg003/seg012 data ownership that keeps the real link free of alias shims."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json, identity  # noqa: E402
import bss_link  # noqa: E402

FILE_C_COMDEF = '075f5f627566696e0062810002085f5f6275666f75740062810002085f5f6275666572720062810002'


class ComdefParserTests(unittest.TestCase):

    def test_pinned_file_c_commons_are_near_512(self):
        rows = bss_link.parse_comdef(bytes.fromhex(FILE_C_COMDEF))
        self.assertEqual([(r['name'], r['kind'], r['size']) for r in rows],
                         [('__bufin', 'near', 512), ('__bufout', 'near', 512), ('__buferr', 'near', 512)])

    def test_numeric_leaves_and_far_form(self):
        body = (bytes([2]) + b'_a' + bytes([0, 0x61]) + bytes([0x84, 0x00, 0x00, 0x01]) + bytes([2]) +
                bytes([2]) + b'_b' + bytes([0, 0x62]) + bytes([0x88, 1, 0, 1, 0]) +
                bytes([2]) + b'_c' + bytes([0, 0x62, 0x7F]))
        rows = bss_link.parse_comdef(body)
        self.assertEqual([(r['name'], r['kind'], r['size']) for r in rows],
                         [('_a', 'far', 0x10000 * 2), ('_b', 'near', 0x10001), ('_c', 'near', 127)])
        with self.assertRaises(ValueError):
            bss_link.parse_comdef(bytes([2]) + b'_d' + bytes([0, 0x62, 0x82, 0, 0]))
        with self.assertRaises(ValueError):
            bss_link.parse_comdef(bytes([2]) + b'_e' + bytes([0, 0x63, 1]))


class CommunalModelTests(unittest.TestCase):
    """The diagnostic LINK 3.65 walk against L6-comdef / L7-lhashB fixture MAPs."""

    def order(self, names, reverse=False):
        seq = list(reversed(names)) if reverse else names
        return [r['name'] for r in bss_link.communal_order(
            [{'name': n, 'size': 1, 'encounter': i} for i, n in enumerate(seq)])]

    def test_map_fwd_order_is_invariant_to_declaration_order(self):
        names = ['_aa_one', '_bb_two', '_cc_three', '_dd_four', '_ee_five']
        expected = ['_bb_two', '_cc_three', '_ee_five', '_aa_one', '_dd_four']
        self.assertEqual(self.order(names), expected)
        self.assertEqual(self.order(names, reverse=True), expected)

    def test_map_ab_duplicate_merge_and_even_alignment(self):
        rows = bss_link.communal_order([
            {'name': '_duplicate_value', 'size': 4, 'encounter': 0},
            {'name': '_alpha_word', 'size': 2, 'encounter': 1},
            {'name': '_beta_one', 'size': 1, 'encounter': 2},
            {'name': '_duplicate_value', 'size': 7, 'encounter': 3},
            {'name': '_omega_word', 'size': 2, 'encounter': 4},
            {'name': '_zeta_one', 'size': 1, 'encounter': 5}])
        self.assertEqual([(r['name'], r['address']) for r in rows],
                         [('_duplicate_value', 0), ('_omega_word', 8), ('_beta_one', 10),
                          ('_alpha_word', 12), ('_zeta_one', 14)])

    def test_bucket_collision_head_insertion(self):
        self.assertEqual(bss_link.name_hash('_x52') & 255, bss_link.name_hash('_x1000') & 255)
        common = [{'name': '_x52', 'size': 1, 'encounter': 2}, {'name': '_x1000', 'size': 1, 'encounter': 3}]
        ext_before = bss_link.communal_order(common, [{'name': '_x1000', 'encounter': 1}])
        self.assertEqual([r['name'] for r in ext_before], ['_x52', '_x1000'])
        # First sight decides: a node seen later is head-inserted and walked first.
        common_before = bss_link.communal_order([{'name': '_x52', 'size': 1, 'encounter': 1},
                                                 {'name': '_x1000', 'size': 1, 'encounter': 2}])
        self.assertEqual([r['name'] for r in common_before], ['_x1000', '_x52'])


class _Obj:
    def __init__(self, bss, fixups, code='UNIT_TEXT'):
        self.segment_lengths = {code: 16, '_BSS': bss}
        self.segment_defs = [{'name': code, 'class': 'CODE'}]
        self.linker_fixups = fixups

    def segment_length(self, name):
        return self.segment_lengths[name]


def _fix(offset, addend):
    return {'target_kind': 'segment', 'target': '_BSS', 'segment': 'UNIT_TEXT', 'loc': 'offset16',
            'self_relative': False, 'offset': offset, 'encoded_addend': addend.to_bytes(2, 'little').hex(),
            'displacement': 0}


class StaticPlacementTests(unittest.TestCase):
    BASE, EDATA = 178032, 199994

    def image(self, words):
        img = bytearray(64)
        for at, value in words:
            img[at:at + 2] = value.to_bytes(2, 'little')
        return bytes(img)

    def check(self, row, obj, img, start=EDATA):
        # integ34: the placement must also follow from the image-derived order
        # (the owner introduces the first code segment; no other _BSS module).
        order = bss_link.independent_static_order(
            [row], [{'id': 'o', 'code': ['UNIT_TEXT'], 'bss': obj.segment_lengths['_BSS'],
                     'bss_align': 'word', 'owners': [row['id']]}], ['UNIT_TEXT'], self.EDATA)
        return bss_link.check_static([row], {row['id']: (obj, 'UNIT_TEXT', 0)}, img,
                                     [{'name': '_BSS', 'start': start}], self.EDATA, self.BASE, order)

    def row(self, start=EDATA, end=EDATA + 4, placement=bss_link.STATIC):
        return {'id': 'o:_BSS', 'start': start, 'end': end, 'placement': placement}

    def test_linked_operands_place_the_contribution(self):
        img = self.image([(4, 0x55CA), (10, 0x55CC)])
        ok, rows = self.check(self.row(), _Obj(4, [_fix(4, 0), _fix(10, 2)]), img)
        self.assertTrue(ok, rows)
        self.assertEqual(rows[0]['linked_starts'], [self.EDATA])

    def test_refusals(self):
        obj = _Obj(4, [_fix(4, 0), _fix(10, 2)])
        moved = self.image([(4, 0x55CC), (10, 0x55CE)])
        self.assertFalse(self.check(self.row(), obj, moved)[0])
        self.assertFalse(self.check(self.row(end=self.EDATA + 6), obj, self.image([(4, 0x55CA), (10, 0x55CC)]))[0])
        self.assertFalse(self.check(self.row(), _Obj(4, []), self.image([]))[0])
        self.assertFalse(self.check(self.row(placement=None), obj, self.image([(4, 0x55CA), (10, 0x55CC)]))[0])
        self.assertFalse(self.check(self.row(), obj, self.image([(4, 0x55CA), (10, 0x55CC)]),
                                    start=self.EDATA + 2)[0])

    def test_raw_bss_debt_is_per_object_placeholders(self):
        # integ42 accepts seg007's complete BSS object and its LINK-recreated pad.
        rows = bss_link.load_partition()
        raw = [r for r in rows if r['kind'] == 'UNRESOLVED_RAW']
        self.assertEqual(raw, [])
        fill = next(r for r in rows if r['form'] == bss_link.BSS_WORD_FILL)
        self.assertEqual((fill['object'], fill['start'], fill['end']), ('obj_seg007', 205383, 205384))
        self.assertEqual(rows[-1]['form'], bss_link.COMMUNAL_UNIT)
        self.assertEqual(rows[-1]['kind'], bss_link.COMMUNAL_KIND)
        self.assertTrue(all(r['object'] for r in raw if r['form'] == bss_link.OBJECT_BSS))

    def test_gate(self):
        good = {'image_equal': True, 'relocation_set_equal': True, 'bank_order_equal': True,
                'packed_equal': True, 'header': {'linked': {'sp': 1, 'maxalloc': 1}, 'oracle': {'sp': 1, 'maxalloc': 2}},
                'link_errors': [], 'alias_shims': 0,
                'bss': {'owners': [{'id': 'o:_BSS', 'problems': []}], 'problems': []}}
        self.assertEqual(bss_link.gate(good, ['o:_BSS'])['status'], 'PLACED')
        for key, value in (('alias_shims', 1), ('image_equal', False), ('packed_equal', None)):
            bad = dict(good, **{key: value})
            self.assertEqual(bss_link.gate(bad, ['o:_BSS'])['status'], 'REFUSED', key)
        self.assertEqual(bss_link.gate(good, ['p:_BSS'])['status'], 'REFUSED')
        header = dict(good, header={'linked': {'sp': 2}, 'oracle': {'sp': 1}})
        self.assertEqual(bss_link.gate(header)['status'], 'REFUSED')
        misplaced = copy.deepcopy(good)
        misplaced['bss']['owners'][0]['problems'] = ['real link placed _BSS at [1]']
        self.assertEqual(bss_link.gate(misplaced)['status'], 'REFUSED')

    def test_communal_rows_use_map_addresses(self):
        row = {'id': 'c', 'communals': [{'name': '_x', 'address': 210000}]}
        self.assertTrue(bss_link.check_communals([row], {'_x': 210000})[0])
        self.assertFalse(bss_link.check_communals([row], {'_x': 210002})[0])
        self.assertFalse(bss_link.check_communals([{'id': 'd'}], {})[0])

    def test_claims(self):
        self.assertTrue(bss_link.claims({'secondary_dgroup_segments': {'_BSS': {}}}))
        # integ39: a recipe claims communal storage through its COMDEF declarations.
        self.assertTrue(bss_link.claims({'communal_declarations': [{'name': '_x', 'size': 2}]}))
        self.assertFalse(bss_link.claims({'communals': [{'name': '_x'}]}))
        self.assertFalse(bss_link.claims({'secondary_dgroup_segments': {'_DATA': {}}}))


class OwnershipRowTests(unittest.TestCase):

    def test_static_bss_owner_row_and_layout_requirement(self):
        from oracle import verify
        from mz import MZ
        import promote
        from build_exact import validate_layout
        result = verify(write=False)
        image = MZ.parse(result[1]).load_image(result[1])
        manifest = read_json(ROOT / 'layout/manifest.json')
        # integ34 published seg000's complete 12-byte _BSS; this checks the row
        # mechanics on the state before that publication.
        # integ35: the state before is seg000's raw placeholder of its complete extent.
        manifest['bss_owners'][0] = {'id': 'raw_bss_obj_seg000', 'kind': 'UNRESOLVED_RAW', 'start': 199994,
                                     'end': 200006, 'raw_form': bss_link.OBJECT_BSS, 'object': 'obj_seg000'}
        for owner in manifest['owners']:
            if owner['id'] == 'obj_seg000':
                owner['data_intervals'] = [i for i in owner['data_intervals'] if i['segment'] != '_BSS']
        recipe = copy.deepcopy(read_json(ROOT / 'recipes/obj_seg000.json'))
        partial = copy.deepcopy(recipe)
        partial['secondary_dgroup_segments']['_BSS'] = {
            'dgroup_offset': 199994 - 178032, 'start': 199994, 'end': 199998, 'target': identity(bytes(4))}
        partial['subsumed_data_owners'] = ['obj_seg000:_DATA']
        with self.assertRaisesRegex(ValueError, 'own object raw placeholder'):
            promote.attach_secondary(copy.deepcopy(manifest), partial, image)
        recipe['subsumed_data_owners'] = ['obj_seg000:_DATA']
        staged = promote.attach_secondary(manifest, recipe, image)
        rows = staged['bss_owners']
        self.assertEqual((rows[0]['kind'], rows[0]['start'], rows[0]['end'], rows[0].get('placement')),
                         ('MATCHING_C_DATA', 199994, 200006, bss_link.STATIC))
        self.assertEqual(rows[1]['start'], 200006)
        validate_layout(staged, len(image))
        del rows[0]['placement']
        with self.assertRaises(ValueError):
            validate_layout(staged, len(image))


class LinkRecipeTests(unittest.TestCase):

    def test_stack_is_crt0_stack_with_st_option(self):
        import reallink
        stack = reallink.crt0_stack()
        self.assertEqual((stack['name'], stack['class'], stack['length'], stack['alignment'],
                          stack['combine'], stack['dgroup']),
                         ('STACK', 'STACK', 2048, 'paragraph', 'stack', True))
        self.assertIn('/ST:%d' % reallink.LINK_STACK, reallink.LINK_OPTIONS)
        # Falsifier: the original header SP is the /ST value, not CRT0's 2,048.
        self.assertEqual(read_json(ROOT / 'layout/oracle.lock.json')['unpacked_mz']['sp'], reallink.LINK_STACK)
        units = reallink.add_special_units([])
        self.assertEqual([u.kind for u in units], ['prelude', 'stack'])
        self.assertEqual((units[1].raw.length, units[1].raw.combine), (2048, 5))


class PublishedDataTests(unittest.TestCase):

    def test_seg003_uses_its_own_horizon_angles(self):
        recipe = read_json(ROOT / 'recipes/obj_seg003.json')
        self.assertNotIn('_word_2C0FC', recipe['object_declarations']['externals'])
        self.assertEqual(recipe['secondary_dgroup_segments']['_DATA']['start'], 179764)
        source = (ROOT / 'src/obj_seg003.c').read_text()
        self.assertIn('horizon_angles[slope - 2]', source)
        self.assertIn('static char init_crak_resource_names[2][5] = { "crak", "cinf" };', source)

    def test_seg012_module_data_owners(self):
        manifest = read_json(ROOT / 'layout/manifest.json')
        data = {o['id']: (o['start'], o['end']) for o in manifest['owners'] if o['kind'] == 'MATCHING_ASM_DATA'}
        self.assertEqual(data['prerender_wheel_raster:_DATA'], (197406, 197418))
        self.assertEqual(data['resource_memory_manager:_DATA'], (196232, 197400))
        self.assertEqual(data['sincos:_DATA'], (197620, 198134))
        publics = {p['name'] for p in read_json(ROOT / 'recipes/timer_counter_deadline_helpers.json')
                   ['object_declarations']['publics']}
        self.assertIn('_line_input_screen_rect', publics)   # integ33 registry name of word_405FE


if __name__ == '__main__':
    unittest.main()
