"""integ40: the whole LINK c_common communal unit published (LINK_COMMUNAL).

* one accepted row owns [207408,222352): 312 communals, every declaring unit an
  accepted exact owner (C tentative definitions, one MASM COMM NEAR, the pinned
  `_file.c` buffers), registry names, grounded size bases;
* interior labels are member/element accesses of their owning object (seg005
  GAMESTATE/GAMEINFO members, seg000/seg009 merged containers), bound through
  reviewed fields and negative folded-index bindings, never OMF patching;
* the clone of a reviewed extent is the same object (no self-overlap)."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
import bss_link  # noqa: E402
import communal_unit as cu  # noqa: E402


def _recipe_of(owner):
    return read_json(ROOT / owner['recipe'])


class UnitTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')
        cls.row = cu.accepted_row(cls.manifest)

    def test_one_accepted_unit_replaces_the_raw_communal_unit(self):
        self.assertIsNotNone(self.row)
        self.assertEqual((self.row['start'], self.row['end'], self.row['kind']), (207408, 222352, cu.KIND))
        self.assertFalse([o for o in self.manifest['bss_owners'] if o.get('raw_form') == 'communal-unit'])
        items = cu.check_row_form(self.row)
        self.assertEqual(len(items), 312)
        kinds = {}
        for i in items:
            kinds[i['size_basis']['kind']] = kinds.get(i['size_basis']['kind'], 0) + 1
            self.assertFalse(cu.is_placeholder(i['name']), i['name'])
        self.assertEqual(kinds, {'declared-type': 298, 'pinned-member': 3, 'absorbed-gap': 2,
                                 'neutral-unreferenced': 9})
        rows = bss_link.load_partition(self.manifest)
        report = bss_link.ownership_report(rows)
        self.assertEqual((report['bytes']['raw_communal'], report['bytes']['accepted_communal']), (0, 14944))

    def test_recipes_declare_exactly_the_unit(self):
        receipt = cu.check_manifest(self.manifest, _recipe_of)
        self.assertEqual(receipt, {'communal_unit': 'c_common', 'communals': 312, 'bytes': 14944})
        declared = cu.manifest_declarers(self.manifest, _recipe_of)
        self.assertEqual({n: [o for o, _, _ in d] for n, d in declared.items() if n.startswith('__buf')},
                         {'__bufin': ['library__file_192064'], '__bufout': ['library__file_192064'],
                          '__buferr': ['library__file_192064']})
        self.assertEqual(declared['_randomseeds'][0][:2], ('obj_seg002', 6))       # MASM COMM NEAR
        # one declaring unit per communal (the semantic owner)
        self.assertTrue(all(len(d) == 1 for d in declared.values()))

    def test_refusals(self):
        def refused(mutate, pattern):
            m = copy.deepcopy(self.manifest)
            mutate(cu.accepted_row(m)['communals'])
            with self.assertRaisesRegex(ValueError, pattern):
                cu.check_manifest(m, _recipe_of)
        refused(lambda items: items[0].update(name='_word_32A30'), 'placeholder')
        refused(lambda items: items[5]['size_basis'].update(declaration='int nowhere;'), 'not in a declaring source')
        refused(lambda items: items[5].update(declarers=['obj_seg031']), 'declarers')
        absorbed = next(i for i in self.row['communals'] if i['size_basis']['kind'] == 'absorbed-gap')
        self.assertEqual((absorbed['name'], absorbed['size'], absorbed['size_basis']['element_size']),
                         ('_rcmapix', 90, 2))

    def test_registry_names_every_communal(self):
        registry = read_json(ROOT / 'layout/names-registry.json')['names']
        for item in self.row['communals']:
            if item['size_basis']['kind'] == 'pinned-member':
                continue
            self.assertEqual('_' + registry[str(item['address'])]['name'], item['name'])
        # interior labels have no registry row of their own
        for address in (214476, 214478, 215466, 220482, 221554, 222181, 222182, 222186):
            self.assertNotIn(str(address), registry)


class InteriorTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        from test_integ24 import Oracle
        cls.image, cls.relocations = Oracle.load()

    def test_seg005_aliases_are_member_accesses(self):
        text = (ROOT / 'src/obj_seg005.c').read_text(encoding='latin-1')
        self.assertIn('core.game_frame_in_sec >= core.game_frames_per_sec', text)
        self.assertIn('globalgamesettings.game_opponenttype != 0', text)
        recipe = read_json(ROOT / 'recipes/obj_seg005.json')
        self.assertFalse({'_word_345CC', '_word_345CE', '_byte_349AA'} & set(recipe['object_declarations']['externals']))
        addends = {(f['target'], bytes.fromhex(f['encoded_addend'])) for f in recipe['expected_fixups']
                   if f['offset'] in (2734, 2738, 2338)}
        self.assertEqual(addends, {('_core', (312).to_bytes(2, 'little')), ('_core', (314).to_bytes(2, 'little')),
                                   ('_globalgamesettings', (6).to_bytes(2, 'little'))})

    def test_merged_containers_bind_through_reviewed_fields(self):
        from data_symbols import resolve_symbols
        got = resolve_symbols(['_resbuftext', '_scrorder_idxs', '_core', '_audio_frmarr'], self.image,
                              self.relocations)
        self.assertEqual(got['_resbuftext']['allowed_addends'], [0, 1, 2, 3, 6])
        self.assertEqual(got['_scrorder_idxs']['allowed_addends'], [0, 2])
        self.assertIn(314, got['_core']['allowed_addends'])      # a clone never overlaps its own source
        layout = read_json(ROOT / 'layout/data-symbols.json')
        self.assertEqual([f['width'] for f in layout['symbols']['_byte_363E4']['fields']], [1, 1, 1, 1])
        bad = copy.deepcopy(layout)
        bad['symbols']['_byte_363E4']['fields'][0]['width'] = 3
        from test_integ24 import layout_override
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'Unsupported reviewed field layout'):
                resolve_symbols(['_byte_363E4'], self.image, self.relocations)

    def test_seg009_negative_folds(self):
        recipe = read_json(ROOT / 'recipes/obj_seg009.json')
        rows = recipe['negative_folded_index_bindings']
        self.assertEqual(len(rows), 8)
        self.assertTrue(all(r['target'] == '_lnoffsets' and r['k'] == 1 and r['element_size'] == 2 and
                            'ptr' in r['index_operand'] for r in rows))


class O15Tests(unittest.TestCase):

    def test_file_tentative_definition_preserves_code_and_fixups(self):
        from compiler import compile_source

        external = b'extern int shared; int read_shared(void) { return shared; }\n'
        tentative = b'int shared; int read_shared(void) { return shared; }\n'
        ext_obj, _ = compile_source(external, 'msc510-medium')
        com_obj, _ = compile_source(tentative, 'msc510-medium', communals=['_shared'])

        self.assertEqual(com_obj.segments, ext_obj.segments)
        self.assertEqual(com_obj.linker_fixups, ext_obj.linker_fixups)
        self.assertIn(('_shared', 'external'), list(zip(ext_obj.externals, ext_obj.external_scopes)))
        self.assertIn(('_shared', 'communal'), list(zip(com_obj.externals, com_obj.external_scopes)))
        self.assertEqual([c['name'] for c in com_obj.communals], ['_shared'])

    def test_block_extern_then_file_definition_emits_both_declarations(self):
        from compiler import compile_source

        source = b'int read_shared(void) { extern int shared; return shared; }\nint shared;\n'
        obj, _ = compile_source(source, 'msc510-medium', communals=['_shared'])
        scopes = [scope for name, scope in zip(obj.externals, obj.external_scopes) if name == '_shared']
        self.assertEqual(sorted(scopes), ['communal', 'external'])
        self.assertEqual([c['name'] for c in obj.communals], ['_shared'])


if __name__ == '__main__':
    unittest.main()
