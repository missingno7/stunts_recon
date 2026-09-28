"""namefit.py -- MSC-aware program-wide global-name (and aggregate) fitting.

MSC 5.10's /O code generator is sensitive to symbol-table pressure: every referenced
file-scope extern costs about 24 bytes + its NAME LENGTH in C2's near heap
(FACT-tu-extern-count-limits-cse).  One program-wide name per global therefore constrains
every TU at once.  This tool applies a global name map (address -> name), optionally an
aggregate map (several globals -> one struct/array), to every accepted C TU and to the major
candidate TUs, recompiles each complete TU with its pinned profile/flags through
tools/compiler.compile_source (pinned runner, pinned pass directory / TEMP), and reports
which TUs stay exact and where they change.

    python namefit.py check  --names MAP.json [--agg AGG.json] [--tus all|accepted|cands|ID,..] [--bisect]
    python namefit.py window --tu ID [--span -120:60] [--step 2] [--knobs N]
    python namefit.py solve  [--names-base MAP.json] [--agg AGG.json] [--verify]
    python namefit.py refs   --tu ID              # referenced globals before/inside each function

MAP.json: {"names": {"<load address>": {"name": ...}}} (s003d form), or {"<addr|0xaddr>": "name"}.
AGG.json: {"aggregates": [{"name", "type_decl", "members": {"<addr>": ".field" | "[k]"},
            "definition_type": "struct X"}]}   (see agg_fuf.json)

Exactness: accepted TUs: object identical to the unchanged source's object (segments, fixup
locations/kinds; names excluded).  Candidate TUs: additionally, members exact outside FIXUPP
fields against the locked oracle image.  DIAGNOSTIC ONLY: no canonical state is touched;
compiles land in build/probes (tools/compiler.py) and results are cached in
build/namefit/nf_cache.json.
"""
import argparse
import collections
import hashlib
import json
import re
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
OUT = ROOT / 'build' / 'namefit'
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(HERE))

from common import read_json                                      # noqa: E402

TOKEN = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_]\w*|->|\s+|.', re.S)
IDENT = re.compile(r'[A-Za-z_]\w*')
KEYWORDS = set('''auto break case char const continue default do double else enum extern float for goto if int
long register return short signed sizeof static struct switch typedef union unsigned void volatile while far near
huge cdecl pascal fortran interrupt _loadds _saveregs _export'''.split())

CANDIDATES = [
    # id, source, recipe (for profile/flags/start)
    ('cand:seg003_abbrev', 'build/workers/s003d/seg003_abbrev.c', 'build/workers/s003d/seg003_abbrev.recipe.json'),
    ('cand:seg003_semantic', 'build/workers/F-uf/semantic.c', 'build/workers/s003d/seg003_abbrev.recipe.json'),
    ('cand:seg003_Fuf', 'build/workers/F-uf/semantic_scene_exact.c', 'build/workers/F-uf/semantic_scene.recipe.json'),
    ('cand:seg005_s005c', 'build/workers/s005c/obj_seg005.c', 'recipes/obj_seg005_prefix.json'),
    ('cand:seg005_patterns', 'build/workers/patterns/tu/s5_scs_local_ucast.c', 'recipes/obj_seg005_prefix.json'),
    ('cand:seg000_integ26', 'build/workers/integ26/seg000_v.c', 'recipes/obj_seg000.json'),
    ('cand:seg000_s000c', 'build/workers/s000c/seg000_s000c.c', 'recipes/obj_seg000.json'),
]

# Additional candidate TUs from the command line (--extra ID=SOURCE,RECIPE).
EXTRA_TUS = []

_lock = threading.RLock()
_image = None


def log(*a):
    print(*a, file=sys.stderr, flush=True)


# ----------------------------------------------------------------------------- symbols
class Symbols:
    def __init__(self):
        ds = read_json(ROOT / 'layout/data-symbols.json')
        self.name_to_addr = {}
        self.addr_names = collections.defaultdict(list)
        for n, s in ds['symbols'].items():
            c = n[1:] if n.startswith('_') else n
            self.name_to_addr[c] = s['load_address']
            self.addr_names[s['load_address']].append(c)
        self.frame = ds['frame_load_address']
        # reference (restunts) names: non-clone names
        self.primary = {}
        for n, s in ds['symbols'].items():
            if not s.get('clone_of'):
                self.primary.setdefault(s['load_address'], n[1:] if n.startswith('_') else n)
        # code (function) names -> entry load address; data wins on a clash
        cs = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        self.code_to_addr = {}
        for n, s in cs.items():
            t = (s.get('mapped_target') or {}).get('start')
            if isinstance(t, int):
                self.code_to_addr[n[1:] if n.startswith('_') else n] = t
        for f in read_json(ROOT / 'evidence/functions.json')['functions']:
            if isinstance(f.get('start'), int) and f.get('name'):
                self.code_to_addr.setdefault(f['name'].lstrip('_'), f['start'])
        for c, a in self.code_to_addr.items():
            if c not in self.name_to_addr:
                self.name_to_addr[c] = a
                self.addr_names[a].append(c)
                self.primary.setdefault(a, c)
        self.code_addrs = set(self.code_to_addr.values())


