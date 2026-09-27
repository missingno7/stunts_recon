"""Natural abbreviations of semantic identifiers (1990-style C spellings).

variants(name) -> list of (spelling, cost).  cost = departure from the reference spelling:
  0    reference spelling
  +1   per word replaced by a dictionary abbreviation (ABBR: common programmer abbreviations)
  +1.5 per dropped word (at least one content word must remain; generic words cost +0.5)
  +0.5 joining words without underscores when the reference used underscores
Compound lowercase words are segmented with the dictionary vocabulary (gameconfig -> game config).
Address-style names (word_2C0FC, byte_3B8F2, unk_..., aString literals) are NEUTRAL project names
and are never abbreviated.  No vowel-dropping, padding or invented syllables: every piece of a
spelling is a reference word, a dictionary abbreviation of it, or omitted.
"""
import functools
import itertools
import re

ABBR = {
    'number': ['num', 'no'], 'count': ['cnt'], 'counter': ['cnt', 'ctr'], 'index': ['idx', 'ix'],
    'indices': ['idxs', 'ixs'], 'buffer': ['buf'], 'pointer': ['ptr'], 'ptr': ['p'], 'palette': ['pal'],
    'sprite': ['spr'], 'player': ['pl', 'plr'], 'position': ['pos'], 'pos': ['p'], 'length': ['len'],
    'address': ['addr'], 'previous': ['prev'], 'current': ['cur'], 'temporary': ['tmp'], 'temp': ['tmp'],
    'character': ['chr'], 'string': ['str'], 'screen': ['scr'], 'value': ['val'], 'table': ['tab', 'tbl'],
    'tables': ['tabs', 'tbls'], 'message': ['msg'], 'maximum': ['max'], 'minimum': ['min'], 'source': ['src'],
    'destination': ['dst'], 'offset': ['ofs', 'off'], 'segment': ['seg'], 'control': ['ctl'],
    'handler': ['hdlr'], 'interrupt': ['intr'], 'keyboard': ['kbd', 'kb'], 'initialize': ['init'],
    'graphics': ['gfx'], 'background': ['bg'], 'sound': ['snd'], 'frame': ['frm'], 'level': ['lvl', 'lev'],
    'button': ['btn'], 'config': ['cfg'], 'configuration': ['cfg'], 'directory': ['dir'], 'error': ['err'],
    'image': ['img'], 'memory': ['mem'], 'vertical': ['vert'], 'horizontal': ['horz'], 'velocity': ['vel'],
    'status': ['stat'], 'update': ['upd'], 'camera': ['cam'], 'custom': ['cust'], 'elevation': ['elev'],
    'azimuth': ['azim'], 'distance': ['dist'], 'angle': ['ang'], 'threshold': ['thr', 'thresh'],
    'detail': ['det'], 'transformed': ['trans'], 'transform': ['trans'], 'shape': ['shp'], 'shapes': ['shps'],
    'lookahead': ['ahead'], 'tiles': ['tls'], 'track': ['trk'], 'center': ['ctr'], 'terrain': ['terr'],
    'height': ['hgt'], 'heights': ['hgts'], 'opponent': ['opp'], 'video': ['vid'], 'flag': ['flg'],
    'flags': ['flgs'], 'random': ['rnd', 'rand'], 'rotate': ['rot'], 'rotation': ['rot'], 'plane': ['pln'],
    'result': ['res'], 'array': ['arr'], 'color': ['col', 'clr'], 'colour': ['col', 'clr'],
    'window': ['wnd'], 'resource': ['res'], 'resources': ['res'], 'header': ['hdr'], 'direction': ['dir'],
    'related': ['rel'], 'check': ['chk'], 'element': ['elem'], 'elements': ['elems'], 'copy': ['cpy'],
    'management': ['mgmt'], 'backlights': ['blights'], 'override': ['ovr'], 'dialog': ['dlg'],
    'font': ['fnt'], 'elapsed': ['elap'], 'time': ['tm'], 'security': ['sec'], 'skybox': ['sky'],
    'ground': ['grd'], 'water': ['wat'], 'objects': ['objs'], 'object': ['obj'], 'codes': ['cds'],
    'code': ['cd'], 'list': ['lst'], 'vector': ['vec'], 'vectors': ['vecs'], 'scene': ['scn'],
    'material': ['mat'], 'preview': ['pvw'], 'rectangle': ['rect'], 'penalty': ['pen'], 'replay': ['rpl'],
    'mode': ['md'], 'widths': ['wids'], 'width': ['wid'], 'meter': ['mtr'], 'needle': ['ndl'],
    'start': ['st'], 'game': ['gm'], 'data': ['dat'], 'unknown': ['unk'],
    'response': ['resp'], 'steering': ['steer'], 'wheel': ['whl'], 'wheels': ['whls'], 'intro': ['intr'],
    'text': ['txt'], 'ingame': ['ig'], 'draw': ['drw'], 'load': ['ld'], 'file': ['f'], 'render': ['rend'],
    'sphere': ['sph'], 'line': ['ln'], 'polygon': ['poly'], 'matrix': ['mtx', 'mat'], 'multiply': ['mul'],
    'scale': ['scl'], 'radius': ['rad'], 'polar': ['pol'], 'mouse': ['ms'], 'timer': ['tmr'],
    'delta': ['dlt'], 'audio': ['aud'], 'helper': ['hlp'], 'format': ['fmt'], 'explosion': ['expl'],
    'projection': ['proj'], 'rects': ['rcs'], 'consts': ['k'], 'constants': ['k'], 'paint': ['pnt'],
    'passed': ['pass'], 'show': ['shw'], 'minus': ['neg'], 'state': ['st'], 'direction_related': ['dir'],
    'transformedshape': ['tshape'], 'transshape': ['tshape'], 'arg2array': ['arg2s'], 'zarray': ['zs'],
    'vecs': ['vs'], 'vec': ['v'], 'elem': ['el'], 'map': ['map'], 'rect': ['rc'], 'cliprect': ['clip'],
    'clrlist': ['clrs'], 'sceneshapes': ['scene'], 'carshapevec': ['carvec'], 'carshapevecs': ['carvecs'],
    'trkobj': ['tobj'], 'fence': ['fnc'], 'hill': ['hill'], 'road': ['rd'], 'opp': ['o'],
    'sdgame': ['sdg'], 'roof': ['rf'], 'bmp': ['bmp'], 'lookup': ['lkup'], 'preRender': ['prer'],
    'orientation': ['orient'], 'position3d': ['pos3d'], 'sincos': ['sc'], 'performance': ['perf'],
    'selection': ['sel'], 'menu': ['mnu'], 'input': ['inp'], 'kevin': ['kev'], 'crash': ['crsh'],
    'explode': ['expl'], 'sinking': ['sink'], 'reset': ['rst'], 'setup': ['set'], 'copied': ['cpy'],
}
GENERIC = {'unk', 'related', 'main', 'the', 'of', 'value', 'data', 'var', 'by', 'as', 'to', 'for', 'is', 'a',
           'and', 'or', 'with', 'from', 'in', 'at', 'on'}
