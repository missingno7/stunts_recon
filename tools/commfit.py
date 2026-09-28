"""commfit.py -- joint communal-name solver for the LINK 3.65 c_common region (DIAGNOSTIC).

Chooses readable C names for the game communals of c_common [207408,_end) so that the LINK 3.65
symbol-table walk (tools/communal_order.py: 8-bit weighted name hash, buckets ascending,
new names head-inserted on first sight) reproduces the target address order, with the pinned
`_file.c` anchors __bufout (bucket 43), __bufin (102), __buferr (233) at their locked
addresses, while keeping the name-length pressure of every affected translation unit inside
its measured band (FACT-tu-extern-count-limits-cse).  DIAGNOSTIC design aid (integ39, landed
from the commfit worker): it never places or accepts storage.  The communal unit is accepted
only as a whole by tools/communal_unit.py, from one real link that reproduces the image.

Generated data lives under build/commfit/ (link_names.json from tools/linkorder.py, bands.json,
hints.json, caches and outputs); nothing is written next to the scripts.

Model.  Rows are the communal objects in target address order.  Row i gets an OMF name n_i;
its encounter e_i = (module index, name-record position) of the first module in LINK
processing order that names it (EXTDEF, PUBDEF or COMDEF; MSC orders EXTDEFs by first use,
so a rename keeps the position).  LINK allocates in order of (bucket ascending, e descending
within a bucket).  So the target order holds iff for consecutive rows
    b(n_{i-1}) < b(n_i)   or   b(n_{i-1}) == b(n_i) and e_{i-1} > e_i.
Given that order and the row sizes, the addresses follow from the allocation rule (even sizes
start even).  The solver is an exact DP over (row, bucket) minimising the naming cost
(registry 0 < semantic alias < readable variant < unreadable existing alias < unnamed slot),
with per-TU length-delta penalties; TU bands are then verified by compiling (namefit).

Commands:
    python tools/commfit.py fixtures            # forward + inverse check on the real-LINK MAP fixtures
    python tools/commfit.py solve [--inventory F] [--types TSV] [--out DIR] [--bands F] [--hints F]
    python tools/commfit.py simulate --names MAP.json   # forward model of a chosen name set
    python tools/commfit.py verify --solution DIR/solution.json   # namefit compile of affected TUs
    python tools/commfit.py suggest --solution DIR/solution.json --address A --names "words"
"""
import argparse
import collections
import csv
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
WORK = ROOT / 'build' / 'commfit'          # generated inputs/outputs (ignored build output)

import communal_order as co                                   # noqa: E402

C_COMMON = 207408     # LINK c_common start: paragraph after the XOE marker (207396), integ39
END = 222352
ANCHORS = {'__bufout': 213134, '__bufin': 215782, '__buferr': 221650}
IDA = re.compile(r'^_?(word|byte|unk|data|dword|off|stru|asc|seg|flt|dbl|loc|a[A-Z][A-Za-z0-9]*)_[0-9A-Fa-f]{3,6}$')
MAXLEN = 30          # registry rule: at most 30 identifier characters (MASM/MSC emit same spelling)

# cost model (lower is better)
COST = {'registry': 0.0, 'accepted': 0.5, 'semantic': 1.0, 'hint': 1.0, 'variant': 3.0,
        'ida': 6.0, 'unnamed': 60.0}
EDIT_COST = 1.5          # per variant transformation
LEN_COST = 0.15          # per character of length change (pressure proxy, weighted by TU)


def bucket(omf):
    return co.bucket(omf)


def bucket_placeholder(address, target_bucket):
    """Measurement-only spelling of an unnamed slot inside its DP bucket (never a
    proposal; tools/communal_unit.py refuses it as a placeholder).  Bounded search:
    suffixes of up to three characters (every bucket is reachable)."""
    import itertools
    alphabet = 'abcdefghijklmnopqrstuvwxyz0123456789_'
    suffixes = (''.join(t) for n in (1, 2, 3) for t in itertools.product(alphabet, repeat=n))
    name = next((s for s in ('_slot%d_%s' % (address, x) for x in suffixes) if bucket(s) == target_bucket), None)
    assert name is not None, 'no placeholder spelling in bucket %d' % target_bucket
    return name


def omf(c_name):
    return c_name if c_name.startswith('__') else '_' + c_name


def cname(omf_name):
    return omf_name[1:] if omf_name.startswith('_') and not omf_name.startswith('__') else omf_name


# ----------------------------------------------------------------------------- link inputs
def load_link(path=WORK / 'link_names.json'):
    from linkorder import key
    mods = json.loads(Path(path).read_text())
    sight = {}
    for m in mods:
        if m.get('unit') == 'bss':           # the raw communal unit (publics only; not a real module)
            continue
        for pos, (k, nm, extra) in enumerate(m['names']):
            if k not in ('ext', 'pub', 'com'):
                continue
            kk = key(nm)
            row = sight.get(kk)
            if row is None:
                row = sight[kk] = {'name': nm, 'encounter': (m['index'], pos), 'refs': []}
            if m['unit'] not in row['refs']:
                row['refs'].append(m['unit'])
    return mods, sight


