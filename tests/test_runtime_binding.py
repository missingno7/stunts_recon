"""Complete pinned runtime binding, storage, relocations and publication guards."""
import copy
import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import sha, read_json, json_bytes
from omf import OmfReader
from oracle import verify
from mz import MZ
from object_probe import read_object, _iterated_data
from runtime_link_probe import reviewed_policy
from runtime_binding import declarations
from library import bind_library
import promote_runtime as promoter
import transaction
from test_pipeline import record


def runtime_owner(module, public, start, end, archive='MLIBCR.LIB', data=None, storage=None):
    path = 'toolchain/msc510/'+archive
    archive_bytes = (ROOT/path).read_bytes()
    blobs = [b for n,b in OmfReader().split_library(archive_bytes) if n == module
             and public in [p['name'] for p in OmfReader().read(b).publics]
             and OmfReader().read(b).segment_length('_TEXT') == end-start]
    if len(blobs) != 1: raise AssertionError('Fixture member ambiguous')
    blob = blobs[0]; policy = reviewed_policy(blob)
    policy = policy if any(r['overlap_offsets'] for r in policy['records']) else None
    obj = read_object(blob,ledata_policy=policy)
    result = {'id':'test_runtime_'+public, 'name':module, 'kind':'KNOWN_TOOLCHAIN_LIBRARY',
              'classification':'RUNTIME_LIBRARY','profile':'msc510-medium',
              'library':path,'library_sha256':sha(archive_bytes),'module':module,'module_sha256':sha(blob),
              'segment':'_TEXT','start':start,'end':end,'publics':obj.publics,'externals':obj.externals,
              'expected_fixups':obj.linker_fixups,'expected_relocations':[],
              'binding':{'mode':'runtime-member-v1','declarations':declarations(obj),
                         'data_bindings':data or {},'storage':storage or {}}}
    if policy: result['ledata_policy']=policy
    return result,blob


class RuntimeBindingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        exe = verify(write=False)[1]; mz = MZ.parse(exe)
        cls.image = mz.load_image(exe); cls.relocs = mz.relocations
        cls.manifest = read_json(ROOT/'layout/manifest.json')

    def owner(self,*args,**kwargs):
        owner,blob = runtime_owner(*args,**kwargs)
        active=[o for o in self.manifest['owners'] if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY'
                and o['start']==owner['start'] and o['end']==owner['end']
                and o['module_sha256']==owner['module_sha256']]
        if active:
            self.assertEqual(len(active),1)
            return copy.deepcopy(active[0]),blob
        ranges = [(owner['start'],owner['end'])]+[(s['start'],s['end']) for s in owner['binding']['storage'].values()]
        owner['expected_relocations'] = [r for r in self.relocs if any(a-1 <= r['load_offset'] < b for a,b in ranges)]
        return owner,blob

    def check(self,owner,manifest=None,relocations=None):
        payload,receipt = bind_library(owner,self.image,self.relocs if relocations is None else relocations,
                                      manifest=self.manifest if manifest is None else manifest)
        self.assertEqual(payload,self.image[owner['start']:owner['end']])
        return receipt

    def test_near_calls_resolve_accepted_exact_publics(self):
        for module,public,start,end,linked in [('itoa.asm','_itoa',123808,123835,'7901'),
                                             ('ultoa.asm','_ultoa',123836,123846,'6201')]:
            owner,_ = self.owner(module,public,start,end)
            self.assertEqual(self.check(owner)['binding']['fixups'][0]['linked'],linked)

    def test_far_method_zero_and_pinned_overlaps(self):
        owner,blob = self.owner('alshr.asm','__aFNalshr',125280,125313,'LIBH.LIB')
        with self.assertRaisesRegex(ValueError,'Overlapping LEDATA'):read_object(blob)
        receipt = self.check(owner)
        self.assertEqual(receipt['binding']['fixups'][0]['address'],125196)
        self.assertEqual(receipt['binding']['fixups'][0]['frame'],117840)
        self.assertEqual([r['load_offset'] for r in receipt['binding']['generated_relocations']],[125297])

    def test_frame_fixup_declaration_and_order_mutations_rejected(self):
        owner,_ = self.owner('alshr.asm','__aFNalshr',125280,125313,'LIBH.LIB')
        for mutate in [lambda o:o['expected_fixups'].clear(),
                       lambda o:o['expected_fixups'][0].update(frame_method=1),
                       lambda o:o['binding']['declarations']['segments'][0].update(length=32),
                       lambda o:o['expected_relocations'].clear(),
                       lambda o:o.update(end=o['end']-1),
                       lambda o:o.update(module_sha256='0'*64)]:
            bad=copy.deepcopy(owner);mutate(bad)
            with self.assertRaises(ValueError):self.check(bad)

    def test_target_cannot_be_relabelled_or_shifted(self):
        owner,_=self.owner('itoa.asm','_itoa',123808,123835)
        bad=copy.deepcopy(self.manifest)
        target=next(o for o in bad['owners'] if o['id']=='library_xtoa')
        next(p for p in target['publics'] if p['name']=='__cxtoa')['offset']+=1
        with self.assertRaises(ValueError):self.check(owner,bad)

    def test_mixed_internal_near_and_grounded_data(self):
        owner,_=self.owner('dos\\brkctl.asm','_brkctl',123426,123622,data={
            '__abrktb':'_crtsp1','__abrkp':'_word_3EDCA','__abrktbe':'_word_3EDCA',
            '__asizds':'_word_3ED74','__psp':'_word_2EDEB'})
        self.check(owner)
        bad=copy.deepcopy(owner);bad['binding']['data_bindings']['__abrktb']='_word_3EDCA'
        with self.assertRaises(ValueError):self.check(bad)

    def test_stackavail_data_only_and_missing_anchor(self):
        owner,_=self.owner('stackava.asm','_stackavail',122962,122982,data={'STKHQQ':'_word_3EE24'})
        self.check(owner)
        owner['binding']['data_bindings']['STKHQQ']='_nonexistent_runtime_data'
        with self.assertRaises(ValueError):self.check(owner)

    def test_external_frame_two_unique_data_and_lidata_dependency(self):
        brk,_=self.owner('dos\\brkctl.asm','_brkctl',123426,123622,data={
            '__abrktb':'_crtsp1','__abrkp':'_word_3EDCA','__abrktbe':'_word_3EDCA',
            '__asizds':'_word_3ED74','__psp':'_word_2EDEB'})
        amalloc,_=self.owner('amalloc.asm','__amalloc',123070,123425,storage={'_DATA':{
            'start':192376,'end':192387,'ownership':'proven-raw','anchor':{'kind':'unique-literal'}}})
        manifest=copy.deepcopy(self.manifest)
        # Replace only the fixture's corresponding active runtime rows, if promoted.
        manifest['owners']=[o for o in manifest['owners'] if not
                            (o['kind']=='KNOWN_TOOLCHAIN_LIBRARY' and o['start'] in (brk['start'],amalloc['start']))]
        manifest['owners'] += [brk,amalloc]
        self.check(amalloc,manifest)
        malloc,_=self.owner('nmalloc.asm','_malloc',122982,123070,storage={'_DATA':{
            'start':192366,'end':192376,'ownership':'proven-raw',
            'anchor':{'kind':'data-alias','symbol':'_word_3EF6E','offset':0}}})
        self.check(malloc,manifest)
        bad=copy.deepcopy(amalloc)
        bad['binding']['storage']['_DATA'].update(start=176582,end=176593)
        with self.assertRaisesRegex(ValueError,'grounded group'):self.check(bad,manifest)

    def test_reviewed_pointer_field_and_excess_addend(self):
        from data_symbols import resolve_symbols
        symbol=resolve_symbols({'_word_3EE0E'},self.image,self.relocs)['_word_3EE0E']
        self.assertEqual(symbol['allowed_addends'],[0,2])
        self.assertNotIn(4,symbol['allowed_addends'])
        from runtime_binding import bind_member
        owner,blob=self.owner('stackava.asm','_stackavail',122962,122982,data={'STKHQQ':'_word_3EE24'})
        obj=read_object(blob)
        obj.linker_fixups[0]['displacement']=2
        owner['expected_fixups']=copy.deepcopy(obj.linker_fixups)
        with self.assertRaisesRegex(ValueError,'exceeds grounded object'):
            bind_member(owner,obj,self.image,self.relocs,self.manifest,(owner['id'],))

    def test_all_ordered_relocations_and_same_target_frame_two_are_required(self):
        from runtime_binding import bind_member
        owner,blob=self.owner('oldnorm.asm','__lmul',124914,124924)
        dep,_=self.owner('almul.asm','__aFNalmul',125244,125280,'LIBH.LIB')
        manifest=copy.deepcopy(self.manifest)
        manifest['owners']=[o for o in manifest['owners'] if not
                            (o['kind']=='KNOWN_TOOLCHAIN_LIBRARY' and o['start']==dep['start'])]+[dep]
        self.check(owner,manifest)
        owner['expected_relocations'].reverse()
        with self.assertRaisesRegex(ValueError,'ordered runtime MZ'):self.check(owner,manifest)
        owner,blob=self.owner('itoa.asm','_itoa',123808,123835)
        obj=read_object(blob);fix=obj.linker_fixups[0]
        fix.update(frame_method=2,frame_kind='external',frame='wrong',frame_index=fix['target_index'])
        owner['expected_fixups']=copy.deepcopy(obj.linker_fixups)
        with self.assertRaisesRegex(ValueError,'same exact target'):
            bind_member(owner,obj,self.image,self.relocs,self.manifest,(owner['id'],))

    def test_complete_secondary_data_stays_explicit_raw(self):
        storage={'_DATA':{'start':192672,'end':192676,'ownership':'proven-raw',
                 'anchor':{'kind':'data-alias','symbol':'_word_3F0A0','offset':0}}}
        owner,_=self.owner('rand.c','_rand',124476,124534,storage=storage)
        receipt=self.check(owner)
        self.assertEqual(receipt['binding']['storage'][1]['ownership'],'proven-raw')
        for mutate in [lambda o:o['binding']['storage'].clear(),
                       lambda o:o['binding']['storage']['_DATA'].update(end=192675),
                       lambda o:o['binding']['storage']['_DATA'].update(start=192673,end=192677)]:
            bad=copy.deepcopy(owner);mutate(bad)
            with self.assertRaises(ValueError):self.check(bad)
        bad=copy.deepcopy(self.manifest)
        next(o for o in bad['owners'] if o['start']<=192672<o['end'])['kind']='MATCHING_C'
        with self.assertRaisesRegex(ValueError,'raw-owned'):self.check(owner,bad)

    def test_declared_bss_is_accounted_for_and_bounded(self):
        from runtime_binding import storage_placements
        owner,blob=self.owner('output.c','__output',120254,122542,storage={
            '_DATA':{'start':192346,'end':192365,'ownership':'proven-raw',
                     'anchor':{'kind':'unique-literal'}},
            '_BSS':{'start':207000,'end':207038,'ownership':'proven-raw',
                    'anchor':{'kind':'data-alias','symbol':'_off_4289A','offset':2}}})
        obj=read_object(blob)
        places=storage_placements(owner,obj,self.image,self.relocs,self.manifest)
        self.assertEqual(places['_BSS']['end']-places['_BSS']['start'],38)
        bad=copy.deepcopy(owner);bad['binding']['storage'].pop('_BSS')
        with self.assertRaisesRegex(ValueError,'Every nonzero runtime'):
            storage_placements(bad,obj,self.image,self.relocs,self.manifest)
        bad=copy.deepcopy(owner);bad['binding']['storage']['_BSS'].update(start=223000,end=223038)
        with self.assertRaises(ValueError):storage_placements(bad,obj,self.image,self.relocs,self.manifest)

    def test_secondary_data_corruption_cannot_hide_in_raw(self):
        owner,_=self.owner('rand.c','_rand',124476,124534,storage={'_DATA':{
            'start':192672,'end':192676,'ownership':'proven-raw',
            'anchor':{'kind':'data-alias','symbol':'_word_3F0A0','offset':0}}})
        bad=bytearray(self.image);bad[192672]^=1
        with self.assertRaises(ValueError):bind_library(owner,bytes(bad),self.relocs,manifest=self.manifest)

    def test_raw_replace_preserves_all_accepted_owners(self):
        m={'owners':[{'id':'c','kind':'MATCHING_C','start':0,'end':2},
                     {'id':'r','kind':'UNRESOLVED_RAW','start':2,'end':10}]}
        candidate={'id':'lib','kind':'KNOWN_TOOLCHAIN_LIBRARY','start':4,'end':8}
        staged=promoter.replace_raw_library(m,candidate)
        self.assertEqual(staged['owners'][0],m['owners'][0])
        self.assertEqual([(o['start'],o['end']) for o in staged['owners']],[(0,2),(2,4),(4,8),(8,10)])
        candidate['start']=1
        with self.assertRaises(ValueError):promoter.replace_raw_library(m,candidate)

    def test_runtime_alias_uses_exact_public_when_inventory_end_overruns_member(self):
        from code_symbols import _complete_target_owner
        owner,_=self.owner('rand.c','_rand',124476,124534,storage={'_DATA':{
            'start':192672,'end':192676,'ownership':'proven-raw',
            'anchor':{'kind':'data-alias','symbol':'_word_3F0A0','offset':0}}})
        target=next(f for f in read_json(ROOT/'evidence/functions.json')['functions']
                    if f['name']=='_rand')
        self.assertEqual((target['start'],target['end']),(124494,124540))
        self.assertTrue(_complete_target_owner(owner,target))
        bad=copy.deepcopy(owner)
        next(p for p in bad['publics'] if p['name']=='_rand')['offset']+=1
        self.assertFalse(_complete_target_owner(bad,target))
        unknown=next(f for f in read_json(ROOT/'evidence/functions.json')['functions']
                     if f['name']=='unknown_libname_4')
        helper,_=self.owner('alshr.asm','__aFNalshr',125280,125313,'LIBH.LIB')
        self.assertTrue(_complete_target_owner(helper,unknown))


