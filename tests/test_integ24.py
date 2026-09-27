"""integ24: per-object flags, whole ASM modules, cross-kind subsumption, MASM
truncation, far-data placement, CS data composition, far-pointer halves,
registry renames, reviewed widths/aliases and tool locations."""
import copy
import sys
import unittest
from contextlib import contextmanager
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json, sha, identity
from oracle import verify
from mz import MZ
import object_flags
from object_flags import recipe_flags, object_control_flags, registered_objects, same_object
from asm_module import (checked_module, checked_cross_kind_link, expected_publics,
                        masm_public)
from function_evidence import current_inventory
from data_symbols import resolve_symbols, resolve_cs_symbols
from code_symbols import resolve_code_symbols
from binder import _cs_operand, bind_far_calls, bind_mixed_far_data
from data_only import bind_far_data
from compiler import compile_source


@contextmanager
def layout_override(module, filename, layout):
    original = read_json
    with patch(module + '.read_json',
               side_effect=lambda path: layout if str(path).endswith(filename) else original(path)):
        yield


class Oracle:
    @classmethod
    def load(cls):
        if not hasattr(Oracle, '_image'):
            result = verify(write=False)
            Oracle._image = MZ.parse(result[1]).load_image(result[1])
            Oracle._relocations = result[2]['unpacked_mz']['relocations']
        return Oracle._image, Oracle._relocations


def owner(owner_id):
    manifest = read_json(ROOT/'layout/manifest.json')
    rows = [o for o in manifest['owners'] if o['id'] == owner_id]
    if rows:
        return rows[0]
    # Once a whole module has reclassified the C owner, rebuild its row from
    # the reclassification record and its retained recipe.
    entries = [r for o in manifest['owners'] for r in o.get('reclassified_owners', [])
               if r['id'] == owner_id]
    if not entries:
        # Subsumed by a later exact whole object: rebuild the row from the
        # retained recipe named by the subsuming owner's recipe.
        subsumer, = [o for o in manifest['owners'] if 'recipe' in o and
                     owner_id in read_json(ROOT/o['recipe']).get('subsumed_owners', [])]
        recipe = read_json(ROOT/'recipes'/(owner_id + '.json'))
        return {'id': owner_id, 'name': owner_id, 'kind': 'MATCHING_C', 'start': recipe['start'],
                'end': recipe['end'], 'classification': 'GAME_C',
                'recipe': 'recipes/' + owner_id + '.json', 'subsumed_by': subsumer['id']}
    entry, = entries
    recipe = read_json(ROOT/entry['recipe'])
    return {'id': owner_id, 'name': entry['name'], 'kind': entry['from'], 'start': recipe['start'],
            'end': recipe['end'], 'classification': 'GAME_C', 'recipe': entry['recipe']}


def pre_promotion(manifest, rows):
    """Reconstruct an earlier ownership window: each row replaces whatever now
    covers it, with explicit raw ownership around it (test fixture only)."""
    result = copy.deepcopy(manifest)
    for row in sorted(rows, key=lambda r: r['start']):
        owners = result['owners']
        overlaps = [o for o in owners if o['start'] < row['end'] and row['start'] < o['end']]
        first = owners.index(overlaps[0])
        pieces = []
        if overlaps[0]['start'] < row['start']:
            pieces.append({'id': f"raw_{overlaps[0]['start']:05x}_{row['start']:05x}",
                           'kind': 'UNRESOLVED_RAW', 'classification': 'UNRESOLVED_MIXED',
                           'start': overlaps[0]['start'], 'end': row['start']})
        pieces.append(row)
        if row['end'] < overlaps[-1]['end']:
            pieces.append({'id': f"raw_{row['end']:05x}_{overlaps[-1]['end']:05x}",
                           'kind': 'UNRESOLVED_RAW', 'classification': 'UNRESOLVED_MIXED',
                           'start': row['end'], 'end': overlaps[-1]['end']})
        owners[first:first + len(overlaps)] = pieces
    return result


def recipe_owner(owner_id, name, kind):
    recipe = read_json(ROOT/'recipes'/(name + '.json'))
    return {'id': owner_id, 'name': name, 'kind': kind, 'start': recipe['start'], 'end': recipe['end'],
            'classification': 'GAME_ASM' if kind == 'MATCHING_ASM' else 'GAME_C',
            'recipe': 'recipes/' + name + '.json'}