def load_map(path):
    if not path:
        return {}
    d = read_json(Path(path))
    names = d.get('names', d)
    out = {}
    for k, v in names.items():
        a = int(k, 0) if isinstance(k, str) else int(k)
        out[a] = v['name'] if isinstance(v, dict) else v
    return out


def load_aggs(path):
    if not path:
        return []
    return read_json(Path(path))['aggregates']


# ----------------------------------------------------------------------------- TUs
def load_tus(which='all'):
    man = read_json(ROOT / 'layout/manifest.json')
    tus = []
    for o in man['owners']:
        if o['kind'] != 'MATCHING_C' or not o.get('recipe'):
            continue
        r = read_json(ROOT / o['recipe'])
        if r.get('kind', 'c') != 'c' or r.get('data_only') or not r.get('source'):
            continue
        tus.append({'id': o['id'], 'source': r['source'], 'recipe': r, 'accepted': True,
                    'accepted_len': (o['end'] - o['start']) if isinstance(o.get('end'), int) else None})
    for cid, src, rec in CANDIDATES + EXTRA_TUS:
        if (ROOT / src).exists():
            r = dict(read_json(ROOT / rec))
            r['source'] = src
            tus.append({'id': cid, 'source': src, 'recipe': r, 'accepted': False, 'accepted_len': None})
    if which in ('accepted',):
        tus = [t for t in tus if t['accepted']]
    elif which in ('cands',):
        tus = [t for t in tus if not t['accepted']]
    elif which not in ('all', None):
        want = set(which.split(','))
        tus = [t for t in tus if t['id'] in want]
    return tus


# ----------------------------------------------------------------------------- source transform
def tokens(text):
    return [m.group(0) for m in TOKEN.finditer(text)]


def statements(toks):
    """File-scope statements: list of (start, end) token index ranges (end exclusive), with kind."""
    out = []
    depth = 0
    start = 0
    i = 0
    n = len(toks)
    while i < n:
        t = toks[i]
        if t == '{':
            depth += 1
        elif t == '}':
            depth -= 1
            if depth == 0:
                # end of a function body or struct/initializer; statement ends at next ';' only for
                # struct/initializer forms; a function body ends here.
                j = i + 1
                while j < n and toks[j].isspace():
                    j += 1
                if j < n and toks[j] == ';' or (j < n and IDENT.fullmatch(toks[j] or '') and _is_decl_tail(toks, j)):
                    i += 1
                    continue
                out.append((start, i + 1, 'body'))
                start = i + 1
        elif t == ';' and depth == 0:
            out.append((start, i + 1, 'decl'))
            start = i + 1
        elif t.startswith('#') and depth == 0:
            pass
        i += 1
    return out


def _is_decl_tail(toks, j):
    # `} name;` / `} name = ...` after a struct body
    k = j + 1
    while k < len(toks) and toks[k].isspace():
        k += 1
    return k < len(toks) and toks[k] in (';', ',', '=', '[')


def alias_casts(text, sym, names):
    """Aliases of one address spelled with different scalar types in one TU: after the merge only
    the first declaration's type survives, so body uses of a differently typed alias are written
    `((T)name)` (reads keep their type; this is the one-name closure's declaration merge)."""
    import cdecls
    try:
        unit = cdecls.Unit(text)
    except Exception:
        return {}
    groups = collections.defaultdict(list)
    for nm, ds in unit.decls.items():
        a = sym.name_to_addr.get(nm)
        if a is None or a in sym.code_addrs:
            continue
        target = names.get(a)
        groups[a].append((ds[0]['line'], nm, ds[0]['type']))
    casts = {}
    for a, lst in groups.items():
        if len(lst) < 2:
            continue
        lst.sort(key=lambda x: x[0])
        keep_t = cdecls.describe(lst[0][2])
        for _, nm, t in lst[1:]:
            d = cdecls.describe(t)
            if d != keep_t and t['k'] == 'base':
                casts[nm] = d
    return casts


