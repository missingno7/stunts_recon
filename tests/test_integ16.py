"""Strict MASM frame, local-table, and self-base binding controls."""
import copy
import re
import sys
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from assembler import assemble_source
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from common import read_json
from function_evidence import current_inventory
from multi_contribution import bind_multi
from mz import MZ
from oracle import verify
from probe_module import probe
from promote import _checked_prerender_table_boundary, checked_asm_function

DATA_ALIASES = {'_word_303BA': '_projection_x_scale',
                '_word_303BC': '_projection_y_scale'}


def _rename(value):
    if isinstance(value, dict):
        return {DATA_ALIASES.get(key, key): _rename(item) for key, item in value.items()}
    if isinstance(value, list):
        return [_rename(item) for item in value]
    return DATA_ALIASES.get(value, value) if isinstance(value, str) else value


def _rename_source(source):
    text = source.decode('latin-1')
    for old, new in DATA_ALIASES.items():
        text = re.sub(r'(?<![A-Za-z0-9_$?@])' + re.escape(old) +
                      r'(?![A-Za-z0-9_$?@])', new, text)
    return text.encode('latin-1')


class Integ16Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle=verify(write=False)
        cls.image=MZ.parse(cls.oracle[1]).load_image(cls.oracle[1])
        cls.relocations=cls.oracle[2]['unpacked_mz']['relocations']
        cls.work=ROOT/'build/workers/integ16'
        cls.cases={}
        for key in ('projectiondata9_f1','projectiondata9_f2','preRender_helper2',
                    'audio_add_driver_timer','audio_remove_driver_timer'):
            recipe=read_json(cls.work/(key+'.recipe.json'))
            source=_rename_source((ROOT/recipe['source']).read_bytes())
            recipe=_rename(recipe)
            obj,_=assemble_source(source,recipe['profile'])
            expected=recipe['expected_fixups']
            actual=obj.linker_fixups
            sites=lambda row:(row['segment'],row['offset'],row['width'],row['loc'])
            if len(expected)!=len(actual) or [sites(x) for x in expected]!=[sites(x) for x in actual]:
                raise AssertionError('Renamed projection fixture changed ordered FIXUPP sites')
            index_fields={'frame_index','target_index'}
            for before,after in zip(expected,actual):
                differences={k for k in set(before)|set(after)
                             if before.get(k)!=after.get(k)}
                if differences-index_fields:
                    raise AssertionError('Renamed projection fixture changed non-index FIXUPP data')
            recipe['expected_fixups']=actual
            recipe['object_declarations']={'segments':obj.segment_defs,'groups':obj.groups,
                'publics':obj.publics,'externals':obj.externals}
            cls.cases[key]=(obj,recipe)

    def bind(self,key,obj=None,recipe=None,symbols=None):
        saved_obj,saved_recipe=self.cases[key]
        obj=obj or saved_obj
        recipe=recipe or saved_recipe
        if symbols is None:
            symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        return bind_contribution(obj,recipe,symbols)

    # The two self-base timer modules emit ascending MASM FIXUPPs while the
    # oracle holds them descending; the pinned LINK keeps FIXUPP order
    # (tests/test_link_order.py), so their order is not reproduced (integ25).
    ORDER_REJECTED=('audio_add_driver_timer','audio_remove_driver_timer')

    def test_all_five_complete_objects_match_oracle(self):
        for key,(obj,recipe) in self.cases.items():
            with self.subTest(key=key):
                if key in self.ORDER_REJECTED:
                    with self.assertRaisesRegex(ValueError,'order differs from its candidate FIXUPP order'):
                        self.bind(key)
                    continue
                payload,receipt=self.bind(key)
                self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
                self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])

    def test_dgroup_group_frame_must_name_dgroup(self):
        obj,recipe=self.cases['projectiondata9_f1']
        wrong=copy.deepcopy(obj)
        wrong.linker_fixups[0]['frame']='OTHER'
        bad=copy.deepcopy(recipe)
        bad['expected_fixups']=wrong.linker_fixups
        with self.assertRaises(ValueError):
            self.bind('projectiondata9_f1',wrong,bad)

    def test_dgroup_external_frame_must_resolve_inside_same_dgroup(self):
        obj,recipe=self.cases['projectiondata9_f2']
        symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        bad=copy.deepcopy(symbols)
        bad['_projection_x_scale']['group']='OTHER'
        with self.assertRaises(ValueError):
            self.bind('projectiondata9_f2',symbols=bad)
        wrong=copy.deepcopy(obj)
        wrong.externals.append('_other_frame')
        wrong.linker_fixups[0].update(frame='_other_frame',frame_index=2)
        bad_recipe=copy.deepcopy(recipe)
        bad_recipe['expected_fixups']=wrong.linker_fixups
        bad_recipe['binding']['declarations']['externals']=wrong.externals
        bad_symbols={**symbols,'_other_frame':{**symbols['_projection_x_scale'],
                     'frame_load_address':symbols['_projection_x_scale']['frame_load_address']+16}}
        with self.assertRaises(ValueError):
            self.bind('projectiondata9_f2',wrong,bad_recipe,bad_symbols)

    def test_dgroup_addend_cannot_escape_reviewed_object(self):
        obj,recipe=self.cases['projectiondata9_f2']
        wrong=copy.deepcopy(obj)
        wrong.linker_fixups[0]['encoded_addend']='0200'
        wrong.segments['_TEXT']=wrong.segments['_TEXT'][:4]+b'\x02\x00'+wrong.segments['_TEXT'][6:]
        bad=copy.deepcopy(recipe)
        bad['expected_fixups']=wrong.linker_fixups
        with self.assertRaises(ValueError):
            self.bind('projectiondata9_f2',wrong,bad)

    def test_local_table_target_must_stay_inside_complete_module(self):
        obj,recipe=self.cases['preRender_helper2']
        wrong=copy.deepcopy(obj)
        wrong.linker_fixups[0]['displacement']=recipe['end']-recipe['start']
        bad=copy.deepcopy(recipe)
        bad['expected_fixups']=wrong.linker_fixups
        with self.assertRaises(ValueError):
            self.bind('preRender_helper2',wrong,bad)
        bad=copy.deepcopy(recipe)
        bad['end']-=1
        with self.assertRaises(ValueError):
            self.bind('preRender_helper2',recipe=bad)

    def test_local_table_group_uses_whole_module_extent(self):
        obj,recipe=self.cases['preRender_helper2']
        obj=copy.deepcopy(obj)
        recipe=copy.deepcopy(recipe)
        obj.publics.append({'name':'_second','segment':'_TEXT','offset':60})
        recipe['object_declarations']['publics']=copy.deepcopy(obj.publics)
        recipe['members']=[
            {'name':'first','public':'_preRender_helper2',
             'start':recipe['start'],'end':recipe['start']+60},
            {'name':'second','public':'_second',
             'start':recipe['start']+60,'end':recipe['end']}]
        recipe['external_binding']=recipe.pop('binding')
        payload,_=bind_multi(obj,recipe,self.image,self.relocations)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        obj.linker_fixups[0]['displacement']=recipe['end']-recipe['start']
        recipe['expected_fixups']=obj.linker_fixups
        with self.assertRaises(ValueError):
            bind_multi(obj,recipe,self.image,self.relocations)

    def test_external_frame_index_rebased_inside_complete_group(self):
        obj,recipe=self.cases['projectiondata9_f2']
        obj=copy.deepcopy(obj)
        recipe=copy.deepcopy(recipe)
        obj.publics.append({'name':'_second','segment':'_TEXT','offset':7})
        obj.externals.insert(0,'_second')
        obj.linker_fixups[0]['target_index']=2
        obj.linker_fixups[0]['frame_index']=2
        recipe['object_declarations']={'segments':obj.segment_defs,'groups':obj.groups,
             'publics':obj.publics,'externals':obj.externals}
        recipe['expected_fixups']=obj.linker_fixups
        recipe['members']=[
            {'name':'first','public':'_projectiondata9_times_ratio',
             'start':recipe['start'],'end':recipe['start']+7},
            {'name':'second','public':'_second',
             'start':recipe['start']+7,'end':recipe['end']}]
        recipe['external_binding']=recipe.pop('binding')
        recipe['original_frame_load_address']=125472
        payload,_=bind_multi(obj,recipe,self.image,self.relocations)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])

    def test_self_base_needs_own_segment_and_exact_relocation_order(self):
        obj,recipe=self.cases['audio_remove_driver_timer']
        wrong=copy.deepcopy(obj)
        own=next(f for f in wrong.linker_fixups if f['loc']=='base16')
        own['target_index']+=1
        bad=copy.deepcopy(recipe)
        bad['expected_fixups']=wrong.linker_fixups
        with self.assertRaises(ValueError):
            self.bind('audio_remove_driver_timer',wrong,bad)
        bad=copy.deepcopy(recipe)
        bad['expected_relocations']=list(reversed(bad['expected_relocations']))
        with self.assertRaises(ValueError):
            probe(bad,self.oracle)
        bad=copy.deepcopy(recipe)
        bad['original_frame_load_address']+=16
        with self.assertRaises(ValueError):
            checked_asm_function('audio_remove_driver_timer',bad,self.image)

    def test_prerender_boundary_requires_pinned_source_and_neighbors(self):
        inventory=current_inventory(self.image)
        f=next(row for row in inventory['functions'] if row['name']=='preRender_helper2')
        self.assertTrue(_checked_prerender_table_boundary(f,inventory,self.image))
        bad=copy.deepcopy(f)
        bad['provenance']['line_end']-=1
        with self.assertRaises(ValueError):
            _checked_prerender_table_boundary(bad,inventory,self.image)
        bad_inv=copy.deepcopy(inventory)
        bad_inv['functions']=[row for row in bad_inv['functions']
                              if row['name']!='preRender_helper3']
        with self.assertRaises(ValueError):
            _checked_prerender_table_boundary(f,bad_inv,self.image)


if __name__=='__main__':
    unittest.main()
