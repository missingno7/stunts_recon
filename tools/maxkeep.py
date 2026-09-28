"""commfit (DIAGNOSTIC): maximum number of existing spellings (registry / current) that can
stay simultaneously under the LINK 3.65 communal order, every other row being a free slot
(any bucket).  Landed from the commfit worker (integ39); output under build/commfit/.

    python tools/maxkeep.py --inventory INVENTORY.json [--link build/commfit/link_names.json]
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import commfit as cf       # noqa: E402


def maxkeep(rows, names):
    res = {}
    for mode in ('registry', 'current'):
        def spelling(r):
            return names.registry.get(r['address']) if mode == 'registry' else (
                cf.cname(r['current']) if r['current'] else None)
        cands = []
        for r in rows:
            if r['fixed']:
                cands.append([(r['current'], 0.0, 'anchor', '')])
                continue
            nm = spelling(r)
            cs = [(None, 1.0, 'slot', '')]
            if nm:
                cs.append((cf.omf(nm), 0.0, mode, ''))
            cands.append(cs)
        _, choice = cf.solve_dp(rows, cands)
        have = sum(1 for r in rows if not r['fixed'] and spelling(r))
        kept = [(r['address'], c[0], c[1]) for r, c in zip(rows, choice) if c[3] == mode]
        dropped = [(r['address'], spelling(r)) for r, c in zip(rows, choice) if c[3] == 'slot' and spelling(r)]
        res[mode] = {'names': have, 'max_kept': len(kept), 'kept': kept, 'must_change': dropped}
        print(mode, 'names', have, 'max simultaneously kept', len(kept))
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--inventory', required=True, help='communal inventory (see commfit.load_inventory)')
    ap.add_argument('--link', default=str(cf.WORK / 'link_names.json'))
    ap.add_argument('--out', default=str(cf.WORK / 'maxkeep.json'))
    a = ap.parse_args()
    mods, sight = cf.load_link(a.link)
    names = cf.Names()
    rows = cf.load_inventory(a.inventory, mods, sight, names)
    Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    Path(a.out).write_text(json.dumps(maxkeep(rows, names), indent=1))


if __name__ == '__main__':
    main()
