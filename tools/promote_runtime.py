"""Serialized FAST -> staged -> canonical publication of pinned runtime owners.

Extends the original check_library.replace_raw_library path using the current
promotion OS lock, input guard and recoverable transaction journal.
"""
import argparse
import copy
import json
from pathlib import Path
from common import ROOT, read_json, require, sha, json_bytes, atomic_bytes, write_json
from build_exact import build, inputs, validate_layout
from library import bind_library
from oracle import verify
from mz import MZ
from transaction import exclusive, ensure_consistent, prepare, apply, finish, rollback, invalidate_receipts


def replace_raw_library(manifest, candidate):
    require(candidate['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY', 'Not a pinned runtime candidate')
    require(candidate['id'] not in {o['id'] for o in manifest['owners']}, 'Runtime owner ID already exists')
    result = copy.deepcopy(manifest)
    start,end = candidate['start'],candidate['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(len(overlaps) == 1 and overlaps[0]['kind'] == 'UNRESOLVED_RAW',
            'Runtime contribution must be wholly raw-owned')
    old = overlaps[0]
    require(old['start'] <= start < end <= old['end'], 'Runtime contribution crosses ownership')
    rows = []
    if old['start'] < start:
        rows.append({**old,'end':start,'id':f"raw_{old['start']:05x}_{start:05x}"})
    rows.append(copy.deepcopy(candidate))
    if end < old['end']:
        rows.append({**old,'start':end,'id':f"raw_{end:05x}_{old['end']:05x}"})
    at = result['owners'].index(old)
    result['owners'][at:at+1] = rows
    return result


def promote_runtime(candidate_path, verify_only=False):
    candidate_path = Path(candidate_path).resolve()
    frozen = candidate_path.read_bytes()
    candidates = json.loads(frozen)
    if isinstance(candidates, dict): candidates = [candidates]
    require(isinstance(candidates,list) and candidates, 'Expected runtime owner or list of owners')
    with exclusive():
        ensure_consistent()
        before = inputs(); manifest = read_json(ROOT/'layout/manifest.json')
        oracle = verify(write=False); mz = MZ.parse(oracle[1]); image = mz.load_image(oracle[1])
        staged = manifest
        for candidate in candidates:
            staged = replace_raw_library(staged,candidate)
        validate_layout(staged,len(image))
        fast = [bind_library(c,image,mz.relocations,manifest=staged)[1] for c in candidates]
        accepted_before = [o for o in manifest['owners'] if o['kind'] != 'UNRESOLVED_RAW']
        require(all(o in staged['owners'] for o in accepted_before), 'Runtime publication changed accepted ownership')
        fresh = build(staged,publish=False)
        require(inputs() == before and fresh['inputs'] == before, 'Inputs changed during staged runtime acceptance')
        require(candidate_path.read_bytes() == frozen, 'Runtime candidate changed during acceptance')
        report = {'status':'VERIFIED_ONLY' if verify_only else 'PROMOTED',
                  'owners':[c['id'] for c in candidates], 'bytes':sum(c['end']-c['start'] for c in candidates),
                  'fast':fast, 'whole_image':fresh['executable'], 'relocation_count':fresh['relocation_count']}
        if verify_only: return report
        changes = {'layout/manifest.json':json_bytes(staged)}
        expected = {**before, **{p:sha(raw) for p,raw in changes.items()}}
        rows = prepare(changes)
        try:
            require(inputs() == before, 'Inputs changed before runtime publication')
            invalidate_receipts(); apply(rows)
            require(inputs() == expected, 'Unexpected edits during runtime publication')
            artifact = {}
            accepted = build(publish=False,allow_pending=True,artifact=artifact)
            require(inputs() == expected and accepted['inputs'] == expected,
                    'Unexpected edits during canonical runtime verification')
            require(candidate_path.read_bytes() == frozen, 'Runtime candidate changed before publication completed')
            finish()
            atomic_bytes(ROOT/'build/exact/mcga.exe',artifact['executable'])
            write_json(ROOT/'build/exact/acceptance.json',accepted)
            require(inputs() == expected, 'Inputs changed while publishing runtime receipt')
        except BaseException:
            invalidate_receipts(); rollback(); raise
        report['inputs'] = expected
        write_json(ROOT/'build/acceptance/runtime/report.json',report)
        return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('candidate',type=Path)
    parser.add_argument('--verify-only',action='store_true')
    args = parser.parse_args()
    result = promote_runtime(args.candidate,args.verify_only)
    print(result['status'],len(result['owners']),'runtime members;',result['bytes'],
          'bytes; fresh HYBRID_EXACT and ordered relocations')


if __name__ == '__main__': main()
