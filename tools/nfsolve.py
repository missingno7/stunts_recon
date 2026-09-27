"""nfsolve.py -- SOLVER mode of namefit.py: one natural program-wide name set.

Constraint model (FACT-tu-extern-count-limits-cse, patterns R2 bands):
  for every pressure-sensitive TU T with critical function F_T,
      dP_T = sum over globals/functions a counted at F_T of (len(name_a) - len(spelling_T(a)))
             - sum over alias pairs merged in T of (OVERHEAD + len(dropped spelling))
  must lie inside one of T's exactness bands (measured, dP = chars ADDED to T's base source).
  Counted at F_T: identifiers declared at file scope (or defined) and referenced in a function body
  up to and including F_T; block-scope (K&R) externs of an earlier function do not count.
Names: natnames.variants() of the reference (Restunts/primary) spelling -- dictionary
abbreviations, dropped words, spelled-out abbreviations; address-style names stay fixed.
Objective: minimise the total naturalness cost (sum of per-name departures), then the number of
renamed globals.
Search: exact 2-D knapsack over (seg003 relief, seg000 deficit) with numpy for items shared by the
two, 1-D knapsack for seg000-only compensation (spelling abbreviations out), then real compiles of
EVERY accepted TU and the candidate TUs (namefit.run_check); on a failure the target point moves
inside the band and the solve repeats (bounded rounds).
"""
import collections
import json
import re
import sys
import time
from pathlib import Path

import numpy as np

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
OUT = HERE.parent / 'build' / 'namefit'
import namefit                                   # noqa: E402
import natnames                                  # noqa: E402

ROOT = namefit.ROOT
OVERHEAD = 24

DEFAULT_CONFIG = {
    'sensitive': {
        # base TU id -> critical function, bands (dP = chars added to that source), preferred point
        'obj_seg000': {'fn': 'end_hiscore', 'bands': [[-14, 2], [16, 1800]], 'safe': [[-10, 1], [24, 1500]],
                       'evidence': 'patterns/seg000_fine.txt, logs/seg000_coarse.log; integ27 pass-dir window'},
        'cand:seg003_semantic': {'fn': 'update_frame', 'bands': [[-880, -520]], 'safe': [[-840, -560]],
                                 'evidence': 'patterns/logs/seg003_semantic_sweep.log (relief 520..880)'},
        'cand:seg005_patterns': {'fn': 'loop_game', 'bands': [[-240, 144]], 'safe': [[-240, 120]],
                                 'evidence': 'typenames/win_seg005_patterns.json (exact -240..+144, fails >= +152)'},
        'obj_seg001': {'fn': None, 'bands': [[-900, 400]], 'safe': [[-800, 300]],
                       'evidence': 'patterns R2: seg001 insensitive over -900..+400'},
    },
    'max_cost_per_name': 4.0,
    'verify_tus': 'all',
}


def sensitive_ids(cfg):
    """(seg003 TU, seg000 TU, seg005 TU) ids of the pressure-sensitive TUs (configurable)."""
    return (cfg.get('seg003_tu', 'cand:seg003_semantic'), 'obj_seg000',
            cfg.get('seg005_tu', 'cand:seg005_patterns'))


