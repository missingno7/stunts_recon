"""commfit staged real link of a communal name set (DIAGNOSTIC; landed from commfit, integ39).

Takes the canonical real-link inputs written by linkorder.py (build/commfit/reallink/link:
M*.OBJ in LINK.RSP order, from tools/reallink.py oracle-derived order), and
  * renames OMF names in every input OBJ (EXTDEF/PUBDEF/COMDEF records; positions unchanged,
    so the first-sight encounters are those of a recompiled TU with the same references),
  * restores the pinned `_file.c` buffers as COMDEFs (the canonical link re-declares them as
    EXTDEFs), and
  * replaces the raw communal unit (last OBJ, one oracle-sized c_common segment with publics)
    by a COMDEF-only module that declares every communal row with its size,
then runs the pinned LINK 3.65 and reads the MAP address of every communal.  It compares the
MAP with the commfit forward model and with the target addresses.

This is a model check on the real program population, not an acceptance path: the renamed
objects are OMF-level renames, not recompiled sources (namefit verifies those separately).
Its output directory can be judged, without publishing, by
`python tools/communal_unit.py dry-run build/commfit/stage/TAG --base-dir build/commfit/reallink/link`.
"""
import json
import re
import shutil
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WORK = ROOT / 'build' / 'commfit'
sys.path.insert(0, str(ROOT / 'tools'))


def rec(kind, body):
    data = bytes([kind]) + struct.pack('<H', len(body) + 1) + body
    return data + bytes([(-sum(data)) & 0xFF])


def nm(s):
    b = s.encode('latin1')
    return bytes([len(b)]) + b


def comlen(n):
    if n < 0x80:
        return bytes([n])
    if n < 0x10000:
        return b'\x81' + struct.pack('<H', n)
    if n < 0x1000000:
        return b'\x84' + n.to_bytes(3, 'little')
    return b'\x88' + struct.pack('<I', n)


def comdef_module(rows, label='COMMFIT'):
    out = rec(0x80, nm(label))
    chunk = b''
    for name, size in rows:
        item = nm(name) + b'\x00' + b'\x62' + comlen(size)
        if len(chunk) + len(item) > 900:
            out += rec(0xB0, chunk)
            chunk = b''
        chunk += item
    if chunk:
        out += rec(0xB0, chunk)
    out += rec(0x8A, b'\x00')
    return out


def _index(b, p):
    v = b[p]
    if v & 0x80:
        return p + 2
    return p + 1


def rename_object(blob, mapping, comdef_restore=None):
    """Rewrite names in EXTDEF/PUBDEF/COMDEF records.  mapping: lower OMF name -> new OMF name.
    comdef_restore: {name: size} -> an EXTDEF record naming exactly those is turned back into COMDEF."""
    out = bytearray()
    at = 0
    changed = 0
    while at < len(blob):
        kind = blob[at]
        ln = struct.unpack_from('<H', blob, at + 1)[0]
        body = blob[at + 3:at + 3 + ln - 1]
        end = at + 3 + ln
        if kind in (0x8C, 0xB0, 0x90, 0x91):
            nb = bytearray()
            p = 0
            if kind in (0x90, 0x91):
                p = _index(body, p)
                seg_at = p
                seg = body[p] if not body[p] & 0x80 else ((body[p] & 0x7F) << 8 | body[p + 1])
                p = _index(body, p)
                if seg == 0:
                    p += 2
                nb += body[:p]
            names_here = []
            while p < len(body):
                n = body[p]
                name = body[p + 1:p + 1 + n].decode('latin1')
                p += 1 + n
                new = mapping.get(name.lower(), name)
                if new != name:
                    changed += 1
                names_here.append(new)
                nb += nm(new)
                q = p
                if kind == 0x8C:
                    p = _index(body, p)
                elif kind == 0xB0:
                    p = _index(body, p)
                    t = body[p]; p += 1
                    for _ in range(1 if t == 0x62 else 2):
                        v = body[p]; p += 1 + {0x81: 2, 0x84: 3, 0x88: 4}.get(v, 0)
                else:
                    p += 4 if kind & 1 else 2
                    p = _index(body, p)
                nb += body[q:p]
            if kind == 0x8C and comdef_restore and set(n.lower() for n in names_here) == set(comdef_restore):
                cb = b''.join(nm(n) + b'\x00\x62' + comlen(comdef_restore[n.lower()]) for n in names_here)
                out += rec(0xB0, cb)
            else:
                out += rec(kind, bytes(nb))
        else:
            out += blob[at:end]
        at = end
    return bytes(out), changed


MAPLINE = re.compile(r'^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+(?:Abs\s+|Imp\s+)?(\S+)\s*$')


