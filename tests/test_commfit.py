"""commfit (DIAGNOSTIC communal-name solver): model and inverse checks on the real-LINK fixtures."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import communal_order as co          # noqa: E402
import commfit                       # noqa: E402

FIX = json.loads((ROOT / 'tests/fixtures/link365_communal_fixtures.json').read_text())['fixtures']


def rows_of(fx):
    first, size = {}, {}
    for enc, name, sz, *kind in fx['commons']:
        k = co._key(name)
        first.setdefault(k, enc)
        size[k] = max(size.get(k, 0), sz)
    for name, enc in fx['others']:
        k = co._key(name)
        first[k] = min(first.get(k, enc), enc)
    near = sorted((off, n) for n, kind, off in fx['observed'] if kind == 'near')
    return [{'address': off, 'encounter': (first[co._key(n)], 0), 'size': size[co._key(n)],
             'fixed': False, 'current': n} for off, n in near]


class CommfitTests(unittest.TestCase):
    def test_true_names_are_the_zero_cost_solution(self):
        for fx in FIX:
            rows = rows_of(fx)
            cands = [[(r['current'], 0.0, 'true', ''), (None, 60.0, 'slot', '')] for r in rows]
            total, choice = commfit.solve_dp(rows, cands)
            self.assertEqual(total, 0.0, fx['label'])
            self.assertTrue(all(c[0] == r['current'] for c, r in zip(choice, rows)))

    def test_hidden_rows_solution_simulates_to_the_map(self):
        al = 'abcdefghijklmnopqrstuvwxyz0123456789'
        sfx = [a + b for a in al for b in al] + ['x' + a + b for a in al for b in al]
        for fx in FIX:
            rows = rows_of(fx)
            cands = []
            for k, r in enumerate(rows):
                if k % 3 == 1:
                    cands.append([(r['current'] + '_' + x, 1.0, 'foreign', '') for x in sfx])
                else:
                    cands.append([(r['current'], 0.0, 'true', '')])
            total, choice = commfit.solve_dp(rows, cands)
            self.assertIsNotNone(choice, fx['label'])
            sim = co.order([{'name': c[0], 'size': r['size'], 'kind': 'near', 'encounter': r['encounter'][0]}
                            for c, r in zip(choice, rows)])
            got = {co._key(x['name']): x['offset'] for x in sim}
            for c, r in zip(choice, rows):
                self.assertEqual(got[co._key(c[0])], r['address'], (fx['label'], c[0]))

    def test_out_of_order_bucket_is_refused(self):
        rows = [{'address': 0, 'encounter': (1, 0), 'size': 2, 'fixed': False, 'current': '_a'},
                {'address': 2, 'encounter': (1, 1), 'size': 2, 'fixed': False, 'current': '_b'}]
        hi = next(n for n in ('_z%d' % i for i in range(999)) if co.bucket(n) == 200)
        lo = next(n for n in ('_y%d' % i for i in range(999)) if co.bucket(n) == 10)
        self.assertIsNone(commfit.solve_dp(rows, [[(hi, 0, '', '')], [(lo, 0, '', '')]])[1])
        # equal buckets need a descending encounter: (1,0) before (1,1) is refused
        eq = next(n for n in ('_w%d' % i for i in range(999)) if co.bucket(n) == 200)
        self.assertIsNone(commfit.solve_dp(rows, [[(hi, 0, '', '')], [(eq, 0, '', '')]])[1])
        rows[0]['encounter'] = (2, 0)
        self.assertIsNotNone(commfit.solve_dp(rows, [[(hi, 0, '', '')], [(eq, 0, '', '')]])[1])

    def test_windows(self):
        rows = [{'encounter': (5 - i, 0)} for i in range(4)]
        self.assertEqual(commfit.feasible_windows(rows, [None, 7, None, None]), [(0, 7), (7, 7), (7, 255), (7, 255)])

    def test_neutral_rows_use_the_final_module_encounter(self):
        # integ40 (nameBfix diag_feasible): a row no linked object names is first seen in the
        # final COMDEF-only module, so it may precede, but never follow, a named row of its bucket.
        rows = [{'encounter': None}, {'encounter': (1, 0)}, {'encounter': None}]
        self.assertEqual(commfit.encounter(rows, 2), (10 ** 6, 2))
        self.assertEqual(commfit.feasible_windows(rows, [7, 7, None]), [(7, 7), (7, 7), (8, 255)])
        self.assertTrue(commfit.tie_after(rows, 0, 1) and not commfit.tie_after(rows, 1, 2))
        name = next(n for n in ('_n%d' % i for i in range(999)) if co.bucket(n) == 7)
        other = next(n for n in ('_m%d' % i for i in range(999)) if co.bucket(n) == 7)
        self.assertIsNotNone(commfit.solve_dp(rows[:2], [[(name, 0, '', '')], [(other, 0, '', '')]])[1])
        self.assertIsNone(commfit.solve_dp(rows[1:], [[(name, 0, '', '')], [(other, 0, '', '')]])[1])


if __name__ == '__main__':
    unittest.main()