# ----------------------------------------------------------------------------- names
class Names:
    def __init__(self, hints=None, types=None):
        ds = json.loads((ROOT / 'layout/data-symbols.json').read_text())['symbols']
        self.by_addr = collections.defaultdict(list)
        for k, v in ds.items():
            self.by_addr[v['load_address']].append((cname(k), v))
        reg = json.loads((ROOT / 'layout/names-registry.json').read_text())['names']
        self.registry = {int(a): r['name'] for a, r in reg.items() if r.get('kind') == 'data'}
        self.hints = {}
        if hints:
            for a, v in json.loads(Path(hints).read_text()).items():
                self.hints[int(a)] = v if isinstance(v, list) else [v]
        self.vocab = self._vocab(ds)
        for f in sorted((ROOT / 'build/references/restunts/src/restunts/c').glob('*.[ch]')):
            for ident in set(re.findall(r'[A-Za-z_][A-Za-z0-9_]{2,}', f.read_text(errors='replace'))):
                for t in re.split(r'_|(?<=[a-z])(?=[A-Z])|(?<=[A-Za-z])(?=[0-9])|(?<=[0-9])(?=[A-Za-z])', ident):
                    if 3 <= len(t) <= 12 and t.isalpha():
                        self.vocab[t.lower()] += 1
        build_segvocab(self.vocab)
        self.types = collections.defaultdict(set)
        # optional C type evidence (TSV: address, C_type_size_evidence), e.g. a worker inventory
        inv = Path(types) if types else None
        if inv is not None and inv.exists():
            with open(inv, newline='') as f:
                for r in csv.DictReader(f, delimiter='	'):
                    for part in (r.get('C_type_size_evidence') or '').split(' | '):
                        bits = part.split(':')
                        if len(bits) >= 3:
                            self.types[int(r['address'])].add(bits[2])

    def is_array(self, addr):
        return any('[' in t for t in self.types.get(addr, ()))

    @staticmethod
    def _vocab(ds):
        words = collections.Counter()
        for k in ds:
            if IDA.match(k):
                continue
            for t in re.split(r'_|(?<=[a-z])(?=[A-Z])|(?<=[A-Za-z])(?=[0-9])|(?<=[0-9])(?=[A-Za-z])', cname(k)):
                if len(t) >= 2 and t.isalpha():
                    words[t.lower()] += 1
        for w in SYN:
            words[w] += 1
            for x in SYN[w]:
                words[x] += 1
        return words

    def sources(self, addr, current=None):
        """[(c_name, kind)] of readable bases for this address, best first."""
        out, seen = [], set()

        def add(n, kind):
            if n and n.lower() not in seen and re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', n):
                seen.add(n.lower())
                out.append((n, kind))
        add(self.registry.get(addr), 'registry')
        if current and not IDA.match(current):
            add(current, 'accepted')
        for n in self.hints.get(addr, []):
            add(n, 'hint')
        for n, v in sorted(self.by_addr.get(addr, []), key=lambda t: (bool(t[1].get('clone_of')), t[0])):
            if not IDA.match(n):
                add(n, 'semantic')
        ida = [n for n, v in self.by_addr.get(addr, []) if IDA.match(n)]
        if current and IDA.match(current):
            ida = [current] + [n for n in ida if n != current]
        if not out:
            for n in ida[:1]:
                add(n, 'ida')
        return out


# readable abbreviation <-> word pairs (both directions are readable spellings)
SYN = {
    'pos': ['position'], 'ptr': ['pointer'], 'cnt': ['count'], 'idx': ['index'], 'ix': ['index', 'idx'],
    'ixs': ['indices', 'idxs'], 'buf': ['buffer'], 'tmp': ['temp'], 'res': ['resource'],
    'cur': ['current', 'curr'], 'trk': ['track'], 'td': ['trackdata'], 'ofs': ['offset', 'off'],
    'ang': ['angle'], 'vec': ['vector', 'v'], 'vecs': ['vectors'], 'hgt': ['height'], 'flg': ['flag'],
    'tbl': ['table'], 'arr': ['array'], 'len': ['length'], 'val': ['value'], 'str': ['string'],
    'wnd': ['window'], 'spr': ['sprite'], 'cfg': ['config'], 'cpy': ['copy'], 'dir': ['direction'],
    'dist': ['distance'], 'elem': ['element'], 'num': ['count'], 'rot': ['rotation'], 'mat': ['matrix'],
    'shp': ['shape'], 'shps': ['shapes'], 'unk': ['unknown'], 'opp': ['opponent'], 'op': ['opponent', 'opp'],
    'pl': ['player'], 'gm': ['game'], 'sz': ['size'], 'clr': ['color'], 'col': ['column'],
    'cols': ['columns'], 'bmp': ['bitmap'], 'tshape': ['transshape'], 'kb': ['keyboard'],
    'ms': ['mouse'], 'snd': ['sound'], 'rect': ['rectangle'], 'rects': ['rectangles'],
    'terr': ['terrain'], 'dashb': ['dashboard'], 'sdg': ['sdgame'], 'obj': ['object'],
    'prev': ['previous'], 'max': ['maximum'], 'min': ['minimum'], 'cam': ['camera'],
    'grp': ['group'], 'lst': ['list'], 'tex': ['texture'], 'disp': ['display'], 'hdr': ['header'],
    'tm': ['time'], 'elap': ['elapsed'], 'ctr': ['center'], 'vid': ['video'], 'mgmt': ['management'],
    'whl': ['wheel'], 'resp': ['response'], 'grd': ['ground'], 'spd': ['speed'], 'appnd': ['append'],
    'bg': ['background'], 'wat': ['water'], 'lvl': ['level'], 'pt': ['point'], 'pts': ['points'],
    'msg': ['message'], 'img': ['image'], 'pal': ['palette'], 'seg': ['segment'], 'addr': ['address'],
    'sel': ['selection'], 'btn': ['button'], 'but': ['button'], 'kbd': ['keyboard'], 'joy': ['joystick'],
    'txt': ['text'], 'eng': ['engine'], 'drv': ['driver'], 'poly': ['polygon'], 'vert': ['vertex'],
    'verts': ['vertices'], 'dat': ['data'], 'chk': ['check'], 'nxt': ['next'], 'sec': ['second'],
    'secs': ['seconds'], 'rpl': ['replay'], 'ptrs': ['pointers'], 'calc': ['calculated'],
    'dims': ['dimensions'], 'wid': ['width'], 'hgts': ['heights'], 'nums': ['numbers'],
    'angs': ['angles'], 'plyr': ['player'], 'veh': ['vehicle'], 'cnts': ['counts'],
    'terr': ['terrain'], 'trk': ['track'], 'res': ['resource'], 'ptr': ['pointer'], 'flg': ['flag'],
    'color': ['colour'], 'center': ['centre'], 'copy': ['saved', 'backup'], 'counter': ['count'],
    'buffer': ['buf'], 'index': ['idx'], 'position': ['pos'],
}
_REV = collections.defaultdict(set)
for _k, _vs in SYN.items():
    for _v in _vs:
        _REV[_v].add(_k)
        _REV[_k].add(_v)
    _REV[_k].update(_vs)