def read_map(text):
    pubs = {}
    part = text.split('Publics by Name')[1].split('Publics by Value')[0] if 'Publics by Name' in text else text
    for line in part.splitlines():
        m = MAPLINE.match(line)
        if m:
            pubs[m.group(3)] = int(m.group(1), 16) * 16 + int(m.group(2), 16)
    segs = re.findall(r'^\s*([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+(\S+)\s+(\S+)', text, re.M)
    return pubs, [(int(a, 16), int(b, 16), int(c, 16), n, k) for a, b, c, n, k in segs]


def run(rows, mapping, tag, base_dir=WORK / 'reallink' / 'link'):
    """rows: [(omf_name, size)] in any order (all communals incl. __buf*); mapping: old->new names."""
    import reallink
    work = WORK / 'stage' / tag
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    listing = json.loads((base_dir / 'objects.json').read_text())
    lower = {k.lower(): v for k, v in mapping.items()}
    bufs = {'__bufout': 512, '__bufin': 512, '__buferr': 512}
    total_changed = 0
    files = []
    for r in listing:
        blob = (base_dir / r['file']).read_bytes()
        if r['unit'] == 'bss':
            blob = comdef_module(rows)
        else:
            blob, ch = rename_object(blob, lower, bufs if r['unit'].startswith('rtd_library__file') else None)
            total_changed += ch
        (work / r['file']).write_bytes(blob)
        files.append(r['file'][:-4])
    shutil.copyfile(base_dir / 'LINK.RSP', work / 'LINK.RSP')
    config, _ = reallink.verify_toolchain('msc510-medium')
    tc = (ROOT / config['directory']).resolve()
    rc, out, argv = reallink.run_dos(tc / 'LINK.EXE', ['@LINK.RSP'], work, 600)
    (work / 'link.log').write_text(out, encoding='utf-8')
    mp = work / 'RESULT.MAP'
    res = {'returncode': rc, 'renamed_references': total_changed, 'log_tail': out[-1500:]}
    if mp.exists():
        pubs, segs = read_map(mp.read_text(errors='replace'))
        res['publics'] = pubs
        res['segments'] = [s for s in segs if s[4] in ('BSS', 'STACK') or s[3] in ('c_common',)]
    return res


if __name__ == '__main__':
    import argparse
    import commfit as cf
    ap = argparse.ArgumentParser()
    ap.add_argument('--solution', help='commfit solution.json (chosen names; unnamed slots get placeholders)')
    ap.add_argument('--current', action='store_true', help='link the CURRENT names (model check only)')
    ap.add_argument('--tag', default='current')
    a = ap.parse_args()
    mods, sight = cf.load_link(WORK / 'link_names.json')
    names = cf.Names(WORK / 'hints.json')
    rows = cf.load_inventory(None, mods, sight, names)
    mapping, chosen = {}, []
    if a.solution:
        sol = {t['address']: t for t in json.loads(Path(a.solution).read_text())['rows']}
    for r in rows:
        if a.current or not a.solution:
            c = r['current']
        else:
            t = sol[r['address']]
            c = t['chosen']
            if not c:
                # measurement-only placeholder spelled into the DP's bucket (never a proposal)
                c = cf.bucket_placeholder(r['address'], t['bucket'])
            if r['current'] and c != r['current']:
                mapping[r['current']] = c
        chosen.append(c)
    # rows whose chosen name is a placeholder are first seen in the COMDEF module (last)
    last = len(json.loads((WORK / 'reallink/link/objects.json').read_text())) - 1
    sim_rows = []
    for r, c in zip(rows, chosen):
        rr = dict(r)
        if rr['encounter'] is None:
            rr['encounter'] = (last, [x for x in chosen].index(c))
        sim_rows.append(rr)
    pred, _ = cf.simulate(sim_rows, chosen)
    res = run([(c, r['size']) for r, c in zip(rows, chosen)], mapping, a.tag)
    pubs = {k.lower(): v for k, v in res.get('publics', {}).items()}
    cmp_rows = []
    for r, c in zip(rows, chosen):
        got = pubs.get(c.lower())
        cmp_rows.append({'address': r['address'], 'name': c, 'map': got, 'model': pred[r['address']],
                         'target': r['address']})
    model_eq = sum(1 for x in cmp_rows if x['map'] == x['model'])
    target_eq = sum(1 for x in cmp_rows if x['map'] == x['target'])
    summary = {'tag': a.tag, 'link_rc': res['returncode'], 'rows': len(cmp_rows),
               'map_equals_model': model_eq, 'map_equals_target': target_eq,
               'renamed_references': res['renamed_references'],
               'c_common': [s for s in res.get('segments', []) if s[3] == 'c_common'],
               'model_mismatches': [x for x in cmp_rows if x['map'] != x['model']][:20]}
    (WORK / 'stage' / a.tag / 'compare.json').write_text(json.dumps({'summary': summary, 'rows': cmp_rows}, indent=1))
    print(json.dumps(summary, indent=1))
