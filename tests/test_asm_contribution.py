import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from assembler import asm_source, assemble_source
from common import identity
from multi_contribution import bind_multi
from oracle import verify
from mz import MZ
from probe_module import probe
from runtime_absolute import ahshift
from binder import bind_contribution, bind_asm_cs_data, bind_mixed_far_data
from code_symbols import resolve_recipe_symbols
from secondary_contribution import bind_single_secondary
from promote import attach_secondary
from build_exact import validate_layout
from common import read_json
from compiler import compile_source


VIDEO = b'''_TEXT segment word public 'CODE'\nassume cs:_TEXT\npublic _video_get_status\n_video_get_status proc far\n mov dx,03DAh\n in al,dx\n and al,08h\n xor ah,ah\n ret\n db 0\n_video_get_status endp\n_TEXT ends\nend\n'''


class AsmContributionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle = verify(write=False)
        cls.image = MZ.parse(cls.oracle[1]).load_image(cls.oracle[1])
        cls.obj, _ = assemble_source(VIDEO, 'masm510-game')
        start, end = 143356, 143366
        cls.recipe = {'id':'video_get_status','kind':'asm','start':start,'end':end,
                      'source':'asm/video_get_status.ASM','profile':'masm510-game',
                      'include_closure':[], 'assembler_flags':['/Mx','/I.'],
                      'object_segment':'_TEXT',
                      'public':'_video_get_status','target':identity(cls.image[start:end]),
                      'object_declarations':{'segments':cls.obj.segment_defs,
                                             'groups':cls.obj.groups,'publics':cls.obj.publics,
                                             'externals':cls.obj.externals},
                      'expected_fixups':[], 'expected_relocations':[]}

    def test_complete_asm_probe(self):
        payload, receipt = probe(self.recipe, self.oracle, VIDEO)
        self.assertEqual(payload, self.image[143356:143366])
        self.assertEqual(receipt['binding']['generated_relocations'], [])

    def test_asm_rejects_changed_declaration_fixup_and_extent(self):
        for change in ({'object_declarations':{}}, {'expected_fixups':[{'offset':0}]},
                       {'end':143365}, {'assembler_flags':['/Mx']}):
            with self.subTest(change=change), self.assertRaises(ValueError):
                probe({**self.recipe, **change}, self.oracle, VIDEO)

    def test_asm_rejects_external_source_closure(self):
        for source in (b'include other.inc\n', b'org 100h\n', b'\xff'):
            with self.subTest(source=source), self.assertRaises(ValueError):
                asm_source(source)

    def test_two_public_module_with_internal_call(self):
        source = b'''_TEXT segment word public 'CODE'\nassume cs:_TEXT\npublic _first, _second\n_first proc far\n call _second\n ret\n_first endp\n_second proc near\n mov ax,1\n ret\n_second endp\n_TEXT ends\nend\n'''
        obj, _ = assemble_source(source, 'masm510-game')
        body = obj.segment_bytes('_TEXT')
        publics = {p['name']:p['offset'] for p in obj.publics}
        self.assertEqual(set(publics), {'_first','_second'})
        self.assertIn(0xe8, body)
        recipe = {'kind':'asm','start':0,'end':len(body),'object_segment':'_TEXT',
                  'object_declarations':{'segments':obj.segment_defs,'groups':obj.groups,
                                         'publics':obj.publics,'externals':obj.externals},
                  'expected_fixups':obj.linker_fixups,'expected_relocations':[],
                  'members':[{'name':'first','public':'_first','start':0,'end':publics['_second']},
                             {'name':'second','public':'_second','start':publics['_second'],'end':len(body)}]}
        payload, binding = bind_multi(obj, recipe, body, [])
        linked = bytearray(body); linked[1:3] = b'\x01\x00'
        self.assertEqual(payload, linked)
        self.assertEqual(binding['generated_relocations'], [])
        bad = copy.deepcopy(recipe)
        bad['object_declarations']['publics'] = []
        with self.assertRaises(ValueError):
            bind_multi(obj, bad, body, [])

    def test_runtime_absolute_public_and_original_operand(self):
        target = ahshift(self.image, self.oracle[2]['unpacked_mz']['relocations'])
        self.assertEqual(target['value'], 12)
        changed = bytearray(self.image); changed[28769] ^= 1
        with self.assertRaises(ValueError):
            ahshift(changed, self.oracle[2]['unpacked_mz']['relocations'])

    def test_ahshift_binds_compiler_huge_pointer_fixup(self):
        source=(b'extern char huge *p; extern int value; '
                b'long f(long i) { p[i] = (char)value; return i*i; }')
        obj,_=compile_source(source,'msc510-medium')
        shift,=[f for f in obj.linker_fixups if f['target']=='__AHSHIFT']
        self.assertEqual((shift['loc'],shift['encoded_addend']),
                         ('loader-offset16','0000'))
        start=0x15000
        relocs=[{'segment':0x1000,'offset':start+f['offset']+2-0x10000,
                 'load_offset':start+f['offset']+2}
                for f in obj.linker_fixups if f['loc']=='pointer32']
        symbols={'__aFlmul':{'kind':'far-code','frame_load_address':0x1cc50,
                             'load_address':0x1e8d8},
                 '_p':{'group':'DGROUP','frame_load_address':0x2b770,
                       'load_address':0x2c000,'width':4,'allowed_addends':[0,2]},
                 '_value':{'group':'DGROUP','frame_load_address':0x2b770,
                           'load_address':0x2c100,'width':2,'allowed_addends':[0]},
                 '__AHSHIFT':ahshift(self.image,self.oracle[2]['unpacked_mz']['relocations'])}
        declarations={'segments':obj.segment_defs,'groups':obj.groups,
                      'publics':obj.publics,'externals':obj.externals}
        args=(obj,'UNIT_TEXT','_f',obj.segment_length('UNIT_TEXT'),
              obj.linker_fixups,declarations,symbols,start,relocs)
        payload,receipt=bind_mixed_far_data(*args)
        self.assertEqual(payload[shift['offset']:shift['offset']+2],b'\x0c\x00')
        self.assertEqual(receipt['generated_relocations'],relocs)
        bad=copy.deepcopy(symbols);bad['__AHSHIFT']['value']=11
        with self.assertRaises(ValueError):
            bind_mixed_far_data(*(args[:6]+(bad,)+args[7:]))

    def test_masm_far_call_binding_and_relocation(self):
        source = b'''_TEXT segment word public 'CODE'\nassume cs:_TEXT\nextrn timer_get_counter:far\npublic _timer_compare_dx\n_timer_compare_dx proc far\n call far ptr timer_get_counter\n ret\n_timer_compare_dx endp\n_TEXT ends\nend\n'''
        obj, _ = assemble_source(source,'masm510-game')
        declarations={'segments':obj.segment_defs,'groups':obj.groups,
                      'publics':obj.publics,'externals':obj.externals}
        relocations=self.oracle[2]['unpacked_mz']['relocations']
        expected=[r for r in relocations if r['load_offset']==141294]
        recipe={'kind':'asm','start':141291,'end':141291+obj.segment_length('_TEXT'),
                'object_segment':'_TEXT','public':'_timer_compare_dx',
                'original_frame_load_address':125472,
                'expected_fixups':obj.linker_fixups,'expected_relocations':expected,
                'binding':{'mode':'asm-external-far-call-v1','declarations':declarations}}
        symbols=resolve_recipe_symbols(recipe,self.image,relocations)
        payload, receipt=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload[:5],self.image[141291:141296])
        self.assertEqual(receipt['generated_relocations'],expected)
        with self.assertRaises(ValueError):
            bind_contribution(obj,{**recipe,'expected_relocations':[]},symbols)
        with self.assertRaises(ValueError):
            bind_contribution(obj,{**recipe,'original_frame_load_address':0},symbols)

    def test_masm_cs_table_binding_and_extent(self):
        source = b'''_TEXT segment word public 'CODE'\nassume cs:_TEXT\nextrn sprite1:byte\npublic _sprite_copy_both_to_arg\n_sprite_copy_both_to_arg proc far\n lea si,sprite1\n ret\n_sprite_copy_both_to_arg endp\n_TEXT ends\nend\n'''
        obj, _ = assemble_source(source,'masm510-game')
        declarations={'segments':obj.segment_defs,'groups':obj.groups,
                      'publics':obj.publics,'externals':obj.externals}
        recipe={'kind':'asm','start':140814,'end':140814+obj.segment_length('_TEXT'),
                'object_segment':'_TEXT','public':'_sprite_copy_both_to_arg',
                'original_frame_load_address':125472,
                'expected_fixups':obj.linker_fixups,'expected_relocations':[],
                'binding':{'mode':'asm-external-cs-offset16-v1','declarations':declarations}}
        symbols=resolve_recipe_symbols(recipe,self.image,
                                       self.oracle[2]['unpacked_mz']['relocations'])
        payload, _=bind_contribution(obj,recipe,symbols)
        self.assertEqual(payload[:4],self.image[140820:140824])
        bad=copy.deepcopy(symbols)
        bad['sprite1']['load_address']=bad['sprite1']['island_end']
        with self.assertRaises(ValueError):
            bind_asm_cs_data(obj,'_TEXT','_sprite_copy_both_to_arg',len(payload),
                             obj.linker_fixups,declarations,bad,125472,[])

    def test_asm_owned_dgroup_data_and_manifest_parent(self):
        source=b'''_DATA segment word public 'DATA'\nfoo db 5Ah\n_DATA ends\nDGROUP group _DATA\n_TEXT segment word public 'CODE'\nassume cs:_TEXT, ds:DGROUP\npublic _secondary_probe\n_secondary_probe proc far\n mov al,foo\n ret\n_secondary_probe endp\n_TEXT ends\nend\n'''
        obj, _=assemble_source(source,'masm510-game')
        layout=read_json(ROOT/'layout/data-symbols.json')
        code_start=100; data_start=layout['frame_load_address']+0x2000
        image=bytearray(max(data_start+1,code_start+4))
        image[code_start:code_start+4]=bytes.fromhex('a00020cb')
        image[data_start]=0x5a
        recipe={'id':'secondary_probe','kind':'asm','start':code_start,'end':code_start+4,
                'object_segment':'_TEXT','public':'_secondary_probe',
                'object_declarations':{'segments':obj.segment_defs,'groups':obj.groups,
                                       'publics':obj.publics,'externals':obj.externals},
                'expected_fixups':obj.linker_fixups,'expected_relocations':[],
                'secondary_dgroup_segments':{'_DATA':{'start':data_start,'end':data_start+1,
                    'dgroup_offset':0x2000,'target':identity(b'\x5a'),'expected_relocations':[]}}}
        with patch('secondary_contribution.checked_dgroup_layout',return_value=layout):
            payload, binding=bind_single_secondary(obj,recipe,image,[])
        self.assertEqual(payload,image[code_start:code_start+4])
        self.assertEqual(binding['secondary_payloads']['_DATA'],'5a')
        manifest={'owners':[{'id':'left','kind':'UNRESOLVED_RAW','start':0,'end':code_start},
                            {'id':'code','kind':'MATCHING_ASM','name':'secondary_probe',
                             'start':code_start,'end':code_start+4,'recipe':'recipes/secondary_probe.json'},
                            {'id':'right','kind':'UNRESOLVED_RAW',
                             'start':code_start+4,'end':len(image)}]}
        staged=attach_secondary(manifest,recipe,image)
        validate_layout(staged,len(image))
        owner,=[o for o in staged['owners'] if o['kind']=='MATCHING_ASM_DATA']
        self.assertEqual(owner['parent'],'code')


if __name__ == '__main__':
    unittest.main()