# ----------------------------------------------------------------------------- reference sets
def counted_refs(text, sym, fn=None):
    """Ordered {address: spelling} counted at fn (None = whole TU): identifiers referenced in
    function bodies up to and including fn (file-scope declared, or block-scope externs of that
    same function), function definitions up to fn, and data DEFINED at file scope before fn's end
    (a definition enters the symbol table like a reference; integ: seg000 defines the camera words)."""
    toks = [t for t in namefit.tokens(text) if not t.isspace() and not t.startswith(('/*', '//', '"', "'"))]
    ident = namefit.IDENT
    depth = 0
    file_decl = set()
    last_ident = None
    cur_fn = None
    block_ext = set()
    in_extern_stmt = False
    stmt = []
    counted = collections.OrderedDict()
    spell_all = collections.defaultdict(set)
    i = 0
    while i < len(toks):
        t = toks[i]
        prevtok = toks[i - 1] if i else ''
        if t == '{':
            if depth == 0 and prevtok == ')' or (depth == 0 and cur_fn is None and last_ident and prevtok == ';'
                                                  and stmt and stmt[0] not in ('extern', 'typedef', 'struct', 'union')):
                cur_fn = last_ident
                block_ext = set()
                a = sym.name_to_addr.get(cur_fn) if cur_fn else None
                if a is not None:
                    counted.setdefault(a, cur_fn)
                stmt = []
            depth += 1
        elif t == '}':
            depth -= 1
            if depth == 0 and cur_fn is not None:
                if fn is not None and cur_fn == fn:
                    break
                cur_fn = None
                stmt = []
        elif t == ';':
            in_extern_stmt = False
            if depth == 0 and stmt:
                if stmt[0] not in ('extern', 'typedef'):
                    for x in stmt:
                        a = sym.name_to_addr.get(x)
                        if a is not None and a not in sym.code_addrs:
                            counted.setdefault(a, x)
                stmt = []
        elif t == 'extern' and depth > 0:
            in_extern_stmt = True
        if depth == 0 and t not in ('{', '}', ';'):
            stmt.append(t)
        if ident.fullmatch(t) and t not in namefit.KEYWORDS:
            if prevtok in ('.', '->'):
                i += 1
                continue
            a = sym.name_to_addr.get(t)
            if depth == 0:
                if i + 1 < len(toks) and toks[i + 1] == '(':
                    last_ident = t
                if a is not None:
                    file_decl.add(t)
                    spell_all[a].add(t)
            elif a is not None:
                spell_all[a].add(t)
                if in_extern_stmt:
                    block_ext.add(t)
                if cur_fn is not None and (t in file_decl or t in block_ext):
                    counted.setdefault(a, t)
        i += 1
    return counted, spell_all


# ----------------------------------------------------------------------------- items
def read_clone_names():
    ds = namefit.read_json(ROOT / 'layout/data-symbols.json')['symbols']
    return {n[1:]: s for n, s in ds.items() if s.get('clone_of')}


def build_items(sym, tus_by_id, cfg, fixed=None):
    fixed = fixed or {}
    sens = cfg['sensitive']
    counted = {}
    spells = collections.defaultdict(dict)
    for tid, sc in sens.items():
        tu = tus_by_id[tid]
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        c, allsp = counted_refs(text, sym, sc['fn'])
        counted[tid] = c
        for a, s in allsp.items():
            spells[a][tid] = sorted(s)
    universe = set()
    for c in counted.values():
        universe |= set(c)
    # every identifier spelled anywhere in any TU (locals, fields, typedefs, other globals): a new
    # name may not coincide with one of them unless it already names the same address
    used = {}
    spelled = collections.defaultdict(collections.Counter)
    for tu in tus_by_id.values():
        ids = namefit.IDENT.findall((ROOT / tu['source']).read_text(encoding='latin-1'))
        for t in set(ids):
            used.setdefault(t, sym.name_to_addr.get(t))
            a = sym.name_to_addr.get(t)
            if a is not None:
                spelled[a][t] += 2 if tu['accepted'] else 1

    def primary_of(a):
        # the reference spelling: the semantic name the sources use most (accepted TUs weigh
        # double); address-style aliases only when no semantic spelling exists
        c = spelled.get(a)
        if c:
            sem = [(n, k) for n, k in c.items() if not natnames.is_address_name(n)
                   and n not in fixed_names_short]
            pool = sem or list(c.items())
            return max(pool, key=lambda nk: (nk[1], len(nk[0]), nk[0]))[0]
        prim = sym.primary.get(a)
        alln = sym.addr_names.get(a, [])
        sem = [n for n in alln if not natnames.is_address_name(n)]
        return (sem or alln or [prim or '?'])[0]
    # s003d-style pressure short names are candidates, never the reference
    fixed_names_short = set()
    for n, s_ in read_clone_names().items():
        fixed_names_short.add(n)
    forbidden = cfg.get('forbidden', [])
    # siblings: names sharing the leading words (custom_camera_*, pState_minusRotate_*)
    allnames = set(sym.primary.values()) | set(sym.code_to_addr)
    by_first = collections.defaultdict(set)
    wcache = {}
    for n in allnames:
        w = [x.lower() for x in natnames.words(n)]
        wcache[n] = w
        if w:
            by_first[w[0]].add(n)

    def siblings(n):
        """names with the same first word sharing at least two words (custom_camera_*,
        plane_*_op, mouse_draw_*_check)"""
        w = wcache.get(n) or [x.lower() for x in natnames.words(n)]
        if len(w) < 2:
            return ()
        ws = set(w)
        return tuple(sorted(m for m in by_first.get(w[0], ()) if m != n and len(ws & set(wcache[m])) >= 2))
    items = {}
    for a in sorted(universe):
        prim = primary_of(a)
        if a in fixed:
            opts = [(fixed[a], 0.0)]
        else:
            opts = natnames.variants(prim, siblings=siblings(prim))
            opts = [(s, c) for s, c in opts if c <= cfg['max_cost_per_name']
                    and (s not in used or used[s] == a) and sym.name_to_addr.get(s, a) == a
                    and s not in namefit.KEYWORDS and s not in forbidden]
        # keep the cheapest spelling per length
        best = {}
        for s, c in opts:
            if len(s) not in best or c < best[len(s)][1]:
                best[len(s)] = (s, c)
        # every departure from the reference spelling costs at least 0.1 (no churn)
        best = {L: (sp, c + (0.1 if sp != prim else 0.0)) for L, (sp, c) in best.items()}
        items[a] = {'primary': prim, 'options': sorted(best.values(), key=lambda x: len(x[0])),
                    'in': {tid: counted[tid][a] for tid in counted if a in counted[tid]},
                    'code': a in sym.code_addrs}
    # alias merges: a TU spelling one address two ways loses one symbol under any single name
    merges = {}
    for tid in counted:
        m = 0
        for a, per in spells.items():
            s = per.get(tid, [])
            if len(s) > 1 and a in counted[tid]:
                m += sum(OVERHEAD + len(x) for x in sorted(s, key=len)[1:])
        merges[tid] = -m
    return items, counted, merges


