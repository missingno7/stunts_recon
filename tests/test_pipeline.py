import sys,struct,unittest,copy
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json,identity
from mz import MZ,header_document,encode_header
from dsi import decode
from exepack import decompress
from oracle import apply_dif,verify,construct
from coordinates import Coordinates
from object_probe import read_object,extract_no_fixups
from omf import OmfReader
from build_exact import validate_layout,inputs
from check_candidate import check_scope,replace_raw


def record(kind,body):
    prefix=bytes([kind])+struct.pack('<H',len(body)+1)+body
    return prefix+bytes([-sum(prefix)&255])


def object_fixture(extra=b'',code=b'\x90\x90\xcb'):
    return b''.join([record(0x80,b'\x01x'),record(0x96,b'\x05_TEXT\x04CODE'),
      record(0x98,b'\x48'+struct.pack('<H',len(code))+b'\x01\x02\0'),
      record(0x90,b'\0\x01\x01f\0\0\0'),record(0xa0,b'\x01\0\0'+code),extra,record(0x8a,b'\0')])


class CodecTests(unittest.TestCase):
    def test_huffman(self):
        self.assertEqual(decode(b'\x02\x04\0\0\x01\x02AB\x60'),b'ABBA')
    def test_huffman_delta(self):
        self.assertEqual(decode(b'\x02\x04\0\0\x81\x02\0\x01\xf0'),b'\x01\x02\x03\x04')
    def test_huffman_truncation(self):
        with self.assertRaises(ValueError):decode(b'\x02\x04\0\0\x01\x02AB')
    def test_huffman_bad_tree(self):
        with self.assertRaises(ValueError):decode(b'\x02\x04\0\0\x01\x03ABC\0')
    def test_huffman_unsupported(self):
        with self.assertRaises(ValueError):decode(b'\x01'+bytes(10))
    def test_dif_two_and_four(self):
        output,records=apply_dif(bytes(8),b'\x01\x80abcd\x04\0EF\0\0')
        self.assertEqual(output,b'abcdEF\0\0');self.assertEqual(records[1]['packed_load_offset'],4)
    def test_dif_fail_closed(self):
        for data in [b'',b'\1\0x',b'\0\0x',b'\xff\x7faa\0\0']:
            with self.subTest(data=data),self.assertRaises(ValueError):apply_dif(bytes(8),data)
    def test_exepack_copy(self):
        self.assertEqual(decompress(b'ABC\x03\0\xb3',3)[0],b'ABC')
    def test_exepack_prefix_fill(self):
        self.assertEqual(decompress(b'xyzZ\x04\0\xb1',7)[0],b'xyzZZZZ')
    def test_exepack_padding(self):
        self.assertEqual(decompress(b'ABC\x03\0\xb3'+b'\xff'*15,3)[0],b'ABC')
    def test_exepack_bad_command(self):
        for data,size in [(b'ABC\x03\0\xb4',3),(b'A\xff\xff\xb3',3),(b'',3)]:
            with self.subTest(data=data),self.assertRaises(ValueError):decompress(data,size)


class ObjectTests(unittest.TestCase):
    def test_extract(self):
        o=read_object(object_fixture());self.assertEqual(extract_no_fixups(o,'_TEXT','f',3),b'\x90\x90\xcb')
        self.assertEqual(o.segment_defs[0]['class'],'CODE')
    def test_truncated(self):
        for cut in [1,2,5]:
            with self.assertRaises(ValueError):read_object(object_fixture()[:-cut])
    def test_checksum(self):
        data=bytearray(object_fixture());data[4]^=1
        with self.assertRaises(ValueError):read_object(bytes(data))
    def test_bakpat_rejected(self):
        with self.assertRaises(ValueError):read_object(object_fixture(record(0xb2,b'\x01\x01\0\0\x01\0')))
    def test_communal_rejected(self):
        with self.assertRaises(ValueError):read_object(object_fixture(record(0xb0,b'')))
    def test_overlap_rejected(self):
        with self.assertRaises(ValueError):read_object(object_fixture(record(0xa0,b'\x01\0\0\x90')))
    def test_big_bss_rejected(self):
        extra=record(0x96,b'\x04_BSS')+record(0x98,b'\x4a\0\0\x03\x02\0')
        with self.assertRaises(ValueError):read_object(object_fixture(extra))
    def test_no_object_trimming(self):
        with self.assertRaises(ValueError):extract_no_fixups(read_object(object_fixture()),'_TEXT','f',2)
    def test_public_binding(self):
        with self.assertRaises(ValueError):extract_no_fixups(read_object(object_fixture()),'_TEXT','wrong',3)
    def test_fixup_blocks_production(self):
        o=read_object(object_fixture());o.linker_fixups=[{'offset':0}]
        with self.assertRaises(ValueError):extract_no_fixups(o,'_TEXT','f',3)
    def test_unknown_record(self):
        with self.assertRaises(ValueError):read_object(object_fixture(record(0x99,b'')))


class OracleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.result=verify(write=False)
    def test_lock(self):self.assertEqual(self.result[2],read_json(ROOT/'layout/oracle.lock.json'))
    def test_deterministic(self):self.assertEqual(construct()[0:3],self.result[0:3])
    def test_header_roundtrip(self):
        for data in self.result[:2]:
            m=MZ.parse(data);self.assertEqual(encode_header(header_document(data),len(data)),data[:m.header_size])
    def test_mz_bounds(self):
        for data in [b'MZ',b'XX'+self.result[0][2:],self.result[0][:-1]]:
            with self.assertRaises(ValueError):MZ.parse(data)
    def test_relocation_count(self):self.assertEqual(len(MZ.parse(self.result[1]).relocations),2588)
    def test_coordinate_spaces(self):
        c=Coordinates(200000,10384,512)
        for offset in [0,684,0x1695e,199999]:
            for space in ['load_image','unpacked_mz_file','restunts_ida']:
                self.assertEqual(c.load(c.from_load(offset,space),space),offset)
        self.assertEqual(c.load(0x142a,'segment_offset',0x270d),0x284fa)
        for space in ['distributed_file','combined_packed_file','driver_integrated']:
            with self.assertRaises(ValueError):c.load(0,space)
    def test_coordinate_out_of_range(self):
        c=Coordinates(200000,10384,512)
        with self.assertRaises(ValueError):c.load(10383,'unpacked_mz_file')
        with self.assertRaises(ValueError):c.from_load(200000,'restunts_ida')
    def test_independent_unpack(self):
        d=read_json(ROOT/'recovery/unp-crosscheck.json')
        self.assertTrue(d['load_image_equal']);self.assertTrue(d['relocation_raw_equal'])
        self.assertEqual(d['load_image_sha256'],self.result[2]['load_image']['sha256'])
    def test_restunts_anchors(self):
        d=read_json(ROOT/'recovery/restunts-inventory.json');image=MZ.parse(self.result[1]).load_image(self.result[1])
        self.assertGreater(len(d['coordinate_proof']['binary_anchors']),700)
        for a in d['coordinate_proof']['binary_anchors']:
            self.assertEqual(identity(image[a['load_start']:a['end']])['sha256'],a['sha256'])
        self.assertEqual(d['coordinate_proof']['differences'],[{'offset':684,'oracle':0,'reference':1}])
    def test_mutated_lock_refused(self):
        bad=copy.deepcopy(self.result[2]);bad['load_image']['sha256']='0'*64
        with patch('oracle.read_json',return_value=bad),self.assertRaises(ValueError):verify(write=False)


