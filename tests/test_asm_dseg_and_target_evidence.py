"""Focused MASM segment-frame and near-transfer binding controls."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from assembler import assemble_source
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from common import read_json
from compiler import compile_source
from data_symbols import checked_dseg_base
from multi_contribution import _checked_asm_near_labels
from mz import MZ
from oracle import verify
from secondary_contribution import bind_secondary

DSEG_SOURCE = b""".model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _timer_elapsed_ticks_low:word
extrn _timer_elapsed_ticks_high:word
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
public _sub_2EAD4
_sub_2EAD4 proc far
 cli
 mov ax, _timer_elapsed_ticks_low
 mov dx, _timer_elapsed_ticks_high
 sti
 retf
_sub_2EAD4 endp
_TEXT ends
end
"""
FAR_SOURCE = b""".8086
extrn _negblockmove:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _synthetic
_synthetic proc far
 call _negblockmove
 retf
_synthetic endp
_TEXT ends
end
"""
NEAR_LABEL_SOURCE = b""".8086
extrn _loc_3180A:near
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _preRender_unk
_preRender_unk proc near
 jmp _loc_3180A
_preRender_unk endp
_TEXT ends
end
"""


def recipe_for(obj, start, public, mode, frame=None):
    result={'kind':'asm','id':public.lstrip('_'),'start':start,
            'end':start+obj.segment_length('_TEXT'),'object_segment':'_TEXT',
            'public':public,'expected_fixups':copy.deepcopy(obj.linker_fixups),
            'expected_relocations':[],
            'binding':{'mode':mode,'declarations':{
                'segments':copy.deepcopy(obj.segment_defs),'groups':copy.deepcopy(obj.groups),
                'publics':copy.deepcopy(obj.publics),'externals':copy.deepcopy(obj.externals)}}}
    if frame is not None:result['original_frame_load_address']=frame
    return result


class Integ17Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        result=verify(write=False)
        cls.image=MZ.parse(result[1]).load_image(result[1])
        cls.relocations=result[2]['unpacked_mz']['relocations']
        cls.dseg_obj,_=assemble_source(DSEG_SOURCE,'masm510-game')
        cls.dseg=recipe_for(cls.dseg_obj,125652,'_sub_2EAD4',
                            'asm-external-dgroup-offset16-v1')
        cls.far_obj,_=assemble_source(FAR_SOURCE,'masm510-game')
        cls.far=recipe_for(cls.far_obj,134105,'_synthetic',
                           'asm-external-far-call-v1',125472)
        cls.far['expected_relocations']=[r for r in cls.relocations
                                          if r['load_offset']==134108]
        cls.label_obj,_=assemble_source(NEAR_LABEL_SOURCE,'masm510-game')
        cls.label_recipe=recipe_for(cls.label_obj,127962,'_preRender_unk',
                                    'asm-external-near-transfer-v1',125472)
        cls.label_recipe['reviewed_near_targets']={
            '_loc_3180A':{'source_line':6552}}
        cls.own_recipe=read_json(ROOT/'tests/fixtures/secondary_data/file_build_path.json')
        cls.own_obj,_=compile_source((ROOT/'tests/fixtures/secondary_data/file_build_path.c').read_bytes(),
                                      cls.own_recipe['profile'])
        cls.labels={'kind':'asm','start':133986,
                    'members':[{'start':133986,'end':134136},
                               {'start':134136,'end':134351},
                               {'start':134351,'end':134521}],
                    'reviewed_near_labels':{
                        '221':{'label':'loc_30C3F','source_line':4925},
                        '230':{'label':'loc_30C48','source_line':4931},
                        '395':{'label':'loc_30CED','source_line':5014}}}

    def test_dseg_positive_and_negative_placement(self):
        self.assertEqual(checked_dseg_base(self.image,self.relocations),178032)
        symbols=resolve_recipe_symbols(self.dseg,self.image,self.relocations)
        payload,_=bind_contribution(self.dseg_obj,self.dseg,symbols)
        self.assertEqual(payload,self.image[125652:125662])
        wrong=copy.deepcopy(symbols)
        wrong['_timer_elapsed_ticks_low']['dseg_frame_load_address']+=16
        with self.assertRaises(ValueError):
            bind_contribution(self.dseg_obj,self.dseg,wrong)
        obj=copy.deepcopy(self.dseg_obj);obj.groups[0]['segment_indices'].remove(3)
        recipe=copy.deepcopy(self.dseg)
        recipe['binding']['declarations']['groups']=obj.groups
        with self.assertRaises(ValueError):
            bind_contribution(obj,recipe,symbols)

    def test_nonempty_dseg_requires_own_code_placement(self):
        obj=copy.deepcopy(self.own_obj)
        obj.segment_lengths['DSEG']=obj.segment_lengths.pop('_DATA')
        obj.segments['DSEG']=obj.segments.pop('_DATA')
        for row in obj.segment_defs:
            if row['name']=='_DATA':row['name']='DSEG'
        for group in obj.groups:
            group['segments']=['DSEG' if n=='_DATA' else n for n in group['segments']]
        for fix in obj.linker_fixups:
            if fix['target_kind']=='segment' and fix['target']=='_DATA':fix['target']='DSEG'
        recipe=copy.deepcopy(self.own_recipe)
        recipe['secondary_dgroup_segments']={'DSEG':recipe['secondary_dgroup_segments']['_DATA']}
        own=[f for f in obj.linker_fixups if f['target_kind']=='segment' and f['target']=='DSEG']
        code=obj.segment_bytes(recipe['object_segment'])
        linked,data,proof,_=bind_secondary(obj,recipe,self.image,self.relocations,code,own)
        self.assertEqual(data['DSEG'],self.image[191401:191403])
        self.assertTrue(proof)
        self.assertNotEqual(linked,code)
        wrong=copy.deepcopy(recipe)
        wrong['secondary_dgroup_segments']['DSEG']['start']+=1
        with self.assertRaises(ValueError):
            bind_secondary(obj,wrong,self.image,self.relocations,code,own)
        wrong_obj=copy.deepcopy(obj)
        wrong_obj.groups[0]['segments'].remove('DSEG')
        with self.assertRaises(ValueError):
            bind_secondary(wrong_obj,recipe,self.image,self.relocations,code,own)


    def test_external_near_reviewed_label(self):
        symbols=resolve_recipe_symbols(self.label_recipe,self.image,self.relocations)
        self.assertEqual(symbols['_loc_3180A']['load_address'],137226)
        payload,_=bind_contribution(self.label_obj,self.label_recipe,symbols)
        self.assertEqual(int.from_bytes(payload[1:3],'little',signed=True),
                         137226-(127962+3))
        wrong=copy.deepcopy(self.label_recipe)
        wrong['reviewed_near_targets']['_loc_3180A']['source_line']+=1
        with self.assertRaises(ValueError):
            resolve_recipe_symbols(wrong,self.image,self.relocations)

        alias='_polycmn'
        alias_obj=copy.deepcopy(self.label_obj)
        alias_recipe=copy.deepcopy(self.label_recipe)
        alias_obj.externals=[alias if name=='_loc_3180A' else name
                             for name in alias_obj.externals]
        for fix in alias_obj.linker_fixups:
            if fix.get('target')=='_loc_3180A':fix['target']=alias
            if fix.get('frame')=='_loc_3180A':fix['frame']=alias
        alias_recipe['expected_fixups']=copy.deepcopy(alias_obj.linker_fixups)
        alias_recipe['binding']['declarations']={
            'segments':copy.deepcopy(alias_obj.segment_defs),
            'groups':copy.deepcopy(alias_obj.groups),
            'publics':copy.deepcopy(alias_obj.publics),
            'externals':copy.deepcopy(alias_obj.externals)}
        alias_recipe['reviewed_near_targets']={alias:{'source_line':6552}}
        alias_symbols=resolve_recipe_symbols(alias_recipe,self.image,self.relocations)
        self.assertEqual(alias_symbols[alias]['load_address'],137226)
        alias_payload,_=bind_contribution(alias_obj,alias_recipe,alias_symbols)
        self.assertEqual(alias_payload,payload)
        wrong=copy.deepcopy(alias_recipe)
        wrong['reviewed_near_targets'][alias]['source_line']+=1
        with self.assertRaises(ValueError):
            resolve_recipe_symbols(wrong,self.image,self.relocations)


    def test_external_far_frame_positive_and_negative_index(self):
        symbols=resolve_recipe_symbols(self.far,self.image,self.relocations)
        payload,proof=bind_contribution(self.far_obj,self.far,symbols)
        self.assertEqual(payload[:5],self.image[134105:134110])
        self.assertEqual(proof['generated_relocations'],self.far['expected_relocations'])
        obj=copy.deepcopy(self.far_obj)
        obj.linker_fixups[0]['frame_index']+=1
        recipe=copy.deepcopy(self.far)
        recipe['expected_fixups']=obj.linker_fixups
        with self.assertRaises(ValueError):
            bind_contribution(obj,recipe,symbols)
        wrong=copy.deepcopy(self.labels)
        wrong['reviewed_near_labels']['221']['label']='loc_30C40'
        with self.assertRaises(ValueError):
            _checked_asm_near_labels(wrong,self.image)


if __name__=='__main__':unittest.main()
