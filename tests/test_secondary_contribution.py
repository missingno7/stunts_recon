"""Complete C DGROUP contribution and callback pointer acceptance controls."""
import copy
import struct
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import identity, read_json
from compiler import compile_source,CompileFailure
from secondary_contribution import bind_secondary
from binder import bind_mixed_far_data
from promote import attach_secondary
from build_exact import validate_layout
from oracle import verify
from mz import MZ
from code_symbols import resolve_code_symbols
from data_symbols import resolve_symbols,checked_dgroup_layout


class SecondaryContributionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.frame=read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
        cls.obj,_=compile_source(
            b'char *label = "abc"; int far f(void) { return label[1]; }',
            'msc510-medium')
        cls.start=500
        cls.data_start=cls.frame+0x5400
        code=bytearray(cls.obj.segment_bytes('UNIT_TEXT'))
        struct.pack_into('<H',code,2,0x5404)
        data=bytearray(cls.obj.segment_bytes('_DATA'))
        struct.pack_into('<H',data,4,0x5400)
        cls.expected_code=bytes(code)
        cls.expected_data=bytes(data)
        cls.image=bytearray(200000)
        cls.image[cls.start:cls.start+len(code)]=code
        cls.image[cls.data_start:cls.data_start+len(data)]=data
        cls.recipe={
            'id':'f','start':cls.start,'end':cls.start+len(code),
            'object_segment':'UNIT_TEXT',
            'secondary_dgroup_segments':{'_DATA':{
                'start':cls.data_start,'end':cls.data_start+len(data),
                'dgroup_offset':0x5400,'target':identity(bytes(data)),
                'expected_relocations':[]}},
        }
        cls.code_fixups=[f for f in cls.obj.linker_fixups if f['segment']=='UNIT_TEXT']

    def bind(self,obj=None,recipe=None,image=None,relocations=None):
        with patch('secondary_contribution.checked_dgroup_layout',
                   return_value=read_json(ROOT/'layout/data-symbols.json')):
            return bind_secondary(obj or self.obj,recipe or self.recipe,
                                  self.image if image is None else image,
                                  [] if relocations is None else relocations,
                                  (obj or self.obj).segment_bytes('UNIT_TEXT'),
                                  [f for f in (obj or self.obj).linker_fixups
                                   if f['segment']=='UNIT_TEXT'])

    def test_initialized_data_with_code_and_data_fixups_is_complete(self):
        code,segments,proof,relocs=self.bind()
        self.assertEqual(code,self.expected_code)
        self.assertEqual(segments,{'_DATA':self.expected_data})
        self.assertEqual(len(proof),2)
        self.assertEqual(relocs,[])

    def test_unowned_wrong_placement_bytes_addend_and_fixups_rejected(self):
        bad=copy.deepcopy(self.recipe);bad['secondary_dgroup_segments']={}
        with self.assertRaises(ValueError):self.bind(recipe=bad)
        bad=copy.deepcopy(self.recipe)
        bad['secondary_dgroup_segments']['_DATA']['start']+=1
        with self.assertRaises(ValueError):self.bind(recipe=bad)
        image=bytearray(self.image);image[self.data_start]+=1
        with self.assertRaises(ValueError):self.bind(image=image)
        image=bytearray(self.image);image[self.start+2]+=1
        with self.assertRaises(ValueError):self.bind(image=image)
        obj=copy.deepcopy(self.obj)
        obj.linker_fixups[1]['encoded_addend']='0600'
        with self.assertRaises(ValueError):self.bind(obj=obj)
        obj=copy.deepcopy(self.obj)
        obj.linker_fixups[0]['loc']='pointer48'
        with self.assertRaises(ValueError):self.bind(obj=obj)
        bad=copy.deepcopy(self.recipe)
        bad['secondary_dgroup_segments']['_DATA']['expected_relocations']=[
            {'segment':0,'offset':0,'load_offset':self.data_start}]
        with self.assertRaises(ValueError):self.bind(recipe=bad)

    def test_bss_zero_extent_and_bounds(self):
        obj,_=compile_source(
            b'static int value; int far f(void) { return value; }',
            'msc510-medium')
        start=200100
        code=bytearray(obj.segment_bytes('UNIT_TEXT'))
        struct.pack_into('<H',code,1,start-self.frame)
        image=bytearray(200000);image[700:704]=code
        recipe={'start':700,'end':704,'object_segment':'UNIT_TEXT',
                'secondary_dgroup_segments':{'_BSS':{
                    'start':start,'end':start+2,'dgroup_offset':start-self.frame,
                    'target':identity(bytes(2))}}}
        with patch('secondary_contribution.checked_dgroup_layout',
                   return_value=read_json(ROOT/'layout/data-symbols.json')):
            result=bind_secondary(obj,recipe,image,[],obj.segment_bytes('UNIT_TEXT'),
                                  obj.linker_fixups)
        self.assertEqual(result[1],{'_BSS':bytes(2)})
        bad=copy.deepcopy(recipe)
        bad['secondary_dgroup_segments']['_BSS']['end']=start+3
        with patch('secondary_contribution.checked_dgroup_layout',
                   return_value=read_json(ROOT/'layout/data-symbols.json')):
            with self.assertRaises(ValueError):
                bind_secondary(obj,bad,image,[],obj.segment_bytes('UNIT_TEXT'),obj.linker_fixups)

    def test_comdef_communals_are_explicitly_deferred(self):
        with self.assertRaisesRegex(CompileFailure,'COMDEF communal allocation is deferred'):
            compile_source(b'int communal; int far f(void) { return communal; }',
                           'msc510-medium')

    def test_initialized_far_code_pointer_fixup_and_relocation(self):
        obj,_=compile_source(
            b'extern void far cb(void); void (far *ptr)(void)=cb; '
            b'int far f(void){ return ptr!=0; }','msc510-medium')
        start=self.frame+0x5410
        code=bytearray(obj.segment_bytes('UNIT_TEXT'))
        struct.pack_into('<H',code,1,0x5410)
        struct.pack_into('<H',code,5,0x5412)
        data=struct.pack('<HH',0x19b12-0x174b0,0x174b0//16)
        image=bytearray(200000)
        image[800:800+len(code)]=code
        image[start:start+4]=data
        site=start+2
        relocation={'segment':(site//65536)*4096,
                    'offset':site%65536,'load_offset':site}
        recipe={'start':800,'end':800+len(code),'object_segment':'UNIT_TEXT',
                'secondary_external_targets':{'code':['_cb'],'data':[]},
                'secondary_dgroup_segments':{'_DATA':{
                    'start':start,'end':start+4,'dgroup_offset':0x5410,
                    'target':identity(data),
                    'expected_relocations':[relocation]}}}
        symbol={'_cb':{'kind':'far-code','frame_load_address':0x174b0,
                       'load_address':0x19b12}}
        with patch('code_symbols.resolve_code_symbols',return_value=symbol),\
             patch('secondary_contribution.checked_dgroup_layout',
                   return_value=read_json(ROOT/'layout/data-symbols.json')):
            result=bind_secondary(obj,recipe,image,[relocation],
                                  obj.segment_bytes('UNIT_TEXT'),
                                  [f for f in obj.linker_fixups if f['segment']=='UNIT_TEXT'])
            self.assertEqual(result[1]['_DATA'],data)
            self.assertEqual(result[3],[relocation])
            bad=copy.deepcopy(recipe)
            bad['secondary_dgroup_segments']['_DATA']['expected_relocations']=[]
            with self.assertRaises(ValueError):
                bind_secondary(obj,bad,image,[relocation],obj.segment_bytes('UNIT_TEXT'),
                               [f for f in obj.linker_fixups if f['segment']=='UNIT_TEXT'])
            bad=copy.deepcopy(recipe);bad['secondary_external_targets']={'code':[],'data':[]}
            with self.assertRaises(ValueError):
                bind_secondary(obj,bad,image,[relocation],obj.segment_bytes('UNIT_TEXT'),
                               [f for f in obj.linker_fixups if f['segment']=='UNIT_TEXT'])

    def test_manifest_partition_and_parent_claim(self):
        size=len(self.image)
        manifest={'owners':[
            {'id':'raw_left','kind':'UNRESOLVED_RAW','start':0,'end':self.start},
            {'id':'code','name':'f','kind':'MATCHING_C','start':self.start,
             'end':self.recipe['end'],'recipe':'recipes/f.json'},
            {'id':'raw_right','kind':'UNRESOLVED_RAW',
             'start':self.recipe['end'],'end':size},
        ]}
        staged=attach_secondary(manifest,self.recipe,self.image)
        validate_layout(staged,size)
        owner,=[o for o in staged['owners'] if o['kind']=='MATCHING_C_DATA']
        self.assertEqual(owner['parent'],'code')
        self.assertEqual(owner['start'],self.data_start)
        subsuming=copy.deepcopy(self.recipe)
        subsuming['subsumed_data_owners']=[owner['id']]
        self.assertEqual(attach_secondary(staged,subsuming,self.image),staged)
        with self.assertRaises(ValueError):
            attach_secondary(staged,self.recipe,self.image)
        altered=copy.deepcopy(staged)
        next(o for o in altered['owners'] if o['kind']=='MATCHING_C_DATA')['parent']='wrong'
        with self.assertRaises(ValueError):validate_layout(altered,size)
        altered=copy.deepcopy(staged)
        next(o for o in altered['owners'] if o['id']=='code')['data_intervals']=[]
        with self.assertRaises(ValueError):validate_layout(altered,size)
        altered=copy.deepcopy(manifest)
        altered['owners'][2]['kind']='KNOWN_TOOLCHAIN_LIBRARY'
        with self.assertRaises(ValueError):attach_secondary(altered,self.recipe,self.image)

    def test_bss_partition_and_duplicate_claim_rejected(self):
        size=len(self.image)
        layout=read_json(ROOT/'layout/data-symbols.json')
        start=layout['bss_start']+32
        recipe={'id':'f','start':self.start,'end':self.recipe['end'],
                'secondary_dgroup_segments':{'_BSS':{
                    'start':start,'end':start+2,
                    'dgroup_offset':start-layout['frame_load_address'],
                    'target':identity(bytes(2))}}}
        manifest={'owners':[
            {'id':'raw_left','kind':'UNRESOLVED_RAW','start':0,'end':self.start},
            {'id':'code','name':'f','kind':'MATCHING_C','start':self.start,
             'end':self.recipe['end'],'recipe':'recipes/f.json'},
            {'id':'raw_right','kind':'UNRESOLVED_RAW',
             'start':self.recipe['end'],'end':size},
        ]}
        staged=attach_secondary(manifest,recipe,self.image)
        validate_layout(staged,size)
        self.assertEqual([o['kind'] for o in staged['bss_owners']],
                         ['UNRESOLVED_RAW','MATCHING_C_DATA','UNRESOLVED_RAW'])
        altered=copy.deepcopy(staged)
        altered['bss_owners'][1]['end']+=1
        with self.assertRaises(ValueError):validate_layout(altered,size)
        altered=copy.deepcopy(staged)
        next(o for o in altered['owners'] if o['id']=='code')['data_intervals'] *= 2
        with self.assertRaises(ValueError):validate_layout(altered,size)


class CodePointerPairTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.obj,_=compile_source(
            b'extern void far cb(void); extern void far setcb(void (far*)(void)); '
            b'extern unsigned char flag; void far f(void) { flag=1; setcb(&cb); }',
            'msc510-medium')
        cls.start=0x15000
        cls.symbols={
            '_setcb':{'kind':'far-code','frame_load_address':0x1ea20,
                      'load_address':0x202aa},
            '_cb':{'kind':'far-code','frame_load_address':0x174b0,
                   'load_address':0x19b12},
            '_flag':{'group':'DGROUP','frame_load_address':0x2b770,
                     'load_address':0x34d06,'allowed_addends':[0]},
        }
        sites=[cls.start+f['offset']+(2 if f['loc']=='pointer32' else 0)
               for f in cls.obj.linker_fixups
               if f['loc'] in ('pointer32','base16')]
        cls.relocations=[{'segment':(site//65536)*4096,
                          'offset':site%65536,'load_offset':site} for site in sites]

    def bind(self,obj=None,symbols=None,relocations=None):
        obj=obj or self.obj
        return bind_mixed_far_data(obj,'UNIT_TEXT','_f',22,obj.linker_fixups,
            {'segments':obj.segment_defs,'groups':obj.groups,
             'publics':obj.publics,'externals':obj.externals},
            symbols or self.symbols,self.start,
            self.relocations if relocations is None else relocations,
            code_pointers=True)

    def test_paired_code_pointer_and_ordered_relocations(self):
        payload,receipt=self.bind()
        self.assertEqual(receipt['generated_relocations'],self.relocations)
        self.assertEqual(payload[6:8],(0x19b12-0x174b0).to_bytes(2,'little'))
        self.assertEqual(payload[9:11],(0x174b0//16).to_bytes(2,'little'))

    def test_unpaired_wrong_kind_and_relocation_rejected(self):
        obj=copy.deepcopy(self.obj);obj.linker_fixups[1]['target']='_setcb'
        with self.assertRaises(ValueError):self.bind(obj=obj)
        obj=copy.deepcopy(self.obj);obj.linker_fixups[1]['loc']='offset16'
        with self.assertRaises(ValueError):self.bind(obj=obj)
        with self.assertRaises(ValueError):self.bind(relocations=list(reversed(self.relocations)))
        bad=copy.deepcopy(self.symbols);bad['_cb']['kind']='near-code'
        with self.assertRaises(ValueError):self.bind(symbols=bad)


class AliasGroundingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle=verify(write=False)
        cls.image=MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations=oracle[2]['unpacked_mz']['relocations']

    def test_callback_call_and_partial_entry_aliases_are_grounded(self):
        names={'_do_joy_restext','_video_set_mode4','_sprite_make_wnd',
               '_ported_sprite_clear_1_color_'}
        resolved=resolve_code_symbols(names,self.image,self.relocations)
        self.assertEqual(resolved['_sprite_make_wnd']['load_address'],150540)
        self.assertEqual(resolved['_ported_sprite_clear_1_color_']['load_address'],144064)
        original=read_json(ROOT/'layout/code-symbols.json')
        import code_symbols
        reader=code_symbols.read_json
        for name,key,change in [
            ('_do_joy_restext','pointer_anchors',lambda row:row[0].update(hex='b80000ba0000')),
            ('_video_set_mode4','anchors',lambda row:row[0].update(hex='9a00000000')),
            ('_sprite_make_wnd','entry_proof',lambda row:row.update(source_line=14156)),
        ]:
            altered=copy.deepcopy(original)
            change(altered['symbols'][name][key])
            def fake(path):
                return altered if str(path).endswith('code-symbols.json') else reader(path)
            with self.subTest(name=name),patch('code_symbols.read_json',side_effect=fake):
                with self.assertRaises(ValueError):
                    resolve_code_symbols({name},self.image,self.relocations)

    def test_indexed_ascii_string_and_far_pointer_extents(self):
        names={'_g_ascii_props','_audiodriverstring','_smouspriteptr',
               '_mmouspriteptr','_mouseunkspriteptr'}
        resolved=resolve_symbols(names,self.image,self.relocations)
        self.assertEqual(len(resolved['_g_ascii_props']['allowed_addends']),256)
        self.assertEqual(resolved['_audiodriverstring']['allowed_addends'],[0,1,2,3,4])
        self.assertEqual(resolved['_smouspriteptr']['allowed_addends'],[0,1,2,3])
        import data_symbols
        reader=data_symbols.read_json
        original=read_json(ROOT/'layout/data-symbols.json')
        for name,width in [('_g_ascii_props',257),('_audiodriverstring',6),
                           ('_mmouspriteptr',2),('_smouspriteptr',2),
                           ('_mouseunkspriteptr',2)]:
            altered=copy.deepcopy(original)
            altered['symbols'][name]['width']=width
            def fake(path):
                return altered if str(path).endswith('data-symbols.json') else reader(path)
            with self.subTest(name=name),patch('data_symbols.read_json',side_effect=fake):
                with self.assertRaises(ValueError):
                    resolve_symbols({name},self.image,self.relocations)

    def test_secondary_placement_uses_verified_dgroup_startup(self):
        layout=checked_dgroup_layout(self.image,self.relocations)
        self.assertEqual(layout['frame_load_address'],0x2b770)
        with self.assertRaises(ValueError):
            checked_dgroup_layout(self.image,[])
        changed=bytearray(self.image)
        changed[layout['frame_relocation']['load_offset']]+=1
        with self.assertRaises(ValueError):
            checked_dgroup_layout(changed,self.relocations)


if __name__=='__main__':unittest.main()
