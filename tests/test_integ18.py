"""Complete module composition, local fixups, and ordered relocation controls."""
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
from multi_contribution import bind_multi
from mz import MZ
from oracle import verify

SYMBOL_ALIASES = {'_word_3F87C': '_timer_elapsed_ticks_low',
                  '_word_3F87E': '_timer_elapsed_ticks_high',
                  '_word_4031E': '_line_pattern_bits',
                  '_word_40320': '_prerender_auxiliary_arg'}


def _rename(value):
    if isinstance(value, dict):
        return {SYMBOL_ALIASES.get(key, key): _rename(item) for key, item in value.items()}
    if isinstance(value, list):
        return [_rename(item) for item in value]
    return SYMBOL_ALIASES.get(value, value) if isinstance(value, str) else value


def _rename_source(source):
    text = source.decode('latin-1')
    for old, new in SYMBOL_ALIASES.items():
        text = re.sub(r'(?<![A-Za-z0-9_$?@])' + re.escape(old) +
                      r'(?![A-Za-z0-9_$?@])', new, text)
    return text.encode('latin-1')


class Integ18Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle=verify(write=False)
        cls.image=MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations=oracle[2]['unpacked_mz']['relocations']
        cls.cases={}
        for name,source,recipe_path in (
            ('locate','asm-b/locate_entries.ASM','asm-b/locate_entries.recipe.json'),
            ('sincos','asm-b/sincos.ASM','asm-b/sincos.recipe.json'),
            ('font','asm-b/font_entries.ASM','integ18/font_entries.recipe.json'),
            ('mmgr','asm-b/mmgr_free_entries.ASM','asm-b/mmgr_free_entries.recipe.json'),
            ('self_far','asm-a/sub_2EAD4_group.ASM','integ18/sub_2EAD4_group.recipe.json'),
            ('near_code','asm-a/preRender_unk_group.ASM','integ18/preRender_unk_group.recipe.json'),
        ):
            recipe=_rename(read_json(ROOT/'build/workers'/recipe_path))
            source_bytes=_rename_source((ROOT/'build/workers'/source).read_bytes())
            obj,_=assemble_source(source_bytes,recipe['profile'])
            expected=recipe['expected_fixups']
            actual=obj.linker_fixups
            sites=lambda row:(row['segment'],row['offset'],row['width'],row['loc'])
            if len(expected)!=len(actual) or [sites(x) for x in expected]!=[sites(x) for x in actual]:
                raise AssertionError('Renamed integration fixture changed ordered FIXUPP sites')
            index_fields={'frame_index','target_index'}
            for before,after in zip(expected,actual):
                differences={k for k in set(before)|set(after)
                             if before.get(k)!=after.get(k)}
                if differences-index_fields:
                    raise AssertionError('Renamed integration fixture changed non-index FIXUPP data')
            recipe['expected_fixups']=actual
            recipe['object_declarations']={'segments':obj.segment_defs,'groups':obj.groups,
                'publics':obj.publics,'externals':obj.externals}
            cls.cases[name]=(obj,recipe)

    def bind(self,name,obj=None,recipe=None):
        original_obj,original_recipe=self.cases[name]
        obj=obj or original_obj;recipe=recipe or original_recipe
        return bind_multi(obj,recipe,self.image,self.relocations)

    def test_complete_mixed_and_local_modules(self):
        for name,(obj,recipe) in self.cases.items():
            with self.subTest(name=name):
                payload,receipt=self.bind(name)
                self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
                self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])

    def test_mixed_base_requires_grounded_dseg(self):
        obj,recipe=self.cases['locate']
        bad=copy.deepcopy(recipe)
        bad['expected_fixups'][0]['frame']='OTHER'
        with self.assertRaises(ValueError):self.bind('locate',recipe=bad)
        bad=copy.deepcopy(recipe)
        bad['expected_relocations'][0]['load_offset']+=1
        with self.assertRaises(ValueError):self.bind('locate',recipe=bad)

    def test_local_table_cannot_escape_complete_module(self):
        obj,recipe=self.cases['sincos']
        bad=copy.deepcopy(recipe)
        bad['expected_fixups'][0]['displacement']=recipe['end']-recipe['start']
        with self.assertRaises(ValueError):self.bind('sincos',recipe=bad)
        bad=copy.deepcopy(recipe)
        bad['end']-=1
        with self.assertRaises(ValueError):self.bind('sincos',recipe=bad)

    def test_local_relative_and_far_targets_stay_inside_module(self):
        for name,loc in (('mmgr','offset16'),('self_far','pointer32')):
            obj,recipe=self.cases[name]
            bad=copy.deepcopy(recipe)
            fix=next(f for f in bad['expected_fixups']
                     if f['target_kind']=='segment' and f['loc']==loc)
            fix['displacement']=recipe['end']-recipe['start']
            with self.subTest(name=name),self.assertRaises(ValueError):
                self.bind(name,recipe=bad)

    def test_external_code_offset_needs_exact_reviewed_target(self):
        obj,recipe=self.cases['near_code']
        bad=copy.deepcopy(recipe)
        bad['reviewed_code_offsets'].remove('_preRender_line')
        with self.assertRaises(ValueError):self.bind('near_code',recipe=bad)
        bad=copy.deepcopy(recipe)
        bad['reviewed_near_targets']['_loc_3180A']['source_line']+=1
        with self.assertRaises(ValueError):self.bind('near_code',recipe=bad)

    def test_indexed_data_addend_stays_inside_declared_table(self):
        recipe=read_json(ROOT/'build/workers/asm-a/get_kb_or_joy_flags.recipe.json')
        obj,_=assemble_source((ROOT/recipe['source']).read_bytes(),recipe['profile'])
        symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        payload,_=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        bad_obj=copy.deepcopy(obj);bad_recipe=copy.deepcopy(recipe)
        index=next(i for i,f in enumerate(recipe['expected_fixups'])
                   if f['target']=='kbscancodes' and f['displacement']==9)
        bad_obj.linker_fixups[index]['displacement']=10
        bad_recipe['expected_fixups'][index]['displacement']=10
        with self.assertRaises(ValueError):
            bind_contribution(bad_obj,bad_recipe,symbols)

    def test_self_text_base_plus_external_far_call(self):
        recipe=read_json(ROOT/'build/workers/asm-c/sprite_copy_2_to_1.recipe.json')
        obj,_=assemble_source((ROOT/recipe['source']).read_bytes(),recipe['profile'])
        symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        payload,receipt=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])
        bad=copy.deepcopy(recipe)
        bad['expected_relocations'][0]['load_offset']+=1
        with self.assertRaises(ValueError):bind_contribution(obj,bad,symbols)

    def test_dseg_base_in_mov_bx(self):
        recipe=read_json(ROOT/'build/workers/integ18/kb_int16_handler.recipe.json')
        obj,_=assemble_source((ROOT/recipe['source']).read_bytes(),recipe['profile'])
        symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        payload,receipt=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])
        bad=copy.deepcopy(obj)
        code=bytearray(bad.segment_bytes('_TEXT'))
        code[2]=0x90
        bad.segments['_TEXT']=bytes(code)
        with self.assertRaises(ValueError):bind_contribution(bad,recipe,symbols)

    def test_dseg_base_with_local_relative_jumps(self):
        recipe=read_json(ROOT/'build/workers/integ18/kb_int9_handler.recipe.json')
        obj,_=assemble_source((ROOT/recipe['source']).read_bytes(),recipe['profile'])
        symbols=resolve_recipe_symbols(recipe,self.image,self.relocations)
        payload,receipt=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])
        bad=copy.deepcopy(recipe)
        fix=next(f for f in bad['expected_fixups'] if f['self_relative'])
        fix['displacement']=recipe['end']-recipe['start']
        with self.assertRaises(ValueError):bind_contribution(obj,bad,symbols)

    def test_new_aliases_have_original_anchors(self):
        from code_symbols import resolve_code_symbols
        from data_symbols import resolve_symbols
        code=resolve_code_symbols({'_sprite_putimage_and','_sprite_putimage_or'},
                                  self.image,self.relocations)
        self.assertEqual({v['load_address'] for v in code.values()},
                         {145552,147588})
        data=resolve_symbols({'_word_9374','_word_411e','_byte_188','_aTcomp'},
                             self.image,self.relocations)
        self.assertEqual(data['_word_9374']['load_address'],215780)
        self.assertEqual(data['_word_411e']['load_address'],194702)
        self.assertEqual(data['_byte_188']['load_address'],178424)
        self.assertEqual(data['_aTcomp']['load_address'],190232)


if __name__=='__main__':unittest.main()
