"""integ25: per-object flag entries, hard-coded program address audit,
C public truncation, seg029 tail/absolute binding, sparse C _DATA holes,
MSC 5.10 intrinsic pragmas, reviewed code/data aliases and diagnostic tools."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
from oracle import verify
from mz import MZ
from assembler import assemble_source
from compiler import compile_source
from binder import require_relocations_from_fixups
from probe_module import probe, ProbeFailure
import object_flags


class Oracle:
    @classmethod
    def load(cls):
        if not hasattr(Oracle, 'result'):
            Oracle.result = verify(write=False)
            Oracle.image = MZ.parse(Oracle.result[1]).load_image(Oracle.result[1])
            Oracle.relocations = Oracle.result[2]['unpacked_mz']['relocations']
        return Oracle


# ---------------------------------------------------------------- item 1
class ObjectFlagEntries(unittest.TestCase):

    def test_seg028_ds_reload_spellings_are_both_accepted_and_nothing_else(self):
        sets = object_flags.accepted_flag_sets('TUFLAG-Ox-seg028')
        self.assertEqual(sets, [['/AM', '/Ox', '/Gs'], ['/AM', '/Au', '/Ox', '/Gs']])
        self.assertEqual(object_flags.accepted_flag_sets('TUFLAG-Ox-seg029'),
                         [['/AM', '/Ox', '/Gs']])

    def test_au_spelling_is_refused_outside_seg028(self):
        recipe = {'id': 'x', 'kind': 'c', 'profile': 'msc510-medium',
                  'compiler_flags': ['/AM', '/Au', '/Ox', '/Gs'],
                  'compiler_flags_register': 'TUFLAG-Ox-seg029'}
        functions = read_json(ROOT/'evidence/functions.json')['functions']
        row = next(f for f in functions if f.get('segment') == 'seg029' and
                   type(f.get('start')) is int and f.get('name'))
        recipe.update(id=row['name'], start=row['start'], end=row['end'])
        with self.assertRaisesRegex(ValueError, 'differ from the registered object flag set'):
            object_flags.recipe_flags(recipe)
        recipe['compiler_flags'] = ['/AM', '/Ox', '/Gs']
        self.assertEqual(object_flags.recipe_flags(recipe), ['/AM', '/Ox', '/Gs'])

    def test_loadds_equals_au(self):
        source = b'extern int g; int far f(int x) { return g + x; }\n'
        loadds = b'extern int g; int far _loadds f(int x) { return g + x; }\n'
        a, _ = compile_source(loadds, 'msc510-medium', ['/AM', '/Ox', '/Gs'])
        b, _ = compile_source(source, 'msc510-medium', ['/AM', '/Au', '/Ox', '/Gs'])
        self.assertTrue(object_flags.same_object(a, b))


# ---------------------------------------------------------------- item 12a
LITERAL_FAR_CALL_ASM = (b"UNIT_TEXT SEGMENT BYTE PUBLIC 'CODE'\r\n ASSUME CS:UNIT_TEXT\r\n"
                        b"PUBLIC _elticks\r\n_elticks PROC FAR\r\n"
                        b" db 09Ah\r\n dw 03D7Ah, 01EA2h\r\n retf\r\n"
                        b"_elticks ENDP\r\nUNIT_TEXT ENDS\r\nEND\r\n")


class HardCodedProgramAddress(unittest.TestCase):
    def test_asm_literal_far_call_at_relocated_site_is_rejected(self):
        o = Oracle.load()
        module = read_json(ROOT/'recipes/obj_seg008.json')
        accepted = next(m for m in module['members'] if m['name'] == 'timer_get_delta_alt')
        accepted = dict(accepted, expected_relocations=[r for r in module['expected_relocations']
                        if accepted['start'] <= r['load_offset'] < accepted['end']])
        obj, _ = assemble_source(LITERAL_FAR_CALL_ASM, 'masm510-game')
        # Bytes equal the original extent, yet no FIXUPP generates the relocation.
        self.assertEqual(obj.segments['UNIT_TEXT'], o.image[accepted['start']:accepted['end']])
        recipe = {'id': 'timer_get_delta_alt', 'kind': 'asm', 'profile': 'masm510-game',
                  'assembler_flags': ['/Mx', '/I.'], 'include_closure': [], 'source': 'x.ASM',
                  'start': accepted['start'], 'end': accepted['end'], 'target': accepted['target'],
                  'object_segment': 'UNIT_TEXT', 'public': '_elticks',
                  'object_declarations': {'segments': obj.segment_defs, 'groups': obj.groups,
                                          'publics': obj.publics, 'externals': obj.externals},
                  'expected_fixups': [], 'expected_relocations': accepted['expected_relocations']}
        with self.assertRaisesRegex(ProbeFailure, 'hard-coded program address'):
            probe(recipe, o.result, LITERAL_FAR_CALL_ASM)

    def test_c_object_with_fixup_replaced_by_literal_is_rejected(self):
        o = Oracle.load()
        recipe = read_json(ROOT/'tests/fixtures/relocation_literals/timer_get_delta_alt.json')
        obj, _ = compile_source((ROOT/recipe['source']).read_bytes(), recipe['profile'])
        require_relocations_from_fixups(obj, recipe, o.relocations)  # accepted form passes
        literal = copy.deepcopy(obj)
        literal.linker_fixups = [f for f in literal.linker_fixups if f['loc'] != 'pointer32']
        with self.assertRaisesRegex(ValueError, 'hard-coded program address'):
            require_relocations_from_fixups(literal, recipe, o.relocations)

    def test_accepted_sources_contain_no_far_pointer_literals(self):
        import re
        pattern = re.compile(rb'0x[0-9a-fA-F]{7,8}[lL]\b|MK_FP')
        for path in sorted((ROOT/'src').glob('*.c')):
            self.assertIsNone(pattern.search(path.read_bytes()), path.name)


# ---------------------------------------------------------------- item 9
INTRINSIC_CASES = {
    'memcpy': ('void *memcpy(void *, const void *, unsigned);', 'memcpy(a, b, 10);'),
    'memset': ('void *memset(void *, int, unsigned);', 'memset(a, 0, 10);'),
    'memcmp': ('int memcmp(const void *, const void *, unsigned);', 'return memcmp(a, b, 10);'),
    'strlen': ('unsigned strlen(const char *);', 'return strlen(a);'),
    'strcpy': ('char *strcpy(char *, const char *);', 'strcpy(a, b);'),
    'strcat': ('char *strcat(char *, const char *);', 'strcat(a, b);'),
    'strcmp': ('int strcmp(const char *, const char *);', 'return strcmp(a, b);'),
    'strset': ('char *strset(char *, int);', 'strset(a, 1);'),
    'abs': ('int abs(int);', 'return abs(n);'),
    'labs': ('long labs(long);', 'return (int)labs((long)n);'),
    '_rotl': ('unsigned _rotl(unsigned, int);', 'return _rotl(n, 3);'),
    '_rotr': ('unsigned _rotr(unsigned, int);', 'return _rotr(n, 3);'),
    '_lrotl': ('unsigned long _lrotl(unsigned long, int);', 'return (int)_lrotl((long)n, 3);'),
    '_lrotr': ('unsigned long _lrotr(unsigned long, int);', 'return (int)_lrotr((long)n, 3);'),
}


class IntrinsicPragmas(unittest.TestCase):
    def test_documented_msc510_intrinsics_are_inlined_and_frozen(self):
        for name, (proto, call) in INTRINSIC_CASES.items():
            with self.subTest(name=name):
                source = ('%s\n#pragma intrinsic(%s)\n'
                          'int f(char *a, char *b, int n) { %s return 0; }\n'
                          % (proto, name, call)).encode('ascii')
                obj, receipt = compile_source(source, 'msc510-medium')
                self.assertEqual(receipt['preprocessor_closure'],
                                 [{'pragma': 'intrinsic', 'names': [name], 'path': '<source>', 'line': 2}])
                # Expanded inline: no external reference to the library routine.
                self.assertNotIn('_' + name, obj.externals)
                self.assertFalse(any(f['loc'] == 'pointer32' for f in obj.linker_fixups))

    def test_function_pragma_restores_the_call(self):
        source = (b'unsigned strlen(const char *);\n#pragma intrinsic(strlen)\n'
                  b'#pragma function(strlen)\nint f(char *a) { return strlen(a); }\n')
        obj, receipt = compile_source(source, 'msc510-medium')
        self.assertIn('_strlen', obj.externals)
        self.assertEqual([p['pragma'] for p in receipt['preprocessor_closure']], ['intrinsic', 'function'])

    def test_undocumented_names_stay_rejected(self):
        import preprocessor
        for bad in (b'#pragma intrinsic(strncpy)\n', b'#pragma intrinsic(memmove)\n',
                    b'#pragma function(sprintf)\n', b'#pragma intrinsic(memcpy, printf)\n'):
            with self.subTest(bad=bad), self.assertRaisesRegex(ValueError, 'Unsupported historical pragma'):
                preprocessor.prepare(bad, 'msc510-medium')


# ------------------------------------------------ item 2: seg029 object form
FIXTURES = ROOT/'recipes'


class Seg029ObjectForm(unittest.TestCase):
    def recipe(self):
        return read_json(FIXTURES/'obj_seg029.json')


    def test_c_public_truncation_is_msc510_only(self):
        from asm_module import c_public, expected_publics
        obj, _ = compile_source(b'void audioresource_compare_chunknames(void) {}\n', 'msc510-medium')
        self.assertEqual(obj.publics[0]['name'], c_public('audioresource_compare_chunknames'))
        self.assertEqual(expected_publics('audioresource_compare_chunknames', 'c'),
                         {'_audioresource_compare_chunknames', '_audioresource_compare_chunkname'})
        self.assertEqual(expected_publics('short_name', 'c'), {'_short_name'})
        old, _ = compile_source(b'void audioresource_compare_chunknames(void) {}\n', 'msc500-medium')
        self.assertEqual(old.publics[0]['name'], '_audioresource_compare_chunknames')

    def test_object_tail_needs_its_exact_reviewed_shape(self):
        o = Oracle.load()
        for field, value, message in (
                ('fill_nops', 1, 'Object tail'), ('declared_uninitialized', 2, 'Object tail'),
                ('kind', 'free-bytes', 'Object tail')):
            with self.subTest(field=field):
                bad = self.recipe()
                bad['object_tail'][field] = value
                with self.assertRaisesRegex(ValueError, message):
                    probe(bad, o.result)
        bad = self.recipe()
        bad['sparse_zero']['UNIT_TEXT']['initialized_prefix'] -= 1
        with self.assertRaises(ValueError):
            probe(bad, o.result)
        bad = self.recipe()
        del bad['object_tail']
        with self.assertRaises(ValueError):
            probe(bad, o.result)

    def test_far_calls_plus_absolute_word_without_data_is_mixed(self):
        # Without the absolute word the far-only object is not a mixed object.
        from binder import bind_mixed_far_data
        with self.assertRaisesRegex(ValueError, 'far CALL/data'):
            bind_mixed_far_data(_FarOnly(), 'T', '_f', 5, _FarOnly.linker_fixups, _FarOnly.declarations,
                                {'_g': {'kind': 'far-code', 'frame_load_address': 0, 'load_address': 0}},
                                0, [{'segment': 0, 'offset': 3, 'load_offset': 3}])


class _FarOnly:
    linker_fixups = [{'segment': 'T', 'offset': 1, 'width': 4, 'loc': 'pointer32', 'self_relative': False,
                      'target_kind': 'external', 'target': '_g', 'displacement': 0, 'frame_method': 5,
                      'frame_index': 0, 'target_method': 2, 'target_index': 2, 'frame_kind': 'target',
                      'frame': '_g', 'encoded_addend': '00000000'}]
    segment_defs, groups = [], [{'name': 'DGROUP', 'index': 1, 'segment_indices': [], 'segments': []}]
    publics, externals = [{'name': '_f', 'segment': 'T', 'offset': 0}], ['__acrtused', '_g']
    segment_lengths = {'T': 5}
    declarations = {'segments': [], 'groups': groups, 'publics': publics, 'externals': externals}

    def segment_length(self, name):
        return 5

    def segment_bytes(self, name):
        return bytes.fromhex('9a00000000')


# ------------------------------------------- items 6/10: TU-owned C _DATA
class TuOwnedData(unittest.TestCase):
    def test_whole_object_group_owns_its_string_literal_pool(self):
        o = Oracle.load()
        recipe = read_json(FIXTURES/'seg034_shape2d_group.json')
        payload, receipt = probe(recipe, o.result)
        spec = recipe['secondary_dgroup_segments']['_DATA']
        pool = bytes.fromhex(receipt['binding']['secondary_payloads']['_DATA'])
        self.assertEqual(pool, o.image[spec['start']:spec['end']])
        for delta in (2, -2):
            with self.subTest(delta=delta):
                bad = copy.deepcopy(recipe)
                bad['secondary_dgroup_segments']['_DATA']['start'] += delta
                bad['secondary_dgroup_segments']['_DATA']['end'] += delta
                bad['secondary_dgroup_segments']['_DATA']['dgroup_offset'] += delta
                with self.assertRaises(ValueError):
                    probe(bad, o.result)

    def test_msc_data_alignment_holes_need_the_reviewed_sparse_policy(self):
        from object_probe import recipe_sparse_zero
        source = (b'char s[] = "ab";\nint b = 1;\nchar t[] = "xyz";\nint c = 2;\n'
                  b'int f(void) { return b + c + s[0] + t[0]; }\n')
        from compiler import CompileFailure
        with self.assertRaisesRegex(CompileFailure, 'Holes'):
            compile_source(source, 'msc510-medium')
        recipe = {'kind': 'c', 'object_segment': 'UNIT_TEXT', 'start': 0, 'end': 10,
                  'secondary_dgroup_segments': {'_DATA': {'start': 100, 'end': 112}},
                  'sparse_zero': {'_DATA': {'initialized_ranges': [[0, 3], [4, 12]],
                                            'declared_length': 12}}}
        policy = recipe_sparse_zero(recipe)
        obj, _ = compile_source(source, 'msc510-medium', sparse_zero=policy)
        self.assertEqual(obj.segment_bytes('_DATA')[3], 0)
        for ranges in ([[0, 2], [4, 12]], [[0, 3], [5, 12]], [[0, 3], [3, 12]], [[1, 3], [4, 12]]):
            with self.subTest(ranges=ranges):
                bad = copy.deepcopy(recipe)
                bad['sparse_zero']['_DATA']['initialized_ranges'] = ranges
                with self.assertRaisesRegex(ValueError, 'word-alignment holes'):
                    recipe_sparse_zero(bad)
        bad = copy.deepcopy(recipe)
        bad['sparse_zero'] = {'_BSS': recipe['sparse_zero']['_DATA']}
        with self.assertRaises(ValueError):
            recipe_sparse_zero(bad)
        bad = copy.deepcopy(recipe)
        bad['kind'] = 'asm'
        with self.assertRaises(ValueError):
            recipe_sparse_zero(bad)
        bad = copy.deepcopy(recipe)
        bad['sparse_zero'] = {'UNIT_TEXT': {'initialized_prefix': 8, 'declared_length': 10}}
        with self.assertRaisesRegex(ValueError, 'object tail'):
            recipe_sparse_zero(bad)
        # Item 20: an unowned TU _DATA is readable only for a record-closed prefix.
        unowned = copy.deepcopy(recipe)
        del unowned['secondary_dgroup_segments']
        with self.assertRaises(ValueError):
            recipe_sparse_zero(unowned)
        unowned['prefix_of_object'] = {}
        self.assertEqual(recipe_sparse_zero(unowned), recipe['sparse_zero'])

    def test_research_compiles_derive_only_msc_alignment_holes(self):
        from pathlib import Path as P
        from compiler import CompileFailure
        from object_probe import msc_alignment_sparse_zero
        source = (b'char s[] = "ab";\nint b = 1;\nchar t[] = "xyz";\nint c = 2;\n'
                  b'int f(void) { return b + c + s[0] + t[0]; }\n')
        try:
            compile_source(source, 'msc510-medium')
            self.fail('holes were not reported')
        except CompileFailure as error:
            data = (P(error.receipt['work_directory'])/'UNIT.OBJ').read_bytes()
        self.assertEqual(msc_alignment_sparse_zero(data),
                         {'_DATA': {'initialized_ranges': [[0, 3], [4, 12]], 'declared_length': 12}})
        clean, _ = compile_source(b'int b = 1;\nint f(void) { return b; }\n', 'msc510-medium')
        self.assertIsNone(msc_alignment_sparse_zero(clean.omf_bytes))
        from prefix_proof import compile_candidate
        obj, receipt, _, _ = compile_candidate(source, flags=None)
        self.assertEqual(receipt['sparse_zero']['_DATA']['initialized_ranges'], [[0, 3], [4, 12]])


# ------------------------------------------------ items 4/5/11: aliases
class ReviewedAliases(unittest.TestCase):

    def test_module_entry_alias_needs_its_accepted_module_public(self):
        import code_symbols
        from unittest.mock import patch
        o = Oracle.load()
        layout = read_json(ROOT/'layout/code-symbols.json')
        original = code_symbols.read_json

        def check(mutate, message):
            bad = copy.deepcopy(layout)
            mutate(bad['symbols']['_mat_invert'])
            with patch.object(code_symbols, 'read_json',
                              side_effect=lambda p: bad if str(p).endswith('code-symbols.json') else original(p)):
                with self.assertRaisesRegex(ValueError, message):
                    code_symbols.resolve_code_symbols(['_mat_invert'], o.image, o.relocations)
        check(lambda s: s['module_entry'].update(owner='asm012_137138'), 'Module-entry alias')
        check(lambda s: s['mapped_target'].update(start=141940), 'Module-entry alias')
        check(lambda s: s.update(frame_load_address=125488), 'Module-entry alias')
        check(lambda s: s['anchors'][0].update(site=s['anchors'][0]['site'] + 1), 'anchor')

    def test_far_data_public_binds_offset_and_relocated_segment_word(self):
        from data_only import resolve_far_data_symbols
        from binder import bind_mixed_far_data
        o = Oracle.load()
        far = resolve_far_data_symbols({'_plan_memres'}, o.image, o.relocations)['_plan_memres']
        self.assertEqual((far['frame_load_address'], far['load_address'], far['width']),
                         (176576, 176576, 34))
        with self.assertRaisesRegex(ValueError, 'far-data module public'):
            resolve_far_data_symbols({'_planptr'}, o.image, o.relocations)
        source = (b'struct P { int a[17]; };\nextern struct P far plan_memres[];\n'
                  b'extern struct P far *planptr;\nextern void far h(void);\n'
                  b'void far g(void) { planptr = plan_memres; h(); }\n')
        obj, _ = compile_source(source, 'msc510-medium')
        seg = 'UNIT_TEXT'
        declarations = {'segments': obj.segment_defs, 'groups': obj.groups,
                        'publics': obj.publics, 'externals': obj.externals}
        symbols = {'_plan_memres': far,
                   '_planptr': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                                'load_address': 0x2b770 + 0x81b4, 'allowed_addends': [0, 1, 2, 3]},
                   '_h': {'kind': 'far-code', 'frame_load_address': 125472, 'load_address': 141938}}
        start = 1000
        sites = [start + f['offset'] + (2 if f['loc'] == 'pointer32' else 0)
                 for f in obj.linker_fixups if f['loc'] in ('pointer32', 'base16')]
        relocations = [{'segment': 0, 'offset': s, 'load_offset': s} for s in sites]
        payload, receipt = bind_mixed_far_data(obj, seg, '_g', obj.segment_length(seg), obj.linker_fixups,
                                               declarations, symbols, start, relocations)
        words = {row['kind']: row['linked_value'] for row in receipt['fixups']
                 if row['kind'].startswith('far-data')}
        self.assertEqual(words['far-data-offset16'], 0)
        self.assertEqual(words['far-data-base16'], 176576 // 16)
        narrow = dict(far, width=0)
        with self.assertRaisesRegex(ValueError, 'far-data'):
            bind_mixed_far_data(obj, seg, '_g', obj.segment_length(seg), obj.linker_fixups,
                                declarations, {**symbols, '_plan_memres': narrow}, start, relocations)


# ------------------------------------------------ item 17: segment frame anchor
class SegmentFrameAnchor(unittest.TestCase):
    def test_seg037_frame_is_qualified_by_its_verified_caller(self):
        import function_evidence
        o = Oracle.load()
        rows = [f for f in function_evidence.current_inventory(o.image)['functions']
                if f.get('segment') == 'seg037']
        self.assertTrue(rows)
        self.assertTrue(all(f['segment_paragraph'] == 0x2B12 for f in rows))
        saved = copy.deepcopy(function_evidence.SEGMENT_FRAME_ANCHORS)
        try:
            for field, value in (('site', 175038), ('relocation_load_offset', 175041),
                                 ('hex', '9a0c00122b'), ('caller', 'parse_shape2d')):
                with self.subTest(field=field):
                    function_evidence.SEGMENT_FRAME_ANCHORS['seg037'] = dict(saved['seg037'], **{field: value})
                    with self.assertRaisesRegex(ValueError, 'Segment frame anchor'):
                        function_evidence.current_inventory(o.image)
        finally:
            function_evidence.SEGMENT_FRAME_ANCHORS.clear()
            function_evidence.SEGMENT_FRAME_ANCHORS.update(saved)


# ------------------------------------------------ item 8: diagnostic classifier


# ------------------------------------------- item 19: C switch-table groups
class SwitchTableGroup(unittest.TestCase):
    def locate(self):
        manifest = read_json(ROOT/'layout/manifest.json')
        rows = [o for o in manifest['owners'] if o['id'] == 'obj_seg006']
        if not rows:
            self.fail('obj_seg006 has no tracked accepted recipe')
        return read_json(ROOT/rows[0]['recipe'])

    def test_c_group_switch_tables_bind_at_the_members_code_frame(self):
        from multi_contribution import bind_multi
        o = Oracle.load()
        recipe = self.locate()
        from object_probe import recipe_sparse_zero
        from communal_unit import recipe_declarations, check_object_communals
        declarations = recipe_declarations(recipe)
        communals = None if declarations is None else [name for name, _ in declarations]
        # integ31: seg006 owns its complete _DATA (MSC alignment hole policy).
        obj, _ = compile_source((ROOT/recipe['source']).read_bytes(), recipe['profile'],
                                sparse_zero=recipe_sparse_zero(recipe), communals=communals)
        check_object_communals(obj, recipe)
        self.assertTrue(any(f['target_kind'] == 'segment' and f['loc'] == 'offset16'
                            for f in obj.linker_fixups))
        payload, _ = bind_multi(obj, recipe, o.image, o.relocations)
        self.assertEqual(payload, o.image[recipe['start']:recipe['end']])
        for frame in (recipe['original_frame_load_address'] + 16, None):
            with self.subTest(frame=frame):
                bad = copy.deepcopy(recipe)
                bad['original_frame_load_address'] = frame
                with self.assertRaisesRegex(ValueError, 'switch-table group frame'):
                    bind_multi(obj, bad, o.image, o.relocations)
        bad = copy.deepcopy(recipe)
        rows = bad['expected_relocations']
        rows[0], rows[1] = rows[1], rows[0]
        with self.assertRaises(ValueError):
            bind_multi(obj, bad, o.image, o.relocations)


if __name__ == '__main__':
    unittest.main()
