"""Runtime BSS operand grounding and diagnostic naming models."""
import json
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import bss_link  # noqa: E402

FIXTURES = ROOT / 'tests' / 'fixtures'


class PlacementAfterRawTests(unittest.TestCase):


    def test_grounded_member_bss_reads_own_fixup_operands(self):
        class Obj:
            segment_defs = [{'name': '_TEXT', 'class': 'CODE'}]
            linker_fixups = [{'target_kind': 'segment', 'target': '_BSS', 'segment': '_TEXT', 'loc': 'offset16',
                              'self_relative': False, 'offset': o, 'encoded_addend': e, 'displacement': 0}
                             for o, e in ((2, '0000'), (6, '0400'))]
        image = bytearray(16)
        image[2:4] = (207000 - 178032).to_bytes(2, 'little')
        image[6:8] = (207004 - 178032).to_bytes(2, 'little')
        self.assertEqual(bss_link.grounded_member_bss(Obj(), 0, bytes(image), 178032), (207000, 2))
        image[6:8] = (207006 - 178032).to_bytes(2, 'little')
        self.assertEqual(bss_link.grounded_member_bss(Obj(), 0, bytes(image), 178032)[0], None)


class DiagnosticModelTests(unittest.TestCase):

    def test_communal_model_reproduces_real_link_maps(self):
        import communal_order
        corpus = json.loads((FIXTURES / 'link365_communal_fixtures.json').read_text())
        self.assertEqual(len(corpus['fixtures']), 5)
        for fx in corpus['fixtures']:
            rows = communal_order.order([tuple(c) for c in fx['commons']], [tuple(o) for o in fx['others']])
            observed = {communal_order._key(n): (k, off) for n, k, off in fx['observed']}
            base = {}
            for row in rows:
                kind, off = observed[communal_order._key(row['name'])]
                self.assertEqual(kind, row['kind'], fx['label'])
                base.setdefault(kind, off)
                self.assertEqual(off - base[kind], row['offset'], (fx['label'], row['name']))
            for kind in ('near', 'far'):
                want = [communal_order._key(r['name']) for r in rows if r['kind'] == kind]
                got = [communal_order._key(n) for n, k, off in sorted(fx['observed'], key=lambda t: t[2])
                       if k == kind]
                self.assertEqual(got, want, fx['label'])
        # the thin bss_link wrapper keeps its integ32 interface
        wrapped = bss_link.communal_order([{'name': '_b', 'size': 2, 'encounter': 1}], near_base=207396)
        self.assertEqual(wrapped[0]['address'], 207396)

    def test_file_c_buffers_fall_in_their_target_buckets(self):
        import communal_order
        self.assertEqual([communal_order.bucket(n) for n in ('__bufout', '__bufin', '__buferr')], [43, 102, 233])

    def test_static_model_reproduces_compiled_fixtures(self):
        corpus = json.loads((FIXTURES / 'msc510_static_order_fixtures.json').read_text())
        self.assertEqual(len(corpus['short_fixtures']), 44)
        for fx in corpus['short_fixtures']:
            names = fx['declared']
            offsets = bss_link.static_order(names, {n: 2 for n in names}, {n: 'short' for n in names})
            self.assertEqual(sorted(offsets, key=offsets.get), fx['observed_order'], fx['fixture'])
            self.assertEqual(max(offsets.values()) + 2, fx['bss_length'], fx['fixture'])
        typed = corpus['typed_fixture']
        names = [n for n, _, _ in typed['declared']]
        sizes = {n: s for n, _, s in typed['declared']}
        types = {n: t for n, t, _ in typed['declared']}
        self.assertEqual(bss_link.static_order_residual(names, sizes, types, typed['observed_offsets']), {})

    def test_models_are_diagnostic_only(self):
        importers = []
        for path in sorted((ROOT / 'tools').glob('*.py')):
            text = path.read_text(encoding='utf-8', errors='replace')
            if re.search(r'^\s*(import|from)\s+(communal_order|msc_static_model)\b', text, re.M):
                importers.append(path.name)
        # No acceptance module imports a diagnostic naming model.
        self.assertEqual(importers, ['bss_link.py'])
        for gate_module in ('communal_unit.py', 'reallink.py', 'promote.py', 'batch_publish.py', 'build_exact.py'):
            text = (ROOT / 'tools' / gate_module).read_text(encoding='utf-8')
            self.assertNotRegex(text, r'^\s*(import|from)\s+(communal_order|msc_static_model|commfit)')
        source = (ROOT / 'tools/bss_link.py').read_text(encoding='utf-8')
        for gate in ('def check_static', 'def gate', 'def independent_static_order', 'def check_communals'):
            body = source.split(gate, 1)[1].split('\ndef ', 1)[0]
            self.assertNotIn('communal_order(', body)
            self.assertNotIn('static_order(', body.replace('independent_static_order(', ''))


if __name__ == '__main__':
    unittest.main()
