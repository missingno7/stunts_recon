"""Untouched historical LINK fixtures and fail-closed relocating contribution tests."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from common import read_json
from compiler import compile_source
from binder import bind_far_calls
from linker_probe import far_experiment, near_transform_experiment
from code_symbols import resolve_code_symbols
from oracle import verify
from mz import MZ


class FarBindingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.obj,_=compile_source(b'long product(long a,long b,long c,long d) { return a*b-c*d; }\n','msc510-medium')
        cls.symbols={'__aFlmul':{'kind':'far-code','frame_load_address':0x1cc50,'load_address':0x1e8d8}}
        cls.relocs=[{'segment':0x1000,'offset':0x5000+f['offset']+2,'load_offset':0x15000+f['offset']+2} for f in cls.obj.linker_fixups]

    def bind(self,obj=None,relocs=None,symbols=None):
        obj=obj or self.obj
        return bind_far_calls(obj,'UNIT_TEXT','_product',obj.segment_length('UNIT_TEXT'),obj.linker_fixups,
            {'segments':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals},
            self.symbols if symbols is None else symbols,0x15000,self.relocs if relocs is None else relocs)

    def test_historical_link_two_calls_both_orders_and_versions(self):
        for profile in ['msc510-medium','msc500-medium']:
            for first in [False,True]:
                row=far_experiment(profile,first)
                self.assertTrue(row['historical_link_equal']);self.assertEqual(len(row['fixups']),2)

    def test_push_cs_near_call_is_emitted_by_compiler(self):
        for profile in ['msc510-medium','msc500-medium']:
            row=near_transform_experiment(profile)
            self.assertTrue(row['fixups'][0]['self_relative']);self.assertFalse(row['relocations'])

    def test_offset_and_segment_are_distinct_and_order_preserved(self):
        payload,binding=self.bind()
        for f in self.obj.linker_fixups:self.assertEqual(payload[f['offset']:f['offset']+4],bytes.fromhex('881cc51c'))
        self.assertEqual(binding['generated_relocations'],self.relocs)
        with self.assertRaises(ValueError):self.bind(relocs=list(reversed(self.relocs)))
        with self.assertRaises(ValueError):self.bind(relocs=[])

    def test_wrong_frame_kind_addend_location_and_target_rejected(self):
        for key,value in [('frame_method',0),('frame_index',1),('target_index',1),('target','wrong'),
                          ('width',2),('loc','offset16'),('displacement',1),('encoded_addend','01000000'),
                          ('self_relative',True),('offset',0)]:
            obj=copy.deepcopy(self.obj);obj.linker_fixups[0][key]=value
            with self.subTest(key=key),self.assertRaises(ValueError):self.bind(obj)

    def test_wrong_resolved_address_cannot_match(self):
        good,_=self.bind();symbols=copy.deepcopy(self.symbols);symbols['__aFlmul']['load_address']+=1
        bad,_=self.bind(symbols=symbols);self.assertNotEqual(good,bad)
        # Final probe compares every operand byte; no relocation masking exists.

    def test_invalid_segment_frame_hidden_data_rejected(self):
        symbols=copy.deepcopy(self.symbols);symbols['__aFlmul']['frame_load_address']+=1
        with self.assertRaises(ValueError):self.bind(symbols=symbols)
        obj=copy.deepcopy(self.obj);obj.segment_lengths['_BSS']=2
        with self.assertRaises(ValueError):self.bind(obj)

    def test_independent_symbol_anchor_and_runtime_owner(self):
        result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1]);relocs=result[2]['unpacked_mz']['relocations']
        self.assertEqual(resolve_code_symbols(['__aFlmul'],image,relocs),self.symbols)
        with self.assertRaises(ValueError):resolve_code_symbols(['__aFlmul'],image,[])
        import code_symbols
        real=code_symbols.read_json
        def altered(path):
            data=real(path)
            if str(path).endswith('code-symbols.json'):data['symbols']['__aFlmul']['frame_load_address']+=16
            return data
        with patch('code_symbols.read_json',side_effect=altered),self.assertRaises(ValueError):
            resolve_code_symbols(['__aFlmul'],image,relocs)


if __name__=='__main__':unittest.main()