SEGVOCAB = set()


def build_segvocab(counter):
    SEGVOCAB.clear()
    for w, c in counter.items():
        if 2 <= len(w) <= 9 and w.isalpha() and c >= 3:
            SEGVOCAB.add(w)
    SEGVOCAB.update(k for k in SYN if k.isalpha())
    for vs in SYN.values():
        SEGVOCAB.update(v for v in vs if v.isalpha())


def segment_word(run, vocab):
    """Greedy-optimal segmentation of a lowercase run into vocabulary words (fallback: whole)."""
    n = len(run)
    best = [None] * (n + 1)
    best[0] = (0, [])
    for i in range(n):
        if best[i] is None:
            continue
        for j in range(i + 1, n + 1):
            w = run[i:j]
            if w in vocab or (len(w) > 2 and w.endswith('s') and w[:-1] in vocab) or (j - i == 1 and not w.isalpha()):
                score = best[i][0] + (j - i) ** 2
                if best[j] is None or score > best[j][0]:
                    best[j] = (score, best[i][1] + [w])
    return best[n][1] if best[n] and best[n][1] else [run]


def tokenize(name, vocab):
    """-> (tokens, separators) ; separators[k] is '_' or '' between token k and k+1."""
    parts = re.split(r'(_+)', name.strip('_'))
    toks, seps = [], []
    for p in parts:
        if not p:
            continue
        if p.startswith('_'):
            if toks:
                seps.append('_')
            continue
        sub = re.split(r'(?<=[a-z])(?=[A-Z])|(?<=[A-Za-z])(?=[0-9])|(?<=[0-9])(?=[A-Za-z])', p)
        words = []
        for s in sub:
            if s.isalpha() and s.islower() and len(s) > 5:
                seg = segment_word(s, SEGVOCAB)
                words += seg if all(len(w) >= 2 for w in seg) else [s]
            else:
                words.append(s)
        for k, w in enumerate(words):
            if toks and len(seps) < len(toks):
                seps.append('')
            toks.append(w)
    while len(seps) < len(toks) - 1:
        seps.append('')
    return toks, seps


def join(toks, seps):
    out = toks[0]
    for s, t in zip(seps, toks[1:]):
        out += s + t
    return out


def _prod(alts, sep_alts, camel, max_edits, cap):
    import itertools
    res = []
    for words in itertools.product(*[range(len(a)) for a in alts]):
        wchg = sum(1 for w in words if w)
        if wchg > max_edits:
            continue
        for sp in itertools.product(*[range(len(a)) for a in sep_alts]):
            removes = sum(1 for k, x in enumerate(sp) if x and sep_alts[k][0])
            if removes > 1:
                continue
            ed = wchg + 0.5 * (sum(sp) - removes) + 1.0 * removes
            if ed > max_edits:
                continue
            ts = [alts[k][w] for k, w in enumerate(words)]
            ss = [sep_alts[k][x] for k, x in enumerate(sp)]
            nm = ts[0]
            for sep, t in zip(ss, ts[1:]):
                if camel and not sep and t[:1].isalpha() and t.islower():
                    t = t[:1].upper() + t[1:]
                nm += sep + t
            if 2 <= len(nm) <= MAXLEN and re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', nm):
                what = ['%s->%s' % (alts[k][0], alts[k][w]) for k, w in enumerate(words) if w]
                what += ['sep%d' % k for k, x in enumerate(sp) if x]
                res.append((nm, ed, ','.join(what)))
                if len(res) >= cap:
                    return res
    return res


def variants(base, vocab, max_edits=3, cap=5000, plural=True):
    """Readable spelling variants of `base`: [(name, edits, what)].  Transformations only:
    abbreviation <-> word (SYN, both directions), '_' or no separator between two alphabetic
    words, plural/singular of the last word, dropping a leading 'g' prefix word.  Never adds
    meaningless text; `edits` counts changed words plus half of the separator toggles."""
    import itertools
    toks, seps = tokenize(base, vocab)
    if not toks:
        return []
    camel = any(c.isupper() for c in base[1:]) and '_' not in base.strip('_')
    alts = []
    for k, t in enumerate(toks):
        a = [t]
        lw = t.lower()
        for r in sorted(_REV.get(lw, ())):
            if r != lw:
                a.append(r)
        if plural and k == len(toks) - 1 and lw.isalpha() and len(lw) > 2:
            a.append(lw[:-1] if lw.endswith('s') and not lw.endswith('ss') else lw + 's')
        alts.append(a)
    sep_alts = []
    for k, sp in enumerate(seps):
        a, b = toks[k], toks[k + 1]
        ok = a.isalpha() and b.isalpha() and len(a) >= 2 and len(b) >= 2
        if sp:
            ok = ok and a.islower() and b.islower() and len(a) >= 3 and len(b) >= 3
        else:
            ok = ok and a.islower() and b.islower()      # split a compressed lowercase run
        sep_alts.append([sp, '' if sp else '_'] if ok else [sp])
    out, n = [], 0
    starts = [0] + ([1] if len(toks) > 1 and toks[0].lower() == 'g' else [])
    if toks[0].lower() != 'g' and toks[0][:1].isalpha() and max_edits >= 1:
        # the readable global-variable prefix g_ as an alternative spelling
        for nm, ed, what in _prod([['g']] + alts, [['_']] + sep_alts, camel, max_edits - 1, cap):
            out.append((nm, ed + 1.5, 'g_ prefix' + (',' + what if what else '')))
    for st in starts:
        for nm, ed, what in _prod(alts[st:], sep_alts[st:], camel, max_edits - st, cap):
            ed += st
            if ed == 0:
                continue
            out.append((nm, ed, ','.join(x for x in (what, 'drop g' if st else '') if x)))
            n += 1
            if n >= cap:
                return out
    return out


