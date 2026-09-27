#!/usr/bin/env python3
"""Repository-wide reconstruction status by classifier state (DIAGNOSTIC ONLY).

Aggregates, per objmap object and program-wide, the bytes in each classify.py state:
manifest ownership (ACCEPTED) is read from layout/manifest.json; every other state comes
from re-classifying the best-known worker candidates listed in candidates.json (plus any
--candidates files).  Nothing here grants ownership, binding or acceptance.

  python build/workers/classifier/status_report.py [--refresh] [--only ID,...]
         [--register PATH] [--symbol-receipts PATH] [--record-proof PATH] [--out DIR]
"""
from __future__ import annotations

import argparse
import hashlib
import json
import sys
import time
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[0]
sys.path.insert(0, str(HERE))
import classify as C  # noqa: E402

RESULTS = C.CACHE / 'results'
FINISHED = {'RECORD_CLOSED_EXACT', 'BYTE_EXACT', 'CODEGEN_EXACT', 'EXACT_OPEN_RECORD'}
BLOCKED = {'BLOCKED_SYMBOL', 'BLOCKED_TU_DATA'}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def cache_key(entry, ctx, extra):
    src = (ROOT / entry['source']).read_bytes()
    parts = {
        'source': sha(src), 'selection': {k: entry.get(k) for k in ('object', 'interval', 'members', 'function', 'flags')},
        'register': sha(ctx.register_path.read_bytes()), 'objmap': sha(ctx.objmap_path.read_bytes()),
        'authority': C.tubench._authority_fingerprint(), 'classify': C.CLASSIFIER_VERSION,
        'extra': extra,
    }
    return sha(json.dumps(parts, sort_keys=True).encode())


def run_candidates(entries, ctx, refresh=False, extra=None, log=print):
    RESULTS.mkdir(parents=True, exist_ok=True)
    out = []
    for entry in entries:
        path = ROOT / entry['source']
        if not path.is_file():
            out.append({'entry': entry, 'status': 'MISSING_SOURCE'})
            log(f"  {entry['id']}: missing {entry['source']}")
            continue
        key = cache_key(entry, ctx, extra)
        cpath = RESULTS / f"{entry['id']}_{key[:16]}.json"
        if cpath.is_file() and not refresh:
            result = json.loads(cpath.read_text(encoding='utf-8'))
            log(f"  {entry['id']}: cached {result.get('summary')}")
        else:
            t0 = time.time()
            result = C.classify_source(path, function=entry.get('function'), members=entry.get('members'),
                                       interval=tuple(entry['interval']) if entry.get('interval') else None,
                                       object_id=entry.get('object'), flags=entry.get('flags'), ctx=ctx)
            result['elapsed_s'] = round(time.time() - t0, 1)
            cpath.write_text(json.dumps(result, indent=1, default=str), encoding='utf-8')
            log(f"  {entry['id']}: {result.get('summary') or result.get('error')} ({result['elapsed_s']} s)")
        out.append({'entry': entry, 'status': result.get('status', 'CLASSIFIED'), 'result': result,
                    'cache': str(cpath)})
    return out


def _quality(member):
    ev = member.get('evidence', {})
    return (C.STATE_RANK.get(member.get('comparison') if member['primary_state'] == 'ACCEPTED'
                             else member['primary_state'], 99),
            ev.get('outside_fixup_differences', 1 << 30))


def best_by_member(runs):
    best = {}
    alternatives = defaultdict(list)
    for run in runs:
        result = run.get('result') or {}
        for m in result.get('members', []):
            if m.get('comparison') == 'NOT_EMITTED':
                continue
            m = dict(m)
            m['candidate_id'] = run['entry']['id']
            m['candidate_source'] = run['entry']['source']
            m['candidate_flags'] = result.get('flags')
            alternatives[m['name']].append((m['candidate_id'], m['primary_state'], m.get('comparison')))
            if m['name'] not in best or _quality(m) < _quality(best[m['name']]):
                best[m['name']] = m
    return best, alternatives


