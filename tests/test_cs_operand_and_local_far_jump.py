"""Complete-module CS operands, far transfers, and nested owner preservation."""
import copy
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from binder import bind_asm_cs_data, bind_composed


def module(code, *, publics, externals, fixups):
    segment={'name':'_TEXT','index':1,'length':len(code)}
    return SimpleNamespace(
        linker_fixups=fixups,segment_defs=[segment],groups=[],
        publics=publics,externals=externals,segment_lengths={'_TEXT':len(code)},
        segment_length=lambda name:len(code),segment_bytes=lambda name:code)


class Integ19Tests(unittest.TestCase):
    def test_cs_direct_operand_requires_override_and_bounded_object(self):
        code=b'\x2e\x3b\x0e\0\0\xcb'
        fix={'segment':'_TEXT','offset':3,'loc':'offset16','width':2,'self_relative':False,
             'target_kind':'external','target_method':2,'target_index':1,
             'target':'_word_2F448','frame_method':0,'frame_kind':'segment',
             'frame':'_TEXT','frame_index':1,'displacement':0,'encoded_addend':'0000'}
        obj=module(code,publics=[{'name':'_entry','segment':'_TEXT','offset':0}],
                   externals=['_word_2F448'],fixups=[fix])
        declarations={'segments':obj.segment_defs,'groups':obj.groups,
                      'publics':obj.publics,'externals':obj.externals}
        symbols={'_word_2F448':{'kind':'cs-data','frame_load_address':125472,
                 'load_address':128072,'width':2,'island_start':128072,'island_end':130526}}
        payload,_=bind_asm_cs_data(obj,'_TEXT','_entry',6,[fix],declarations,
                                   symbols,125472,[])
        self.assertEqual(payload,b'\x2e\x3b\x0e\x28\x0a\xcb')
        bad=module(b'\x90'+code[1:],publics=obj.publics,externals=obj.externals,fixups=[fix])
        with self.assertRaises(ValueError):
            bind_asm_cs_data(bad,'_TEXT','_entry',6,[fix],declarations,
                             symbols,125472,[])

    def test_same_module_far_jmp_has_exact_relocation(self):
        code=b'\xea\0\0\0\0\xcb'
        fix={'segment':'_TEXT','offset':1,'loc':'pointer32','width':4,'self_relative':False,
             'target_kind':'segment','target_method':0,'target_index':1,
             'target':'_TEXT','frame_method':0,'frame_kind':'segment',
             'frame':'_TEXT','frame_index':1,'displacement':5,
             'encoded_addend':'00000000'}
        obj=module(code,publics=[{'name':'_entry','segment':'_TEXT','offset':0}],
                   externals=[],fixups=[fix])
        recipe={'kind':'asm','start':0x20000,'end':0x20006,
                'object_segment':'_TEXT','public':'_entry',
                'original_frame_load_address':0x20000,
                'binding':{'mode':'asm-external-far-call-v1','declarations':
                           {'segments':obj.segment_defs,'groups':obj.groups,
                            'publics':obj.publics,'externals':obj.externals}},
                'expected_fixups':[fix],
                'expected_relocations':[{'segment':0x2000,'offset':3,
                                         'load_offset':0x20003}]}
        symbols={'_TEXT':{'kind':'local-text','frame_load_address':0x20000}}
        payload,receipt=bind_composed(obj,recipe,symbols)
        self.assertEqual(payload,b'\xea\x05\0\0\x20\xcb')
        self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])
        bad=copy.deepcopy(recipe)
        bad['expected_fixups'][0]['displacement']=6
        with self.assertRaises(ValueError):bind_composed(obj,bad,symbols)


if __name__=='__main__':unittest.main()