# ----------------------------------------------------------------------------- inventory
def load_inventory(path, mods, sight, names):
    """Rows in address order: dict(address, size, current (OMF), refs, encounter, fixed, kind).

    Supported inputs: None (derive from the canonical link: every name the accepted objects
    reference inside [C_COMMON,_end), sizes to the next start), an L12 json inventory (list of
    dicts or {'variables': [...]}) with address/size/referencers/declaring unit fields, or the
    L11 communal_name_table.tsv (addresses; referencing modules from the link)."""
    from linkorder import key
    raw = next(m for m in mods if m.get('unit') == 'bss')
    # the raw communal unit's publics are offsets from its (paragraph-aligned) start
    pubs = sorted((o + C_COMMON, n) for k, n, o in raw['names'] if k == 'pub')
    by_addr = collections.defaultdict(list)
    for a, n in pubs:
        by_addr[a].append(n)
    rows = []
    if path is None:
        for a in sorted(by_addr):
            rows.append({'address': a, 'names_seen': by_addr[a]})
    else:
        p = Path(path)
        if p.suffix == '.json' and 'variable_start_candidates' in json.loads(p.read_text()):
            data = json.loads(p.read_text())
            for it in data['variable_start_candidates']:
                a = it['address']
                named = bool(it.get('accepted_C_or_ASM_recipe_EXTDEFs')) or a in by_addr
                other = [o for o in it.get('referencing_code_objects') or []
                         if not o.startswith('rt_dos_crt0dat')]      # the XOE marker, not a communal
                if it.get('possible_interiors_of_positive_spans') and not named:
                    continue
                if not (named or other or it.get('runtime_library_COMDEFs')):
                    continue
                r = {'address': a, 'names_seen': by_addr.get(a, []), 'l12': it['id']}
                if isinstance(it.get('candidate_size'), int) and it.get('size_status', '').startswith(('pinned',)):
                    r['size'] = it['candidate_size']
                if other and not named:
                    r['raw_referencers'] = other
                rows.append(r)
            for anc, adr in ANCHORS.items():
                if not any(r['address'] == adr for r in rows):
                    rows.append({'address': adr, 'names_seen': by_addr.get(adr, [anc])})
        elif p.suffix == '.json':
            data = json.loads(p.read_text())
            items = data.get('variables') or data.get('rows') or data.get('inventory') or data \
                if isinstance(data, dict) else data
            for it in items:
                a = int(it.get('address', it.get('start')))
                r = {'address': a, 'names_seen': by_addr.get(a, [])}
                for k in ('size', 'type', 'declaring_unit', 'referencers', 'neutral', 'interior'):
                    for kk in (k, k.replace('_unit', ''), k + 's'):
                        if kk in it:
                            r[k] = it[kk]
                            break
                if 'name' in it:
                    r['inventory_name'] = it['name']
                if r.get('interior'):
                    continue
                rows.append(r)
        else:
            with open(p, newline='') as f:
                for it in csv.DictReader(f, delimiter='\t'):
                    a = int(it['address'])
                    rows.append({'address': a, 'names_seen': by_addr.get(a, [])})
            known = {r['address'] for r in rows}
            for a in sorted(by_addr):
                if a not in known:
                    rows.append({'address': a, 'names_seen': by_addr[a]})
    rows.sort(key=lambda r: r['address'])
    for i, r in enumerate(rows):
        nxt = rows[i + 1]['address'] if i + 1 < len(rows) else END
        if not isinstance(r.get('size'), int):
            r['size'] = nxt - r['address']
            r['size_basis'] = 'next start (diagnostic)'
        cur = [n for n in r['names_seen'] if key(n) in sight]
        r['current'] = cur[0] if cur else (omf(r['inventory_name']) if r.get('inventory_name') else None)
        r['fixed'] = r['current'] in ANCHORS
        s = sight.get(key(r['current'])) if r['current'] else None
        r['encounter'] = tuple(s['encounter']) if s else None
        r['refs'] = s['refs'] if s else list(r.get('referencers') or [])
        mod_of = {m['unit']: m['index'] for m in mods}
        if r['encounter'] is None and r.get('declaring_unit') in mod_of:
            r['encounter'] = (mod_of[r['declaring_unit']], 10 ** 6)
        r['neutral'] = r['current'] is None
    decl = declared_tus()
    for r in rows:
        r['pressure_tus'] = decl.get(r['address'], [])
    return rows


def declared_tus(lo=C_COMMON, hi=END):
    """address -> sorted accepted C TU ids whose source mentions any alias of that address."""
    import hashlib
    cache = WORK / 'declared_tus.json'
    man = (ROOT / 'layout/manifest.json').read_bytes() + (ROOT / 'layout/data-symbols.json').read_bytes()
    key = hashlib.sha256(man).hexdigest()
    if cache.exists():
        d = json.loads(cache.read_text())
        if d.get('key') == key:
            return {int(a): v for a, v in d['map'].items()}
    import namefit
    sym = namefit.Symbols()
    out = collections.defaultdict(set)
    for tu in namefit.load_tus('accepted'):
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        for t in set(namefit.IDENT.findall(text)):
            a = sym.name_to_addr.get(t)
            if a is not None and lo <= a < hi:
                out[a].add(tu['id'])
    m = {a: sorted(v) for a, v in out.items()}
    WORK.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps({'key': key, 'map': {str(a): v for a, v in m.items()}}))
    return m