def member_blockers(m):
    """Blocker kinds (with sub-kinds) for a non-accepted classified member."""
    state = m['primary_state']
    kinds = []
    if state in ('BYTE_EXACT', 'CODEGEN_EXACT'):
        kinds.append('record_proof_pending (prefix-proof hook)')
    if state == 'EXACT_OPEN_RECORD':
        kinds.append('record_open (candidate CODE record shares non-exact bytes)')
    if state == 'CODEGEN_EXACT':
        kinds.append('symbol_closure_unreviewed (consistent by evidence only)')
    cats = (m.get('fixups') or {}).get('categories', {})
    if state in ('BLOCKED_SYMBOL', 'BLOCKED_TU_DATA') or m.get('comparison') in ('BLOCKED_SYMBOL', 'BLOCKED_TU_DATA'):
        for c, n in cats.items():
            if c in C.FIELD_SYMBOL or c in C.FIELD_TU:
                kinds.append(f'{"tu_data" if c in C.FIELD_TU else "symbol"}:{c}')
    if state in ('CODEGEN_MISMATCH', 'PROFILE_UNCERTAIN'):
        sub = m.get('mismatch_subflags') or []
        for s in sub:
            kinds.append(f'codegen:{s}')
        reasons = m.get('mismatch_reasons') or []
        aligned = 'size_delta' not in reasons and 'bytes_outside_fixups' not in reasons
        for r in reasons:
            # Field/relocation reasons are causes only when the code is otherwise aligned and equal.
            if r in ('relocation_shape', 'call_target_differs', 'internal_differs', 'tu_data_content_differs',
                     'shape_differs', 'addend_differs'):
                if aligned:  # after a size shift these are consequences, not causes
                    kinds.append(f'codegen:{r}')
        if not sub:
            kinds.append('codegen:unclassified')
    if state == 'PROFILE_UNCERTAIN':
        kinds.append('profile:' + (m.get('evidence', {}).get('profile', {}).get('status') or '?'))
        for s in m.get('evidence', {}).get('unresolved_symptoms', []):
            kinds.append(f'profile:symptom_{s}')
    if state == 'BOUNDARY_UNCERTAIN':
        for b in m.get('evidence', {}).get('boundary', []) or ['candidate_extent_conflict']:
            kinds.append('boundary:' + b)
    for f in m.get('flags', []):
        if f == 'flags_register_only_not_production_reviewed':
            kinds.append('profile:flags_register_only')
        if f == 'has_near_calls_needs_group_or_closure' and state in FINISHED | BLOCKED:
            kinds.append('group_closure_required (near calls)')
    return kinds


