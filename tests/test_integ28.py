"""integ28: composed __AHSHIFT, fill ownership, folded negative index and other rulings."""
import copy
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from binder import bind_composed


class ComposedAhshiftTests(unittest.TestCase):
    """The pinned absolute __AHSHIFT word inside a composed C module (seg005)."""

    def setUp(self):
        # call far _first; mov cx,__AHSHIFT; table word -> own segment
        code = b'\x9a\0\0\0\0\xb9\0\0\x04\x00\x90\x90'
        segment = {'name': 'UNIT_TEXT', 'index': 1, 'length': len(code)}
        self.code = code
        self.obj = SimpleNamespace(
            segment_defs=[segment], groups=[],
            publics=[{'name': '_entry', 'segment': 'UNIT_TEXT', 'offset': 0}],
            externals=['_first', '__AHSHIFT'], segment_lengths={'UNIT_TEXT': len(code)},
            segment_length=lambda name: len(code), segment_bytes=lambda name: self.code,
            local_publics=[], local_externals=[], linker_fixups=[])
        self.obj.linker_fixups = [
            {'segment': 'UNIT_TEXT', 'offset': 8, 'loc': 'offset16', 'width': 2, 'self_relative': False,
             'target_kind': 'segment', 'target_method': 0, 'target_index': 1, 'target': 'UNIT_TEXT',
             'frame_method': 0, 'frame_kind': 'segment', 'frame': 'UNIT_TEXT', 'frame_index': 1,
             'displacement': 0, 'encoded_addend': '0400'},
            {'segment': 'UNIT_TEXT', 'offset': 6, 'loc': 'loader-offset16', 'width': 2,
             'self_relative': False, 'target_kind': 'external', 'target_method': 2, 'target_index': 2,
             'target': '__AHSHIFT', 'frame_method': 5, 'frame_kind': 'target', 'frame': '__AHSHIFT',
             'frame_index': 0, 'displacement': 0, 'encoded_addend': '0000'},
            {'segment': 'UNIT_TEXT', 'offset': 1, 'loc': 'pointer32', 'width': 4, 'self_relative': False,
             'target_kind': 'external', 'target_method': 2, 'target_index': 1, 'target': '_first',
             'frame_method': 5, 'frame_kind': 'target', 'frame': '_first', 'frame_index': 0,
             'displacement': 0, 'encoded_addend': '00000000'}]
        self.recipe = {
            'id': 'entry', 'start': 0x10000, 'end': 0x10000 + len(code), 'object_segment': 'UNIT_TEXT',
            'public': '_entry', 'original_frame_load_address': 0x10000,
            'expected_fixups': self.obj.linker_fixups,
            'expected_relocations': [{'segment': 0x1000, 'offset': 3, 'load_offset': 0x10003}],
            'binding': {'mode': 'external-far-call-code-pointer-dgroup-offset16-v1', 'declarations': {
                'segments': self.obj.segment_defs, 'groups': self.obj.groups,
                'publics': self.obj.publics, 'externals': self.obj.externals}}}
        self.symbols = {'_first': {'kind': 'far-code', 'frame_load_address': 0x20000, 'load_address': 0x20010},
                        '__AHSHIFT': {'kind': 'absolute-runtime-word', 'value': 12, 'public': '__AHSHIFT'},
                        'UNIT_TEXT': {'kind': 'local-text', 'frame_load_address': 0x10000}}

    def test_absolute_word_binds_to_twelve_without_relocation(self):
        payload, receipt = bind_composed(self.obj, self.recipe, self.symbols)
        self.assertEqual(payload[6:8], b'\x0c\x00')
        self.assertEqual(receipt['generated_relocations'], self.recipe['expected_relocations'])

    def test_wrong_absolute_value_is_refused(self):
        bad = copy.deepcopy(self.symbols)
        bad['__AHSHIFT']['value'] = 11
        with self.assertRaisesRegex(ValueError, '__AHSHIFT'):
            bind_composed(self.obj, self.recipe, bad)

    def test_absolute_word_outside_mov_cx_is_refused(self):
        self.code = b'\x9a\0\0\0\0\xba\0\0\x04\x00\x90\x90'  # mov dx,imm16
        with self.assertRaisesRegex(ValueError, '__AHSHIFT'):
            bind_composed(self.obj, self.recipe, self.symbols)

    def test_absolute_word_with_addend_is_refused(self):
        self.code = b'\x9a\0\0\0\0\xb9\x01\0\x04\x00\x90\x90'
        fixes = copy.deepcopy(self.obj.linker_fixups)
        fixes[1]['encoded_addend'] = '0100'
        self.obj.linker_fixups = fixes
        recipe = copy.deepcopy(self.recipe)
        recipe['expected_fixups'] = fixes
        with self.assertRaises(ValueError):
            bind_composed(self.obj, recipe, self.symbols)


