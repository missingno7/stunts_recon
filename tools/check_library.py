"""Serial FAST -> staged full build -> canonical full build for pinned libraries."""
import argparse
import copy
import os
import json
from common import ROOT, read_json, write_json, require, sha, json_bytes
from build_exact import build, inputs
from library import bind_library
from oracle import verify
from mz import MZ


def replace_raw_library(manifest, candidate):
    require(candidate['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY', 'Not a library candidate')
    require(candidate['id'] not in {o['id'] for o in manifest['owners']}, 'Library is already owned')
    result = copy.deepcopy(manifest)
    start, end = candidate['start'], candidate['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(len(overlaps) == 1 and overlaps[0]['kind'] == 'UNRESOLVED_RAW', 'Library must be wholly raw-owned')
    old = overlaps[0]
    require(old['start'] <= start < end <= old['end'], 'Library crosses ownership')
    replacement = []
    if old['start'] < start:
        replacement.append({**old, 'end':start, 'id':f"raw_{old['start']:05x}_{start:05x}"})
    replacement.append(copy.deepcopy(candidate))
    if end < old['end']:
        replacement.append({**old, 'start':end, 'id':f"raw_{end:05x}_{old['end']:05x}"})
    at = result['owners'].index(old)
    result['owners'][at:at+1] = replacement
    return result


def check(task, promote=False):
    before = inputs()
    candidates = read_json(ROOT/'layout/library-candidates.json')
    require(task in candidates, 'Unknown reviewed library candidate')
    candidate = candidates[task]
    require(task == candidate['id'], 'Library candidate ID mismatch')
    oracle = verify(write=False); mz = MZ.parse(oracle[1])
    payload, fast = bind_library(candidate, mz.load_image(oracle[1]), mz.relocations)
    require(inputs() == before, 'Inputs changed during library FAST')
    if not promote:
        return {'status':'FAST_PASS_ONLY', 'task':task, 'bytes':len(payload), 'receipt':fast}
    lock = ROOT/'build/promotion.lock'; lock.parent.mkdir(exist_ok=True)
    fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY); os.close(fd)
    changed, committed = False, False
    original_manifest = original_plan = None
    try:
        require(inputs() == before, 'Inputs changed after FAST before library lock')
        original_manifest = (ROOT/'layout/manifest.json').read_bytes()
        original_plan = (ROOT/'layout/production-plan.json').read_bytes()
        manifest = replace_raw_library(read_json(ROOT/'layout/manifest.json'), candidate)
        fresh = build(manifest, publish=False)
        require(fresh['inputs'] == before and inputs() == before, 'Inputs changed during staged library build')
        plan = json.loads(original_plan)
        plan['modules'] = [
            {'owner':o['id'], 'recipe':o['recipe'], 'source':read_json(ROOT/o['recipe'])['source']}
            if o['kind'] == 'MATCHING_C' else {'owner':o['id'], 'library':o['library'], 'module':o['module']}
            for o in manifest['owners'] if o['kind'] != 'UNRESOLVED_RAW']
        expected_canonical = {**before, 'layout/manifest.json':sha(json_bytes(manifest)),
                              'layout/production-plan.json':sha(json_bytes(plan))}
        changed = True
        write_json(ROOT/'layout/manifest.json', manifest)
        write_json(ROOT/'layout/production-plan.json', plan)
        require(inputs() == expected_canonical, 'Unexpected edits during canonical library staging')
        acceptance = build()
        require(acceptance['inputs'] == expected_canonical and inputs() == expected_canonical,
                'Canonical library build absorbed out-of-scope edits')
        record = {'status':'PROMOTED', 'task':task, 'bytes':len(payload), 'fast':fast,
                  'staged_full_build':fresh['executable'], 'canonical_full_build':acceptance['executable'],
                  'library_reopened_fresh_at_least':3}
        write_json(ROOT/'recovery/promotions'/(task+'.json'), record)
        committed = True
        return record
    finally:
        if not committed:
            if changed:
                (ROOT/'layout/manifest.json').write_bytes(original_manifest)
                (ROOT/'layout/production-plan.json').write_bytes(original_plan)
            (ROOT/'build/exact/acceptance.json').unlink(missing_ok=True)
        lock.unlink(missing_ok=True)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('task'); p.add_argument('--promote', action='store_true'); a = p.parse_args()
    result = check(a.task, a.promote)
    print(result['status'], result['task'], result['bytes'])


if __name__ == '__main__': main()