# ----------------------------------------------------------------------------- candidates
def candidates(row, names, tu_weight):
    """[(omf_name, cost, kind, detail)] ; anchors are fixed."""
    if row['fixed']:
        return [(row['current'], 0.0, 'anchor', 'pinned _file.c COMDEF')]
    cur_c = cname(row['current']) if row['current'] else None
    w = max([tu_weight.get(u, 1.0) for u in row['refs']] or [1.0])
    out = {}

    def put(nm, cost, kind, detail):
        o = omf(nm)
        k = o.lower()
        if k not in out or out[k][1] > cost:
            out[k] = (o, cost, kind, detail)
    ref_len = len(cur_c) if cur_c else None
    for base, kind in names.sources(row['address'], cur_c):
        lc = LEN_COST * w * abs(len(base) - ref_len) if ref_len else 0.0
        put(base, COST[kind] + lc, kind, base)
        if kind == 'ida':
            continue
        for nm, d, what in variants(base, names.vocab, plural=names.is_array(row['address'])):
            lc = LEN_COST * w * abs(len(nm) - ref_len) if ref_len else 0.0
            put(nm, COST['variant'] + COST.get(kind, 1) + EDIT_COST * (d - 1) + lc, 'variant', f'{base}: {what}')
    res = list(out.values())
    # an unnamed slot: any bucket, flagged for semantic naming
    res.append((None, COST['unnamed'], 'unnamed', 'needs a semantic name in the reported bucket interval'))
    return res


# ----------------------------------------------------------------------------- DP
def encounter(rows, i):
    """Row i's first-sight encounter.  A row no linked object names yet (a neutral or
    unnamed row) is first seen in the final COMDEF-only module at its record position, the
    same synthetic encounter simulate() and stage_link use (integ40: nameBfix found that a
    None encounter made every mixed real/neutral row list INFEASIBLE)."""
    return rows[i]['encounter'] or (10 ** 6, i)


def tie_after(rows, i, j):
    """Row j may share row i's bucket and still follow it: LINK allocates a bucket's
    communals in descending first-sight order, so enc(i) > enc(j)."""
    return encounter(rows, i) > encounter(rows, j)


def solve_dp(rows, cands):
    """Exact DP over (row, bucket).  cands[i]: list of (omf, cost, kind, detail); omf None = any bucket.
    Returns (total cost, choice list [(omf, bucket, cost, kind, detail)]) or (inf, None)."""
    INF = float('inf')
    n = len(rows)
    # best[i][b] = (cost, cand index, prev bucket)
    best = [[(INF, None, None)] * 256 for _ in range(n)]
    for i in range(n):
        per_b = [(INF, None)] * 256
        for ci, (o, c, kind, det) in enumerate(cands[i]):
            bs = range(256) if o is None else [bucket(o)]
            for b in bs:
                if c < per_b[b][0]:
                    per_b[b] = (c, ci)
        if i == 0:
            for b in range(256):
                if per_b[b][1] is not None:
                    best[0][b] = (per_b[b][0], per_b[b][1], None)
            continue
        prev = best[i - 1]
        tie_ok = tie_after(rows, i - 1, i)
        run_min, run_arg = INF, None
        for b in range(256):
            # min over prev buckets < b
            cand_prev = (run_min, run_arg)
            if tie_ok and prev[b][0] < cand_prev[0]:
                cand_prev = (prev[b][0], b)
            if per_b[b][1] is not None and cand_prev[0] < INF:
                best[i][b] = (cand_prev[0] + per_b[b][0], per_b[b][1], cand_prev[1])
            if prev[b][0] < run_min:
                run_min, run_arg = prev[b][0], b
    last = min(range(256), key=lambda b: best[n - 1][b][0])
    if best[n - 1][last][0] == INF:
        return INF, None
    choice = [None] * n
    b = last
    for i in range(n - 1, -1, -1):
        cost, ci, pb = best[i][b]
        o, c, kind, det = cands[i][ci]
        choice[i] = (o, b, c, kind, det)
        b = pb
    return best[n - 1][last][0], choice


def margins(rows, choice):
    """Per row: allowed bucket interval with neighbours fixed, and the binding side(s)."""
    out = []
    n = len(rows)
    for i in range(n):
        b = choice[i][1]
        lo, hi, why = 0, 255, []
        if i > 0:
            pb = choice[i - 1][1]
            tie = tie_after(rows, i - 1, i)
            lo = pb if tie else pb + 1
        if i + 1 < n:
            nb = choice[i + 1][1]
            tie = tie_after(rows, i, i + 1)
            hi = nb if tie else nb - 1
        if b - lo == 0 and i > 0:
            why.append('prev %s' % (choice[i - 1][0] or 'slot'))
        if hi - b == 0 and i + 1 < n:
            why.append('next %s' % (choice[i + 1][0] or 'slot'))
        out.append({'lo': lo, 'hi': hi, 'margin': min(b - lo, hi - b), 'binding': why})
    return out


def feasible_windows(rows, pinned):
    """Name-independent bucket windows: pinned[i] is a bucket or None (free slot).  Returns
    [(lo, hi)] such that every row can take any bucket in its window with SOME completion of the
    other free rows (lo from a forward pass, hi from a backward pass; ties need a descending
    encounter).  lo > hi marks an infeasible pin."""
    n = len(rows)

    def tie(i, j):
        return tie_after(rows, i, j)
    lo, hi = [0] * n, [255] * n
    for i in range(n):
        if i:
            lo[i] = lo[i - 1] if tie(i - 1, i) else lo[i - 1] + 1
        if pinned[i] is not None:
            lo[i] = max(lo[i], pinned[i]) if pinned[i] >= lo[i] else 10 ** 3
    for i in range(n - 1, -1, -1):
        if i < n - 1:
            hi[i] = hi[i + 1] if tie(i, i + 1) else hi[i + 1] - 1
        if pinned[i] is not None:
            hi[i] = min(hi[i], pinned[i]) if pinned[i] <= hi[i] else -1
    return list(zip(lo, hi))