def build_status(runs, ctx):
    best, alternatives = best_by_member(runs)
    objects = []
    members_all = []
    for obj in ctx.objects:
        rows = obj['members'] or [{'name': f"DATA:{obj['id']}", 'start': obj['start'], 'end': obj['end'],
                                   'bytes': obj['bytes'], 'status': 'DATA'}]
        state_bytes = Counter()
        blockers = Counter()
        mrows = []
        prof = C.object_profile(obj.get('segment'), ctx.flag_rulings)
        for r in rows:
            size = r['end'] - r['start']
            acc, _ = ctx.accepted_bytes(r['start'], r['end'])
            rest = size - acc
            if acc:
                state_bytes['ACCEPTED'] += acc
            cand = best.get(r['name'])
            if rest <= 0:
                state = 'ACCEPTED'
            elif cand is not None and cand['target']['start'] == r['start'] and cand['primary_state'] != 'ACCEPTED':
                state = cand['primary_state']
            elif cand is not None and cand['primary_state'] == 'ACCEPTED':
                state = cand.get('comparison') or 'NO_CANDIDATE'
            else:
                inv = ctx.verified_by_name.get(r['name'])
                if r['name'].startswith('unmapped_') or r.get('status') == 'UNMAPPED_BYTES' or \
                        (not inv and not r['name'].startswith('DATA:')):
                    state = 'BOUNDARY_UNCERTAIN'
                else:
                    state = 'NO_CANDIDATE'
            if rest > 0:
                state_bytes[state] += rest
            rec = {'name': r['name'], 'start': r['start'], 'end': r['end'], 'bytes': size,
                   'accepted_bytes': acc, 'state': state, 'object': obj['id'], 'object_kind': obj['kind']}
            if cand is not None:
                rec.update(candidate_id=cand['candidate_id'], candidate_source=cand['candidate_source'],
                           comparison=cand.get('comparison'), flags=cand.get('flags', []),
                           mismatch_subflags=cand.get('mismatch_subflags', []),
                           field_categories=(cand.get('fixups') or {}).get('categories', {}),
                           alternatives=alternatives.get(r['name'], []))
            if rest > 0:
                if cand is not None and state == cand['primary_state']:
                    kinds = member_blockers(cand)
                elif state == 'NO_CANDIDATE':
                    kinds = [f"no_candidate:{obj['kind']}"]
                    if prof['status'] == 'UNCERTAIN':
                        kinds.append('profile:UNCERTAIN')
                elif state == 'BOUNDARY_UNCERTAIN':
                    kinds = ['boundary:unmapped_or_unverified_inventory_row']
                else:
                    kinds = [f'state:{state}']
                rec['blockers'] = kinds
                for k in kinds:
                    blockers[k] += rest
            mrows.append(rec)
        members_all.extend(mrows)
        total = obj['end'] - obj['start']
        exact = state_bytes['ACCEPTED'] + sum(state_bytes[s] for s in FINISHED)
        objects.append({
            'id': obj['id'], 'kind': obj['kind'], 'segment': obj.get('segment'), 'start': obj['start'],
            'end': obj['end'], 'bytes': total, 'profile': prof['status'], 'profile_flags': prof['flags'],
            'state_bytes': dict(state_bytes), 'accepted_bytes': state_bytes['ACCEPTED'],
            'finished_not_accepted_bytes': sum(state_bytes[s] for s in FINISHED),
            'blocked_bytes': sum(state_bytes[s] for s in BLOCKED),
            'remaining_nonexact_bytes': total - exact,
            'remaining_unfinished_bytes': total - exact - sum(state_bytes[s] for s in BLOCKED),
            'completion_accepted': round(state_bytes['ACCEPTED'] / total, 4) if total else 1.0,
            'top_blockers': blockers.most_common(6), 'members': mrows,
            'confidence': [obj.get('confidence_start'), obj.get('confidence_end'), obj.get('confidence_single_object')],
        })
    return objects, members_all, best


