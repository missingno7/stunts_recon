"""autosym (tools/autosym.py): automated symbol closure, integrated by integ25.

The merged layout entries are withheld from the derivation context in memory
and must be re-derived from immutable facts (oracle, inventory, pinned
reference listings); the re-derived proposals then resolve through the
production resolvers with the layouts merged IN MEMORY (no file is written).
Negative: wrong address, cross-object addend, unanchored target,
candidate-driven inference.  Aliases are binding names only.
"""
import copy
import json
import sys
import unittest
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT_DIR / 'tools'))
import autosym  # noqa: E402
import code_symbols  # noqa: E402
import data_symbols  # noqa: E402
from common import ROOT, read_json  # noqa: E402

OPENED = []


def _audit(event, args):
    if event == 'open' and args and isinstance(args[0], (str, bytes, Path)):
        OPENED.append(str(args[0]))


class Layouts:
    """Patch the resolvers' read_json so layout files come from memory."""

    def __init__(self, code, data):
        self.code, self.data = code, data

    def __enter__(self):
        self.saved = (code_symbols.read_json, data_symbols.read_json)
        code_path = (ROOT / 'layout/code-symbols.json').resolve()
        data_path = (ROOT / 'layout/data-symbols.json').resolve()

        def reader(path, original=read_json):
            resolved = Path(path).resolve()
            if resolved == code_path:
                return copy.deepcopy(self.code)
            if resolved == data_path:
                return copy.deepcopy(self.data)
            return original(path)
        code_symbols.read_json = data_symbols.read_json = reader
        return self

    def __exit__(self, *exc):
        code_symbols.read_json, data_symbols.read_json = self.saved


def is_autosym_code(row):
    return str(row.get('proposal_basis', '')).startswith('autosym')


def withheld_context():
    """Derivation context with the merged autosym aliases withheld in memory.

    Code aliases carry their autosym proposal basis; data aliases are the
    reviewed pinned-label forms (reference_label spelling).  Removing them from
    the context makes the derivation re-propose every one that raw code still
    references, from immutable facts only."""
    ctx = autosym.load_context()
    code = ctx['code_layout']['symbols']
    data = ctx['data_layout']['symbols']
    ctx['withheld_code'] = {n: code.pop(n) for n in [n for n, r in code.items() if is_autosym_code(r)]}
    ctx['withheld_data'] = {n: data.pop(n) for n in [n for n, r in data.items()
                                                     if 'reference_label' in r and
                                                     r.get('storage') != 'code_island']}
    return ctx


class AutosymTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        sys.addaudithook(_audit)
        OPENED.clear()
        cls.ctx = withheld_context()
        cls.result = autosym.derive(cls.ctx)
        cls.opened = list(OPENED)
        cls.code = copy.deepcopy(cls.ctx['code_layout'])
        cls.data = copy.deepcopy(cls.ctx['data_layout'])
        cls.code['symbols'].update(cls.result['code_symbols'])
        cls.data['symbols'].update(cls.result['data_symbols'])
        for name, extent in cls.result['width_additions'].items():
            cls.data['symbols'][name].update(extent)
        cls.image, cls.relocations = cls.ctx['image'], cls.ctx['relocations']

    # ------------------------------------------------------------ positive
    def test_proposals_resolve_through_production_resolvers(self):
        pcode, pdata = self.result['code_symbols'], self.result['data_symbols']
        self.assertTrue(pcode and pdata)
        with Layouts(self.code, self.data):
            code = code_symbols.resolve_code_symbols(sorted(pcode), self.image, self.relocations)
            data = data_symbols.resolve_symbols(set(pdata), self.image, self.relocations)
        for name, row in pcode.items():
            self.assertEqual(code[name]['load_address'], row['mapped_target']['start'])
        for name, row in pdata.items():
            self.assertEqual(data[name]['load_address'], row['load_address'])
            self.assertEqual(max(data[name]['allowed_addends']) + 1, row.get('width', 1))

    def test_every_reference_is_classified(self):
        statuses = {r['status'] for r in self.result['references']}
        self.assertLessEqual(statuses, {'existing', 'existing+addend', 'existing+width-addition',
                                        'proposed', 'proposed+addend', 'not-provable', 'not-alias'})
        for r in self.result['references']:
            if r['status'] == 'not-provable':
                self.assertTrue(r.get('class'), r)
            if r['status'].startswith('proposed'):
                self.assertIn(r['alias'], {**self.result['code_symbols'], **self.result['data_symbols']})

    def test_neutral_code_spelling_is_reviewed_alias(self):
        for name, row in self.result['code_symbols'].items():
            self.assertTrue(name.startswith('_code_load_'), name)
            self.assertEqual(row['reviewed_alias'], name)
            self.assertIn('not an original PUBDEF', row['proposal_basis'])

    # ------------------------------------------------- negative: wrong address
    def test_code_alias_at_wrong_address_rejected(self):
        name, row = next(iter(self.result['code_symbols'].items()))
        bad = copy.deepcopy(self.code)
        bad['symbols'][name]['mapped_target']['start'] += 1
        with Layouts(bad, self.data), self.assertRaises(ValueError):
            code_symbols.resolve_code_symbols([name], self.image, self.relocations)
        bad = copy.deepcopy(self.code)
        bad['symbols'][name]['frame_load_address'] += 16
        with Layouts(bad, self.data), self.assertRaises(ValueError):
            code_symbols.resolve_code_symbols([name], self.image, self.relocations)

    def test_data_alias_at_wrong_address_rejected(self):
        reviewed = [n for n, r in self.result['data_symbols'].items() if 'reference_label' in r]
        for name in reviewed[:25]:
            for delta in (1, 2, -2):
                bad = copy.deepcopy(self.data)
                bad['symbols'][name]['load_address'] += delta
                with Layouts(self.code, bad), self.assertRaises(ValueError, msg=name):
                    data_symbols.resolve_symbols({name}, self.image, self.relocations)

    def test_audit_reports_address_spelling_mismatch(self):
        existing = {'_word_3B772': {'load_address': self.ctx['frame'] + 6, 'allowed_addends': [0],
                                    'width': None, 'storage': 'initialized'}}
        findings = autosym.audit_existing(self.ctx, {}, existing)
        self.assertTrue(any(f['name'] == '_word_3B772' for f in findings))

    # --------------------------------------------- negative: cross-object addend
    def test_addend_never_reaches_next_label(self):
        with Layouts(self.code, self.data):
            data = data_symbols.resolve_symbols(set(self.result['data_symbols']), self.image,
                                                self.relocations)
        for name, row in self.result['data_symbols'].items():
            width = row.get('width')
            self.assertNotIn(width or 1, data[name]['allowed_addends'])
            if width:
                placement = autosym.label_containing(self.ctx, row['load_address'] - self.ctx['frame'])
                self.assertEqual(placement['span'], width)

    def test_partial_overlap_gets_no_width_and_addend_is_unprovable(self):
        # Only a partially overlapping object refuses a label-span width now.
        refused = [n for n, r in self.result['receipts'].items()
                   if r.get('extent', {}).get('width') is None and 'overlaps' in
                   (r.get('extent', {}).get('reason') or '')]
        for name in refused:
            self.assertNotIn('width', self.result['data_symbols'].get(name, {}))
        outside = [r for r in self.result['references'] if r.get('class') == 'addend-outside-grounded-extent']
        self.assertTrue(all(r['status'] == 'not-provable' for r in outside))

    def test_resolver_rejects_width_that_is_not_the_placed_span(self):
        name = next(n for n, r in self.result['data_symbols'].items() if r.get('width'))
        bad = copy.deepcopy(self.data)
        bad['symbols'][name]['width'] = bad['symbols'][name]['width'] + 2
        with Layouts(self.code, bad), self.assertRaises(ValueError):
            data_symbols.resolve_symbols({name}, self.image, self.relocations)
        bad = copy.deepcopy(self.data)
        bad['symbols'][name]['width'] = 64
        bad['symbols'][name]['width_provenance'] = 'reference-label-span: dseg.asm:1-2'
        with Layouts(self.code, bad), self.assertRaises(ValueError):
            data_symbols.resolve_symbols({name}, self.image, self.relocations)

    # ------------------------------ integ25: merged layout and interior policy
    def test_rederived_proposals_match_merged_layout(self):
        code_layout = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        data_layout = read_json(ROOT / 'layout/data-symbols.json')['symbols']
        by_address = {}
        for name, row in data_layout.items():
            if row.get('storage') != 'code_island':
                by_address.setdefault(row['load_address'], {})[name] = row
        self.assertTrue(self.result['code_symbols'] and self.result['data_symbols'])
        for name, row in self.result['code_symbols'].items():
            self.assertEqual(code_layout.get(name), row, name)
        for name, row in self.result['data_symbols'].items():
            # Withholding a reviewed alias may let the derivation choose the
            # compact form at the same address; the merged layout must already
            # name that address, with the same width when one is derived.
            here = by_address.get(row['load_address'], {})
            self.assertTrue(here, name)
            if name in here and row.get('width') and here[name].get('width') is not None:
                self.assertEqual(here[name]['width'], row['width'], name)
        for name, extent in self.result['width_additions'].items():
            if data_layout.get(name, {}).get('width') is not None:
                self.assertEqual({k: data_layout[name].get(k) for k in extent}, extent, name)

    def test_container_width_keeps_interior_field_aliases(self):
        data_layout = read_json(ROOT / 'layout/data-symbols.json')['symbols']
        rows = [(r['load_address'], n, r) for n, r in data_layout.items()
                if r.get('storage') != 'code_island']
        containers = {}
        for base, name, row in rows:
            width = row.get('width')
            if not str(row.get('width_provenance', '')).startswith('reference-label-span'):
                continue
            inside = [n for a, n, r in rows if base < a < base + width]
            if inside:
                containers[name] = inside
        self.assertTrue(containers)
        names = set(containers) | {n for inside in containers.values() for n in inside}
        resolved = data_symbols.resolve_symbols(names, self.image, self.relocations)
        for name, inside in containers.items():
            base, width = data_layout[name]['load_address'], data_layout[name]['width']
            self.assertEqual(max(resolved[name]['allowed_addends']) + 1, width)
            for field in inside:
                # A field alias is wholly inside and keeps its own address.
                end = data_layout[field]['load_address'] + (data_layout[field].get('width') or 1)
                self.assertLessEqual(end, base + width, (name, field))
                self.assertEqual(resolved[field]['load_address'], data_layout[field]['load_address'])

    def test_reviewed_width_refusal_is_not_granted(self):
        data_layout = read_json(ROOT / 'layout/data-symbols.json')['symbols']
        for name in autosym.WIDTH_REFUSED:
            self.assertNotIn(name, self.result['width_additions'])
            self.assertNotIn('width', data_layout.get(name, {}))

    def test_unpatched_resolver_refuses_neutral_reviewed_spelling(self):
        # Ruling: data aliases keep the pinned label spelling; an address-derived
        # neutral spelling of a reviewed form is not admitted by the resolver.
        name, row = next((n, r) for n, r in self.result['data_symbols'].items()
                         if 'reference_label' in r)
        bad = copy.deepcopy(self.data)
        neutral = '_dg_%04x' % (row['load_address'] - self.ctx['frame'])
        bad['symbols'][neutral] = bad['symbols'].pop(name)
        with Layouts(self.code, bad), self.assertRaises(ValueError):
            data_symbols.resolve_symbols({neutral}, self.image, self.relocations)

    # ------------------------------------------ negative: unanchored target
    def test_code_alias_without_anchor_rejected(self):
        name = next(iter(self.result['code_symbols']))
        for mutate in ('drop', 'unverified-caller', 'unrelocated'):
            bad = copy.deepcopy(self.code)
            row = bad['symbols'][name]
            if mutate == 'drop':
                row.pop('anchors', None); row.pop('pointer_anchors', None)
            elif mutate == 'unverified-caller':
                for a in row.get('anchors', []) + row.get('pointer_anchors', []):
                    a['caller_task'] = 'load_fffff'
            else:
                for a in row.get('anchors', []) + row.get('pointer_anchors', []):
                    a['relocation'] = {**a['relocation'], 'load_offset': a['relocation']['load_offset'] + 1}
            with Layouts(bad, self.data), self.assertRaises(ValueError, msg=mutate):
                code_symbols.resolve_code_symbols([name], self.image, self.relocations)

    def test_unverified_or_partial_targets_not_proposed(self):
        partial = {f['start'] for f in self.ctx['inventory']
                   if f.get('status') not in autosym.CODE_TARGET_STATUSES and isinstance(f.get('start'), int)}
        for row in self.result['code_symbols'].values():
            self.assertNotIn(row['mapped_target']['start'], partial)
        for r in self.result['references']:
            if r.get('class') in ('relocation-in-unverified-code', 'target-boundary-or-owner-unverified',
                                  'target-not-an-inventory-entry', 'caller-not-instruction-verified'):
                self.assertEqual(r['status'], 'not-provable')
                self.assertNotIn('alias', r)

    def test_compact_data_alias_without_anchor_rejected(self):
        # Interior bytes of the pinned copyright string (DGROUP 0x10-0x3f): no
        # original DS displacement names them, so a compact alias is refused.
        frame = self.ctx['frame']
        refused = 0
        for offset in (0x21, 0x23, 0x25, 0x27):
            bad = copy.deepcopy(self.data)
            bad['symbols']['_dg_test'] = {'load_address': frame + offset, 'storage': 'initialized'}
            with Layouts(self.code, bad):
                try:
                    data_symbols.resolve_symbols({'_dg_test'}, self.image, self.relocations)
                except ValueError as error:
                    refused += 'lacks an exact unrelocated original instruction' in str(error)
        self.assertEqual(refused, 4)
    # --------------------------------- negative: candidate-driven inference
    def test_derivation_reads_no_candidate_or_recipe(self):
        # Accepted-owner recipes are ownership facts read by the production
        # _complete_target_owner; candidate/scratch/search inputs are never read.
        owned = {str((ROOT / o['recipe']).resolve()).lower() for o in self.ctx['owners'] if o.get('recipe')}
        forbidden = ('\\src\\', '/src/', '\\asm\\', '/asm/', '\\recipes\\', '/recipes/',
                     'build\\workers', 'build/workers', 'build\\search', 'build/search',
                     'build\\acceptance', 'build/acceptance', 'build\\tubench', 'build/tubench')
        root = str(ROOT)
        offending = []
        for path in self.opened:
            rel = path.replace(root, '')
            if 'restunts' in rel.lower() and 'references' in rel.lower():
                continue  # pinned reference listings (identity-checked)
            if str(Path(path).resolve()).lower() in owned:
                continue
            if any(token in rel for token in forbidden) and 'autosym' not in rel:
                offending.append(path)
        self.assertEqual(offending, [])
        self.assertTrue(any(p.endswith('oracle.lock.json') for p in self.opened))

    def test_derive_has_no_candidate_parameter_and_is_deterministic(self):
        import inspect
        self.assertEqual(list(inspect.signature(autosym.derive).parameters), ['ctx', 'spelling'])
        again = autosym.derive(self.ctx)
        self.assertEqual(json.dumps(again['code_symbols'], sort_keys=True),
                         json.dumps(self.result['code_symbols'], sort_keys=True))
        self.assertEqual(json.dumps(again['data_symbols'], sort_keys=True),
                         json.dumps(self.result['data_symbols'], sort_keys=True))

    def test_merge_refuses_canonical_layout(self):
        if (ROOT / '.git').exists():
            with self.assertRaises(ValueError):
                autosym.merge_layouts(ROOT / 'layout', ROOT / 'build' / 'autosym', ROOT / 'layout')


if __name__ == '__main__':
    unittest.main(verbosity=2)
