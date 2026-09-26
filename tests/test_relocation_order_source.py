"""A candidate's OMF traversal, not the oracle, determines relocation order."""
import copy
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from binder import bind_contribution
from probe_module import probe


class RelocationOrderSourceTests(unittest.TestCase):
    def setUp(self):
        code = b'\x9a\0\0\0\0\x9a\0\0\0\0'
        self.obj = SimpleNamespace(
            linker_fixups=[], segment_defs=[{'name':'_TEXT','index':1,'length':10}],
            groups=[], publics=[{'name':'_entry','segment':'_TEXT','offset':0}],
            externals=['_first','_second'], segment_lengths={'_TEXT':10},
            segment_length=lambda name: 10, segment_bytes=lambda name: code,
            local_publics=[], local_externals=[])
        for index, offset, name in ((1,1,'_first'),(2,6,'_second')):
            self.obj.linker_fixups.append({
                'segment':'_TEXT','offset':offset,'loc':'pointer32','width':4,
                'self_relative':False,'target_kind':'external','target_method':2,
                'target_index':index,'target':name,'frame_method':5,
                'frame_kind':'target','frame':name,'frame_index':0,
                'displacement':0,'encoded_addend':'00000000'})
        self.recipe = {
            'id':'entry','start':0x10000,'end':0x1000a,'object_segment':'_TEXT',
            'public':'_entry','expected_fixups':self.obj.linker_fixups,
            'expected_relocations':[
                {'segment':0x1000,'offset':3,'load_offset':0x10003},
                {'segment':0x1000,'offset':8,'load_offset':0x10008}],
            'binding':{'mode':'external-far-call-v1','declarations':{
                'segments':self.obj.segment_defs,'groups':self.obj.groups,
                'publics':self.obj.publics,'externals':self.obj.externals}}}
        self.symbols = {name:{'kind':'far-code','frame_load_address':0x10000,
                              'load_address':0x10020+i*0x10}
                        for i,name in enumerate(self.obj.externals)}

    def test_candidate_fixupp_order_passes(self):
        _, receipt = bind_contribution(self.obj,self.recipe,self.symbols)
        self.assertEqual(receipt['generated_relocations'],
                         self.recipe['expected_relocations'])

    def test_same_sites_in_oracle_order_fail(self):
        wrong = copy.deepcopy(self.recipe)
        wrong['expected_relocations'].reverse()
        with self.assertRaisesRegex(ValueError,'Source relocation obligations differ'):
            bind_contribution(self.obj,wrong,self.symbols)

    def test_oracle_order_recipe_field_is_refused(self):
        wrong = copy.deepcopy(self.recipe)
        wrong['relocation_order_basis'] = 'oracle-complete-interval-v1'
        with self.assertRaisesRegex(ValueError,'cannot supply a relocation order basis'):
            bind_contribution(self.obj,wrong,self.symbols)
        with self.assertRaisesRegex(ValueError,'cannot supply a relocation order basis'):
            probe(wrong)


if __name__ == '__main__':
    unittest.main()