class PerObjectFlags(unittest.TestCase):
    WHEEL = {'id':'preRender_wheel_helper', 'start':159424, 'end':159530, 'kind':'c',
             'profile':'msc510-medium', 'compiler_flags':['/AM','/Oa','/Gs'],
             'compiler_flags_register':'TUFLAG-Oa-seg023-025'}

    def test_registered_object_flags_are_accepted(self):
        self.assertEqual(recipe_flags(self.WHEEL), ['/AM','/Oa','/Gs'])
        self.assertEqual(registered_objects()['seg027'], ('TUFLAG-Ox-seg027', ['/AM','/Ox','/Gs']))

    def test_canonical_recipe_uses_profile_flags(self):
        recipe = {k:v for k,v in self.WHEEL.items() if not k.startswith('compiler_flags')}
        self.assertIsNone(recipe_flags(recipe))

    def test_flags_refused_outside_registered_object(self):
        recipe = dict(self.WHEEL, id='copy_string', start=None, end=None)
        functions = read_json(ROOT/'evidence/functions.json')['functions']
        row, = [f for f in functions if f['name'] == 'copy_string']
        recipe.update(start=row['start'], end=row['end'])
        with self.assertRaisesRegex(ValueError, 'SUPPORTED TUFLAG'):
            recipe_flags(recipe)

    def test_flags_must_equal_registered_set_and_register(self):
        with self.assertRaisesRegex(ValueError, 'registered object flag set'):
            recipe_flags(dict(self.WHEEL, compiler_flags=['/AM','/Ox','/Gs']))
        with self.assertRaisesRegex(ValueError, 'registered object flag set'):
            recipe_flags(dict(self.WHEEL, compiler_flags_register='TUFLAG-Ox-seg027'))

    def test_register_status_change_refuses_override(self):
        register = object_flags._register()
        changed = copy.deepcopy(register)
        changed['TUFLAG-Oa-seg023-025']['status'] = 'FALSIFIED'
        with patch.object(object_flags, '_register', return_value=changed):
            with self.assertRaisesRegex(ValueError, 'SUPPORTED TUFLAG'):
                recipe_flags(self.WHEEL)

    def test_flags_apply_to_the_whole_object(self):
        # A canonical-flag contribution in a flagged object must also be
        # reproduced under the object's flag set; others need no control.
        flag2 = read_json(ROOT/owner('audio_flag2_group')['recipe'])
        self.assertEqual(object_control_flags(flag2), ['/AM','/Ox','/Gs'])
        copy_string = read_json(ROOT/'recipes/copy_string.json')
        self.assertIsNone(object_control_flags(copy_string))
        # A group spanning a flagged and an unflagged object is refused.
        spanning = {'id':'x', 'kind':'c', 'members':[
            {'name':'sub_3702E','start':159790,'end':159930},
            {'name':'toupper','start':159930,'end':159954}]}
        with self.assertRaisesRegex(ValueError, 'flagged object boundary'):
            object_control_flags(spanning)

    def test_flag_control_compares_complete_objects(self):
        source = b'int f(int *p, int *q) { *p = 1; *q = 2; return *p; }\n'
        canonical, _ = compile_source(source, 'msc510-medium')
        again, _ = compile_source(source, 'msc510-medium')
        noalias, _ = compile_source(source, 'msc510-medium', ['/AM','/Oa','/Gs'])
        self.assertTrue(same_object(canonical, again))
        self.assertFalse(same_object(canonical, noalias))