def dP(items, choice, tid, merges):
    tot = merges.get(tid, 0)
    for a, it in items.items():
        if tid in it['in']:
            tot += len(choice[a]) - len(it['in'][tid])
    return tot


def cost(items, choice):
    c = 0.0
    for a, it in items.items():
        for s, k in it['options']:
            if s == choice[a]:
                c += k
                break
        else:
            c += 99
    return c


# ----------------------------------------------------------------------------- knapsack solve
def solve_model(items, merges, cfg, target3, target0, allow_shared=True, t5max=None):
    """Choose one option per item minimising cost s.t. dP3 in target3 (inclusive range), dP0 in any of
    target0 ranges and dP5 <= t5max (if given; model).  Exact DP over (dP3, dP0) for items touching
    seg003 or seg000; seg005 checked on the result (its band is wide)."""
    T3, T0, T5 = sensitive_ids(cfg)
    base_choice = {}
    # start: every item at its reference spelling (cost 0) if present, else the TU spelling
    for a, it in items.items():
        zero = [s for s, c in it['options'] if c == 0]
        base_choice[a] = zero[0] if zero else it['options'][0][0]
        if not allow_shared and T0 in it['in']:
            # plan A: every name seg000 uses keeps seg000's own spelling
            sp = it['in'][T0]
            if sp not in [o[0] for o in it['options']]:
                it['options'] = sorted(it['options'] + [(sp, 0.1)], key=lambda x: len(x[0]))
            base_choice[a] = sp
    b3 = dP(items, base_choice, T3, merges)
    b0 = dP(items, base_choice, T0, merges)
    lo3, hi3 = target3
    # relevant items
    rel = [a for a, it in items.items() if (T3 in it['in'] or T0 in it['in']) and len(it['options']) > 1]
    if not allow_shared:
        rel = [a for a in rel if not (T3 in items[a]['in'] and T0 in items[a]['in'])]
        rel = [a for a in rel if T3 in items[a]['in']]
    # DP grid: x = dP3 offset, y = dP0 offset
    X0, X1 = -1400, 200
    Y0, Y1 = -500, 700
    nx, ny = X1 - X0 + 1, Y1 - Y0 + 1
    INF = 1e9
    grid = np.full((nx, ny), INF, dtype=np.float64)
    grid[b3 - X0 if X0 <= b3 <= X1 else 0, b0 - Y0] = 0.0
    back = []
    for a in rel:
        it = items[a]
        base_len = len(base_choice[a])
        new = np.full_like(grid, INF)
        arg = np.full(grid.shape, -1, dtype=np.int16)
        for k, (s, c) in enumerate(it['options']):
            d = len(s) - base_len
            dx = d if T3 in it['in'] else 0
            dy = d if T0 in it['in'] else 0
            base_c = [cc for ss, cc in it['options'] if ss == base_choice[a]][0]
            shifted = np.full_like(grid, INF)
            xs = slice(max(0, dx), nx + min(0, dx))
            xd = slice(max(0, -dx), nx + min(0, -dx))
            ys = slice(max(0, dy), ny + min(0, dy))
            yd = slice(max(0, -dy), ny + min(0, -dy))
            shifted[xs, ys] = grid[xd, yd] + (c - base_c)
            better = shifted < new
            new[better] = shifted[better]
            arg[better] = k
        grid = new
        back.append((a, arg))
    # pick the best terminal state
    best = None
    for (y_lo, y_hi) in target0:
        sub = grid[lo3 - X0:hi3 - X0 + 1, y_lo - Y0:y_hi - Y0 + 1]
        if sub.size == 0:
            continue
        idx = np.unravel_index(np.argmin(sub), sub.shape)
        v = sub[idx]
        if v < INF and (best is None or v < best[0]):
            best = (v, idx[0] + lo3 - X0, idx[1] + y_lo - Y0)
    if best is None:
        return None
    # backtrack
    _, x, y = best
    choice = dict(base_choice)
    for a, arg in reversed(back):
        k = int(arg[x, y])
        it = items[a]
        s = it['options'][k][0]
        choice[a] = s
        d = len(s) - len(base_choice[a])
        x -= d if T3 in it['in'] else 0
        y -= d if T0 in it['in'] else 0
    return choice