class LinkFillTests(unittest.TestCase):
    """LINK paragraph fill after an object's last byte (RULING integ28)."""

    def setUp(self):
        import link_fill
        self.L = link_fill
        self.image = bytearray([0xcb] * 0x40)
        self.image[0x14:0x20] = bytes(12)
        self.fill = link_fill.fill_row('obj', 0x14, 0x20)
        self.manifest = {'owners': [
            {'id': 'obj', 'kind': 'MATCHING_C', 'start': 0, 'end': 0x14, 'recipe': 'recipes/obj.json'},
            self.fill,
            {'id': 'far', 'kind': 'MATCHING_C_DATA', 'start': 0x20, 'end': 0x40,
             'recipe': 'recipes/far.json', 'module_form': 'data-only'}]}
        self.recipe = {'start': 0x20, 'object_segment': 'FAR', 'object_declarations': {
            'segments': [{'name': 'FAR', 'alignment': 'paragraph'}]}}

    def check(self, image=None, recipe=None, manifest=None):
        from unittest.mock import patch
        with patch.object(self.L, 'read_json', return_value=recipe or self.recipe):
            return self.L.checked_fill((manifest or self.manifest)['owners'][1], manifest or self.manifest,
                                       bytes(image or self.image))

    def test_fill_before_paragraph_aligned_segment(self):
        receipt = self.check()
        self.assertEqual(receipt['next_segment']['alignment'], 'paragraph')
        self.assertEqual(receipt['fill'], [0x14, 0x20])

    def test_nonzero_fill_is_refused(self):
        image = bytearray(self.image); image[0x18] = 1
        with self.assertRaisesRegex(ValueError, 'not zero'):
            self.check(image=image)

    def test_word_aligned_following_segment_is_refused(self):
        recipe = {**self.recipe, 'object_declarations': {'segments': [{'name': 'FAR', 'alignment': 'word'}]}}
        from unittest.mock import patch
        import function_evidence
        with patch.object(function_evidence, 'current_inventory', return_value={'functions': []}):
            with self.assertRaisesRegex(ValueError, 'paragraph-aligned following segment'):
                self.check(recipe=recipe)

    def test_fill_must_follow_an_accepted_object(self):
        import copy
        manifest = copy.deepcopy(self.manifest)
        manifest['owners'][0]['kind'] = 'UNRESOLVED_RAW'
        with self.assertRaisesRegex(ValueError, 'complete accepted object'):
            self.check(manifest=manifest)

    def test_fill_must_end_at_the_next_paragraph(self):
        import copy
        manifest = copy.deepcopy(self.manifest)
        manifest['owners'][0]['end'] = 0x04
        manifest['owners'][1] = self.L.fill_row('obj', 0x04, 0x20)
        image = bytearray(self.image); image[0x04:0x20] = bytes(0x1c)
        with self.assertRaisesRegex(ValueError, 'next paragraph'):
            self.check(image=image, manifest=manifest)

    def test_prefix_owner_cannot_end_an_object(self):
        import copy
        manifest = copy.deepcopy(self.manifest)
        manifest['owners'][0]['contribution_form'] = 'prefix_of_object'
        with self.assertRaisesRegex(ValueError, 'complete accepted object'):
            self.check(manifest=manifest)


class NegativeFoldedIndexTests(unittest.TestCase):
    """RULING: `trackrows[row - 1]` in whole seg004 binds as _trackrows-2 only
    with the recorded original indexed access and same-index witness."""

    @classmethod
    def setUpClass(cls):
        from common import read_json
        from compiler import compile_source
        from object_flags import recipe_flags
        from object_probe import recipe_sparse_zero
        from oracle import verify
        from mz import MZ
        cls.recipe = read_json(ROOT / 'recipes/obj_seg004.json')
        source = (ROOT / cls.recipe['source']).read_bytes()
        cls.obj, _ = compile_source(source, cls.recipe['profile'], recipe_flags(cls.recipe),
                                    sparse_zero=recipe_sparse_zero(cls.recipe))
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']

    def bind(self, recipe):
        from multi_contribution import bind_multi
        return bind_multi(self.obj, recipe, self.image, self.relocations)

    def test_reviewed_negative_fold_binds_whole_object(self):
        payload, _ = self.bind(self.recipe)
        self.assertEqual(payload, self.image[self.recipe['start']:self.recipe['end']])

    def variant(self, **change):
        recipe = copy.deepcopy(self.recipe)
        for row in recipe['negative_folded_index_bindings']:
            row.update(change)
        return recipe

    def test_unreviewed_negative_addend_is_refused(self):
        recipe = copy.deepcopy(self.recipe)
        del recipe['negative_folded_index_bindings']
        with self.assertRaisesRegex(ValueError, 'leaves independently grounded object'):
            self.bind(recipe)

    def test_wrong_element_count_is_refused(self):
        with self.assertRaisesRegex(ValueError, 'form differs'):
            self.bind(self.variant(k=2))

    def test_witness_must_access_the_same_array(self):
        with self.assertRaisesRegex(ValueError, 'non-negative access'):
            self.bind(self.variant(witness_site=64346 - 2))

    def test_witness_must_share_the_index_variable(self):
        with self.assertRaisesRegex(ValueError, 'same index variable'):
            self.bind(self.variant(index_operand='word ptr [bp + 6]'))

    def test_only_listed_fixups_may_use_the_addend(self):
        recipe = copy.deepcopy(self.recipe)
        recipe['negative_folded_index_bindings'] = recipe['negative_folded_index_bindings'][:1]
        with self.assertRaisesRegex(ValueError, 'unreviewed FIXUPP'):
            self.bind(recipe)


class SearchLogicalToolchainTests(unittest.TestCase):
    def test_register_gated_profile_files_use_logical_identities(self):
        import search
        env, _ = search._environment('msc600a-medium-zi', None, None, None)
        self.assertIn('toolchain/msc600a/API.LIB', env['tool_files'])
        self.assertFalse((ROOT / 'toolchain/msc600a/API.LIB').exists())


if __name__ == '__main__':
    unittest.main()
