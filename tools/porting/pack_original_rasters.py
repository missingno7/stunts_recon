"""Extract the race's clipped horizon polygons from a sealed ORIGINAL capture."""
from __future__ import annotations
import argparse
import json
from pathlib import Path
import struct
from .compare_step_streams import original
from .compare_tick_streams import read_rows
from .capture_storage import byte_size,discard_raw_frames


def pack(folder):
    receipt=original(folder)['receipt']  # verify all original seals first
    cases=[]
    for row in read_rows(folder/'boundaries.jsonl'):
        if row.get('name')!='drawpolypt' or not row['active'] or row['frame'] not in (5,6,18,19,20,21,22):
            continue
        regions={r['name']:r for r in row['regions']}
        points=list(struct.unpack('<'+'h'*(len(regions['points']['before'])//4),bytes.fromhex(regions['points']['before'])))
        clip=list(struct.unpack('<5H',bytes.fromhex(regions['clip']['before'])))
        ys=points[1::2]
        if min(ys)>=max(ys) or max(min(ys),clip[2])!=min(max(ys),clip[3]-1):
            continue
        cases.append(dict(frame=row['frame'],name=row['name'],args=row['args'][:2],
            points=points,clip=clip,row9_before=regions['row9']['before'],row9_after=regions['row9']['after']))
    if not cases:raise ValueError('capture contains no clipped horizon cases')
    return dict(schema='stunts-original-raster-cases-v1',provenance=receipt,cases=cases)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('capture',type=Path);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--keep-raw',action='store_true',help='retain the uncompressed original.frames dump')
    a=p.parse_args()
    if a.out.exists():p.error('refusing to overwrite an expectation')
    result=pack(a.capture);a.out.write_bytes((json.dumps(result,indent=2)+'\n').encode())
    removed=0 if a.keep_raw else discard_raw_frames(a.capture)
    print(f"{len(result['cases'])} original calls: {a.out}; packed {a.out.stat().st_size} bytes; "
          f"removed {removed} raw frame bytes; capture directory now {byte_size(a.capture)} bytes")

if __name__=='__main__':main()
