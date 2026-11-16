"""Pack sealed original DOSBox-X checkpoints; never accepts candidate frames."""
from __future__ import annotations
import argparse,json,sys,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT))
from tools.porting.compare_scenarios import original_data,sha
from tools.porting.capture_storage import byte_size,discard_raw_frames


def pack(capture,out,selections,replace=False,keep_raw=False):
    data=original_data(capture,include_boundaries=False);out.mkdir(parents=True,exist_ok=True)
    written=0
    for name,label in selections:
        if not name.replace('-','').replace('_','').isalnum():raise ValueError('invalid fixture name')
        target=out/(name+'.json')
        if target.exists() and not replace:raise FileExistsError(target)
        payload=data['payloads'][label];compressed=zlib.compress(payload,9)
        metadata=dict(schema='stunts-original-screen-v2',label=name,fixture=name,checkpoint=label,
            payload=name+'.idxz',width=320,height=200,compression='zlib',uncompressed_bytes=len(payload),
            compressed_bytes=len(compressed),payload_sha256=sha(payload),indices_sha256=sha(payload[:64000]),
            palette_rgb6_sha256=sha(payload[64000:]),original_receipt=data['receipt'])
        (out/metadata['payload']).write_bytes(compressed)
        target.write_bytes((json.dumps(metadata,indent=2,sort_keys=True)+'\n').encode())
        written+=len(compressed)+target.stat().st_size
        print(name,label,metadata['payload_sha256'])
    removed=0 if keep_raw else discard_raw_frames(capture)
    print(f"packed {written} bytes; removed {removed} raw frame bytes; "
          f"capture directory now {byte_size(capture)} bytes")


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('capture',type=Path)
    p.add_argument('selections',nargs='+',metavar='NAME=CHECKPOINT');p.add_argument('--out',type=Path,required=True)
    p.add_argument('--replace',action='store_true');p.add_argument('--keep-raw',action='store_true',
        help='retain the uncompressed original.frames dump');a=p.parse_args()
    pack(a.capture,a.out,[v.split('=',1) for v in a.selections],a.replace,a.keep_raw)

if __name__=='__main__':main()
