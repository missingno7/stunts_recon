"""integ40: the whole LINK c_common communal unit published (LINK_COMMUNAL).

* one accepted row owns [207408,222352): 303 communals, every declaring unit an
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
import communal_unit as cu  # noqa: E402


def _recipe_of(owner):
    return read_json(ROOT / owner['recipe'])


class UnitTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')
        cls.row = cu.accepted_row(cls.manifest)


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


class InteriorTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        from test_object_flags_modules_and_data import Oracle
        cls.image, cls.relocations = Oracle.load()


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
        from test_object_flags_modules_and_data import layout_override
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'Unsupported reviewed field layout'):
                resolve_symbols(['_byte_363E4'], self.image, self.relocations)


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