MODIFIERS = {'cur', 'current', 'new', 'old', 'prev', 'previous', 'last', 'next', 'temp', 'tmp', 'p', 'g',
             'max', 'min', 'total', 'num', 'is', 'my', 'the'}
VOCAB = set(ABBR) | GENERIC | {'car', 'shape', 'vec', 'trans', 'cur', 'fence', 'obj', 'codes', 'hill', 'road',
                                'arg', 'z', 'x', 'y', 'ptr', 'list', 'res', 'sd', 'game', 'in', 'on', 'off',
                                'row', 'rows', 'col', 'cols', 'lo', 'hi', 'up', 'down', 'min', 'max', 'buf'}


def is_address_name(name):
    if any(re.fullmatch(r'[0-9A-Fa-f]{4,6}', w) and re.search(r'\d', w) for w in name.split('_')):
        return True
    return bool(re.match(r'^(word|byte|dword|unk|off|data|stru|asc|loc|sub|nullsub|nopsub|flt|dbl|seg)_[0-9A-Fa-f]{3,6}(_\w+)?$', name)) \
        or bool(re.match(r'^a[A-Z0-9][A-Za-z0-9_]*$', name))


@functools.lru_cache(maxsize=None)
def segment(word):
    """Split a lowercase compound into vocabulary words (longest-first DP); None if impossible."""
    w = word.lower()
    if len(w) <= 3 or w in VOCAB:
        return None
    best = {0: []}
    for i in range(1, len(w) + 1):
        for j in range(max(0, i - 16), i):
            if j in best and w[j:i] in VOCAB and len(w[j:i]) >= 2:
                cand = best[j] + [w[j:i]]
                if i not in best or len(cand) < len(best[i]):
                    best[i] = cand
    parts = best.get(len(w))
    return parts if parts and len(parts) > 1 else None


