"""Strict integration checks for local OMF, reviewed islands and crosscheck dispatch."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from binder import bind_contribution
from code_symbols import resolve_code_symbols
from common import read_json
from compiler import compile_source
from crosscheck_runner import bind_recipe_object
from function_evidence import reviewed_functions
from multi_contribution import bind_multi
from mz import MZ
from object_probe import read_object
from oracle import verify


class IntegrationPass3(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle=verify(write=False)
        cls.image=MZ.parse(cls.oracle[1]).load_image(cls.oracle[1])
        cls.relocations=cls.oracle[2]['unpacked_mz']['relocations']

    def test_local_omf_symbols_are_internal_complete_members(self):
        source=(b'static int far helper(void) { return 7; } '
                b'int far entry(void) { return helper(); }')
        obj,compile_receipt=compile_source(source,'msc510-medium')
        self.assertEqual(obj.local_publics,[{'name':'helper','segment':'UNIT_TEXT','offset':0}])
        self.assertEqual(obj.local_externals,['helper'])
        self.assertIn('local',obj.external_scopes)
        self.assertEqual(len(obj.publics),2)
        with self.assertRaisesRegex(ValueError,'complete reviewed group recipe'):
            bind_contribution(obj,{'object_segment':'UNIT_TEXT','public':'helper',
                                   'start':0,'end':obj.segment_length('UNIT_TEXT'),
                                   'expected_fixups':obj.linker_fixups,
                                   'expected_relocations':[]})
        length=obj.segment_length('UNIT_TEXT')
        members=[{'public':p['name'],'start':p['offset'],
                  **({'local_symbol':True} if p in obj.local_publics else {})} for p in obj.publics]
        recipe={'start':0,'end':length,'object_segment':'UNIT_TEXT','members':members,
                'object_declarations':{'segments':obj.segment_defs,'groups':obj.groups,
                    'publics':obj.publics,'externals':obj.externals},
                'expected_fixups':obj.linker_fixups,'expected_relocations':[]}
        payload,_=bind_multi(obj,recipe,b'',[])
        self.assertEqual(len(payload),length)
        self.assertNotEqual(payload,obj.segment_bytes('UNIT_TEXT'))
        self.assertEqual(payload[6:8],b'\xf8\xff')
        raw=bytearray((Path(compile_receipt['work_directory'])/'UNIT.OBJ').read_bytes())
        original_raw=bytes(raw)
        cursor=0
        while cursor<len(raw):
            size=int.from_bytes(raw[cursor+1:cursor+3],'little');end=cursor+3+size
            if raw[cursor]==0xb4:
                body=raw[cursor+3:end-1]
                self.assertIn(b'helper',body)
                at=cursor+3+body.index(b'helper')
                raw[at:at+6]=b'helpex'
                raw[end-1]=(-sum(raw[cursor:end-1]))&255
                break
            cursor=end
        with self.assertRaisesRegex(ValueError,'Unresolved local OMF external'):
            read_object(bytes(raw))
        unsupported=bytearray(original_raw);unsupported[cursor]=0xb7
        unsupported[end-1]=(-sum(unsupported[cursor:end-1]))&255
        with self.assertRaisesRegex(ValueError,'unsupported OMF record b7'):
            read_object(bytes(unsupported))
        bad=copy.deepcopy(recipe);bad['members'][0].pop('local_symbol')
        with self.assertRaisesRegex(ValueError,'Local helper publics'):
            bind_multi(obj,bad,b'',[])
        bad=copy.deepcopy(recipe);bad['members']=bad['members'][1:]
        with self.assertRaises(ValueError):bind_multi(obj,bad,b'',[])

    def test_crosscheck_uses_group_member_gate_and_binding(self):
        recipe=read_json(ROOT/'recipes/obj_seg027.json')
        source=(ROOT/recipe['source']).read_bytes()
        obj,_=compile_source(source,recipe['profile'],recipe.get('compiler_flags'),
                             sparse_zero=recipe.get('sparse_zero'),
                             communals=[d['name'] for d in recipe.get('communal_declarations', [])])
        payload,receipt=bind_recipe_object(obj,recipe,self.image,self.relocations)
        self.assertEqual(payload,self.image[recipe['start']:recipe['end']])
        self.assertEqual(receipt['generated_relocations'],recipe['expected_relocations'])
        bad=copy.deepcopy(recipe);bad['members'][1]['start']+=1
        with self.assertRaises(ValueError):
            bind_recipe_object(obj,bad,self.image,self.relocations)

    def test_runtime_and_sine_aliases_require_grounded_anchors(self):
        targets=resolve_code_symbols({'_strlen','_rand','_int86','_sin_fast'},
                                     self.image,self.relocations)
        self.assertEqual({n:v['load_address'] for n,v in targets.items()},
                         {'_strlen':123780,'_rand':124494,'_int86':123916,'_sin_fast':141022})
        original=read_json(ROOT/'layout/code-symbols.json')
        changed=copy.deepcopy(original);changed['symbols']['_sin_fast']['anchors'][0]['hex']='9a00000000'
        with patch('code_symbols.read_json',side_effect=lambda p: changed if p==ROOT/'layout/code-symbols.json' else read_json(p)):
            with self.assertRaises(ValueError):
                resolve_code_symbols({'_sin_fast'},self.image,self.relocations)

    def test_reviewed_extents_and_jump_table_are_complete(self):
        rows=reviewed_functions(self.image)
        self.assertEqual((rows['subst_hillroad_track']['start'],rows['subst_hillroad_track']['end']),
                         (72282,72570))
        self.assertEqual(rows['subst_hillroad_track']['padding_offsets'][-1],72569)
        self.assertEqual((rows['mouse_set_minmax']['end'],rows['mouse_get_position']['start'],
                          rows['mouse_hide_cursor']['end']),(158128,158128,158252))
        self.assertEqual(rows['sin_fast']['data_islands'][0]['hex'],
                         self.image[141042:141050].hex())
        original=read_json(ROOT/'layout/function-evidence.json')
        changed=copy.deepcopy(original)
        next(r for r in changed['functions'] if r['name']=='sin_fast')['data_islands'][0]['hex']='0000000000000000'
        with patch('function_evidence.read_json',return_value=changed):
            with self.assertRaises(ValueError):reviewed_functions(self.image)


if __name__=='__main__':unittest.main()