def apply_map(text, sym, names, aggs=(), rename_code=None):
    """Rename identifiers resolving to mapped addresses; fold aggregate members.
    Returns (new_text, info)."""
    toks = tokens(text)
    idents = set(t for t in toks if IDENT.fullmatch(t))
    casts = alias_casts(text, sym, names) if names else {}
    member = {}
    for ag in aggs:
        for a, acc in ag['members'].items():
            member[int(a, 0) if isinstance(a, str) else a] = (ag, acc)
    renamed, folded = {}, collections.Counter()
    cast_uses = collections.Counter()
    addr_of_new = {}
    prev = ''
    out = []
    depth = 0
    in_decl = False
    for i, t in enumerate(toks):
        if t == '{':
            depth += 1
        elif t == '}':
            depth -= 1
        elif t == ';':
            in_decl = False
        elif t == 'extern' or (depth == 0 and IDENT.fullmatch(t) and t in KEYWORDS):
            in_decl = True
        if IDENT.fullmatch(t) and prev not in ('.', '->') and t not in KEYWORDS:
            a = sym.name_to_addr.get(t)
            if a is not None and a in member:
                ag, acc = member[a]
                out.append(ag['name'] + acc)
                folded[t] += 1
                prev = t
                continue
            if a is not None and a in names and names[a] != t:
                renamed.setdefault(t, names[a])
                addr_of_new[names[a]] = a
                if t in casts and depth > 0 and not in_decl:
                    out.append(f'(({casts[t]}){names[a]})')
                    cast_uses[t] += 1
                else:
                    out.append(names[a])
                prev = t
                continue
            if rename_code and t in rename_code:
                renamed.setdefault(t, rename_code[t])
                out.append(rename_code[t])
                prev = t
                continue
        out.append(t)
        if not t.isspace() and not t.startswith(('/*', '//')):
            prev = t
    collisions = sorted({n for o, n in renamed.items() if n in idents and sym.name_to_addr.get(n) != sym.name_to_addr.get(o)})
    new_text = ''.join(out)
    info = {'renamed': renamed, 'collisions': collisions, 'folded': dict(folded),
            'alias_casts': {k: [casts[k], v] for k, v in cast_uses.items()}}
    if aggs and folded:
        new_text = fold_declarations(new_text, aggs, info)
    new_text, merged = merge_duplicate_externs(new_text, set(renamed.values()))
    info['merged_duplicates'] = merged
    return new_text, info


def merge_duplicate_externs(text, newnames):
    """After renaming two aliases of one address to one name, remove later file-scope `extern`
    declarators that redeclare a name already declared (single- or multi-declarator statements;
    the first declaration's type wins).  Returns (text, merged names)."""
    if not newnames:
        return text, []
    toks = tokens(text)
    seen = set()
    edits = []
    merged = []
    ptrq = {'*', 'far', 'near', 'huge', '_far', '_near', '_huge', 'const', 'volatile'}
    for s0, e0, kind in statements(toks):
        if kind != 'decl':
            continue
        sig = [i for i in range(s0, e0) if not toks[i].isspace() and not toks[i].startswith(('/*', '//'))]
        if not sig or toks[sig[0]] != 'extern' or any(toks[i] in ('(', '{') for i in sig):
            for i in sig:
                if IDENT.fullmatch(toks[i]) and toks[i] in newnames:
                    seen.add(toks[i])
            continue
        # split declarators at depth-0 commas
        segs, cur, depth = [], [], 0
        for i in sig[:-1]:            # drop final ';'
            t = toks[i]
            depth += t in ('[', '(')
            depth -= t in (']', ')')
            if t == ',' and depth == 0:
                segs.append(cur)
                cur = []
            else:
                cur.append(i)
        segs.append(cur)
        names = []
        for seg in segs:
            idents = [i for i in seg if IDENT.fullmatch(toks[i]) and toks[i] not in KEYWORDS]
            # the declarator name: last identifier before any '['
            br = next((k for k, i in enumerate(seg) if toks[i] == '['), len(seg))
            cand = [i for i in idents if seg.index(i) < br]
            names.append(cand[-1] if cand else None)
        dup = [n is not None and toks[n] in newnames and toks[n] in seen for n in names]
        for n in names:
            if n is not None and toks[n] in newnames:
                seen.add(toks[n])
        if not any(dup):
            continue
        merged += [toks[n] for n, d in zip(names, dup) if d]
        if all(dup):
            edits.append((s0, e0, ''))
            continue
        # rebuild: specifiers (first segment up to its declarator) + kept declarators
        first = segs[0]
        nidx = first.index(names[0]) if names[0] is not None else len(first)
        k = nidx
        while k > 0 and toks[first[k - 1]] in ptrq:
            k -= 1
        spec = ''.join(toks[i] for i in range(first[0], first[k])) if k > 0 else ''
        decls = []
        for j, seg in enumerate(segs):
            if dup[j]:
                continue
            part = seg[k:] if j == 0 else seg
            decls.append(''.join(toks[i] for i in range(part[0], part[-1] + 1)).strip())
        edits.append((s0, e0, (''.join(toks[i] for i in range(s0, sig[0]))) + spec.rstrip() + ' ' + ', '.join(decls) + ';'))
    if not edits:
        return text, []
    out, pos = [], 0
    for st, en, rep in sorted(edits):
        out.append(''.join(toks[pos:st]))
        out.append(rep)
        pos = en
    out.append(''.join(toks[pos:]))
    return ''.join(out), merged


