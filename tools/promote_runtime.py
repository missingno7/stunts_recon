"""Serialized FAST -> staged -> canonical publication of pinned runtime owners
(and, integ34, of CODE word-alignment LINK_FILL rows between complete code owners;
integ36: DGROUP word-alignment fills before owned runtime data, `linked` storage
of accepted members, and one staged real link placing all linked storage;
integ38: the in-image prefix of the first _BSS contribution, BSS_IN_IMAGE).

Extends the original check_library.replace_raw_library path using the current
promotion OS lock, input guard and recoverable transaction journal.
"""
import argparse
import copy
import json
from pathlib import Path
from common import ROOT, read_json, require, sha, json_bytes, atomic_bytes, write_json
from build_exact import build, inputs, validate_layout, BSS_IN_IMAGE, checked_bss_in_image
from library import bind_library
from oracle import verify
from mz import MZ
from transaction import (exclusive, ensure_consistent, prepare, apply, finish, rollback, invalidate_receipts,
                         lock_free_snapshot, publishing)
from bss_link import runtime_gate


def replace_raw_bss_in_image(manifest, candidate):
    """integ38: the in-image prefix of the first _BSS contribution
    (build_exact.BSS_IN_IMAGE) replaces exactly the raw bytes [bss_start, image
    end) of the last raw owner row; any raw bytes before bss_start stay raw."""
    require(set(candidate) == {'kind', 'bss_owner'} and type(candidate['bss_owner']) is str,
            'Unsupported in-image BSS candidate form')
    result = copy.deepcopy(manifest)
    start = read_json(ROOT/'layout/data-symbols.json')['bss_start']
    last = result['owners'][-1]
    require(last['kind'] == 'UNRESOLVED_RAW' and last['start'] <= start < last['end'],
            'In-image BSS bytes are not the raw tail of the image')
    row = {'id': candidate['bss_owner'] + '@image', 'kind': BSS_IN_IMAGE, 'start': start,
           'end': last['end'], 'bss_owner': candidate['bss_owner']}
    rows = ([{**last, 'end': start, 'id': f"raw_{last['start']:05x}_{start:05x}"}]
            if last['start'] < start else []) + [row]
    result['owners'][-1:] = rows
    return result


def replace_raw_library(manifest, candidate):
    if candidate.get('kind') == BSS_IN_IMAGE:
        return replace_raw_bss_in_image(manifest, candidate)
    if candidate.get('kind') == 'LINK_FILL':
        return replace_raw_code_fill(manifest, candidate)
    if candidate.get('kind') == STORAGE_KIND:
        return link_accepted_storage(manifest, candidate)
    if candidate.get('module_form') == 'data-only':
        return add_data_member(manifest, candidate)
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
    return attach_linked_storage(result, candidate)


# integ37: a hash-pinned data-only member (no code) is appended to manifest
# `runtime_data_members`; its linked storage rows are carved out of raw DGROUP.
def add_data_member(manifest, candidate):
    from runtime_binding import check_data_member_form, runtime_owners
    check_data_member_form(candidate)
    require(candidate['id'] not in {o['id'] for o in manifest['owners']} and
            candidate['id'] not in {o['id'] for o in runtime_owners(manifest)},
            'Runtime owner ID already exists')
    require(not any(o.get('module') == candidate['module'] and o.get('module_sha256') == candidate['module_sha256']
                    for o in runtime_owners(manifest)), 'Pinned runtime member already accepted')
    require('_BSS' not in candidate['binding']['storage'], 'Data-only member _BSS is not modelled')
    result = copy.deepcopy(manifest)
    result.setdefault('runtime_data_members', []).append(copy.deepcopy(candidate))
    return attach_linked_storage(result, candidate)


def _member_bytes(candidate):
    if candidate.get('module_form') == 'data-only':
        return sum(r['end'] - r['start'] for r in candidate['binding']['storage'].values())
    return candidate['end'] - candidate['start']


