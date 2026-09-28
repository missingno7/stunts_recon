"""commfit (DIAGNOSTIC): module processing order and per-name first sight from real LINK inputs.

Runs the canonical real link (tools/reallink.py, oracle-derived order; DIAGNOSTIC) with its
output redirected to build/commfit/reallink/, or reads an existing link directory, and parses every
input OBJ in LINK.RSP order.  For each OMF name (EXTDEF 8C, PUBDEF 90/91, COMDEF B0; LINK
keys case-insensitively) it records the first-sight encounter (module index, record index,
position in record).  The raw communal unit (bss_c_common) and alias shims are reported
separately so the solver can exclude them.  Landed from the commfit worker (integ39).

    python tools/linkorder.py --run      # canonical real link -> build/commfit/link_names.json
"""
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WORK = ROOT / 'build' / 'commfit'
sys.path.insert(0, str(ROOT / 'tools'))


def records(blob):
    at = 0
    while at + 3 <= len(blob):
        kind = blob[at]; ln = struct.unpack_from('<H', blob, at + 1)[0]
        yield kind, blob[at + 3:at + 3 + ln - 1]
        at += 3 + ln


def _index(b, p):
    v = b[p]
    if v & 0x80:
        return ((v & 0x7F) << 8) | b[p + 1], p + 2
    return v, p + 1


def _comnum(b, p):
    t = b[p]; p += 1
    if t < 0x80:
        return t, p
    n = {0x81: 2, 0x84: 3, 0x88: 4}[t]
    return int.from_bytes(b[p:p + n], 'little'), p + n


def names_of(blob):
    """[(kind, name, extra)] in record order. kind: ext, pub, com, lext, lpub, theadr."""
    out = []
    for kind, body in records(blob):
        if kind == 0x80:
            n = body[0]; out.append(('theadr', body[1:1 + n].decode('latin1'), None))
        elif kind in (0x8C, 0xB4, 0xB5):
            p = 0
            while p < len(body):
                n = body[p]; nm = body[p + 1:p + 1 + n].decode('latin1'); p += 1 + n
                _, p = _index(body, p)
                out.append(('ext' if kind == 0x8C else 'lext', nm, None))
        elif kind in (0x90, 0x91, 0xB6, 0xB7):
            p = 0
            _, p = _index(body, p); seg, p = _index(body, p)
            if seg == 0:
                p += 2
            w = 4 if kind & 1 else 2
            while p < len(body):
                n = body[p]; nm = body[p + 1:p + 1 + n].decode('latin1'); p += 1 + n
                off = int.from_bytes(body[p:p + w], 'little'); p += w
                _, p = _index(body, p)
                out.append(('pub' if kind in (0x90, 0x91) else 'lpub', nm, off))
        elif kind == 0xB0:
            p = 0
            while p < len(body):
                n = body[p]; nm = body[p + 1:p + 1 + n].decode('latin1'); p += 1 + n
                _, p = _index(body, p)
                t = body[p]; p += 1
                if t == 0x62:
                    size, p = _comnum(body, p); out.append(('com', nm, {'kind': 'near', 'size': size}))
                elif t == 0x61:
                    e, p = _comnum(body, p); cnt, p = _comnum(body, p)
                    out.append(('com', nm, {'kind': 'far', 'size': e * cnt}))
                else:
                    raise ValueError('COMDEF type %02x' % t)
    return out


def read_link_dir(link_dir):
    rsp = (link_dir / 'LINK.RSP').read_text(errors='replace')
    files = []
    for line in rsp.splitlines():
        line = line.strip()
        if not line:
            continue
        files += [f for f in line.rstrip('+').split('+') if f]
        if not line.endswith('+'):
            break
    listing = {}
    if (link_dir / 'objects.json').exists():
        listing = {r['file'][:-4].upper(): r for r in json.loads((link_dir / 'objects.json').read_text())}
    mods = []
    for i, f in enumerate(files):
        blob = (link_dir / (f + '.OBJ' if not f.upper().endswith('.OBJ') else f)).read_bytes()
        r = listing.get(f.upper().replace('.OBJ', ''), {})
        mods.append({'index': i, 'file': f, 'unit': r.get('unit'), 'kind': r.get('kind'),
                     'names': names_of(blob)})
    return mods


def key(n):
    return bytes(ch | 0x20 for ch in n.encode('latin1'))


def first_sight(mods, skip_units=()):
    """name key -> dict(first=(mod, rec_pos), module, kind, spelling, refs=[module idx...])."""
    tab = {}
    for m in mods:
        if m.get('unit') in skip_units:
            continue
        for pos, (k, nm, extra) in enumerate(m['names']):
            if k not in ('ext', 'pub', 'com'):
                continue
            kk = key(nm)
            row = tab.get(kk)
            if row is None:
                row = tab[kk] = {'name': nm, 'encounter': [m['index'], pos], 'module': m.get('unit') or m['file'],
                                 'kind': k, 'refs': []}
            if not row['refs'] or row['refs'][-1] != m['index']:
                row['refs'].append(m['index'])
    return tab


def run_canonical(tag='commfit-base', stage=None):
    """The canonical real link with its outputs under build/commfit/reallink/."""
    import reallink
    previous = reallink.OUT
    reallink.OUT = WORK / 'reallink'
    try:
        return reallink.run(runtime='members', partial='raw', order='oracle', tag=tag, stage=stage or None)
    finally:
        reallink.OUT = previous


def unit_map(report):
    """M%03d.OBJ -> unit id via report order (after merge_raw_runs, names differ; use link listing)."""
    link = report.get('link', {})
    return link.get('objects') or link.get('listing')


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('--run', action='store_true', help='run the canonical real link first')
    ap.add_argument('--link-dir', default=str(WORK / 'reallink' / 'link'))
    ap.add_argument('--out', default=str(WORK / 'link_names.json'))
    a = ap.parse_args()
    if a.run:
        rep = run_canonical()
        print(json.dumps({k: rep.get(k) for k in ('path',)}, indent=1))
        (WORK / 'base_link_listing.json').write_text(json.dumps(rep.get('link', {}).get('listing'), indent=1, default=str))
    mods = read_link_dir(Path(a.link_dir))
    Path(a.out).write_text(json.dumps(mods))
    print(len(mods), 'modules', sum(len(m['names']) for m in mods), 'names')