class IteratedRuntimeTests(unittest.TestCase):
    def fixture(self,block,length=6,extra=b''):
        return b''.join([record(0x80,b'\x01x'),record(0x96,b'\x05_TEXT\x04CODE'),
            record(0x98,b'\x48'+struct.pack('<H',length)+b'\x01\x02\0'),
            record(0xa2,b'\x01\0\0'+block),extra,record(0x8a,b'\0')])

    def test_complete_nested_lidata(self):
        block=struct.pack('<HH',2,1)+struct.pack('<HHB',3,0,1)+b'Z'
        self.assertEqual(read_object(self.fixture(block)).segment_bytes('_TEXT'),b'ZZZZZZ')

    def test_malformed_oversized_and_fixup_lidata_rejected(self):
        good=struct.pack('<HHB',6,0,1)+b'Z'
        for block in [good[:-1],struct.pack('<HHB',65535,0,2)+b'XY']:
            with self.assertRaises(ValueError):read_object(self.fixture(block))
        with self.assertRaisesRegex(ValueError,'iterated LIDATA'):
            read_object(self.fixture(good,extra=record(0x9c,b'\xc4\0\x54\1')))
        with self.assertRaisesRegex(ValueError,'Holes or overflow'):
            read_object(self.fixture(good,length=7))

    def test_crt_absolute_marker_is_narrow_and_cannot_bind_storage(self):
        archive=(ROOT/'toolchain/msc510/MLIBCR.LIB').read_bytes()
        blob=next(b for n,b in OmfReader().split_library(archive) if n=='dos\\crt0msg.asm')
        obj=read_object(blob)
        marker=next(p for p in obj.publics if p['segment']=='?0')
        self.assertEqual((marker['name'],marker['offset']),('__acrtmsg',0x9876))
        records=[]
        for kind,body in OmfReader.records(blob):
            if kind==0x90 and b'__acrtmsg' in body:
                body=body.replace(b'\x76\x98',b'\x00\x00')
            records.append(record(kind,body))
        with self.assertRaisesRegex(ValueError,'Unknown absolute runtime public'):
            read_object(b''.join(records))