class AcceptanceTests(unittest.TestCase):
    def test_partition(self):validate_layout(read_json(ROOT/'layout/manifest.json'),200000)
    def test_partition_rejects_gap(self):
        m=read_json(ROOT/'layout/manifest.json');m['owners'][0]['start']=1
        with self.assertRaises(ValueError):validate_layout(m,200000)
    def test_duplicate_owner_ids(self):
        m=read_json(ROOT/'layout/manifest.json');m['owners'][1]['id']=m['owners'][0]['id']
        with self.assertRaises(ValueError):validate_layout(m,200000)
    def test_promotion_rechecks_fast_baseline_and_cleans_lock(self):
        import tempfile,check_candidate,json
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as directory:
            temp=Path(directory);(temp/'recipes').mkdir();(temp/'build').mkdir()
            (temp/'recipes/x.json').write_text(json.dumps({'source':'unused'}))
            with patch('check_candidate.ROOT',temp),patch('check_candidate.probe',return_value=(b'x',{})),patch('check_candidate.inputs',side_effect=[{}, {}, {'changed':'yes'}]),self.assertRaises(ValueError):
                check_candidate.check('x',promote=True,scope=False)
            self.assertFalse((temp/'build/promotion.lock').exists())
    def test_scope(self):
        snapshot={'src/a.c':'1','tools/x.py':'2'};recipe={'source':'src/a.c'}
        with patch('check_candidate.inputs',return_value={'src/a.c':'changed','tools/x.py':'2'}):check_scope({'scope_snapshot':snapshot},recipe)
        with patch('check_candidate.inputs',return_value={'src/a.c':'1','tools/x.py':'changed'}),self.assertRaises(ValueError):check_scope({'scope_snapshot':snapshot},recipe)
    def test_double_promotion_refused(self):
        with self.assertRaises(ValueError):replace_raw(read_json(ROOT/'layout/manifest.json'),read_json(ROOT/'recipes/rect_is_inside.json'))
    def test_queue_unique(self):
        q=read_json(ROOT/'recovery/queue.json');self.assertEqual(len(q['tasks']),len({x['id'] for x in q['tasks']}));self.assertEqual(len(q['tasks']),sum(q['counts'].values()))
    def test_failed_build_invalidates_receipt(self):
        import tempfile,build_exact
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as directory:
            temp=Path(directory);out=temp/'build/exact';out.mkdir(parents=True)
            receipt=out/'acceptance.json';receipt.write_text('stale PASS')
            bad=read_json(ROOT/'layout/manifest.json');bad['owners'][0]['start']=1
            with patch('build_exact.ROOT',temp),self.assertRaises(ValueError):build_exact.build(bad)
            self.assertFalse(receipt.exists())
    def test_real_msc_external_far_call_fixup(self):
        from compiler import compile_source
        obj,receipt=compile_source(b'extern int helper(int); int use_helper(int x) { return helper(x); }\n','msc510-medium')
        fix=[f for f in obj.linker_fixups if f['target']=='_helper']
        self.assertEqual(len(fix),1)
        self.assertEqual((fix[0]['loc'],fix[0]['width'],fix[0]['encoded_addend']),('pointer32',4,'00000000'))
        self.assertEqual(fix[0]['target_kind'],'external')
        with self.assertRaises(ValueError):extract_no_fixups(obj,'UNIT_TEXT','_use_helper',obj.segment_length('UNIT_TEXT'))
    def test_toolchain_hash_mismatch(self):
        from compiler import verify_toolchain
        bad=read_json(ROOT/'layout/toolchain.json');bad['profiles']['msc510-medium']['files'][0]['sha256']='0'*64
        with patch('compiler.read_json',return_value=bad),self.assertRaises(ValueError):verify_toolchain('msc510-medium')
    def test_full_fresh_hybrid(self):
        from build_exact import build
        result=build()
        self.assertEqual(result['status'],'HYBRID_EXACT')
        owners=read_json(ROOT/'layout/manifest.json')['owners']
        totals={kind:sum(o['end']-o['start'] for o in owners if o['kind']==kind)
                for kind in ['MATCHING_C','KNOWN_TOOLCHAIN_LIBRARY','UNRESOLVED_RAW']}
        self.assertEqual(result['matching_c_bytes'],totals['MATCHING_C'])
        self.assertEqual(result['library_production_bytes'],totals['KNOWN_TOOLCHAIN_LIBRARY'])
        self.assertEqual(result['raw_initialized_bytes'],totals['UNRESOLVED_RAW'])
        self.assertGreaterEqual(result['matching_c_bytes'],206)
        self.assertGreaterEqual(result['library_production_bytes'],725)
        self.assertEqual(len([r for r in result['compiler_receipts'] if 'work_directory' in r]),sum(o['kind']=='MATCHING_C' for o in owners))
    def test_stale_inputs_rejected(self):
        from build_exact import build
        raw=read_json(ROOT/'layout/manifest.json')
        raw['owners']=[{'id':'raw','start':0,'end':200000,'kind':'UNRESOLVED_RAW'}]
        with patch('build_exact.inputs',side_effect=[{}, {'changed':'yes'}]),self.assertRaises(ValueError):build(raw,publish=False)

if __name__=='__main__':unittest.main()