def fold_declarations(text, aggs, info):
    """Remove file-scope declarations of folded member names, insert the aggregate's type and
    extern declaration (or definition, when the TU defined the members) at the first removal."""
    toks = tokens(text)
    stmts = statements(toks)
    edits = []   # (start, end, replacement)
    for ag in aggs:
        mnames = set()
        for a, acc in ag['members'].items():
            mnames.add(ag['name'] + acc)
        # declarations mention the folded text form `name.field` because apply_map rewrote them;
        # find statements whose declarators are all folded members
        first = None
        inits = {}
        defined = False
        for s, e, kind in stmts:
            if kind != 'decl':
                continue
            seg = ''.join(toks[s:e])
            flat = re.sub(r'/\*.*?\*/|//[^\n]*', '', seg, flags=re.S)
            if not any(re.search(r'(?<![\w.])%s(?![\w])' % re.escape(m), flat) for m in mnames):
                continue
            if '(' in flat and not re.search(r'=', flat):
                continue       # prototype mentioning it (unlikely)
            if re.search(r'\{', flat) and 'struct' in flat and '=' not in flat:
                continue
            is_def = not re.match(r'\s*extern\b', flat)
            # initializers per member
            for m in mnames:
                mm = re.search(r'(?<![\w.])%s\s*=\s*([^,;]+)' % re.escape(m), flat)
                if mm:
                    inits[m] = mm.group(1).strip()
            defined = defined or is_def
            if first is None:
                first = (s, e)
            else:
                edits.append((s, e, ''))
        if first is None:
            continue
        order = sorted(ag['members'].items(), key=lambda kv: int(kv[0], 0) if isinstance(kv[0], str) else kv[0])
        if defined:
            vals = [inits.get(ag['name'] + acc, '0') for _, acc in order]
            if ag.get('array'):
                decl = f"{ag['definition_type']} {ag['name']}{ag.get('dims', '')} = {{ {', '.join(vals)} }};"
            else:
                decl = f"{ag['definition_type']} {ag['name']} = {{ {', '.join(vals)} }};"
        else:
            decl = f"extern {ag['definition_type']} {ag['name']}{ag.get('dims', '')};"
        typ = ag.get('type_decl', '')
        edits.append((first[0], first[1], (typ + '\n' if typ and typ not in text else '') + decl))
        info.setdefault('aggregates', []).append({'name': ag['name'], 'defined': defined})
    if not edits:
        return text
    edits.sort()
    out = []
    pos = 0
    for s, e, rep in edits:
        out.append(''.join(toks[pos:s]))
        out.append(rep)
        pos = e
    out.append(''.join(toks[pos:]))
    return ''.join(out)


# ----------------------------------------------------------------------------- compile + compare
class Cache:
    def __init__(self, path):
        self.path = path
        try:
            self.data = json.loads(path.read_text())
        except (OSError, ValueError):
            self.data = {}
        self.dirty = 0

    def get(self, k):
        with _lock:
            return self.data.get(k)

    def put(self, k, v):
        with _lock:
            self.data[k] = v
            self.dirty += 1
            if self.dirty >= 20:
                self.flush()

    def flush(self):
        with _lock:
            self.path.parent.mkdir(parents=True, exist_ok=True)
            tmp = self.path.with_suffix('.tmp')
            tmp.write_text(json.dumps(self.data))
            tmp.replace(self.path)
            self.dirty = 0


CACHE = Cache(OUT / 'nf_cache.json')
STATS = collections.Counter()


