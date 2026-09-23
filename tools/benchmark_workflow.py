"""Equivalent-evidence storage/serialization costs, plus measured packet/refresh cost."""
import contextlib
import io
import json
import statistics
import time
from common import ROOT, read_json, write_json, json_bytes, require
from workflow import load_snapshot


def measure():
    from reconstruction_factory import refresh
    start=time.perf_counter()
    with contextlib.redirect_stdout(io.StringIO()):refresh()
    refresh_seconds=time.perf_counter()-start
    queue=read_json(ROOT/'recovery/queue.json');cards=[read_json(ROOT/r['card']) for r in queue['tasks']]
    shared={};expanded=[]
    for card in cards:
        full=dict(card)
        for key in ['scope_snapshot','workflow_snapshot']:
            ref=card[key];snapshot=load_snapshot(ref);full[key]=snapshot
            shared[ref['path']]=snapshot
        expanded.append(full)
        # Re-expanding the compact representation must retain every original field.
        require({**card,**{k:load_snapshot(card[k]) for k in ['scope_snapshot','workflow_snapshot']}}==full,
                'Benchmark dropped evidence')
    def encode(values):
        times=[];size=0
        for _ in range(5):
            start=time.perf_counter();size=sum(len(json_bytes(v)) for v in values);times.append(time.perf_counter()-start)
        return {'bytes':size,'serialization_seconds_median':statistics.median(times)}
    legacy=encode(expanded);compact=encode(cards+list(shared.values()))
    from context import packet
    start=time.perf_counter();text=json.dumps(packet('is_facing_camera'),indent=2)
    packet_seconds=time.perf_counter()-start
    result={'method':'Same current cards and all dependency evidence, once inline per card vs hash references plus unique snapshots. Five warm serialization runs. Not token measurements.',
        'cards':len(cards),'snapshot_files':len(shared),'equivalent_inline':legacy,'equivalent_shared':compact,
        'bytes_saved':legacy['bytes']-compact['bytes'],'refresh_seconds':refresh_seconds,
        'canary_packet':{'bytes_utf8':len(text.encode()),'characters':len(text),'seconds':packet_seconds,
                         'scope':'Default bounded packet; explicit expansions preserve access to omitted material. No claimed before/after token savings.'}}
    write_json(ROOT/'recovery/workflow-cost.json',result)
    return result


if __name__=='__main__':print(json.dumps(measure(),indent=2))