class WholeModules(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = Oracle.load()
        cls.inventory = current_inventory(cls.image)

    def module_132056(self, **proof):
        names = ['set_bios_mode3','kb_parse_key','kb_reg_callback','nopsub_304AF','nopsub_304B6',
                 'kb_get_char','get_kb_or_joy_flags','nopsub_305C8','get_joy_flags','sub_307B4',
                 'sub_307D2','sub_307E3','nopsub_307FA']
        rows = {f['name']:f for f in self.inventory['functions'] if f['name'] in names}
        members = [{'name':n, 'public':'_'+n, 'start':rows[n]['start'],
                    'stable_id':rows[n].get('stable_id')} for n in names]
        module_proof = {'kind':'asm-module-extent-v1',
                        'start_boundary':{'kind':'zero-fill-after-return'},
                        'end_boundary':{'kind':'zero-fill-included'},
                        'cross_kind_links':[{'owner':'load_207b4','kind':'shared-asm-frame'}]}
        module_proof.update(proof)
        return {'id':'asm012_132056', 'kind':'asm', 'start':132056, 'end':133138,
                'original_frame_load_address':125472, 'members':members,
                'module_proof':module_proof}

    def test_grounded_module_extent_and_entries(self):
        checked_module(self.module_132056(), self.image)

    def test_module_start_needs_zero_fill_or_frame_change(self):
        recipe = self.module_132056()
        recipe['start'] = recipe['members'][0]['start'] = 132100
        recipe['members'][0].update(name='kb_parse_key', public='_kb_parse_key')
        recipe['members'] = recipe['members'][:1] + recipe['members'][2:]
        with self.assertRaisesRegex(ValueError, 'zero fill'):
            checked_module(recipe, self.image)

    def test_module_end_needs_its_fill(self):
        recipe = self.module_132056(end_boundary={'kind':'zero-fill-after-return'})
        with self.assertRaisesRegex(ValueError, 'following zero fill'):
            checked_module(recipe, self.image)

    def test_entry_must_be_inventory_start(self):
        recipe = self.module_132056()
        recipe['members'][3]['start'] += 1
        with self.assertRaisesRegex(ValueError, 'Module entry differs'):
            checked_module(recipe, self.image)

    def test_module_cannot_cut_a_verified_procedure(self):
        recipe = self.module_132056(end_boundary={'kind':'zero-fill-included'})
        recipe['end'] = 133100
        recipe['members'] = recipe['members'][:-2]
        with self.assertRaisesRegex(ValueError, 'terminal zero fill|crosses the module end'):
            checked_module(recipe, self.image)

    def test_frame_change_boundary(self):
        # seg002 is bounded by functions of other code frames on both sides.
        rows = [f for f in self.inventory['functions'] if f.get('segment') == 'seg002']
        members = [{'name':f['name'], 'public':'_'+f['name'], 'start':f['start'],
                    'stable_id':f.get('stable_id')} for f in sorted(rows, key=lambda f:f['start'])]
        recipe = {'id':'obj_seg002', 'kind':'asm', 'start':40390, 'end':40724,
                  'original_frame_load_address':40384, 'members':members,
                  'module_proof':{'kind':'asm-module-extent-v1',
                      'start_boundary':{'kind':'segment-frame-change','neighbour':'setup_aero_trackdata'},
                      'end_boundary':{'kind':'segment-frame-change','neighbour':'sub_19F14'},
                      'embedded_publics':[{'public':'_byte_19F07','offset':321}]}}
        checked_module(recipe, self.image)
        bad = copy.deepcopy(recipe)
        bad['module_proof']['start_boundary']['neighbour'] = 'polarRadius3D'
        with self.assertRaisesRegex(ValueError, 'another code frame'):
            checked_module(bad, self.image)
        bad = copy.deepcopy(recipe)
        bad['module_proof']['embedded_publics'][0]['offset'] = 0
        with self.assertRaisesRegex(ValueError, 'Embedded module public'):
            checked_module(bad, self.image)

    def test_partial_entry_needs_relocated_callers(self):
        rows = {f['name']:f for f in self.inventory['functions']}
        mat = rows['mat_invert']
        recipe = {'kind':'asm', 'start':141362, 'end':143366, 'original_frame_load_address':125472,
                  'members':[], 'module_proof':{'kind':'asm-module-extent-v1',
                  'start_boundary':{'kind':'zero-fill-after-return'},
                  'end_boundary':{'kind':'zero-fill-included'}}}
        for name in ['font_op','font_op2','preRender_patterned','nopsub_328C9','nopsub_328DB',
                     'mat_mul_vector','mat_multiply','mat_invert','file_unflip_shape2d',
                     'file_decomp_vle','nopsub_32FEE','video_get_status']:
            f = rows[name]
            recipe['members'].append({'name':name, 'public':'_'+name, 'start':f['start'],
                                      'stable_id':f.get('stable_id')})
        anchor = {'site':22546, 'hex':self.image[22546:22551].hex(),
                  'relocation':{'segment':0,'offset':22549,'load_offset':22549}}
        with self.assertRaisesRegex(ValueError, 'inventory-verified entry: mat_invert'):
            checked_module(recipe, self.image)
        recipe['members'][7]['entry_anchors'] = [anchor]
        checked_module(recipe, self.image)
        recipe['members'][7]['entry_anchors'] = [dict(anchor, site=22547)]
        with self.assertRaisesRegex(ValueError, 'anchor differs'):
            checked_module(recipe, self.image)


class CrossKindSubsumption(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = Oracle.load()
        cls.inventory = current_inventory(cls.image)

    def recipe(self, start, end, links, end_kind='zero-fill-included'):
        return {'start':start, 'end':end, 'original_frame_load_address':125472,
                'module_proof':{'kind':'asm-module-extent-v1',
                                'start_boundary':{'kind':'zero-fill-after-return'},
                                'end_boundary':{'kind':end_kind},
                                'cross_kind_links':links}}

    def test_odd_start_link(self):
        c_owner = owner('load_20a55')
        recipe = self.recipe(133660, 133840, [{'owner':'load_20a55','kind':'odd-start'}])
        self.assertEqual(checked_cross_kind_link(c_owner, recipe, self.image, self.inventory)['kind'],
                         'odd-start')

    def test_odd_start_claim_on_even_owner_rejects(self):
        c_owner = owner('load_207b4')
        recipe = self.recipe(132056, 133138, [{'owner':'load_207b4','kind':'odd-start'}])
        with self.assertRaisesRegex(ValueError, 'Odd-start'):
            checked_cross_kind_link(c_owner, recipe, self.image, self.inventory)

    def test_shared_frame_link_and_missing_link(self):
        c_owner = owner('load_207b4')
        recipe = self.recipe(132056, 133138, [{'owner':'load_207b4','kind':'shared-asm-frame'}])
        checked_cross_kind_link(c_owner, recipe, self.image, self.inventory)
        with self.assertRaisesRegex(ValueError, 'same-module link'):
            checked_cross_kind_link(c_owner, self.recipe(132056, 133138, []), self.image, self.inventory)
        # The owner may not sit on the module edge.
        edge = self.recipe(133044, 133138, [{'owner':'load_207b4','kind':'shared-asm-frame'}])
        with self.assertRaisesRegex(ValueError, 'Shared-frame'):
            checked_cross_kind_link(c_owner, edge, self.image, self.inventory)

    def test_short_branch_link_must_cross(self):
        c_owner = owner('load_20a55')
        recipe = self.recipe(133660, 133840, [{'owner':'load_20a55','kind':'short-branch',
                                               'site':133700,'target':133702}])
        with self.assertRaisesRegex(ValueError, 'Short-branch'):
            checked_cross_kind_link(c_owner, recipe, self.image, self.inventory)

    def test_ordinary_group_cannot_subsume_c(self):
        from promote import replace_group
        oracle = verify(write=False)
        manifest = pre_promotion(read_json(ROOT/'layout/manifest.json'), [owner('load_207b4')])
        recipe = {'id':'x', 'kind':'asm', 'start':133044, 'end':133074,
                  'subsumed_owners':['load_207b4'],
                  'members':[{'name':'sub_307B4','start':133044,'end':133074}]}
        with self.assertRaisesRegex(ValueError, 'Group crosses an accepted owner'):
            replace_group(manifest, recipe, oracle)


class MasmTruncation(unittest.TestCase):
    def test_truncated_public_spelling(self):
        self.assertEqual(masm_public('nopsub_kb_get_readchar_callback'), '_nopsub_kb_get_readchar_callbac')
        self.assertIn('_nopsub_kb_get_readchar_callbac',
                      expected_publics('nopsub_kb_get_readchar_callback', 'asm'))
        self.assertEqual(expected_publics('nopsub_kb_get_readchar_callback', 'c'),
                         {'_nopsub_kb_get_readchar_callback'})
        self.assertEqual(expected_publics('kb_check', 'asm'), {'_kb_check'})

    def test_truncated_code_alias_needs_review(self):
        image, relocations = Oracle.load()
        layout = read_json(ROOT/'layout/code-symbols.json')
        entry = copy.deepcopy(layout['symbols']['_file_load_shape2d_res_nofatal'])
        bad = copy.deepcopy(layout)
        bad['symbols']['_file_load_shape2d_res_nofata'] = entry
        with layout_override('code_symbols', 'code-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'explicit review'):
                resolve_code_symbols({'_file_load_shape2d_res_nofata'}, image, relocations)
        long_name = '_' + 'x'*40
        target = dict(entry['mapped_target'], name='x'*40)
        good = copy.deepcopy(layout)
        good['symbols'][long_name[:31]] = dict(entry, mapped_target=target,
                                               masm_truncated_public=True)
        with layout_override('code_symbols', 'code-symbols.json', good):
            # The spelling rule passes; the unknown inventory name then fails.
            with self.assertRaisesRegex(ValueError, 'unique verified mapped target'):
                resolve_code_symbols({long_name[:31]}, image, relocations)


class FarData(unittest.TestCase):
    SOURCE = (b'struct VECTOR { int x, y, z; };\nstruct MATRIX { int m[9]; };\n'
              b'struct PLANE { int plane_yz; int plane_xy; struct VECTOR plane_origin;\n'
              b'    struct VECTOR plane_normal; struct MATRIX plane_rotation; };\n'
              b'struct PLANE far plan_memres = { 0, 0, { 0, 0, 0 }, { 0, 8192, 0 } };\n'
              b'int far unk_3B1E2[7] = { 0 };\n')

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = Oracle.load()
        cls.obj, _ = compile_source(cls.SOURCE, 'msc510-medium')

    def recipe(self, **changes):
        obj = self.obj
        recipe = {'id':'fardata_11036', 'data_only':True, 'far_data':True,
                  'start':176576, 'end':176624, 'target':identity(self.image[176576:176624]),
                  'object_segment':'UNIT5_DATA', 'expected_fixups':[], 'expected_relocations':[],
                  'required_pointer_sites':[],
                  'object_declarations':{'segments':obj.segment_defs,'groups':obj.groups,
                                         'publics':obj.publics,'externals':obj.externals},
                  'public_object_sizes':{'_plan_memres':34,'_unk_3B1E2':14},
                  'far_placement':{'basis':'relocated-segment-word-v1','anchors':[
                      {'kind':'code-immediate','site':37888,'hex':'c706549d1c2b',
                       'relocation':{'segment':0,'offset':37892,'load_offset':37892}}],
                      'offset_anchors':[{'site':37882,'hex':'c706529d0000','operand_offset':4,
                                         'public':'_plan_memres'}]}}
        recipe.update(changes)
        return recipe

    def test_far_segment_placed_by_relocated_segment_word(self):
        payload, binding = bind_far_data(self.obj, self.recipe(), self.image, self.relocations)
        self.assertEqual(payload, self.image[176576:176624])
        self.assertEqual(binding['mode'], 'data-only-far-segment-v1')

    def test_paragraph_must_match_start(self):
        recipe = self.recipe(start=176592, end=176640, target=identity(self.image[176592:176640]))
        with self.assertRaisesRegex(ValueError, 'relocated paragraph word'):
            bind_far_data(self.obj, recipe, self.image, self.relocations)

    def test_anchor_must_be_relocated_immediate_in_verified_code(self):
        bad = self.recipe()
        bad['far_placement']['anchors'][0]['relocation'] = {'segment':0,'offset':37890,'load_offset':37890}
        with self.assertRaisesRegex(ValueError, 'relocated paragraph word'):
            bind_far_data(self.obj, bad, self.image, self.relocations)
        missing = self.recipe()
        missing['far_placement'] = {'basis':'relocated-segment-word-v1','anchors':[]}
        with self.assertRaisesRegex(ValueError, 'placement anchor'):
            bind_far_data(self.obj, missing, self.image, self.relocations)

    def test_object_extents_and_offset_anchor(self):
        with self.assertRaisesRegex(ValueError, 'cover full segment'):
            bind_far_data(self.obj, self.recipe(public_object_sizes={'_plan_memres':30,'_unk_3B1E2':14}),
                          self.image, self.relocations)
        bad = self.recipe()
        bad['far_placement']['offset_anchors'][0]['public'] = '_unk_3B1E2'
        with self.assertRaisesRegex(ValueError, 'offset anchor'):
            bind_far_data(self.obj, bad, self.image, self.relocations)

    def test_dgroup_data_is_not_far_data(self):
        obj, _ = compile_source(b'int near_table[24] = { 1 };\n', 'msc510-medium')
        recipe = self.recipe(object_segment='_DATA')
        with self.assertRaisesRegex(ValueError, 'Far-data module segment'):
            bind_far_data(obj, recipe, self.image, self.relocations)


class CsDataComposition(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = Oracle.load()

    def test_reviewed_cs_operand_forms(self):
        def at(prefix):
            return bytes.fromhex(prefix) + b'\0\0', len(bytes.fromhex(prefix))
        for form in ('2e8e06', '2e0306', '2ef73e', '2e8b1e', '2ea1', '8d36'):
            payload, position = at(form)
            self.assertTrue(_cs_operand(payload, position), form)
        for form in ('2ef736', '2e8e26', '8e06', '2e8bc0'):
            payload, position = at(form)
            self.assertFalse(_cs_operand(payload, position), form)

    def test_generic_code_island_resolves_and_rechecks(self):
        resolved = resolve_cs_symbols({'_incnums'}, self.image, self.relocations)['_incnums']
        self.assertEqual((resolved['load_address'], resolved['width']), (154824, 256))
        layout = read_json(ROOT/'layout/data-symbols.json')
        bad = copy.deepcopy(layout); bad['symbols']['_incnums']['width'] = 257
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'width differs'):
                resolve_cs_symbols({'_incnums'}, self.image, self.relocations)
        bad = copy.deepcopy(layout); bad['symbols']['_incnums']['offset_anchor']['site'] = 148851
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaises(ValueError):
                resolve_cs_symbols({'_incnums'}, self.image, self.relocations)

    def test_cs_pointer_pair_composes_with_far_calls_and_dgroup(self):
        from code_symbols import resolve_recipe_symbols
        def fix(offset, loc, target, width=2):
            return {'offset':offset, 'loc':loc, 'target':target, 'width':width,
                    'target_kind':'external', 'self_relative':False, 'displacement':0,
                    'frame_method':5, 'frame_kind':'target', 'frame':target, 'frame_index':0,
                    'target_method':2, 'encoded_addend':'00'*width}
        recipe = {'kind':'c', 'object_segment':'UNIT_TEXT',
                  'binding':{'mode':'external-far-call-dgroup-offset16-v1'},
                  'expected_fixups':[fix(1,'offset16','_mcgawndsprite'), fix(37,'offset16','_sprite2'),
                                     fix(40,'base16','_sprite2'),
                                     fix(45,'pointer32','_sprite_set_1_from_argptr',4)]}
        symbols = resolve_recipe_symbols(recipe, self.image, self.relocations)
        self.assertEqual(symbols['_sprite2']['kind'], 'cs-data')
        self.assertEqual(symbols['_mcgawndsprite']['group'], 'DGROUP')
        self.assertEqual(symbols['_mcgawndsprite']['allowed_addends'], [0, 1, 2, 3])
        self.assertEqual(symbols['_sprite_set_1_from_argptr']['kind'], 'far-code')
        bad = dict(recipe, expected_fixups=recipe['expected_fixups']+[fix(50,'pointer32','_sprite2',4)])
        with self.assertRaisesRegex(ValueError, 'CS data fixup form'):
            resolve_recipe_symbols(bad, self.image, self.relocations)

    def test_cs_pair_must_be_adjacent(self):
        obj = type('Obj', (), {})()
        payload = bytearray(bytes.fromhex('b80000ba00009a00000000'))
        segdefs = [{'index':1,'name':'UNIT_TEXT','class':'CODE','length':len(payload)}]
        obj.linker_fixups = [
            {'segment':'UNIT_TEXT','offset':1,'loc':'offset16','width':2,'self_relative':False,
             'target_kind':'external','target':'_sprite2','target_method':2,'target_index':1,
             'frame_method':5,'frame_kind':'target','frame':'_sprite2','frame_index':0,
             'displacement':0,'encoded_addend':'0000'},
            {'segment':'UNIT_TEXT','offset':7,'loc':'pointer32','width':4,'self_relative':False,
             'target_kind':'external','target':'_f','target_method':2,'target_index':2,
             'frame_method':5,'frame_kind':'target','frame':'_f','frame_index':0,
             'displacement':0,'encoded_addend':'00000000'}]
        obj.segment_defs = segdefs; obj.groups = [{'index':1,'name':'DGROUP'}]
        obj.publics = [{'name':'_p','segment':'UNIT_TEXT','offset':0}]
        obj.externals = ['_sprite2','_f']
        obj.segment_lengths = {'UNIT_TEXT':len(payload)}
        obj.segment_length = lambda name: obj.segment_lengths[name]
        obj.segment_bytes = lambda name: bytes(payload)
        symbols = {'_sprite2':{'kind':'cs-data','frame_load_address':125472,'load_address':149854,
                               'width':30,'island_start':149824,'island_end':149884},
                   '_f':{'kind':'far-code','frame_load_address':125472,'load_address':125482}}
        declarations = {'segments':segdefs,'groups':obj.groups,'publics':obj.publics,
                        'externals':obj.externals}
        with self.assertRaisesRegex(ValueError, 'adjacent MOV pairs'):
            bind_mixed_far_data(obj, 'UNIT_TEXT', '_p', len(payload), obj.linker_fixups,
                                declarations, symbols, 0, [{'segment':0,'offset':9,'load_offset':9}])


class FarTransfers(unittest.TestCase):
    def fixture(self, opcode):
        obj = type('Obj', (), {})()
        payload = bytes([opcode]) + bytes(4)
        segdefs = [{'index':1,'name':'_TEXT','class':'CODE','length':5}]
        obj.linker_fixups = [{'segment':'_TEXT','offset':1,'loc':'pointer32','width':4,
                              'self_relative':False,'target_kind':'external','target':'_t',
                              'target_method':2,'target_index':1,'frame_method':2,
                              'frame_kind':'external','frame':'_t','frame_index':1,
                              'displacement':0,'encoded_addend':'00000000'}]
        obj.segment_defs = segdefs; obj.groups = []
        obj.publics = [{'name':'_thunk','segment':'_TEXT','offset':0}]
        obj.externals = ['_t']
        obj.segment_lengths = {'_TEXT':5}
        obj.segment_length = lambda name: 5
        obj.segment_bytes = lambda name: payload
        declarations = {'segments':segdefs,'groups':[],'publics':obj.publics,'externals':['_t']}
        symbols = {'_t':{'kind':'far-code','frame_load_address':174544,'load_address':174588}}
        return obj, declarations, symbols

    def test_external_far_jmp_thunk_binds(self):
        obj, declarations, symbols = self.fixture(0xea)
        payload, _ = bind_far_calls(obj, '_TEXT', '_thunk', 5, obj.linker_fixups, declarations,
                                    symbols, 145510, [{'segment':8192,'offset':14441,'load_offset':145513}],
                                    asm_frame=125472)
        self.assertEqual(payload, bytes.fromhex('ea2c009d2a'))

    def test_non_transfer_opcode_rejects(self):
        obj, declarations, symbols = self.fixture(0xe8)
        with self.assertRaisesRegex(ValueError, 'CALL/JMP operands only'):
            bind_far_calls(obj, '_TEXT', '_thunk', 5, obj.linker_fixups, declarations, symbols,
                           145510, [{'segment':8192,'offset':14441,'load_offset':145513}],
                           asm_frame=125472)

    def test_far_jmp_anchor_grounds_code_alias(self):
        image, relocations = Oracle.load()
        resolved = resolve_code_symbols({'_parse_shape2d'}, image, relocations)['_parse_shape2d']
        self.assertEqual(resolved['load_address'], 175516)

    def test_neighbour_bounded_alias_needs_review(self):
        image, relocations = Oracle.load()
        resolve_code_symbols({'_putpixel_line1_maybe'}, image, relocations)
        layout = read_json(ROOT/'layout/code-symbols.json')
        bad = copy.deepcopy(layout)
        bad['symbols']['_putpixel_line1_maybe'].pop('boundary_proof')
        with layout_override('code_symbols', 'code-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'unique verified mapped target'):
                resolve_code_symbols({'_putpixel_line1_maybe'}, image, relocations)


class DataRegistry(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = Oracle.load()
        cls.layout = read_json(ROOT/'layout/data-symbols.json')

    def test_far_pointer_halves_are_in_object(self):
        resolved = resolve_symbols(['_td14_elem_map_main', '_wallptr', '_timer_callback_counter'],
                                   self.image, self.relocations)
        for name in resolved:
            self.assertEqual(resolved[name]['allowed_addends'], [0, 1, 2, 3])

    def test_unwidened_alias_keeps_address_only(self):
        self.assertEqual(resolve_symbols(['_word_9260'], self.image, self.relocations)
                         ['_word_9260']['allowed_addends'], [0])

    def test_reviewed_widths(self):
        resolved = resolve_symbols(['_timerintr', '_terrainrows', '_g_kevinrandom_seed',
                                    '_old_intr0_handler', '_mcgawndsprite', '_atantable'],
                                   self.image, self.relocations)
        widths = {n: max(r['allowed_addends'])+1 for n, r in resolved.items()}
        self.assertEqual(widths, {'_timerintr':28, '_terrainrows':60, '_g_kevinrandom_seed':6,
                                  '_old_intr0_handler':4, '_mcgawndsprite':4, '_atantable':258})

    def test_no_alias_name_is_placed_elsewhere(self):
        from data_symbols import _reference_label_offsets
        lines = (ROOT/'build/references/restunts/src/restunts/asmorig/dseg.asm').read_text(
            encoding='latin1').splitlines()
        offsets = _reference_label_offsets(lines)
        frame = self.layout['frame_load_address']
        for name, symbol in self.layout['symbols'].items():
            if symbol.get('storage') == 'code_island':
                continue
            label = name[1:] if name.startswith('_') else name
            if label in offsets:
                self.assertEqual(offsets[label][0], symbol['load_address']-frame, name)

    def test_renames_are_recorded_and_rederived(self):
        renamed = {row['old']: row for row in self.layout['renamed_aliases']}
        self.assertEqual(renamed['_td22_row_from_path']['address'], 218326)
        self.assertEqual(self.layout['symbols']['_td22_row_from_path']['load_address'], 219610)
        self.assertEqual(self.layout['symbols']['_trackcenterpos2']['load_address'], 220626)
        resolve_symbols(['_td21_col_from_path', '_td22_row_from_path', '_trackcenterpos2',
                         '_mouse_oldy'], self.image, self.relocations)

    def test_field_anchor_stays_inside_placed_extent(self):
        name = '_aCopyrightCUnlimitedSoftwareIn'
        resolve_symbols([name], self.image, self.relocations)
        bad = copy.deepcopy(self.layout)
        bad['symbols'][name]['references'][0]['field_offset'] = 76
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'outside its placed extent'):
                resolve_symbols([name], self.image, self.relocations)
        bad = copy.deepcopy(self.layout)
        bad['symbols'][name].pop('masm_truncated_public')
        with layout_override('data_symbols', 'data-symbols.json', bad):
            with self.assertRaisesRegex(ValueError, 'label/use differs'):
                resolve_symbols([name], self.image, self.relocations)


class GapEntries(unittest.TestCase):
    def test_reviewed_gap_entries_are_verified(self):
        image, _ = Oracle.load()
        rows = {f['name']: f for f in current_inventory(image)['functions']}
        self.assertEqual((rows['nopsub_36AF2']['start'], rows['nopsub_36AF2']['end']), (158450, 158452))
        self.assertEqual(rows['nopsub_38570']['status'], 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')

    def test_gap_entry_needs_both_verified_neighbours(self):
        image, _ = Oracle.load()
        document = read_json(ROOT/'layout/function-evidence.json')
        for field, value in (('predecessor', 'nopsub_36A9A'), ('successor', 'nopsub_36ACA')):
            bad = copy.deepcopy(document)
            row = next(f for f in bad['functions'] if f['name'] == 'nopsub_36AF2')
            row['gap_entry_proof'][field] = value
            with layout_override('function_evidence', 'function-evidence.json', bad):
                with self.assertRaisesRegex(ValueError, 'verified neighbouring boundaries'):
                    current_inventory(image)


class ModuleReclassification(unittest.TestCase):
    # The historical ownership window re-probes retired pre-integ29 recipes
    # whose code segment is still `_TEXT`; the link-faithful segment rule
    # (tests/test_integ29.py) is orthogonal to the cross-kind link proof.
    def setUp(self):
        from unittest import mock
        patcher = mock.patch('link_frames.check_asm_object', lambda obj, recipe: None)
        patcher.start()
        self.addCleanup(patcher.stop)

    @staticmethod
    def manifest():
        # The ownership window before asm012_133660 was published.
        return pre_promotion(read_json(ROOT/'layout/manifest.json'), [
            recipe_owner('load_20a21', 'kb_read_char', 'MATCHING_ASM'),
            recipe_owner('load_20a35', 'kb_checking', 'MATCHING_ASM'),
            owner('load_20a55'),
            recipe_owner('load_20a68', 'kb_check', 'MATCHING_ASM')])

    def recipe(self, links):
        image, _ = Oracle.load()
        rows = {f['name']: f for f in current_inventory(image)['functions']}
        names = ['kb_call_readchar_callback','kb_read_char','kb_checking',
                 'nopsub_kb_set_readchar_callback','nopsub_kb_get_readchar_callback',
                 'flush_stdin','kb_check','nopsub_30A77','nopsub_30A97']
        members = [{'name':n, 'public':masm_public(n), 'start':rows[n]['start'],
                    'stable_id':rows[n].get('stable_id')} for n in names]
        return {'id':'asm012_133660', 'kind':'asm', 'start':133660, 'end':133840,
                'target':identity(image[133660:133840]), 'original_frame_load_address':125472,
                'members':members,
                'subsumed_owners':['load_20a21','load_20a35','load_20a55','load_20a68'],
                'module_proof':{'kind':'asm-module-extent-v1',
                                'start_boundary':{'kind':'zero-fill-after-return'},
                                'end_boundary':{'kind':'zero-fill-included'},
                                'cross_kind_links':links}}

    def test_module_reclassifies_linked_c_owner(self):
        from promote import replace_group
        oracle = verify(write=False)
        manifest = self.manifest()
        staged = replace_group(manifest, self.recipe([{'owner':'load_20a55','kind':'odd-start'}]), oracle)
        row, = [o for o in staged['owners'] if o['id'] == 'asm012_133660']
        self.assertEqual((row['kind'], row['module_form']), ('MATCHING_ASM', 'asm-module'))
        self.assertEqual([(r['id'], r['from'], r['link']) for r in row['reclassified_owners']],
                         [('load_20a55', 'MATCHING_C', 'odd-start')])
        self.assertFalse(any(o['id'] == 'load_20a55' for o in staged['owners']))

    def test_module_without_link_cannot_reclassify(self):
        from promote import replace_group
        oracle = verify(write=False)
        manifest = self.manifest()
        with self.assertRaisesRegex(ValueError, 'same-module link'):
            replace_group(manifest, self.recipe([]), oracle)


class ToolLocations(unittest.TestCase):
    def test_independent_runner_is_the_pinned_copy(self):
        import crosscheck_runner
        runner = Path(crosscheck_runner.DOSBOX_X)
        self.assertEqual(runner.as_posix(), 'C:/tools/dosbox-x/dosbox-x.exe')
        self.assertTrue(sha(runner.read_bytes()).startswith('b028a4d3'))


if __name__ == '__main__':
    unittest.main()