def image():
    global _image
    if _image is None:
        from oracle import verify
        from mz import MZ
        r = verify(write=False)
        _image = MZ.parse(r[1]).load_image(r[1])
    return _image


def compile_summary(tu, text):
    """Compile a full TU text; summary with per-segment hashes, fixup layout and member exactness."""
    from compiler import compile_source, CompileFailure
    from object_flags import recipe_flags
    from object_probe import recipe_sparse_zero
    from communal_unit import recipe_declarations, check_object_communals
    r = tu['recipe']
    flags = recipe_flags(r)
    declarations = recipe_declarations(r)
    communals = None if declarations is None else [name for name, _ in declarations]
    # Diagnostic cache: keyed by the complete TU text, profile, flags, start and
    # the pinned toolchain lock (profile files, pass environment).
    toolchain = hashlib.sha256((ROOT / 'layout/toolchain.json').read_bytes()).hexdigest()
    communal_key = json.dumps(r.get('communal_declarations'), sort_keys=True, separators=(',', ':'))
    key = hashlib.sha256((text + '|' + r['profile'] + '|' + ' '.join(flags or []) + '|' + str(r.get('start')) +
                          '|' + communal_key +
                          '|' + toolchain).encode('latin-1', 'replace')).hexdigest()
    hit = CACHE.get(key)
    if hit is not None and 'fx_list' not in hit and 'error' not in hit:
        hit = None                      # older cache entry: recompile for the fixup mask
    if hit is not None:
        STATS['cache'] += 1
        return hit
    t0 = time.time()
    try:
        obj, receipt = compile_source(text.encode('latin-1'), r['profile'], flags,
                                      sparse_zero=recipe_sparse_zero(r), communals=communals)
        check_object_communals(obj, r)
    except CompileFailure as error:
        logp = Path(error.receipt.get('work_directory', '.')) / 'compiler.log'
        errs = [l.strip() for l in (logp.read_text(errors='replace').splitlines() if logp.exists() else []) if 'error' in l]
        out = {'error': str(error)[:200], 'compiler_errors': errs[:5]}
        CACHE.put(key, out)
        return out
    STATS['compiles'] += 1
    STATS['compile_seconds'] += time.time() - t0
    seg = r.get('object_segment', 'UNIT_TEXT')
    code = obj.segments.get(seg, b'')
    segs = {n: hashlib.sha256(b).hexdigest()[:16] for n, b in obj.segments.items()}
    fx = sorted((f['segment'], f['offset'], f['loc'], f.get('width')) for f in obj.linker_fixups)
    fxh = hashlib.sha256(json.dumps(fx).encode()).hexdigest()[:16]
    pubs = sorted((p['offset'], p['name']) for p in obj.publics if p['segment'] == seg)
    lpubs = sorted((p['offset'], p['name']) for p in getattr(obj, 'local_publics', []) or [] if p.get('segment') == seg)
    allp = sorted(set(pubs + lpubs))
    # member exactness vs original image (outside fixup fields)
    members = {}
    start = r.get('start')
    if isinstance(start, int):
        img = image()
        mask = bytearray(len(code))
        for f in obj.linker_fixups:
            if f['segment'] == seg:
                for k in range(f['offset'], min(len(code), f['offset'] + (f.get('width') or 2))):
                    mask[k] = 1
        for i, (off, name) in enumerate(allp):
            end = allp[i + 1][0] if i + 1 < len(allp) else len(code)
            diffs = [k for k in range(off, end) if not mask[k] and (start + k >= len(img) or code[k] != img[start + k])]
            members[name] = [end - off, len(diffs), diffs[0] if diffs else None]
    fxl = [[f['offset'], f.get('width') or 2] for f in obj.linker_fixups if f['segment'] == seg]
    out = {'segments': segs, 'fixups': fxh, 'code_len': len(code), 'members': members, 'fx_list': fxl,
           'publics': allp, 'externals': len(obj.externals), 'code_hex_sha': segs.get(seg)}
    # keep the code bytes for first-difference reports (hex, compact)
    out['code'] = code.hex()
    CACHE.put(key, out)
    return out


def first_diff(a, b):
    ca, cb = bytes.fromhex(a['code']), bytes.fromhex(b['code'])
    n = min(len(ca), len(cb))
    i = next((k for k in range(n) if ca[k] != cb[k]), n if len(ca) != len(cb) else None)
    if i is None:
        return None
    owner = [nm for off, nm in a['publics'] if off <= i]
    return {'offset': i, 'function': owner[-1] if owner else None, 'lengths': [len(ca), len(cb)]}


