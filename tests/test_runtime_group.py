"""Positive and negative proofs for the staged pinned runtime group."""
import copy
import sys
import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json
from compiler import toolchain_path
from omf import OmfReader
from object_probe import read_object
from oracle import verify
from mz import MZ
from library import bind_library
from promote_runtime import replace_raw_library
from runtime_binding import group_public, crt0dat_data_public, file_data_public
from test_runtime_binding import runtime_owner


class RuntimeGroupTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        exe=verify(write=False)[1]; mz=MZ.parse(exe)
        cls.image=mz.load_image(exe); cls.relocs=mz.relocations
        cls.manifest=read_json(ROOT/'layout/manifest.json')

    def group(self):
        rows=[]
        for module,public,start,end,storage in [
            ('dos\\crt0msg.asm','__FF_MSGBANNER',118410,118446,{
                'MSG':(199734,199864,'unique-literal'),
                '_DATA':(192028,192032,'data-alias'),
                'PAD':(199973,199975,'common-v1')}),
            ('chksum.asm','__NMSG_TEXT',118488,118526,{
                'NULL':(178032,178098,'unique-literal'),
                'MSG':(199903,199940,'unique-literal'),
                'PAD':(199973,199975,'common-v1')}),
            ('dos\\nmsghdr.asm','__NMSG_WRITE',119038,119124,{
                'HDR':(199726,199734,'unique-literal'),
                'PAD':(199973,199975,'common-v1'),
                'EPAD':(199992,199993,'common-v1')})]:
            active=[o for o in self.manifest['owners'] if o.get('module')==module
                    and o['start']==start and o['end']==end]
            if active:
                self.assertEqual(len(active),1)
                rows.append(copy.deepcopy(active[0]))
                continue
            # Some module publics are internal labels; select by the pinned
            # source's complete TEXT extent when the fixture public differs.
            archive=toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
            blob=next(b for n,b in OmfReader().split_library(archive) if n==module)
            obj=read_object(blob)
            if public not in [p['name'] for p in obj.publics]:
                public=next(p['name'] for p in obj.publics if p['segment']=='_TEXT')
            row,_=runtime_owner(module,public,start,end)
            row['binding']['mode']='runtime-owner-group-v2'
            row['binding']['storage']={}
            for name,(lo,hi,kind) in storage.items():
                anchor={'kind':kind}
                if kind=='data-alias': anchor.update(symbol='_unk_3EE1C',offset=0)
                row['binding']['storage'][name]={'start':lo,'end':hi,
                                                   'ownership':'proven-raw','anchor':anchor}
            spans=[(start,end)]+[(x['start'],x['end']) for x in row['binding']['storage'].values()]
            row['expected_relocations']=[r for r in self.relocs
                if any(a-1<=r['load_offset']<b for a,b in spans)]
            rows.append(row)
        manifest=copy.deepcopy(self.manifest)
        for row in rows:
            if not any(o['id']==row['id'] for o in manifest['owners']):
                manifest=replace_raw_library(manifest,row)
        return rows,manifest

    def test_closed_group_binds_all_three_members(self):
        rows,manifest=self.group()
        for row in rows:
            payload,proof=bind_library(row,self.image,self.relocs,manifest=manifest)
            self.assertEqual(payload,self.image[row['start']:row['end']])
            self.assertEqual(proof['binding']['mode'],'runtime-owner-group-v2')

    def test_group_public_and_complete_common_are_not_forgeable(self):
        rows,manifest=self.group()
        shifted=copy.deepcopy(manifest)
        next(p for o in shifted['owners'] if o.get('module')=='dos\\nmsghdr.asm'
             for p in o['publics'] if p['name']=='__NMSG_WRITE')['offset']+=1
        with self.assertRaises(ValueError):group_public('__NMSG_WRITE',shifted)
        missing=copy.deepcopy(manifest)
        next(o for o in missing['owners'] if o.get('module')=='dos\\nmsghdr.asm')\
            ['binding']['storage']['EPAD']['end']-=1
        with self.assertRaises(ValueError):bind_library(next(o for o in missing['owners']
            if o.get('module')=='dos\\nmsghdr.asm'),self.image,self.relocs,manifest=missing)

    def test_sparse_zero_requires_exact_tail(self):
        archive=toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
        blob=next(b for n,b in OmfReader().split_library(archive)
                  if n=='dos\\crt0dat.asm')
        with self.assertRaisesRegex(ValueError,'Holes'):
            read_object(blob)
        obj=read_object(blob,sparse_zero={'CDATA':{'initialized_prefix':2,'declared_length':14}})
        self.assertEqual(obj.segment_bytes('CDATA'),bytes(14))
        with self.assertRaises(ValueError):
            read_object(blob,sparse_zero={'CDATA':{'initialized_prefix':3,'declared_length':14}})

    def test_crt0_checksum_exception_keeps_exact_record_and_sparse_extent(self):
        archive=toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
        blob=next(b for n,b in OmfReader().split_library(archive)
                  if n=='dos\\crt0.asm')
        policy={'mode':'crt0-comment-checksum-v1',
            'module_sha256':'d5b8b4a264adea82a75056189745d9d786e81192af65e9d4713e4ab0a687a973',
            'record_offset':466,'kind':0x88,'size':4,
            'body_sha256':'c6ef173229e1869cd33073657375349eaf71a2329203c009ecd0c9c7c8410aad',
            'checksum':0xd1}
        sparse={'_DATA':{'initialized_ranges':[[0,10],[86,88]],'declared_length':88}}
        with self.assertRaisesRegex(ValueError,'checksum'):
            read_object(blob,sparse_zero=sparse)
        obj=read_object(blob,record_policy=policy,sparse_zero=sparse)
        self.assertEqual((obj.segment_length('_DATA'),len(obj.linker_fixups)),(88,27))
        bad=copy.deepcopy(policy);bad['body_sha256']='0'*64
        with self.assertRaisesRegex(ValueError,'checksum'):
            read_object(blob,record_policy=bad,sparse_zero=sparse)
        with self.assertRaisesRegex(ValueError,'Holes'):
            read_object(blob,record_policy=policy)

    def test_chkstk_end_uses_load_coordinates(self):
        frame=read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
        chk=frame+int.from_bytes(self.image[192036:192038],'little')-256
        crt=frame+int.from_bytes(self.image[117968:117970],'little')
        self.assertEqual((chk,crt),(222352,222352))
        self.assertNotEqual(frame+int.from_bytes(self.image[192036+10384:192038+10384],'little')-256,crt)

    def test_signal_complete_data_and_crt_publics(self):
        self.assertEqual(crt0dat_data_public('__child',self.image,self.relocs)['address'],192020)
        self.assertEqual(crt0dat_data_public('__fpinit',self.image,self.relocs)['address'],199710)
        row,_=runtime_owner('dos\\signal.asm','_signal',124534,124903)
        row['binding']['mode']='runtime-owner-group-v2'
        row['binding']['storage']={'_DATA':{'start':192676,'end':192704,
            'ownership':'proven-raw','anchor':{'kind':'code-field-v1',
            'site':124734,'target_offset':0,'hex':'3439'}}}
        row['binding']['data_bindings']={'__sigintseg':'_word_3EF98',
                                         '__sigintoff':'_word_3EF9A'}
        row['binding']['raw_crt0dat_publics']=['__child','__fpinit']
        row['binding']['code_offset_fixups']=[106,124,141,148,174]
        spans=[(row['start'],row['end']),(192676,192704)]
        row['expected_relocations']=[r for r in self.relocs if any(
            a-1<=r['load_offset']<b for a,b in spans)]
        active=[o for o in self.manifest['owners'] if o.get('module')=='dos\\signal.asm'
                and o['start']==124534]
        if active:
            self.assertEqual(len(active),1)
            row=copy.deepcopy(active[0]);manifest=self.manifest
        else:
            manifest=replace_raw_library(self.manifest,row)
        self.assertEqual(bind_library(row,self.image,self.relocs,manifest=manifest)[0],
                         self.image[row['start']:row['end']])
        bad=copy.deepcopy(row)
        bad['binding']['storage']['_DATA']['anchor']['hex']='3539'
        with self.assertRaisesRegex(ValueError,'code-field'):
            bind_library(bad,self.image,self.relocs,manifest=manifest)
        bad=copy.deepcopy(row)
        bad['binding']['code_offset_fixups'].remove(174)
        with self.assertRaisesRegex(ValueError,'absolute code offset'):
            bind_library(bad,self.image,self.relocs,manifest=manifest)

    def test_stdio_comdef_and_initialized_data_are_complete(self):
        self.assertEqual(file_data_public('__iob',self.image,self.relocs)['extent'],160)
        for name,address in [('__bufin',215782),('__bufout',213134),('__buferr',221650)]:
            self.assertEqual(file_data_public(name,self.image,self.relocs)['address'],address)
        bad=bytearray(self.image);bad[192080]^=1
        with self.assertRaises(ValueError):
            file_data_public('__iob',bytes(bad),self.relocs)

if __name__=='__main__':unittest.main()