def simulate(rows, chosen):
    """Forward model: communal_order.order over the chosen names; returns {address: predicted}."""
    commons = []
    for i, (r, o) in enumerate(zip(rows, chosen)):
        # a name first seen in the final COMDEF-only module: its record position (stage_link)
        enc = encounter(rows, i)
        commons.append({'name': o, 'size': r['size'], 'kind': 'near', 'encounter': enc[0] * 100000 + enc[1]})
    res = co.order(commons, near_base=rows[0]['address'])
    by = {row['name'].lower(): row for row in res}
    return {r['address']: by[o.lower()]['address'] for r, o in zip(rows, chosen)}, res


# ----------------------------------------------------------------------------- bands
def load_bands(path):
    if path and Path(path).exists():
        return json.loads(Path(path).read_text())
    return json.loads((WORK / 'bands.json').read_text()) if (WORK / 'bands.json').exists() else {}


def tu_weights(bands):
    w = {}
    for u, b in bands.items():
        lo, hi = b.get('lo', -8), b.get('hi', 8)
        w[u] = 1.0 + 12.0 / max(1, min(abs(lo), abs(hi)) + 1)
    return w


def tu_deltas(rows, choice):
    d = collections.defaultdict(int)
    names = collections.defaultdict(list)
    for r, ch in zip(rows, choice):
        if not r['current'] or not ch[0] or r['fixed']:
            continue
        delta = len(ch[0]) - len(r['current'])
        if ch[0] != r['current']:
            for u in r['pressure_tus']:
                d[u] += delta
                names[u].append((cname(r['current']), cname(ch[0])))
    return d, names


