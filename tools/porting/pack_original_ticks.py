"""Pack references ONLY from a sealed DOSBox-X original capture.

Never reads a port trace or comparison. Full stream hashes retain failed
comparisons; narrowly scoped reference subsets are labelled explicitly.
"""
import argparse
import hashlib
import json
from pathlib import Path
import zlib
try:
    from .compare_tick_streams import original_streams,read_rows,FRAME_BYTES
    from .capture_storage import byte_size,discard_raw_frames
except ImportError:
    from compare_tick_streams import original_streams,read_rows,FRAME_BYTES
    from capture_storage import byte_size,discard_raw_frames


def canonical_hash(value):
    return hashlib.sha256(json.dumps(value,sort_keys=True,separators=(',',':')).encode()).hexdigest()


def pack(folder,scenario):
    receipt=json.loads((folder/'receipt.json').read_text(encoding='utf-8'))
    if receipt.get('schema')!='stunts-dosbox-tick-receipt-v1' or receipt['returncode']!=0:
        raise ValueError('cannot pack an incomplete/non-DOSBox capture')
    for name,field in [('original.jsonl','capture_sha256'),('original.frames','frames_sha256'),
                       ('original.exe','executable_sha256'),('bindings.json','bindings_sha256')]:
        if hashlib.sha256((folder/name).read_bytes()).hexdigest()!=receipt[field]:
            raise ValueError('unsealed original capture: '+name)
    streams=original_streams(folder/'original')
    audio=receipt['audio'];rows=read_rows(folder/'original.jsonl')
    first=next(r for r in rows if r['kind']=='frame')
    ordinal=3 if audio=='ad15' else 4
    checkpoint=next(r for r in rows if r['kind']=='frame' and r['ordinal']==ordinal)
    with (folder/'original.frames').open('rb') as f:
        f.seek(first['payload']*FRAME_BYTES);payload=f.read(FRAME_BYTES)
        f.seek(checkpoint['payload']*FRAME_BYTES);checkpoint_payload=f.read(FRAME_BYTES)
    return dict(schema='stunts-dosbox-tick-fixture-v1',scenario=scenario,audio=audio,
        script=f'inputs/{scenario}.json',ticks=receipt['requested_ticks'],
        provenance={k:receipt[k] for k in ('executable_sha256','oracle_lock_sha256','dosbox_sha256',
            'observer_sha256','bindings_sha256','script_sha256','driver_sha256','capture_sha256','frames_sha256')},
        stream_counts={k:len(v) for k,v in streams.items()},
        stream_sha256={k:canonical_hash(v) for k,v in streams.items()},
        # Bounded observations, never a claim that a complete scenario agrees.
        driver_initialisation=streams['sound'][:15 if audio=='ad15' else 1],
        driver_startup_observations=streams['sound'][:15 if audio=='ad15' else 12],
        first_frame=streams['frames'][0],first_frame_zlib_hex=zlib.compress(payload).hex(),
        frame_checkpoint_ordinal=ordinal,frame_checkpoint=streams['frames'][ordinal],
        frame_checkpoint_zlib_hex=zlib.compress(checkpoint_payload).hex(),
        scans=streams['scans'],keys=streams['keys'],
        # Separate content diagnostic, preserving the original ticks.
        active_states=[r for r in streams['states'] if r['game_inputmode']][:84],
        replay_play_states=[r for r in streams['states'] if 1850<=r['raw_tick']<2100]
            if scenario=='replay' else [],
        seek_tick_prefix=[r for r in streams['states'] if r['raw_tick']==2240][:25]
            if scenario=='replay' else [])


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('capture',type=Path)
    p.add_argument('--scenario',choices=('startup','selection','race','replay'),required=True)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--keep-raw',action='store_true',help='retain the uncompressed original.frames dump')
    a=p.parse_args()
    if a.out.exists():p.error('refusing to overwrite an expectation')
    fixture=pack(a.capture.resolve(),a.scenario)
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(fixture,indent=2)+'\n',encoding='utf-8')
    removed=0 if a.keep_raw else discard_raw_frames(a.capture.resolve())
    print(f"packed {a.out.stat().st_size} bytes; removed {removed} raw frame bytes; "
          f"capture directory now {byte_size(a.capture.resolve())} bytes")
