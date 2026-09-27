"""integ26: reviewed dispatch overlays (run_option_menu, audio_map_song_tracks),
LIDATA zero tails in TU _DATA, TU data publics, `_loadds` DGROUP base16 in C,
ASM module private prefixes and data publics, CS-island MOV immediates,
reviewed extents (_resID_byte1, _audiochunks_unk) and own-public far pointers."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
from oracle import verify
from mz import MZ


class Oracle:
    @classmethod
    def load(cls):
        if not hasattr(Oracle, 'result'):
            Oracle.result = verify(write=False)
            Oracle.image = MZ.parse(Oracle.result[1]).load_image(Oracle.result[1])
            Oracle.relocations = Oracle.result[2]['unpacked_mz']['relocations']
        return Oracle


def symbols_layout():
    return read_json(ROOT/'layout/data-symbols.json')


# ------------------------------------------------------ item 1 / 6: overlays
class DispatchOverlays(unittest.TestCase):
    def test_new_rows_are_instruction_verified(self):
        from function_evidence import current_inventory
        o = Oracle.load()
        rows = {f['name']: f for f in current_inventory(o.image)['functions']}
        for name, extent in (('run_option_menu', (12106, 12664)),
                             ('audio_map_song_tracks', (164436, 165114))):
            self.assertEqual((rows[name]['start'], rows[name]['end']), extent)
            self.assertEqual(rows[name]['status'], 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')

    def test_dispatch_rows_need_their_reviewed_site(self):
        import function_evidence
        o = Oracle.load()
        for name in ('run_option_menu', 'audio_map_song_tracks'):
            saved = function_evidence.GENERIC_DISPATCH_SITES.pop(name)
            try:
                with self.assertRaises(ValueError):
                    function_evidence.reviewed_functions(o.image)
            finally:
                function_evidence.GENERIC_DISPATCH_SITES[name] = saved

    def test_changed_table_word_or_bound_is_rejected(self):
        import function_evidence
        o = Oracle.load()
        document = read_json(ROOT/'layout/function-evidence.json')
        row = next(f for f in document['functions'] if f['name'] == 'run_option_menu')
        bad = copy.deepcopy(row)
        bad['dispatch_tables'][0]['valid_indices'] = list(range(7))
        bad['dispatch_tables'][0]['used_indices'] = list(range(7))
        bad['table_proofs'][0]['valid_indices'] = list(range(7))
        original = function_evidence.read_json
        def patched(path):
            data = original(path)
            if Path(path).name == 'function-evidence.json':
                data = copy.deepcopy(data)
                data['functions'] = [bad if f['name'] == 'run_option_menu' else f
                                     for f in data['functions']]
            return data
        function_evidence.read_json = patched
        try:
            with self.assertRaisesRegex(ValueError, 'complete index domain'):
                function_evidence.reviewed_functions(o.image)
        finally:
            function_evidence.read_json = original


# ------------------------------------------------ item 1: LIDATA zero tails
class ZeroTailData(unittest.TestCase):
    def test_lidata_zero_tail_counts_as_initialized(self):
        from compiler import compile_source, CompileFailure
        from object_probe import msc_alignment_sparse_zero
        source = (b'char a1[3] = "ab";\nchar big[82] = {0};\nint w = 5;\n'
                  b'int f(void) { return a1[0] + big[1] + w; }\n')
        try:
            compile_source(source, 'msc510-medium')
            self.fail('alignment hole was not reported')
        except CompileFailure as error:
            data = (Path(error.receipt['work_directory'])/'UNIT.OBJ').read_bytes()
        self.assertEqual(msc_alignment_sparse_zero(data),
                         {'_DATA': {'initialized_ranges': [[0, 3], [4, 88]],
                                    'declared_length': 88}})
        obj, _ = compile_source(source, 'msc510-medium',
                                sparse_zero=msc_alignment_sparse_zero(data))
        self.assertEqual(obj.segment_bytes('_DATA')[4:86], bytes(82))

    def test_multi_byte_holes_stay_refused(self):
        from object_probe import recipe_sparse_zero
        recipe = {'kind': 'c', 'object_segment': 'UNIT_TEXT',
                  'secondary_dgroup_segments': {'_DATA': {'start': 0, 'end': 10}},
                  'sparse_zero': {'_DATA': {'initialized_ranges': [[0, 3], [6, 10]],
                                            'declared_length': 10}}}
        with self.assertRaisesRegex(ValueError, 'word-alignment holes'):
            recipe_sparse_zero(recipe)


# ------------------------------------------------------ TU data publics
class DataPublics(unittest.TestCase):
    class Obj:
        def __init__(self, publics):
            self.publics = publics

    def spec(self):
        return {'_DATA': {'start': 178098, 'end': 179538}}

    def test_public_at_its_alias_address_is_accepted(self):
        from secondary_contribution import check_data_publics
        check_data_publics(self.Obj([{'name': '_byte_3B80C', 'segment': '_DATA', 'offset': 90},
                                     {'name': '_scenery_names', 'segment': '_DATA', 'offset': 264},
                                     {'name': '_main', 'segment': 'UNIT_TEXT', 'offset': 0}]),
                           self.spec(), 'UNIT_TEXT')

    def test_public_spelled_like_an_alias_elsewhere_is_refused(self):
        from secondary_contribution import check_data_publics
        with self.assertRaisesRegex(ValueError, 'conflicts with the reviewed alias'):
            check_data_publics(self.Obj([{'name': '_carmenu_buttons_x2', 'segment': '_DATA',
                                          'offset': 896}]), self.spec(), 'UNIT_TEXT')

    def test_public_outside_its_segment_or_unowned_is_refused(self):
        from secondary_contribution import check_data_publics
        with self.assertRaisesRegex(ValueError, 'outside its complete placed segment'):
            check_data_publics(self.Obj([{'name': '_x', 'segment': '_DATA', 'offset': 1440}]),
                               self.spec(), 'UNIT_TEXT')
        with self.assertRaisesRegex(ValueError, 'unowned segment'):
            check_data_publics(self.Obj([{'name': '_x', 'segment': 'CONST', 'offset': 0}]),
                               self.spec(), 'UNIT_TEXT')


# ------------------------------------------- item 2: `_loadds` DGROUP base16
class DgroupBase16(unittest.TestCase):
    DECL = {'segments': [{'index': 1, 'name': 'UNIT_TEXT', 'class': 'CODE', 'length': 10},
                         {'index': 2, 'name': '_DATA', 'class': 'DATA', 'length': 0}],
            'groups': [{'index': 1, 'name': 'DGROUP', 'segment_indices': [2], 'segments': ['_DATA']}]}
    FIX = {'segment': 'UNIT_TEXT', 'offset': 8, 'width': 2, 'loc': 'base16', 'self_relative': False,
           'target_kind': 'segment', 'target': '_DATA', 'displacement': 0, 'frame_method': 1,
           'frame_index': 1, 'target_method': 0, 'target_index': 2, 'frame_kind': 'group',
           'frame': 'DGROUP', 'encoded_addend': '0000'}

    def recipe(self, **changes):
        fix = {**self.FIX, **changes.pop('fix', {})}
        declarations = copy.deepcopy(self.DECL)
        if 'length' in changes:
            declarations['segments'][1]['length'] = changes.pop('length')
        return {'kind': 'c', 'expected_fixups': [fix], 'object_declarations': declarations}

    def test_zero_length_data_with_group_frame_is_the_reviewed_shape(self):
        from code_symbols import _c_dgroup_base_fixups
        self.assertTrue(_c_dgroup_base_fixups(self.recipe()))

    def test_other_shapes_are_refused(self):
        from code_symbols import _c_dgroup_base_fixups
        self.assertFalse(_c_dgroup_base_fixups(self.recipe(length=4)))
        self.assertFalse(_c_dgroup_base_fixups(self.recipe(fix={'frame_method': 0,
                                                                'frame_kind': 'segment',
                                                                'frame': '_DATA', 'frame_index': 2})))
        self.assertFalse(_c_dgroup_base_fixups(self.recipe(fix={'loc': 'offset16'})))
        self.assertFalse(_c_dgroup_base_fixups(self.recipe(fix={'encoded_addend': '0200'})))

    def test_resolver_grounds_the_dgroup_paragraph(self):
        from code_symbols import resolve_recipe_symbols
        o = Oracle.load()
        recipe = {**self.recipe(), 'object_segment': 'UNIT_TEXT',
                  'binding': {'mode': 'external-dgroup-offset16-v1'}}
        symbols = resolve_recipe_symbols(recipe, o.image, o.relocations)
        self.assertEqual(symbols['_DATA'], {'kind': 'local-dgroup-base',
                                            'frame_load_address': 178032})
        bad = copy.deepcopy(recipe)
        bad['object_declarations']['segments'][1]['length'] = 2
        with self.assertRaisesRegex(ValueError, 'Unreviewed composed segment target'):
            resolve_recipe_symbols(bad, o.image, o.relocations)

    SOURCE = (b"extern char far sprite2[];\nextern void far nopsub_36AF2(void);\n"
              b"void far * far _loadds f(int i) { if (i) return (void far *)sprite2;"
              b" return (void far *)nopsub_36AF2; }\n")

    def composed(self, mutate=None):
        from compiler import compile_source
        from binder import bind_contribution
        from code_symbols import resolve_recipe_symbols
        from prefix_proof import relocation_stream
        o = Oracle.load()
        obj, _ = compile_source(self.SOURCE, 'msc510-medium')
        if mutate:
            mutate(obj)
        start = 0x10000
        recipe = {'id': 'f', 'kind': 'c', 'start': start,
                  'end': start + obj.segment_length('UNIT_TEXT'),
                  'object_segment': 'UNIT_TEXT', 'public': '_f',
                  'original_frame_load_address': start, 'expected_fixups': obj.linker_fixups,
                  'expected_relocations': relocation_stream(obj.linker_fixups, start),
                  'binding': {'mode': 'external-far-call-code-pointer-dgroup-offset16-v1',
                              'declarations': {'segments': obj.segment_defs, 'groups': obj.groups,
                                               'publics': obj.publics, 'externals': obj.externals}}}
        symbols = resolve_recipe_symbols(recipe, o.image, o.relocations)
        return obj, bind_contribution(obj, recipe, symbols)

    def test_ds_reload_cs_pair_and_code_pointer_compose(self):
        obj, (payload, receipt) = self.composed()
        word = lambda at: int.from_bytes(payload[at:at+2], 'little')
        at = {(f['loc'], f['target']): f['offset'] for f in obj.linker_fixups}
        self.assertEqual(word(at[('base16', '_DATA')]), 178032 // 16)
        self.assertEqual(word(at[('offset16', '_sprite2')]), 149854 - 125472)
        self.assertEqual(word(at[('base16', '_sprite2')]), 125472 // 16)
        self.assertEqual(word(at[('base16', '_nopsub_36AF2')]) * 16 +
                         word(at[('loader-offset16', '_nopsub_36AF2')]), 158450)
        self.assertEqual(len(receipt['generated_relocations']), 3)

    def test_unpaired_cs_or_code_pointer_words_are_refused(self):
        def drop(loc, target):
            def mutate(obj):
                obj.linker_fixups = [f for f in obj.linker_fixups
                                     if (f['loc'], f['target']) != (loc, target)]
            return mutate
        for loc, target in (('base16', '_sprite2'), ('loader-offset16', '_nopsub_36AF2')):
            with self.subTest(target=target), self.assertRaises(ValueError):
                self.composed(drop(loc, target))


# ------------------------------------------------ item 3: ASM module forms
class ModuleForms(unittest.TestCase):
    def rows(self):
        from asm_module import _function_rows
        from function_evidence import current_inventory
        o = Oracle.load()
        return o, _function_rows(current_inventory(o.image), o.image)

    def test_private_prefix_enters_its_module(self):
        from asm_module import _checked_private_prefix
        o, rows = self.rows()
        proof = {'kind': 'private-entry-prefix-v1', 'length': 14}
        self.assertEqual(_checked_private_prefix(proof, 150408, 154406, 125472, rows, o.image), 150422)
        with self.assertRaises(ValueError):   # cuts an instruction
            _checked_private_prefix({**proof, 'length': 12}, 150408, 154406, 125472, rows, o.image)
        with self.assertRaises(ValueError):   # the module ends before the entered procedure
            _checked_private_prefix(proof, 150408, 150422 + 1, 125472, [], o.image)
        with self.assertRaises(ValueError):
            _checked_private_prefix({**proof, 'extra': 1}, 150408, 154406, 125472, rows, o.image)

    def test_data_public_needs_its_pinned_label_outside_procedures(self):
        from asm_module import _checked_data_public
        o, rows = self.rows()
        good = {'public': '_next_wnd_def', 'offset': 348,
                'reference_path': 'src/restunts/asmorig/seg012.asm', 'reference_line': 14245}
        self.assertEqual(_checked_data_public(good, 150408, 154406, 125472, rows, set(), o.image), 150756)
        for bad in ({**good, 'reference_line': 14246},       # another label
                    {**good, 'offset': 200},                  # inside sprite_make_wnd
                    {**good, 'public': '_sprite1'}):
            with self.assertRaises(ValueError):
                _checked_data_public(bad, 150408, 154406, 125472, rows, set(), o.image)

    def test_cs_island_mov_immediate_binds_in_its_own_frame(self):
        from assembler import assemble_source
        from binder import bind_contribution
        from code_symbols import resolve_recipe_symbols
        o = Oracle.load()
        source = (b".8086\r\n_TEXT segment word public 'CODE'\r\nassume cs:_TEXT\r\n"
                  b"extrn sprite2:byte\r\nextrn _sprite_set_1_from_argptr:far\r\n"
                  b"public _sprite_copy_2_to_1\r\n_sprite_copy_2_to_1 proc far\r\n"
                  b"    mov ax, seg _TEXT\r\n    push ax\r\n    mov ax, offset sprite2\r\n"
                  b"    push ax\r\n    call _sprite_set_1_from_argptr\r\n    add sp, 4\r\n"
                  b"    retf\r\n    db 0\r\n_sprite_copy_2_to_1 endp\r\n_TEXT ends\r\nend\r\n")
        obj, _ = assemble_source(source, 'masm510-game')
        recipe = {'id': 'sprite_copy_2_to_1', 'kind': 'asm', 'start': 154388, 'end': 154406,
                  'object_segment': '_TEXT', 'public': '_sprite_copy_2_to_1',
                  'original_frame_load_address': 125472, 'expected_fixups': obj.linker_fixups,
                  'expected_relocations': [{'segment': 8192, 'offset': 23317, 'load_offset': 154389},
                                           {'segment': 8192, 'offset': 23327, 'load_offset': 154399}],
                  'binding': {'mode': 'asm-external-dgroup-offset16-v1', 'declarations': {
                      'segments': obj.segment_defs, 'groups': obj.groups,
                      'publics': obj.publics, 'externals': obj.externals}}}
        symbols = resolve_recipe_symbols(recipe, o.image, o.relocations)
        payload, _ = bind_contribution(obj, recipe, symbols)
        self.assertEqual(payload, o.image[154388:154406])
        # The same word as a data offset of an unrelated segment frame is refused.
        bad = copy.deepcopy(recipe)
        for fix in bad['expected_fixups']:
            if fix['target'] == 'sprite2':
                fix['frame_method'], fix['frame_kind'], fix['frame'], fix['frame_index'] = 5, 'target', 'sprite2', 0
        obj.linker_fixups = bad['expected_fixups']
        with self.assertRaises(ValueError):
            bind_contribution(obj, bad, symbols)


# ------------------------------------------------ items 4/6: reviewed extents
class ReviewedExtents(unittest.TestCase):
    def test_name_buffer_and_stride_table_resolve(self):
        from data_symbols import resolve_symbols
        o = Oracle.load()
        r = resolve_symbols({'_resID_byte1', '_audiochunks_unk', '_audiochunks_unk2'},
                            o.image, o.relocations)
        self.assertEqual(r['_resID_byte1']['allowed_addends'], [0, 1, 2, 3])
        self.assertEqual(max(r['_audiochunks_unk']['allowed_addends']), 1823)
        self.assertIn(-1216, r['_audiochunks_unk2']['folded_addends'])

    def test_changed_width_or_witness_is_refused(self):
        import data_symbols
        o = Oracle.load()
        layout = symbols_layout()
        with self.assertRaisesRegex(ValueError, 'Unreviewed name-buffer extent'):
            data_symbols._check_name_buffer_extent(
                '_resID_byte1', {**layout['symbols']['_resID_byte1'], 'width': 6},
                layout, o.image, o.relocations)
        with self.assertRaisesRegex(ValueError, 'Unreviewed counted-stride extent'):
            data_symbols._check_counted_stride_extent(
                '_audiochunks_unk', {**layout['symbols']['_audiochunks_unk'], 'width': 25 * 76},
                layout, o.image, o.relocations)
        saved = data_symbols._NAME_BUFFER_WITNESS['callee']
        data_symbols._NAME_BUFFER_WITNESS['callee'] = (saved[0], [(135105, 'b90500')])
        try:
            with self.assertRaisesRegex(ValueError, 'witness instruction differs'):
                data_symbols._check_name_buffer_extent(
                    '_resID_byte1', layout['symbols']['_resID_byte1'], layout, o.image, o.relocations)
        finally:
            data_symbols._NAME_BUFFER_WITNESS['callee'] = saved

    def test_partial_overlap_with_the_stride_table_is_refused(self):
        import data_symbols
        o = Oracle.load()
        layout = copy.deepcopy(symbols_layout())
        layout['symbols']['_probe'] = {'load_address': 211308 + 1800, 'storage': 'bss', 'width': 40}
        with self.assertRaisesRegex(ValueError, 'partially overlaps'):
            data_symbols._check_counted_stride_extent(
                '_audiochunks_unk', layout['symbols']['_audiochunks_unk'], layout,
                o.image, o.relocations)

    def test_rect_table_and_word_pairs_resolve(self):
        from data_symbols import resolve_symbols
        o = Oracle.load()
        r = resolve_symbols({'_rect_unk', '_word_449FC', '_skybox_res_ofs', '_dastshapeptr'},
                            o.image, o.relocations)
        self.assertEqual(max(r['_rect_unk']['allowed_addends']), 119)
        for name in ('_word_449FC', '_skybox_res_ofs', '_dastshapeptr'):
            self.assertEqual(r[name]['allowed_addends'], [0, 1, 2, 3])

    def test_word_pair_needs_its_witnesses_and_labels(self):
        import data_symbols
        o = Oracle.load()
        layout = symbols_layout()
        symbol = layout['symbols']['_word_449FC']
        with self.assertRaisesRegex(ValueError, 'Unreviewed word-pair extent'):
            data_symbols._check_pair_extent('_word_449FC', {**symbol, 'width': 6}, layout,
                                            o.image, o.relocations)
        with self.assertRaisesRegex(ValueError, 'Unreviewed word-pair extent'):
            data_symbols._check_pair_extent('_word_449FE', symbol, layout, o.image, o.relocations)
        saved = data_symbols._PAIR_EXTENTS['_word_449FC']
        data_symbols._PAIR_EXTENTS['_word_449FC'] = {**saved, 'witnesses': (
            ('sub_19F14', 40787, 'a166ac' 'a38c92'),)}
        try:
            with self.assertRaisesRegex(ValueError, 'witness differs'):
                data_symbols._check_pair_extent('_word_449FC', symbol, layout, o.image, o.relocations)
        finally:
            data_symbols._PAIR_EXTENTS['_word_449FC'] = saved
        saved = data_symbols._STRIDE_TABLES['_rect_unk']
        data_symbols._STRIDE_TABLES['_rect_unk'] = {**saved, 'end_label': 'rect_unk9'}
        try:
            with self.assertRaisesRegex(ValueError, 'pinned endpoints differ'):
                data_symbols._check_counted_stride_extent(
                    '_rect_unk', layout['symbols']['_rect_unk'], layout, o.image, o.relocations)
        finally:
            data_symbols._STRIDE_TABLES['_rect_unk'] = saved

    def test_pointer_list_table_tiles_its_extent(self):
        import data_symbols
        o = Oracle.load()
        layout = symbols_layout()
        symbol = layout['symbols']['_unk_3C0A2']
        self.assertEqual(symbol['width'], 36)
        data_symbols._check_list_table_extent('_unk_3C0A2', symbol, layout, o.image, o.relocations)
        with self.assertRaisesRegex(ValueError, 'do not tile'):
            data_symbols._check_list_table_extent('_unk_3C0A2', {**symbol, 'width': 40}, layout,
                                                  o.image, o.relocations)
        with self.assertRaisesRegex(ValueError, 'Unreviewed list-table extent'):
            data_symbols._check_list_table_extent('_unk_3C0EE', symbol, layout, o.image, o.relocations)
        # The fence pair table stays without a width (see docs: word_2C0FC).
        self.assertNotIn('width', layout['symbols']['_unk_3C0EE'])

    def test_negative_folded_addends_stay_refused(self):
        from data_symbols import resolve_symbols
        o = Oracle.load()
        r = resolve_symbols({'_trackrows', '_audio_filetemp'}, o.image, o.relocations)
        self.assertNotIn(-2 % 65536, r['_trackrows']['allowed_addends'])
        self.assertTrue(all(a >= 0 for a in r['_audio_filetemp']['allowed_addends']))


# ------------------------------------------- item 6: own-public far pointers
class OwnPublicPointers(unittest.TestCase):
    def fixes(self):
        base = {'segment': 'UNIT_TEXT', 'width': 2, 'self_relative': False, 'target_kind': 'external',
                'target': '_cb', 'displacement': 0, 'frame_method': 5, 'frame_index': 0,
                'target_method': 2, 'target_index': 1, 'frame_kind': 'target', 'frame': '_cb',
                'encoded_addend': '0000'}
        return [{**base, 'offset': 4, 'loc': 'base16'}, {**base, 'offset': 1, 'loc': 'loader-offset16'}]

    def run_bind(self, fixes, code):
        import multi_contribution
        class Obj: externals = ['_cb']
        recipe = {'start': 159954, 'end': 159954 + len(code), 'original_frame_load_address': 159952,
                  'members': [{'name': 'init_audio_resources'}]}
        rows = []
        return multi_contribution._bind_own_pointers(Obj, recipe, Oracle.load().image,
                                                     bytearray(code), fixes, {'_cb': 6}, rows)

    def test_adjacent_pair_binds_offset_and_paragraph(self):
        linked = self.run_bind(self.fixes(), bytes.fromhex('b80000ba0000'))
        self.assertEqual(linked, bytes.fromhex('b80800') + bytes.fromhex('ba') + (159952 // 16).to_bytes(2, 'little'))

    def test_unpaired_or_non_mov_words_are_refused(self):
        with self.assertRaisesRegex(ValueError, 'adjacent'):
            self.run_bind(self.fixes()[1:], bytes.fromhex('b80000ba0000'))
        with self.assertRaisesRegex(ValueError, 'own-public pointer datum'):
            self.run_bind(self.fixes(), bytes.fromhex('b80000900000'))


if __name__ == '__main__':
    unittest.main()