def words(name):
    parts = []
    for chunk in name.split('_'):
        if not chunk:
            continue
        pieces = re.findall(r'[A-Z]+(?![a-z])|[A-Z]?[a-z]+\d*|\d+', chunk) or [chunk]
        for p in pieces:
            if len(p) == 1 and parts and parts[-1][-1:].isdigit():
                parts[-1] = parts[-1] + p          # polarRadius2D -> Radius2D
                continue
            if p.isdigit() and parts:
                parts[-1] = parts[-1] + p          # keep numbers with their word (td14, pos2)
                continue
            m = re.match(r'^([A-Za-z]+)(\d*)$', p)
            seg = list(segment(m.group(1)) or []) if m else None
            if seg:
                seg[-1] += m.group(2)
                parts += seg
            else:
                parts.append(p)
    return parts


EXPAND = {}
for _k, _vs in ABBR.items():
    for _v in _vs:
        if len(_v) >= 3 and _v not in ABBR and '_' not in _k:
            EXPAND.setdefault(_v, _k)
for _amb in ('mat', 'rnd', 'rel', 'res', 'dir', 'st', 'cur', 'pos', 'tab', 'col', 'intr', 'ld', 'pen'):
    EXPAND.pop(_amb, None)          # ambiguous abbreviations are never spelled out
EXPAND.update({'td': 'trackdata', 'rpl': 'replay', 'res': 'resource', 'cfg': 'config', 'ptr': 'pointer',
               'idx': 'index', 'cnt': 'count', 'pos': 'position', 'trk': 'track', 'vec': 'vector',
               'obj': 'object', 'buf': 'buffer', 'kb': 'keyboard', 'joy': 'joystick', 'hdr': 'header',
               'dlg': 'dialog', 'fnt': 'font', 'clr': 'color', 'mgmt': 'management',
               'rect': 'rectangle', 'rects': 'rectangles', 'spr': 'sprite', 'wnd': 'window',
               'shp': 'shape', 'intr': 'interrupt', 'tmr': 'timer',
               'snd': 'sound', 'aud': 'audio', 'misc': 'miscellaneous', 'gfx': 'graphics'})


def _word_options(orig, expand=False):
    low = orig.lower()
    m = re.match(r'^([a-z]+)(\d*)$', low)
    stem, num = (m.group(1), m.group(2)) if m else (low, '')
    cand = {orig: 0.0}
    for a in ABBR.get(stem, []) + ABBR.get(low, []):
        cand.setdefault(a + num, 1.0)
    if expand and stem in EXPAND:
        cand.setdefault(EXPAND[stem] + num, 1.0)      # natural lengthening: spell an abbreviation out
    return cand