def program_summary(objects, members_all, runs, best):
    state_bytes = Counter()
    for o in objects:
        state_bytes.update(o['state_bytes'])
    by_kind = defaultdict(Counter)
    for o in objects:
        by_kind[o['kind']].update(o['state_bytes'])
    blockers = Counter()
    for m in members_all:
        for k in m.get('blockers', []):
            blockers[k] += m['bytes'] - m['accepted_bytes']
    total = sum(o['bytes'] for o in objects)
    finished = [m for m in members_all if m['state'] in FINISHED]
    blocked = [m for m in members_all if m['state'] in BLOCKED]
    symbols = defaultdict(lambda: {'members': set(), 'categories': Counter(), 'target_names': set()})
    for name, m in best.items():
        if m['primary_state'] == 'ACCEPTED' or m.get('comparison') not in (
                'BLOCKED_SYMBOL', 'BLOCKED_TU_DATA', 'CODEGEN_EXACT', 'BYTE_EXACT'):
            continue  # symbols only from code-exact members (shifted fields carry no evidence)
        for f in m.get('field_details', []):
            cat = f.get('category')
            if cat in C.FIELD_SYMBOL or cat in C.FIELD_CONSISTENT:
                key = f.get('target')
                symbols[key]['members'].add(name)
                symbols[key]['categories'][cat] += 1
                for n in f.get('target_names') or []:
                    symbols[key]['target_names'].add(n)
                if f.get('target_address') is not None:
                    symbols[key].setdefault('target_addresses', set()).add(f['target_address'])
    top_symbols = sorted(({'symbol': k, 'members': sorted(v['members']), 'categories': dict(v['categories']),
                           'target_names': sorted(v['target_names'])[:4],
                           'target_addresses': sorted(v.get('target_addresses', []))[:4]}
                          for k, v in symbols.items()), key=lambda r: (-len(r['members']), r['symbol']))
    closest = sorted((o for o in objects if o['remaining_nonexact_bytes'] > 0),
                     key=lambda o: (o['remaining_nonexact_bytes'], -o['bytes']))
    return {
        'authority': C.AUTHORITY,
        'total_bytes_in_objmap_objects': total,
        'state_bytes': dict(state_bytes),
        'state_bytes_by_object_kind': {k: dict(v) for k, v in by_kind.items()},
        'finished_not_accepted_bytes': sum(state_bytes[s] for s in FINISHED),
        'finished_modulo_binding_bytes': sum(state_bytes[s] for s in FINISHED | BLOCKED),
        'finished_not_accepted_members': sorted(({'name': m['name'], 'object': m['object'], 'bytes': m['bytes'] - m['accepted_bytes'],
                                                  'state': m['state'], 'candidate': m.get('candidate_id'),
                                                  'blockers': m.get('blockers', [])} for m in finished),
                                                key=lambda r: -r['bytes']),
        'blocked_members': sorted(({'name': m['name'], 'object': m['object'], 'bytes': m['bytes'] - m['accepted_bytes'],
                                    'state': m['state'], 'candidate': m.get('candidate_id'),
                                    'blockers': m.get('blockers', [])} for m in blocked), key=lambda r: -r['bytes']),
        'closest_to_complete': [{'id': o['id'], 'kind': o['kind'], 'bytes': o['bytes'],
                                 'remaining_nonexact_bytes': o['remaining_nonexact_bytes'],
                                 'remaining_unfinished_bytes': o['remaining_unfinished_bytes'],
                                 'state_bytes': o['state_bytes'], 'top_blockers': o['top_blockers'][:3]}
                                for o in closest[:25]],
        'closest_to_complete_c': [{'id': o['id'], 'bytes': o['bytes'],
                                   'remaining_nonexact_bytes': o['remaining_nonexact_bytes'],
                                   'remaining_unfinished_bytes': o['remaining_unfinished_bytes'],
                                   'state_bytes': o['state_bytes'], 'top_blockers': o['top_blockers'][:3]}
                                  for o in closest if o['kind'] == 'C'][:15],
        'top_blockers_by_bytes': blockers.most_common(40),
        'top_unresolved_symbols': top_symbols[:60],
        'candidate_runs': [{'id': r['entry']['id'], 'source': r['entry']['source'], 'status': r['status'],
                            'flags': (r.get('result') or {}).get('flags'),
                            'summary': (r.get('result') or {}).get('summary'),
                            'error': (r.get('result') or {}).get('error')} for r in runs],
    }


