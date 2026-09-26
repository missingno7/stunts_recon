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
from multi_contribution import _checked_asm_near_labels, bind_multi
from mz import MZ
from oracle import verify
from secondary_contribution import bind_secondary

DSEG_SOURCE = b""".model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _word_3F87C:word
extrn _word_3F87E:word
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
public _sub_2EAD4
_sub_2EAD4 proc far
 cli
 mov ax, _word_3F87C
 mov dx, _word_3F87E
 sti
 retf
_sub_2EAD4 endp
_TEXT ends
end
"""
NEAR_SOURCE = b""".8086
extrn _file_decomp_rle_seq:near
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_decomp_rle_single
_file_decomp_rle_single proc near
 jmp _file_decomp_rle_seq
_file_decomp_rle_single endp
_TEXT ends
end
"""
FAR_SOURCE = b""".8086
extrn _copy_paras_reverse:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _synthetic
_synthetic proc far
 call _copy_paras_reverse
 retf
_synthetic endp
_TEXT ends
end
"""
NEAR_GROUP_SOURCE = b""".8086
extrn _file_decomp_rle_seq:near
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_decomp_rle_single, _synthetic_tail
_file_decomp_rle_single proc near
 jmp _file_decomp_rle_seq
_file_decomp_rle_single endp
_synthetic_tail proc near
 ret
_synthetic_tail endp
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
        cls.near_obj,_=assemble_source(NEAR_SOURCE,'masm510-game')
        cls.near=recipe_for(cls.near_obj,134136,'_file_decomp_rle_single',
                            'asm-external-near-transfer-v1',125472)
        cls.far_obj,_=assemble_source(FAR_SOURCE,'masm510-game')
        cls.far=recipe_for(cls.far_obj,134105,'_synthetic',
                           'asm-external-far-call-v1',125472)
        cls.far['expected_relocations']=[r for r in cls.relocations
                                          if r['load_offset']==134108]
        cls.near_group_obj,_=assemble_source(NEAR_GROUP_SOURCE,'masm510-game')
        group=cls.near_group_obj
        cls.near_group={
            'kind':'asm','id':'file_decomp_rle_single','start':134136,
            'end':134136+group.segment_length('_TEXT'),'object_segment':'_TEXT',
            'original_frame_load_address':125472,
            'members':[{'name':'file_decomp_rle_single','public':'_file_decomp_rle_single',
                        'start':134136,'end':134139},
                       {'name':'synthetic_tail','public':'_synthetic_tail',
                        'start':134139,'end':134140}],
            'object_declarations':{'segments':copy.deepcopy(group.segment_defs),
                                   'groups':copy.deepcopy(group.groups),
                                   'publics':copy.deepcopy(group.publics),
                                   'externals':copy.deepcopy(group.externals)},
            'expected_fixups':copy.deepcopy(group.linker_fixups),
            'expected_relocations':[],
            'external_binding':{'mode':'asm-external-near-transfer-v1'}}
        cls.label_obj,_=assemble_source(NEAR_LABEL_SOURCE,'masm510-game')
        cls.label_recipe=recipe_for(cls.label_obj,127962,'_preRender_unk',
                                    'asm-external-near-transfer-v1',125472)
        cls.label_recipe['reviewed_near_targets']={
            '_loc_3180A':{'source_line':6552}}
        cls.own_recipe=read_json(ROOT/'recipes/file_build_path.json')
        cls.own_obj,_=compile_source((ROOT/'src/file_build_path.c').read_bytes(),
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
        wrong['_word_3F87C']['dseg_frame_load_address']+=16
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

    def test_external_near_positive_and_negative_frame(self):
        symbols=resolve_recipe_symbols(self.near,self.image,self.relocations)
        payload,proof=bind_contribution(self.near_obj,self.near,symbols)
        self.assertEqual(payload,b'\xe9\xd4\x00')
        self.assertEqual(proof['generated_relocations'],[])
        wrong=copy.deepcopy(symbols)
        wrong['_file_decomp_rle_seq']['frame_load_address']+=16
        with self.assertRaises(ValueError):
            bind_contribution(self.near_obj,self.near,wrong)
        recipe=copy.deepcopy(self.near)
        recipe['original_frame_load_address']+=16
        with self.assertRaises(ValueError):
            resolve_recipe_symbols(recipe,self.image,self.relocations)
        recipe=copy.deepcopy(self.near)
        recipe['expected_relocations']=[{'segment':0,'offset':0,'load_offset':0}]
        with self.assertRaises(ValueError):
            bind_contribution(self.near_obj,recipe,symbols)

    def test_group_dispatches_external_near_and_checks_index(self):
        payload,_=bind_multi(self.near_group_obj,self.near_group,self.image,self.relocations)
        self.assertEqual(payload,b'\xe9\xd4\x00\xc3')
        obj=copy.deepcopy(self.near_group_obj)
        obj.linker_fixups[0]['frame_index']+=1
        recipe=copy.deepcopy(self.near_group)
        recipe['expected_fixups']=obj.linker_fixups
        with self.assertRaises(ValueError):
            bind_multi(obj,recipe,self.image,self.relocations)

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

    def test_interior_near_labels_positive_and_negative(self):
        self.assertEqual(_checked_asm_near_labels(self.labels,self.image),
                         {221:'loc_30C3F',230:'loc_30C48',395:'loc_30CED'})
        wrong=copy.deepcopy(self.labels)
        wrong['reviewed_near_labels']['221']['source_line']+=1
        with self.assertRaises(ValueError):
            _checked_asm_near_labels(wrong,self.image)

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