def codegen_equal(a, b):
    if a.get('fixups') != b.get('fixups') or 'fx_list' not in a or 'fx_list' not in b:
        return False
    ca, cb = bytes.fromhex(a['code']), bytes.fromhex(b['code'])
    if len(ca) != len(cb):
        return False
    mask = bytearray(len(ca))
    for off, w in a['fx_list'] + b['fx_list']:
        for k in range(off, min(len(ca), off + w)):
            mask[k] = 1
    return all(mask[k] or ca[k] == cb[k] for k in range(len(ca))) and         {k: v for k, v in a['segments'].items() if k != 'UNIT_TEXT'} == {k: v for k, v in b['segments'].items() if k != 'UNIT_TEXT'}


def same_object(a, b):
    return a.get('segments') == b.get('segments') and a.get('fixups') == b.get('fixups')


def evaluate_tu(tu, sym, names, aggs, bisect=False, rename_code=None):
    text = (ROOT / tu['source']).read_text(encoding='latin-1')
    new, info = apply_map(text, sym, names, aggs, rename_code)
    row = {'tu': tu['id'], 'source': tu['source'], 'accepted': tu['accepted'],
           'renamed': info['renamed'], 'folded': info['folded'], 'collisions': info['collisions'],
           'merged_duplicates': info.get('merged_duplicates', []), 'alias_casts': info.get('alias_casts', {}),
           'name_chars_delta': sum(len(n) - len(o) for o, n in info['renamed'].items())}
    if not info['renamed'] and not info['folded']:
        row['result'] = 'UNTOUCHED'
        return row
    if info['collisions']:
        row['result'] = 'NAME_COLLISION'
        return row
    base = compile_summary(tu, text)
    cur = compile_summary(tu, new)
    if 'error' in base or 'error' in cur:
        row['result'] = 'COMPILE_FAILED'
        row['detail'] = cur.get('compiler_errors') or base.get('compiler_errors')
        return row
    row['result'] = 'EXACT' if same_object(base, cur) else 'CHANGED'
    if row['result'] == 'CHANGED' and codegen_equal(base, cur):
        row['result'] = 'CODEGEN_EXACT'     # only fixup fields differ (aggregate addends / binding)
    if row['result'] == 'CHANGED':
        d = first_diff(base, cur)
        row['first_diff'] = d
        if d and tu.get('accepted_len') is not None:
            row['inside_accepted_extent'] = d['offset'] < tu['accepted_len']
    if base.get('members'):
        bm = {k: v[1] == 0 for k, v in base['members'].items()}
        cm = {k: v[1] == 0 for k, v in cur['members'].items()}
        row['members_exact'] = [sum(bm.values()), sum(cm.values()), len(cm)]
        row['members_changed'] = {k: [bm.get(k), cm.get(k), cur['members'][k][1]] for k in cm if bm.get(k) != cm.get(k)}
        row['nonexact_members'] = {k: v[1] for k, v in cur['members'].items() if v[1]}
    if bisect and row['result'] == 'CHANGED' and info['renamed'] and not aggs:
        row['culprit'] = bisect_culprit(tu, text, sym, names, base)
    return row


def bisect_culprit(tu, text, sym, names, base):
    """Smallest prefix (first-occurrence order) of the renames that changes the object."""
    toks = [t for t in tokens(text) if IDENT.fullmatch(t)]
    order = []
    for t in toks:
        a = sym.name_to_addr.get(t)
        if a in names and names[a] != t and a not in order:
            order.append(a)
    lo, hi = 0, len(order)
    probes = 0
    while hi - lo > 1:
        mid = (lo + hi) // 2
        sub = {a: names[a] for a in order[:mid]}
        new, _ = apply_map(text, sym, sub)
        probes += 1
        if same_object(base, compile_summary(tu, new)):
            lo = mid
        else:
            hi = mid
    culprit = order[hi - 1] if order else None
    prefix = {a: names[a] for a in order[:hi]}
    delta = 0
    for a in prefix:
        olds = [n for n in sym.addr_names.get(a, []) if re.search(r'\b%s\b' % re.escape(n), text)]
        if olds:
            delta += len(names[a]) - len(olds[0])
    return {'address': culprit, 'name': names.get(culprit), 'prefix_len': hi, 'prefix_chars_delta': delta,
            'probes': probes, 'order': [names[a] for a in order]}