def render_markdown(summary, objects):
    L = []
    w = L.append
    w('# Reconstruction status by classifier state')
    w('')
    w(f"> {summary['authority']}")
    w('> States: ACCEPTED from layout/manifest.json; all other states from re-classifying worker candidates '
      '(build/workers/classifier/candidates.json). Record closure is diagnostic; RECORD_CLOSED_EXACT requires the prefix-proof hook.')
    w('')
    total = summary['total_bytes_in_objmap_objects']
    w(f"Program bytes in objmap objects: **{total:,}**. Finished but not accepted (exact family): "
      f"**{summary['finished_not_accepted_bytes']:,}** B; finished modulo binding (+ BLOCKED_*): "
      f"**{summary['finished_modulo_binding_bytes']:,}** B.")
    w('')
    w('| state | bytes | % |')
    w('|---|---:|---:|')
    for s in C.STATES:
        b = summary['state_bytes'].get(s, 0)
        if b:
            w(f'| {s} | {b:,} | {100 * b / total:.2f} |')
    w('')
    w('## By object kind')
    w('')
    kinds = sorted(summary['state_bytes_by_object_kind'])
    w('| state | ' + ' | '.join(kinds) + ' |')
    w('|---|' + '---:|' * len(kinds))
    for s in C.STATES:
        vals = [summary['state_bytes_by_object_kind'][k].get(s, 0) for k in kinds]
        if any(vals):
            w(f'| {s} | ' + ' | '.join(f'{v:,}' for v in vals) + ' |')
    w('')
    w('## Closest to complete (remaining non-exact bytes)')
    w('')
    w('| object | kind | bytes | remaining non-exact | remaining unfinished | states | top blockers |')
    w('|---|---|---:|---:|---:|---|---|')
    for o in summary['closest_to_complete'][:20]:
        st = ', '.join(f'{k}:{v}' for k, v in sorted(o['state_bytes'].items(), key=lambda kv: -kv[1]))
        bl = '; '.join(f'{k} ({v})' for k, v in o['top_blockers'])
        w(f"| {o['id']} | {o['kind']} | {o['bytes']} | {o['remaining_nonexact_bytes']} | "
          f"{o['remaining_unfinished_bytes']} | {st} | {bl} |")
    w('')
    w('## Closest C objects')
    w('')
    w('| object | bytes | remaining non-exact | remaining unfinished | states | top blockers |')
    w('|---|---:|---:|---:|---|---|')
    for o in summary['closest_to_complete_c']:
        st = ', '.join(f'{k}:{v}' for k, v in sorted(o['state_bytes'].items(), key=lambda kv: -kv[1]))
        bl = '; '.join(f'{k} ({v})' for k, v in o['top_blockers'])
        w(f"| {o['id']} | {o['bytes']} | {o['remaining_nonexact_bytes']} | {o['remaining_unfinished_bytes']} | {st} | {bl} |")
    w('')
    w('## Finished but not accepted')
    w('')
    w('| member | object | bytes | state | candidate | blockers |')
    w('|---|---|---:|---|---|---|')
    for m in summary['finished_not_accepted_members']:
        w(f"| {m['name']} | {m['object']} | {m['bytes']} | {m['state']} | {m['candidate']} | {'; '.join(m['blockers'])} |")
    w('')
    w('## Code-exact but binding-blocked')
    w('')
    w('| member | object | bytes | state | candidate | blockers |')
    w('|---|---|---:|---|---|---|')
    for m in summary['blocked_members']:
        w(f"| {m['name']} | {m['object']} | {m['bytes']} | {m['state']} | {m['candidate']} | {'; '.join(m['blockers'])} |")
    w('')
    w('## Top blockers (bytes of non-accepted members; kinds overlap)')
    w('')
    w('| blocker | bytes |')
    w('|---|---:|')
    for k, v in summary['top_blockers_by_bytes'][:30]:
        w(f'| {k} | {v:,} |')
    w('')
    w('## Top unresolved symbols (input for symbol closure)')
    w('')
    w('| candidate symbol | members | categories | target names | target addresses |')
    w('|---|---:|---|---|---|')
    for s in summary['top_unresolved_symbols'][:30]:
        w(f"| {s['symbol']} | {len(s['members'])} | {s['categories']} | {', '.join(s['target_names'])} | "
          f"{', '.join(str(a) for a in s['target_addresses'])} |")
    w('')
    w('## Per object')
    w('')
    w('| object | kind | profile | bytes | accepted | finished | blocked | mismatch/profile | no-cand/boundary | remaining non-exact |')
    w('|---|---|---|---:|---:|---:|---:|---:|---:|---:|')
    for o in objects:
        sb = o['state_bytes']
        mis = sb.get('CODEGEN_MISMATCH', 0) + sb.get('PROFILE_UNCERTAIN', 0)
        nc = sb.get('NO_CANDIDATE', 0) + sb.get('BOUNDARY_UNCERTAIN', 0)
        w(f"| {o['id']} | {o['kind']} | {o['profile']} | {o['bytes']} | {o['accepted_bytes']} | "
          f"{o['finished_not_accepted_bytes']} | {o['blocked_bytes']} | {mis} | {nc} | {o['remaining_nonexact_bytes']} |")
    w('')
    w('## Candidate runs')
    w('')
    w('| id | source | flags | status | summary |')
    w('|---|---|---|---|---|')
    for r in summary['candidate_runs']:
        w(f"| {r['id']} | {r['source']} | {' '.join(r['flags'] or [])} | {r['status']} | "
          f"{r['summary'] if r['summary'] else (r['error'] or '')} |")
    return '\n'.join(L) + '\n'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--candidates', action='append', help='extra candidate registry JSON (same schema)')
    ap.add_argument('--only', help='comma-separated candidate ids to (re)run')
    ap.add_argument('--refresh', action='store_true', help='ignore cached classifications')
    ap.add_argument('--register', help='toolchain-hypotheses snapshot')
    ap.add_argument('--symbol-receipts', help='symbol-closure receipts (hook)')
    ap.add_argument('--record-proof', help='prefix-proof output (hook)')
    ap.add_argument('--out', default=str(ROOT / 'build/classifier/out'), help='output directory')
    args = ap.parse_args()
    if args.record_proof == 'auto':
        ap.error('--record-proof auto is per candidate: use tools/classify.py SOURCE --record-proof auto')
    ctx = C.Context(register_path=args.register, symbol_receipts=args.symbol_receipts,
                    record_proof=args.record_proof)
    entries = json.loads((ROOT / 'build/workers/classifier/candidates.json').read_text())['candidates']
    for extra in args.candidates or []:
        entries += json.loads(Path(extra).read_text())['candidates']
    if args.only:
        keep = set(args.only.split(','))
        entries = [e for e in entries if e['id'] in keep]
    def receipts_identity(value):
        if not value:
            return None
        if value == 'autosym' or Path(value).is_dir():
            base = C.AUTOSYM_OUT if value == 'autosym' else Path(value)
            return sha(b''.join((base / name).read_bytes() for name in
                                ('autosym_code_symbols.json', 'autosym_data_symbols.json')))
        return sha(Path(value).read_bytes())
    extra = {'receipts': receipts_identity(args.symbol_receipts),
             'record_proof': args.record_proof and sha(Path(args.record_proof).read_bytes())}
    print(C.AUTHORITY)
    print(f'classifying {len(entries)} candidates')
    runs = run_candidates(entries, ctx, refresh=args.refresh, extra=extra)
    objects, members_all, best = build_status(runs, ctx)
    summary = program_summary(objects, members_all, runs, best)
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    (out / 'status.json').write_text(json.dumps({'summary': summary, 'objects': objects}, indent=1, default=list),
                                     encoding='utf-8')
    (out / 'status.md').write_text(render_markdown(summary, objects), encoding='utf-8')
    print(f"total {summary['total_bytes_in_objmap_objects']:,} B; states {summary['state_bytes']}")
    print(f"finished not accepted {summary['finished_not_accepted_bytes']:,} B; "
          f"finished modulo binding {summary['finished_modulo_binding_bytes']:,} B")
    print(f"wrote {out / 'status.json'} and {out / 'status.md'}")


if __name__ == '__main__':
    main()