class RuntimePublicationTests(unittest.TestCase):
    def exercise(self,fail=False):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as temp:
            root=Path(temp);(root/'layout').mkdir();(root/'build').mkdir()
            manifest={'owners':[{'id':'r','kind':'UNRESOLVED_RAW','start':0,'end':10}]}
            path=root/'layout/manifest.json';path.write_bytes(json_bytes(manifest));original=path.read_bytes()
            candidate=root/'candidate.json';candidate.write_bytes(json_bytes({'id':'lib','kind':'KNOWN_TOOLCHAIN_LIBRARY','start':2,'end':4}))
            def snapshot():return {'layout/manifest.json':sha(path.read_bytes())}
            def fake_build(*args,**kwargs):
                if kwargs.get('allow_pending') and fail:raise ValueError('canonical failure')
                if 'artifact' in kwargs:kwargs['artifact']['executable']=b'fixture'
                return {'inputs':snapshot(),'executable':{'sha256':'fixture'},'relocation_count':0}
            with patch.object(transaction,'ROOT',root),patch.object(promoter,'ROOT',root),\
                 patch.object(promoter,'inputs',side_effect=snapshot),\
                 patch.object(promoter,'verify',return_value=(None,b'',None)),\
                 patch.object(promoter.MZ,'parse',return_value=SimpleNamespace(load_image=lambda x:bytes(10),relocations=[])),\
                 patch.object(promoter,'bind_library',return_value=(b'XX',{})),\
                 patch.object(promoter,'build',side_effect=fake_build):
                if fail:
                    with self.assertRaisesRegex(ValueError,'canonical failure'):promoter.promote_runtime(candidate)
                    self.assertEqual(path.read_bytes(),original)
                else:
                    result=promoter.promote_runtime(candidate,verify_only=True)
                    self.assertEqual(result['status'],'VERIFIED_ONLY');self.assertEqual(path.read_bytes(),original)
                    result=promoter.promote_runtime(candidate)
                    self.assertEqual(result['status'],'PROMOTED')
                    self.assertEqual(read_json(path)['owners'][1]['id'],'lib')
                self.assertFalse((root/'build/publication.json').exists())

    def test_verify_only_and_transaction_publication(self):self.exercise()
    def test_canonical_failure_rolls_back_runtime_manifest(self):self.exercise(fail=True)


if __name__ == '__main__':unittest.main()