# ----------------------------------------------------------------------------- commands
def cmd_solve(a):
    mods, sight = load_link(a.link)
    names = Names(a.hints, a.types)
    rows = load_inventory(a.inventory, mods, sight, names)
    out = Path(a.out)
    out.mkdir(parents=True, exist_ok=True)
    base_cands = []
    fixed = json.loads(Path(a.fixed).read_text()) if a.fixed else {}
    bands = load_bands(a.bands)
    for r in rows:
        cs = candidates(r, names, {})
        if a.keep_registry and not r['fixed']:
            reg = names.registry.get(r['address'])
            if reg:
                cs = [c for c in cs if c[0] is None or c[0].lower() == omf(reg).lower()] or cs
        caps = [bands[u]['per_name_hi'] for u in r.get('pressure_tus', []) if 'per_name_hi' in bands.get(u, {})]
        if caps and r['current'] and not r['fixed']:
            cap = min(caps)
            cs = [c for c in cs if c[0] is None or len(c[0]) - len(r['current']) <= cap]
        if str(r['address']) in fixed:
            cs = [(omf(fixed[str(r['address'])]), 0.0, 'fixed', 'reviewed fixed name')]
        base_cands.append(cs)
    # Lagrangian relaxation of the per-TU name-length budgets: signed multipliers let
    # renames in one TU compensate each other; |delta| keeps a small tie-break weight.
    lam = collections.defaultdict(float)
    mu = {u: 0.4 / (1 + min(abs(b.get('lo', -8)), abs(b.get('hi', 8)))) for u, b in bands.items()}
    best = None
    for rnd in range(a.rounds):
        cands = []
        for r, cs in zip(rows, base_cands):
            cur = len(r['current']) if r['current'] else None
            res = []
            for o, c, k, d in cs:
                extra = 0.0
                if o and cur is not None and not r['fixed']:
                    dl = len(o) - cur
                    for u in r['pressure_tus']:
                        extra += lam[u] * dl + mu.get(u, 0.02) * abs(dl)
                res.append((o, c + extra, k, d))
            cands.append(res)
        total, choice = solve_dp(rows, cands)
        if choice is None:
            print('INFEASIBLE')
            return 1
        deltas, dnames = tu_deltas(rows, choice)
        excess = {}
        for u, b in bands.items():
            dv = deltas.get(u, 0)
            if dv > b.get('hi', 999):
                excess[u] = dv - b['hi']
            elif dv < b.get('lo', -999):
                excess[u] = dv - b['lo']
        named = sum(1 for ch in choice if ch[0])
        score = (sum(abs(x) for x in excess.values()), -named)
        if best is None or score < best[0]:
            best = (score, total, choice, dict(deltas), dict(lam))
        print(f'round {rnd}: cost {total:.1f} named {named} band excess {excess}')
        if not excess and rnd >= 1:
            break
        for u, e in excess.items():
            lam[u] += 0.05 * e
    _, total, choice, deltas, lam = best
    marg = margins(rows, choice)
    win = feasible_windows(rows, [ch[1] if ch[0] else None for ch in choice])
    free = feasible_windows(rows, [bucket(r['current']) if r['fixed'] else None for r in rows])
    chosen = [ch[0] if ch[0] else bucket_placeholder(r['address'], ch[1]) for r, ch in zip(rows, choice)]
    pred, _ = simulate(rows, chosen)
    table = []
    for r, ch, m, w, fw in zip(rows, choice, marg, win, free):
        o, b, c, kind, det = ch
        table.append({'address': r['address'], 'size': r['size'], 'current': r['current'],
                      'registry': names.registry.get(r['address']),
                      'chosen': o, 'bucket': b, 'kind': kind, 'detail': det, 'cost': round(c, 2),
                      'lo': m['lo'], 'hi': m['hi'], 'margin': m['margin'], 'binding': m['binding'],
                      'encounter': r['encounter'], 'refs': r['refs'],
                      'predicted_address': pred[r['address']],
                      'slot_window': list(w), 'anchor_only_window': list(fw),
                      'pressure_tus': r['pressure_tus'],
                      'changed_from_current': bool(o and r['current'] and o != r['current']),
                      'changed_from_registry': bool(names.registry.get(r['address']) and o and
                                                    cname(o) != names.registry[r['address']])})
    exact = sum(1 for t in table if t['predicted_address'] == t['address'])
    summary = {'rows': len(table), 'predicted_exact': exact, 'cost': round(total, 2),
               'kinds': dict(collections.Counter(t['kind'] for t in table)),
               'changed_from_current': sum(t['changed_from_current'] for t in table),
               'changed_from_registry': sum(t['changed_from_registry'] for t in table),
               'unnamed_slots': [(t['address'], t['lo'], t['hi']) for t in table if t['kind'] == 'unnamed'],
               'tu_length_deltas': dict(deltas), 'bands': bands, 'multipliers': lam}
    (out / 'solution.json').write_text(json.dumps({'summary': summary, 'rows': table}, indent=1, default=str))
    with open(out / 'solution.tsv', 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t')
        cols = ['address', 'size', 'current', 'registry', 'chosen', 'bucket', 'lo', 'hi', 'margin', 'kind',
                'slot_window', 'anchor_only_window', 'binding', 'detail', 'pressure_tus']
        w.writerow(cols)
        for t in table:
            w.writerow([t[c] if not isinstance(t[c], list) else ';'.join(map(str, t[c])) for c in cols])
    namemap = {str(t['address']): cname(t['chosen']) for t in table
               if t['chosen'] and t['current'] and t['changed_from_current']}
    (out / 'namefit_map.json').write_text(json.dumps(namemap, indent=1))
    print(json.dumps({k: v for k, v in summary.items() if k != 'bands'}, indent=1, default=str)[:4000])
    return 0


def cmd_fixtures(a):
    """(1) forward: communal_order.order reproduces every real-LINK MAP fixture;
    (2) inverse: with each true name hidden among its readable variants, the DP recovers an
    assignment whose forward simulation reproduces the observed MAP offsets;
    (3) negative: a perturbation to a wrong bucket is refused."""
    fx = json.loads((ROOT / 'tests/fixtures/link365_communal_fixtures.json').read_text())['fixtures']
    report = []
    for f in fx:
        commons = [tuple(c) for c in f['commons']]
        others = [tuple(o) for o in f['others']]
        pred = co.order(commons, others)
        obs = {co._key(n): (k, off) for n, k, off in f['observed']}
        fwd = all(obs.get(co._key(r['name'])) == (r['kind'], r['offset']) for r in pred
                  if co._key(r['name']) in obs) and len(obs) == len({co._key(r['name']) for r in pred} & set(obs))
        # inverse on the near chain in observed order
        first = {}
        for c in commons:
            k = co._key(c[1])
            first.setdefault(k, c[0])
        for o in others:
            k = co._key(o[0])
            first[k] = min(first.get(k, o[1]), o[1])
        near = sorted([(off, n) for n, k, off in f['observed'] if k == 'near'])
        size = {}
        for c in commons:
            k = co._key(c[1])
            size[k] = max(size.get(k, 0), c[2])
        rows = [{'address': off, 'encounter': (first[co._key(n)], 0), 'size': size[co._key(n)], 'fixed': False,
                 'current': n} for off, n in near]
        cands = []
        for r in rows:
            cs = [(r['current'], 0.0, 'true', '')]
            if re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', r['current']):
                cs += [(v, 1.0 + d, 'variant', w) for v, d, w in variants(r['current'], {}, 1, 40)]
            cands.append(cs)
        total, choice = solve_dp(rows, cands)
        inv = choice is not None and all(ch[0] == r['current'] for ch, r in zip(choice, rows))
        # hidden-name inverse: every third true name is withheld; the candidates are foreign
        # spellings (other fixture names with a suffix).  Any DP solution must simulate to the
        # observed offsets when the rows keep their encounters and sizes.
        import random
        rnd = random.Random(f['label'])
        al = 'abcdefghijklmnopqrstuvwxyz0123456789'
        sfx = [a + b for a in al for b in al] + ['x' + a + b for a in al for b in al]
        hc = []
        hidden = 0
        for k, (r, cs) in enumerate(zip(rows, cands)):
            if k % 3 == 1:
                hc.append([(r['current'] + '_' + x, 1.0, 'foreign', '') for x in sfx
                           if co._key(r['current'] + '_' + x) not in first])
                hidden += 1
            else:
                hc.append(cs[:1])
        _, hch = solve_dp(rows, hc)
        hidden_ok = 'infeasible'
        if hch is not None:
            names_used = [ch[0] for ch in hch]
            if len({co._key(x) for x in names_used}) == len(names_used):
                sim = co.order([{'name': nm, 'size': r['size'], 'kind': 'near', 'encounter': r['encounter'][0]}
                                for nm, r in zip(names_used, rows)])
                got = {co._key(x['name']): x['offset'] for x in sim}
                hidden_ok = all(got[co._key(nm)] == r['address'] for nm, r in zip(names_used, rows))
        # negative: force row k (middle) to a bucket that breaks order, expect infeasible
        neg = None
        if len(rows) > 3:
            k = len(rows) // 2
            b_prev = co.bucket(rows[k - 1]['current'])
            bad = [c for c in cands[k] if c[0] and co.bucket(c[0]) < b_prev]
            if bad:
                cc = list(cands)
                cc[k] = bad[:1]
                neg = solve_dp(rows, cc)[1] is None
        report.append({'label': f['label'], 'commons': len(pred), 'forward_map_exact': fwd,
                       'inverse_recovers_true_names': inv, 'negative_refused': neg,
                       'hidden_rows': hidden, 'hidden_solution_simulates_exact': hidden_ok})
        print(report[-1])
    WORK.mkdir(parents=True, exist_ok=True)
    (WORK / 'fixture_check.json').write_text(json.dumps(report, indent=1))
    return 0 if all(r['forward_map_exact'] and r['inverse_recovers_true_names'] and r['negative_refused'] in (True, None)
                    for r in report) else 1


def cmd_simulate(a):
    mods, sight = load_link(a.link)
    names = Names(a.hints, a.types)
    rows = load_inventory(a.inventory, mods, sight, names)
    m = json.loads(Path(a.names).read_text()) if a.names else {}
    chosen = [omf(m[str(r['address'])]) if str(r['address']) in m else (r['current'] or '_SLOT_%d' % r['address'])
              for r in rows]
    pred, res = simulate(rows, chosen)
    bad = [(r['address'], c, pred[r['address']], bucket(c)) for r, c in zip(rows, chosen) if pred[r['address']] != r['address']]
    print(json.dumps({'rows': len(rows), 'exact': len(rows) - len(bad), 'first_mismatches': bad[:30]}, indent=1))
    return 0


def cmd_verify(a):
    sol = json.loads(Path(a.solution).read_text())
    mp = Path(a.solution).with_name('namefit_map.json')
    tus = sorted({u for t in sol['rows'] if t['changed_from_current'] for u in t['refs']})
    ids = []
    rep = json.loads((WORK / 'reallink/report-commfit-base.json').read_text())   # tools/linkorder.py --run
    unit_to_id = {r['unit']: r['piece'] for r in rep['accepted_objects']}
    for u in tus:
        if u in unit_to_id and u.startswith('c_'):
            ids.append(unit_to_id[u])
    print('affected accepted C TUs:', ids, '; other units (no C pressure):',
          [u for u in tus if not u.startswith('c_')])
    cmd = [sys.executable, str(ROOT / 'tools/namefit.py'), 'check', '--names', str(mp), '--tus', ','.join(ids),
           '--out', str(Path(a.solution).with_name('namefit_check.json'))]
    if a.bisect:
        cmd.append('--bisect')
    print(' '.join(cmd))
    return subprocess.call(cmd, cwd=ROOT)


def cmd_suggest(a):
    """Spell given base names inside the row's window [lo, hi] of a solution (its neighbours
    fixed), cheapest and most length-neutral first.  For a naming worker: supply the semantic
    words; the tool only varies spelling (abbreviation, separators, plural for arrays, g_)."""
    sol = json.loads(Path(a.solution).read_text())
    row = next(t for t in sol['rows'] if t['address'] == a.address)
    names = Names()
    cur = cname(row['current']) if row['current'] else None
    bases = [b.strip() for b in a.names.split(',') if b.strip()] or \
        [b for b, k in names.sources(a.address, cur) if k != 'ida']
    seen, out = set(), []
    for b in bases:
        for nm, d, what in [(b, 0, 'as given')] + variants(b, names.vocab, plural=names.is_array(a.address)):
            o = omf(nm)
            bk = bucket(o)
            wl, wh = row['slot_window'] if row.get('kind') == 'unnamed' else (row['lo'], row['hi'])
            if o.lower() in seen or not (wl <= bk <= wh):
                continue
            seen.add(o.lower())
            dl = len(nm) - len(cur) if cur else 0
            out.append((d + 0.25 * abs(dl), nm, bk, dl, what))
    out.sort()
    print(json.dumps({'address': a.address, 'current': row['current'], 'window': row['slot_window'] if row.get('kind') == 'unnamed' else [row['lo'], row['hi']],
                      'bases': bases}, default=str))
    for c, nm, bk, dl, what in out[:a.limit]:
        print(f'{nm:32} bucket {bk:3}  len{dl:+d}  {what}')
    if not out:
        print('no spelling of these bases lands in the window; supply other semantic words')
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('solve')
    s.add_argument('--inventory')
    s.add_argument('--link', default=str(WORK / 'link_names.json'))
    s.add_argument('--hints', default=str(WORK / 'hints.json') if (WORK / 'hints.json').exists() else None)
    s.add_argument('--types', help='optional TSV of C type evidence per address')
    s.add_argument('--bands', default=str(WORK / 'bands.json'))
    s.add_argument('--out', default=str(WORK / 'out'))
    s.add_argument('--rounds', type=int, default=40)
    s.add_argument('--keep-registry', action='store_true', help='only registry names where one exists')
    s.add_argument('--fixed', help='json {address: C name} of reviewed names to keep')
    g = sub.add_parser('suggest', help='readable spellings of given words/names inside a row window')
    g.add_argument('--solution', required=True)
    g.add_argument('--address', type=int, required=True)
    g.add_argument('--names', default='', help='comma-separated base names (C spelling)')
    g.add_argument('--limit', type=int, default=40)
    f = sub.add_parser('fixtures')
    m = sub.add_parser('simulate')
    m.add_argument('--inventory')
    m.add_argument('--link', default=str(WORK / 'link_names.json'))
    m.add_argument('--hints')
    m.add_argument('--types')
    m.add_argument('--names')
    v = sub.add_parser('verify')
    v.add_argument('--solution', required=True)
    v.add_argument('--bisect', action='store_true')
    a = ap.parse_args()
    return {'solve': cmd_solve, 'fixtures': cmd_fixtures, 'simulate': cmd_simulate, 'verify': cmd_verify,
            'suggest': cmd_suggest}[a.cmd](a)


if __name__ == '__main__':
    sys.exit(main())
