"""Focused positive and negative controls for data-only binding."""
import copy
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from common import identity
from data_only import bind_data_only
from mz import MZ
from oracle import verify


class SmallObject:
    def __init__(self, payload):
        self.payload=payload
        self.segment_lengths={'UNIT_TEXT':0,'_DATA':len(payload),'CONST':0,'_BSS':0}
        self.segment_defs=[{'name':n,'index':i,'alignment':'word'}
                           for i,n in enumerate(self.segment_lengths,1)]
        self.groups=[{'name':'DGROUP','segments':['CONST','_BSS','_DATA']}]
        self.publics=[{'name':'_aBarn','offset':0,'segment':'_DATA'}]
        self.externals=['__acrtused']
        self.linker_fixups=[]

    def segment_length(self, name): return self.segment_lengths[name]
    def segment_bytes(self, name): return self.payload if name=='_DATA' else b''


class DataOnlyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle=verify(write=False)
        cls.image=MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations=oracle[2]['unpacked_mz']['relocations']
        start=0x2c1c0;end=start+4
        cls.obj=SmallObject(cls.image[start:end])
        cls.recipe={
            'id':'small_data_control','data_only':True,'object_segment':'_DATA',
            'start':start,'end':end,'dgroup_offset':start-178032,
            'target':identity(cls.image[start:end]),'expected_fixups':[],
            'required_pointer_sites':[],
            'public_object_sizes':{'_aBarn':4},
            'expected_relocations':[],'external_data_targets':[],
            'object_declarations':{'segments':cls.obj.segment_defs,
                                   'groups':cls.obj.groups,'publics':cls.obj.publics,
                                   'externals':cls.obj.externals},
            'placement_anchors':{'basis':'oracle-code-operands-v1',
                'public_offsets':{'_aBarn':0},'reference_files':[],
                'code_operands':[{'public':'_aBarn','load_offset':0xfef4,
                                  'instruction_hex':cls.image[0xfef4:0xfef7].hex(),
                                  'operand_offset':1}]}}

    def test_exact_small_module(self):
        payload,_=bind_data_only(self.obj,self.recipe,self.image,self.relocations)
        self.assertEqual(payload,self.image[self.recipe['start']:self.recipe['end']])

    def test_no_code_anchor(self):
        recipe=copy.deepcopy(self.recipe)
        recipe['placement_anchors']['code_operands']=[]
        with self.assertRaisesRegex(ValueError,'placement'):
            bind_data_only(self.obj,recipe,self.image,self.relocations)

    def test_shifted_public(self):
        recipe=copy.deepcopy(self.recipe)
        recipe['placement_anchors']['public_offsets']['_aBarn']=2
        with self.assertRaisesRegex(ValueError,'placement'):
            bind_data_only(self.obj,recipe,self.image,self.relocations)

    def test_object_extent_gap(self):
        recipe=copy.deepcopy(self.recipe)
        recipe['public_object_sizes']['_aBarn']=3
        with self.assertRaisesRegex(ValueError,'object extents'):
            bind_data_only(self.obj,recipe,self.image,self.relocations)

    def test_unexpected_fixup(self):
        obj=copy.copy(self.obj)
        obj.linker_fixups=[{'segment':'_DATA'}]
        with self.assertRaisesRegex(ValueError,'FIXUPP'):
            bind_data_only(obj,self.recipe,self.image,self.relocations)

    def test_missing_semantic_pointer_fixup(self):
        recipe=copy.deepcopy(self.recipe)
        recipe['required_pointer_sites']=[0]
        with self.assertRaisesRegex(ValueError,'pointer fields'):
            bind_data_only(self.obj,recipe,self.image,self.relocations)

    def test_extra_segment(self):
        obj=copy.copy(self.obj)
        obj.segment_lengths={**obj.segment_lengths,'CONST':2}
        with self.assertRaisesRegex(ValueError,'nonempty segment'):
            bind_data_only(obj,self.recipe,self.image,self.relocations)

    def test_moved_interval(self):
        recipe=copy.deepcopy(self.recipe)
        recipe['start']+=2;recipe['end']+=2
        with self.assertRaisesRegex(ValueError,'placement'):
            bind_data_only(self.obj,recipe,self.image,self.relocations)


if __name__=='__main__': unittest.main()