def run_check(tus, sym, names, aggs, jobs=6, bisect=False, rename_code=None):
    rows = []
    with ThreadPoolExecutor(jobs) as ex:
        futs = [ex.submit(evaluate_tu, tu, sym, names, aggs, bisect, rename_code) for tu in tus]
        for f in futs:
            row = f.result()
            rows.append(row)
            extra = row.get('first_diff') or ''
            me = row.get('members_exact') or ''
            log(f"{row['result']:15} {row['tu']:28} d={row['name_chars_delta']:+5} {me} {extra}")
    CACHE.flush()
    summary = collections.Counter(r['result'] for r in rows)
    return rows, dict(summary)


# ----------------------------------------------------------------------------- references / windows
def referenced_globals(text, sym, upto=None, code=False):
    """Data globals (addresses) referenced at file scope order; upto = function name: stop after
    that function's body.  Returns ordered list of (identifier, address)."""
    toks = tokens(text)
    seen = []
    have = set()
    depth = 0
    cur_fn = None
    last_ident = None
    stop = False
    for i, t in enumerate(toks):
        if t == '{':
            if depth == 0:
                cur_fn = last_ident
            depth += 1
        elif t == '}':
            depth -= 1
            if depth == 0 and upto and cur_fn == upto:
                break
        if IDENT.fullmatch(t):
            if depth == 0 and i + 1 < len(toks) and '(' in ''.join(toks[i + 1:i + 3]):
                last_ident = t
            a = sym.name_to_addr.get(t)
            if a is not None and not code and a in sym.code_addrs:
                a = None
            if a is not None and depth > 0 and a not in have:
                have.add(a)
                seen.append((t, a))
    return seen


def knob_names(text, sym, fn, count):
    """Pick measurement knobs: referenced data externs (declared, not defined here) used before or in fn."""
    refs = referenced_globals(text, sym, fn)
    out = []
    for t, a in refs:
        if re.search(r'^\s*extern\b[^;]*\b%s\b' % re.escape(t), text, re.M):
            out.append((t, a))
        if len(out) >= count:
            break
    return out


def window(tu, sym, span, step, count, fn, jobs=6, ref_tu=None):
    """DIAGNOSTIC pressure window: vary the lengths of `count` referenced extern names so that the
    total name length changes by delta; knob spellings are measurement-only (never candidates)."""
    text = (ROOT / tu['source']).read_text(encoding='latin-1')
    # reference object: the ORIGINAL (unrenamed) TU when a name map was applied first
    ref = ref_tu or tu
    base = compile_summary(ref, (ROOT / ref['source']).read_text(encoding='latin-1'))
    knobs = knob_names(text, sym, fn, count)
    lens = [len(t) for t, _ in knobs]
    # criterion: identical to the reference object when that object is exact against the image;
    # otherwise (candidate TUs) every member exact outside fixups against the image
    target_mode = bool(base.get('members')) and any(v[1] for v in base['members'].values())
    lo, hi = span
    deltas = list(range(lo, hi + 1, step))

    def spell(i, n):
        stem = f'k{i}'
        return (stem + 'qzqzqzqzqzqzqzqzqzqzqzqzqzqzqzqz')[:max(n, len(stem))]

    def one(d):
        # distribute delta across knobs within 2..31 chars
        want = list(lens)
        rem = d
        for i in range(len(want)):
            room = (31 - want[i]) if rem > 0 else (2 - want[i]) if rem < 0 else 0
            take = max(rem, room) if rem < 0 else min(rem, room)
            want[i] += take
            rem -= take
        if rem:
            return d, None, 'out of range'
        names = {a: spell(i, want[i]) for i, (_, a) in enumerate(knobs)}
        new, _ = apply_map(text, sym, names)
        s = compile_summary(tu, new)
        if 'error' in s:
            return d, None, s.get('compiler_errors')
        if target_mode:
            bad = {k: v[1] for k, v in s.get('members', {}).items() if v[1]}
            ok = not bad
            where = None if ok else sorted(bad)[0]
        else:
            ok = same_object(base, s)
            where = None if ok else (first_diff(base, s) or {}).get('function')
        return d, ok, where
    res = []
    with ThreadPoolExecutor(jobs) as ex:
        for d, ok, where in ex.map(one, deltas):
            res.append({'delta': d, 'exact': ok, 'first_diff_fn': where})
            log(f"  {tu['id']} delta {d:+5}: {'EXACT' if ok else 'diff ' + str(where)}")
    CACHE.flush()
    return {'tu': tu['id'], 'knobs': [t for t, _ in knobs], 'knob_lengths': lens, 'results': res}