# integ36: an ACCEPTED member's proven-raw storage rows become `linked` (owned
# pinned runtime data), once the real link links that member OBJ itself.  Only
# the listed rows' ownership changes; everything else of the owner row stays.
STORAGE_KIND = 'RUNTIME_STORAGE'


def link_accepted_storage(manifest, candidate):
    require(set(candidate) == {'kind', 'owner', 'segments'} and isinstance(candidate['segments'], list) and
            candidate['segments'] and len(set(candidate['segments'])) == len(candidate['segments']),
            'Unsupported runtime storage candidate form')
    rows = [o for o in manifest['owners'] if o['id'] == candidate['owner'] and
            o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' and o.get('binding')]
    require(len(rows) == 1, 'Runtime storage candidate names no accepted runtime member')
    owner = copy.deepcopy(rows[0])
    for name in candidate['segments']:
        row = owner['binding'].get('storage', {}).get(name)
        # integ37: a common-v1 row only as the complete final MSG COMMON overlay
        # (runtime_binding.common_overlay_owner decides it on every bind).
        from runtime_binding import COMMON_EXTENTS
        require(row is not None and row['ownership'] == 'proven-raw' and row['end'] > row['start'] and
                row['anchor']['kind'] != 'mz-stack-v1' and
                (row['anchor']['kind'] != 'common-v1' or (row['start'], row['end']) == COMMON_EXTENTS.get(name)),
                'Runtime storage candidate row is not a proven-raw DGROUP contribution')
        row['ownership'] = 'linked'
    result = copy.deepcopy(manifest)
    result['owners'][result['owners'].index(rows[0])] = owner
    return attach_linked_storage(result, owner, only=set(candidate['segments']))


def _storage_update_only(before, after, candidates):
    """An accepted owner may change only by the storage ownership its
    RUNTIME_STORAGE candidate lists."""
    listed = {c['owner']: set(c['segments']) for c in candidates if c.get('kind') == STORAGE_KIND}
    if before['id'] not in listed:
        return False
    expected = copy.deepcopy(before)
    for name in listed[before['id']]:
        expected['binding']['storage'][name]['ownership'] = 'linked'
    return expected == after


def _carve_raw(manifest, row):
    """Replace exactly [start,end) of one UNRESOLVED_RAW owner by `row`."""
    start, end = row['start'], row['end']
    overlaps = [o for o in manifest['owners'] if o['start'] < end and start < o['end']]
    require(len(overlaps) == 1 and overlaps[0]['kind'] == 'UNRESOLVED_RAW' and
            overlaps[0]['start'] <= start < end <= overlaps[0]['end'],
            'Linked runtime storage must replace wholly raw-owned bytes')
    old = overlaps[0]
    rows = []
    if old['start'] < start:
        rows.append({**old, 'end': start, 'id': f"raw_{old['start']:05x}_{start:05x}"})
    rows.append(row)
    if end < old['end']:
        rows.append({**old, 'start': end, 'id': f"raw_{end:05x}_{old['end']:05x}"})
    at = manifest['owners'].index(old)
    manifest['owners'][at:at+1] = rows


def attach_linked_storage(manifest, candidate, image=None, only=None):
    """integ36: a candidate's `linked` storage rows become owned pinned runtime
    data (runtime_binding.RUNTIME_DATA): initialized segments are carved out of
    raw DGROUP owners; the member's own `_BSS` replaces exactly its object's raw
    BSS placeholder.  Placement is proven by the staged real link, bytes by the
    binder on every build."""
    from runtime_binding import RUNTIME_DATA, RUNTIME_BSS_PLACEMENT
    from common import identity
    storage = candidate.get('binding', {}).get('storage', {})
    linked = {n: r for n, r in storage.items() if r.get('ownership') == 'linked' and r['end'] > r['start']
              and (only is None or n in only)}
    if not linked:
        return manifest
    if image is None:
        oracle = verify(write=False)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
    segments = {s['name']: s for s in candidate['binding']['declarations']['segments']}
    for name, row in sorted(linked.items(), key=lambda t: t[1]['start']):
        start, end = row['start'], row['end']
        base = {'id': f"{candidate['id']}:{name}", 'kind': RUNTIME_DATA, 'classification': 'PINNED_RUNTIME',
                'parent': candidate['id'], 'segment': name, 'start': start, 'end': end}
        if name == '_BSS':
            import bss_link
            rows = manifest.get('bss_owners', [])
            objects = read_json(ROOT/'layout/link-objects.json')['objects']
            hosts = [o['id'] for o in objects if o['start'] <= candidate['start'] < o['end']]
            hits = [r for r in rows if r['kind'] == 'UNRESOLVED_RAW' and r.get('raw_form') == bss_link.OBJECT_BSS
                    and [r.get('object')] == hosts]
            require(len(hits) == 1 and (hits[0]['start'], hits[0]['end']) == (start, end),
                    'Linked runtime _BSS must replace exactly its object placeholder')
            rows[rows.index(hits[0])] = {**base, 'placement': RUNTIME_BSS_PLACEMENT,
                                         'target': identity(bytes(end - start))}
        else:
            require(name in segments and segments[name]['length'] == end - start, 'Linked storage SEGDEF differs')
            _carve_raw(manifest, {**base, 'target': identity(image[start:end])})
    return manifest


def replace_raw_code_fill(manifest, candidate):
    """integ34: a CODE word-alignment fill row (link_fill.CODE_WORD_BASIS) over
    exactly one raw-owned byte.  It confers no ownership; like every LINK_FILL
    it is re-derived from both neighbours on every build.  Published through
    this manifest-row transaction because most such bytes lie between pinned
    runtime members, which have no recipe to carry the claim.  integ36: the
    same transaction publishes a DGROUP word-alignment fill (link_fill.WORD_BASIS)
    whose follower is owned pinned runtime data, which has no recipe either."""
    from link_fill import (CODE_WORD_BASIS, WORD_BASIS, DOSSEG_LEAD_BASIS, code_word_fill_row, word_fill_row,
                           dosseg_lead_row)
    require(set(candidate) == {'kind', 'basis', 'start', 'object'} and
            candidate['basis'] in (CODE_WORD_BASIS, WORD_BASIS, DOSSEG_LEAD_BASIS) and
            type(candidate['start']) is int, 'Unsupported code fill candidate form')
    make = {CODE_WORD_BASIS: code_word_fill_row, WORD_BASIS: word_fill_row,
            DOSSEG_LEAD_BASIS: dosseg_lead_row}[candidate['basis']]
    row = make(candidate['object'], candidate['start'])
    result = copy.deepcopy(manifest)
    hits = [o for o in result['owners'] if o['start'] <= row['start'] < o['end']]
    require(len(hits) == 1 and hits[0]['kind'] == 'UNRESOLVED_RAW' and
            (hits[0]['start'], hits[0]['end']) == (row['start'], row['end']),
            'Code fill is not exactly its raw-owned bytes')
    result['owners'][result['owners'].index(hits[0])] = row
    return result


def _receipt(candidate, image, relocations, staged):
    if candidate.get('kind') == BSS_IN_IMAGE:
        row = [o for o in staged['owners'] if o['kind'] == BSS_IN_IMAGE]
        require(len(row) == 1, 'Staged in-image BSS row missing')
        return checked_bss_in_image(row[0], staged, len(image))
    if candidate.get('kind') == 'LINK_FILL':
        from link_fill import checked_fill
        row = [o for o in staged['owners'] if o['kind'] == 'LINK_FILL' and o['start'] == candidate['start']]
        require(len(row) == 1, 'Staged code fill row missing')
        return {'link_fill': checked_fill(row[0], staged, image)}
    if candidate.get('kind') == STORAGE_KIND:
        owner = next(o for o in staged['owners'] if o['id'] == candidate['owner'])
        return bind_library(owner, image, relocations, manifest=staged)[1]
    return bind_library(candidate, image, relocations, manifest=staged)[1]


def _stage_runtime(candidates, candidate_path, frozen, before, verify_only):
    """Staged runtime acceptance; reads canonical state, writes nothing canonical."""
    manifest = read_json(ROOT/'layout/manifest.json')
    oracle = verify(write=False); mz = MZ.parse(oracle[1]); image = mz.load_image(oracle[1])
    staged = manifest
    for candidate in candidates:
        staged = replace_raw_library(staged,candidate)
    validate_layout(staged,len(image))
    fast = [_receipt(c,image,mz.relocations,staged) for c in candidates]
    accepted_before = [o for o in manifest['owners'] if o['kind'] != 'UNRESOLVED_RAW']
    after = {o['id']: o for o in staged['owners']}
    require(all(o in staged['owners'] or _storage_update_only(o, after.get(o['id']), candidates)
                for o in accepted_before), 'Runtime publication changed accepted ownership')
    data_before = manifest.get('runtime_data_members', [])
    require(staged.get('runtime_data_members', [])[:len(data_before)] == data_before,
            'Runtime publication changed accepted data-only members')
    fresh = build(staged,publish=False)
    require(inputs() == before and fresh['inputs'] == before, 'Inputs changed during staged runtime acceptance')
    members = [c for c in candidates if c.get('kind') not in ('LINK_FILL', STORAGE_KIND, BSS_IN_IMAGE)]
    storage = [c for c in candidates if c.get('kind') == STORAGE_KIND]
    link = None
    if members or storage:
        # integ36: placement by the real link.  One staged run of the pinned
        # LINK 3.65 over the complete candidate state (pinned member OBJs linked
        # themselves) must reproduce the image and place every accepted BSS
        # owner and every linked runtime storage row (bss_link.runtime_gate).
        link = runtime_gate(candidates)
        require(inputs() == before, 'Inputs changed during the staged runtime real link')
    require(candidate_path.read_bytes() == frozen, 'Runtime candidate changed during acceptance')
    fills = [c for c in candidates if c.get('kind') == 'LINK_FILL']
    report = {'status':'VERIFIED_ONLY' if verify_only else 'PROMOTED', 'real_link': link,
              'owners':[c['id'] for c in members], 'bytes':sum(_member_bytes(c) for c in members),
              'link_fill':[c['start'] for c in fills],
              'bss_in_image':[c['bss_owner'] for c in candidates if c.get('kind') == BSS_IN_IMAGE],
              'linked_storage':[[c['owner'], c['segments']] for c in storage],
              'fast':fast, 'whole_image':fresh['executable'], 'relocation_count':fresh['relocation_count']}
    return report, staged


def promote_runtime(candidate_path, verify_only=False):
    candidate_path = Path(candidate_path).resolve()
    frozen = candidate_path.read_bytes()
    candidates = json.loads(frozen)
    if isinstance(candidates, dict): candidates = [candidates]
    require(isinstance(candidates,list) and candidates, 'Expected runtime owner or list of owners')
    if verify_only:
        # Read-only: consistent canonical snapshot instead of the writer lock.
        return lock_free_snapshot(
            lambda before: _stage_runtime(candidates, candidate_path, frozen, before, True)[0], inputs)
    with exclusive():
        ensure_consistent()
        before = inputs()
        report, staged = _stage_runtime(candidates, candidate_path, frozen, before, False)
        changes = {'layout/manifest.json':json_bytes(staged)}
        expected = {**before, **{p:sha(raw) for p,raw in changes.items()}}
        with publishing():
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
          'bytes;',len(result.get('link_fill',[])),'fill rows;',len(result.get('linked_storage',[])),
          'linked storage updates; fresh HYBRID_EXACT and ordered relocations;',
          'real link', (result.get('real_link') or {}).get('status'))


if __name__ == '__main__': main()