def required_words(w, siblings=()):
    """Indices that every natural spelling keeps: the head (first word that is not a modifier or
    connective; for functions the verb), the last content word, and the words that distinguish the
    name from its siblings (names sharing its leading words)."""
    low = [x.lower() for x in w]
    content = [i for i, x in enumerate(low) if x not in GENERIC]
    if not content:
        return set()
    head = next((i for i in content if low[i] not in MODIFIERS and len(low[i]) > 1), content[0])
    req = {head, content[-1]} | {i for i in content if re.search(r'\d', low[i])}
    return req


def sibling_sets(w, siblings):
    """For each sibling: the indices of this name's content words the sibling lacks; a natural
    spelling keeps at least one of them (mouse_draw_opaque_check vs mouse_draw_transparent_check)."""
    low = [x.lower() for x in w]
    out = []
    for sname in siblings:
        sw = {x.lower() for x in words(sname)}
        diff = frozenset(i for i, x in enumerate(low) if x not in GENERIC and x not in sw)
        if diff:
            out.append(diff)
    return out


def variants(name, limit=20000, expand=True, siblings=()):
    """-> list of (spelling, cost) sorted by cost then length.  Deterministic."""
    if is_address_name(name) or len(name) <= 4:
        return [(name, 0.0)]
    w = words(name)
    if len(w) > 7:
        w = w[:6] + [''.join(w[6:])]
    has_us = '_' in name
    required = required_words(w, siblings)
    distinct = sibling_sets(w, siblings)
    opts = []
    for idx, orig in enumerate(w):
        cand = _word_options(orig, expand)
        drop_cost = 0.5 if orig.lower() in GENERIC else 2.0
        if idx not in required:
            cand.setdefault('', drop_cost)
        opts.append(sorted(cand.items(), key=lambda kv: (kv[1], -len(kv[0]))))
    out = {name: 0.0}
    content = [i for i, o in enumerate(w) if o.lower() not in GENERIC]
    for combo in itertools.islice(itertools.product(*opts), limit):
        parts = [p for p, _ in combo if p]
        kept = [i for i, (p, _) in enumerate(combo) if p]
        kept_content = [i for i in kept if i in content]
        if not kept_content:
            continue
        # naturalness rules: required words (head, last, sibling-distinguishing) are never dropped
        # (enforced above); a multi-word name keeps at least two content words, and a name of four
        # or more content words keeps at least half of them (rounded down)
        if distinct and any(not (d & set(kept)) for d in distinct):
            continue
        if len(content) >= 2 and len(kept_content) < 2:
            continue
        if len(content) >= 4 and len(kept_content) < len(content) // 2:
            continue
        cost = sum(c for _, c in combo)
        if len(parts) == 1 and len(parts[0]) < 4:
            continue
        # underscores: keep the reference style, or join (1990 style); never add underscores to a
        # name that had none, and never re-split an unchanged name into snake_case
        spells = []
        if has_us and cost > 0:
            spells.append(('_'.join(parts), 0.0))
        joined = ''.join(p.lower() if i else p for i, p in enumerate(parts))
        spells.append((joined, 0.5 if has_us else 0.0))
        for sp, extra in spells:
            if not re.match(r'^[A-Za-z_]\w*$', sp) or len(sp) > 31 or sp == name:
                continue
            c = cost + extra
            if sp not in out or out[sp] > c:
                out[sp] = c
    return sorted(out.items(), key=lambda kv: (kv[1], -len(kv[0]), kv[0]))


def by_length(name):
    """Cheapest spelling for each length: {len: (spelling, cost)}."""
    best = {}
    for sp, c in variants(name):
        L = len(sp)
        if L not in best or c < best[L][1]:
            best[L] = (sp, c)
    return best


if __name__ == '__main__':
    import sys
    for n in sys.argv[1:]:
        b = by_length(n)
        print(n, words(n), ' '.join(f'{sp}({c:g})' for L, (sp, c) in sorted(b.items())))