def describe_solution(items, choice, merges, cfg):
    per = {}
    for tid in cfg['sensitive']:
        per[tid] = dP(items, choice, tid, merges)
    changed = {a: {'name': choice[a], 'primary': items[a]['primary'],
                   'cost': next((c for s, c in items[a]['options'] if s == choice[a]), None),
                   'tus': sorted(items[a]['in']), 'function': items[a]['code']}
               for a in items if choice[a] != items[a]['primary']}
    return per, changed


def names_map(items, choice):
    """address -> name for every item whose chosen name differs from any TU spelling."""
    out = {}
    for a, it in items.items():
        if any(sp != choice[a] for sp in it['in'].values()) or choice[a] != it['primary']:
            out[a] = choice[a]
    return out


def solve(args, sym):
    t0 = time.time()
    cfg = dict(DEFAULT_CONFIG)
    cp = Path(args.config)
    if cp.exists():
        cfg.update(json.loads(cp.read_text()))
    tus = namefit.load_tus('all')
    by_id = {t['id']: t for t in tus}
    fixed = {}
    if getattr(args, 'fixed', None):
        fixed = namefit.load_map(args.fixed)
    items, counted, merges = build_items(sym, by_id, cfg, fixed)
    log = []
    report = {'schema': 'typenames-names-solution-v1', 'model': __doc__.strip().splitlines()[0],
              'config': cfg, 'merges': merges, 'rounds': []}
    namefit.log(f'items {len(items)}; counted per TU ' + str({k: len(v) for k, v in counted.items()}))
    T3, T0, T5 = sensitive_ids(cfg)
    cap = collections.Counter()
    for a, it in items.items():
        if T3 not in it['in']:
            continue
        mx = len(it['in'][T3]) - min(len(sp) for sp, _ in it['options'])
        cap['seg003_only' if T0 not in it['in'] else 'shared_with_seg000'] += max(0, mx)
        cap['functions' if it['code'] else 'data'] += max(0, mx)
    report['seg003_natural_capacity'] = dict(cap)
    report['seg003_alias_merge_relief'] = merges.get(T3)
    namefit.log('seg003 natural shortening capacity', dict(cap), 'merges', merges)
    # options to evaluate
    s3 = cfg['sensitive'][T3]['safe'][0]
    s0 = cfg['sensitive']['obj_seg000']['safe']
    plans = []
    if getattr(args, 'mode', 'both') in ('A', 'both'):
        plans.append(('A_natural_no_shared', False, [s0[0]]))
    if getattr(args, 'mode', 'both') in ('B', 'both'):
        plans.append(('B_natural_shared_plus_seg000_compensation', True, s0))
    if getattr(args, 'mode', 'both') in ('C', 'both'):
        # like B, but seg000 is placed well inside its wide upper band (margins on every TU)
        plans.append(('C_natural_shared_seg000_upper_band', True, [[30, 400]]))
    results = []
    for label, allow_shared, t0ranges in plans:
        mid = (s3[0] + s3[1]) // 2
        targets3 = [[mid - 60, mid + 60], s3, [s3[0], mid]]
        for rnd, tr3 in enumerate(targets3[:args.max_rounds]):
            for _ in range(20):
                choice = solve_model(items, merges, cfg, tr3, t0ranges, allow_shared)
                if choice is None:
                    break
                seen, dups = {}, []
                for a in sorted(choice):
                    if choice[a] in seen and choice[a] != items[a]['primary']:
                        dups.append(a)
                    seen.setdefault(choice[a], a)
                if not dups:
                    break
                for a in dups:        # the later address loses that spelling
                    items[a]['options'] = [o for o in items[a]['options'] if o[0] != choice[a]] or items[a]['options']
            if choice is None:
                results.append({'plan': label, 'round': rnd, 'target3': tr3, 'status': 'MODEL_INFEASIBLE'})
                namefit.log(label, rnd, 'model infeasible for', tr3, t0ranges)
                continue
            per, changed = describe_solution(items, choice, merges, cfg)
            nm = names_map(items, choice)
            namefit.log(label, rnd, 'model dP', per, 'renamed', len(changed), 'cost', round(cost(items, choice), 1))
            row = {'plan': label, 'round': rnd, 'target3': tr3, 'model_dP': per,
                   'cost': round(cost(items, choice), 2), 'renamed_count': len(changed),
                   'renamed': {str(a): v for a, v in sorted(changed.items())}, 'names': {str(a): n for a, n in nm.items()}}
            if args.verify:
                vt = namefit.load_tus(cfg.get('verify_tus', 'all'))
                rows, summary = namefit.run_check(vt, sym, nm, [], args.jobs)
                row['verify_summary'] = summary
                row['verify'] = [{k: r.get(k) for k in ('tu', 'result', 'name_chars_delta', 'first_diff', 'members_exact',
                                                          'nonexact_members', 'merged_duplicates', 'alias_casts', 'detail',
                                                          'accepted', 'inside_accepted_extent', 'members_changed')}
                                 for r in rows]

                def acc_ok(r):
                    if r['result'] in ('EXACT', 'UNTOUCHED'):
                        return True
                    # a prefix owner whose whole-TU source changes only beyond the accepted extent and
                    # loses no exact member (typically: update_frame becomes exact)
                    return (r['result'] == 'CHANGED' and r.get('inside_accepted_extent') is False
                            and all(v[1] or not v[0] for v in (r.get('members_changed') or {}).values()))
                ok = all(acc_ok(r) for r in rows if r['accepted'])
                row['accepted_failures'] = [r['tu'] for r in rows if r['accepted'] and not acc_ok(r)]
                s3ok = all(not r.get('nonexact_members') for r in rows if r['tu'] == T3)
                s5ok = all(not r.get('nonexact_members') for r in rows if r['tu'] == T5)
                row['all_accepted_exact'] = ok
                row['seg003_semantic_all_members_exact'] = s3ok
                row['seg005_patterns_all_members_exact'] = s5ok
                namefit.log(label, rnd, 'VERIFY', summary, 'accepted exact', ok, 'seg003', s3ok, 'seg005', s5ok)
                results.append(row)
                if ok and s3ok and s5ok:
                    break
            else:
                results.append(row)
                break
    report['rounds'] = results
    good = [r for r in results if r.get('all_accepted_exact') and r.get('seg003_semantic_all_members_exact')
            and r.get('seg005_patterns_all_members_exact')]
    if good:
        worst = lambda r: max([v['cost'] or 0 for v in r['renamed'].values()] or [0])
        best = min(good, key=lambda r: (worst(r), r['cost'], r['renamed_count']))
        report['solution'] = {'plan': best['plan'], 'round': best['round'], 'cost': best['cost'],
                              'renamed_count': best['renamed_count'], 'model_dP': best['model_dP'],
                              'names': best['names'], 'renamed': best['renamed']}
    else:
        report['solution'] = None
    report['runtime_seconds'] = round(time.time() - t0, 1)
    report['compile_stats'] = dict(namefit.STATS)
    Path(args.out).write_text(json.dumps(report, indent=1))
    namefit.log('written', args.out, report['runtime_seconds'], 's')