# ----------------------------------------------------------------------------- main
def main():
    # nfsolve imports this module by name: share one module instance.
    sys.modules.setdefault('namefit', sys.modules[__name__])
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    c = sub.add_parser('check')
    c.add_argument('--names')
    c.add_argument('--agg')
    c.add_argument('--tus', default='all')
    c.add_argument('--bisect', action='store_true')
    c.add_argument('--jobs', type=int, default=6)
    c.add_argument('--out')
    w = sub.add_parser('window')
    w.add_argument('--tu', required=True)
    w.add_argument('--names', help='apply this map first (window around it)')
    w.add_argument('--span', default='-60:30')
    w.add_argument('--step', type=int, default=2)
    w.add_argument('--knobs', type=int, default=6)
    w.add_argument('--fn', help='critical function (knobs referenced before/in it)')
    w.add_argument('--jobs', type=int, default=6)
    w.add_argument('--out')
    r = sub.add_parser('refs')
    r.add_argument('--tu', required=True)
    r.add_argument('--upto')
    s = sub.add_parser('solve')
    s.add_argument('--config', default=str(OUT / 'solve_config.json'))
    s.add_argument('--out', default=str(OUT / 'names_solution.json'))
    s.add_argument('--jobs', type=int, default=6)
    s.add_argument('--max-rounds', type=int, default=3)
    s.add_argument('--mode', choices=['A', 'B', 'C', 'both'], default='both')
    s.add_argument('--verify', action='store_true', help='compile every TU with the solution')
    s.add_argument('--fixed', help='name map whose entries are fixed (e.g. a registry to test option B)')
    a = sub.add_parser('apply', help='write the renamed source of one TU (candidate preparation)')
    a.add_argument('--tu', required=True)
    a.add_argument('--names', required=True)
    a.add_argument('--agg')
    a.add_argument('--out', required=True)
    for parser in (c, w, r, s, a):
        parser.add_argument('--extra', action='append', default=[],
                            help='extra candidate TU: ID=SOURCE,RECIPE (paths relative to the root)')
    args = ap.parse_args()
    for spec in args.extra:
        tu_id, _, paths = spec.partition('=')
        source, _, recipe = paths.partition(',')
        if not (tu_id and source and recipe):
            ap.error('--extra needs ID=SOURCE,RECIPE')
        EXTRA_TUS.append((tu_id, source, recipe))
    t0 = time.time()
    sym = Symbols()
    if args.cmd == 'check':
        names = load_map(args.names)
        aggs = load_aggs(args.agg)
        tus = load_tus(args.tus)
        rows, summary = run_check(tus, sym, names, aggs, args.jobs, args.bisect)
        out = {'schema': 'namefit-check-v1', 'names': args.names, 'agg': args.agg, 'summary': summary,
               'runtime_seconds': round(time.time() - t0, 1), 'stats': dict(STATS), 'results': rows}
        if args.out:
            Path(args.out).write_text(json.dumps(out, indent=1))
        log(json.dumps(summary), f'{time.time() - t0:.1f}s', dict(STATS))
    elif args.cmd == 'refs':
        tu = load_tus(args.tu)[0]
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        for t, a in referenced_globals(text, sym, args.upto):
            print(f'{a:6} 0x{a:05x} {t}')
    elif args.cmd == 'window':
        tu = dict(load_tus(args.tu)[0])
        ref_tu = dict(tu)
        if args.names:
            names = load_map(args.names)
            text = (ROOT / tu['source']).read_text(encoding='latin-1')
            new, _ = apply_map(text, sym, names)
            p = OUT / 'nf_tmp' / (tu['id'].replace(':', '_') + '.c')
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(new, encoding='latin-1')
            tu['source'] = str(p.relative_to(ROOT)).replace('\\', '/')
        lo, hi = (int(x) for x in args.span.split(':'))
        res = window(tu, sym, (lo, hi), args.step, args.knobs, args.fn, args.jobs, ref_tu)
        res['runtime_seconds'] = round(time.time() - t0, 1)
        if args.out:
            Path(args.out).write_text(json.dumps(res, indent=1))
        ex = [x['delta'] for x in res['results'] if x['exact']]
        log('exact deltas:', ex)
    elif args.cmd == 'apply':
        tu = load_tus(args.tu)[0]
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        new, info = apply_map(text, sym, load_map(args.names), load_aggs(args.agg))
        Path(args.out).write_text(new, encoding='latin-1', newline='')
        log(json.dumps(info if isinstance(info, dict) else {'info': str(info)})[:2000])
    elif args.cmd == 'solve':
        import nfsolve
        nfsolve.solve(args, sym)


if __name__ == '__main__':
    main()
