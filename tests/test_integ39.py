"""integ39: communal acceptance infrastructure.

* the c_common paragraph fill [207396,207408) owned as LINK_FILL and the
  accepted communal unit [207408,222352);
* the MASM 5.10 `COMM NEAR` / MSC 5.10 tentative-definition COMDEF fixture
  (pinned tools) and the strict whole-unit gate (communal_unit) on it;
* recipe COMDEF declarations through the strict probe, the atomic communal
  batch transaction, placeholder/registry names and size bases."""
import copy
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
import bss_link  # noqa: E402
import communal_unit as cu  # noqa: E402

FIXTURE = ROOT / 'tests/fixtures/masm510_comm_near_fixture.json'


def _fixture_items(fx):
    """The fixture's c_common as a unit row (declarers and sizes from its COMDEFs)."""
    rows = []
    for name, decl in fx['declared'].items():
        size = max(s for _, s, _ in decl)
        rows.append({'name': name, 'address': fx['publics'][name], 'size': size,
                     'declarers': sorted({u for u, _, _ in decl}),
                     'size_basis': {'kind': 'declared-type', 'declaration': name}})
    return sorted(rows, key=lambda r: r['address'])


def _declared(fx):
    return {n: [tuple(x) for x in rows] for n, rows in fx['declared'].items()}


class FillTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')

    def test_canonical_fill_and_accepted_unit(self):
        rows = bss_link.load_partition(self.manifest)
        self.assertEqual([(r['kind'], r['form'], r['start'], r['end']) for r in rows[-2:]],
                         [('LINK_FILL', cu.FILL_BASIS, 207396, 207408),
                          (cu.KIND, bss_link.COMMUNAL_UNIT, 207408, 222352)])
        fill = self.manifest['bss_owners'][-2]
        self.assertEqual(cu.checked_communal_fill(fill, self.manifest)['fill'], [207396, 207408])
        import runtime_binding
        self.assertEqual(runtime_binding.bss_class_end(self.manifest), 207396)

    def test_fill_refusals(self):
        def refused(mutate, pattern):
            m = copy.deepcopy(self.manifest)
            mutate(m)
            with self.assertRaisesRegex(ValueError, pattern):
                bss_link.load_partition(m)
                cu.checked_communal_fill(m['bss_owners'][-2], m)

        def short(m):
            m['bss_owners'][-2]['end'] -= 2
            m['bss_owners'][-1]['start'] -= 2
        refused(short, 'paragraph fill|reviewed placeholder')

        def moved_xoe(m):
            crt = next(o for o in m['owners'] if o['id'] == cu.XOE_ANCHOR[0])
            crt['binding']['storage']['XOE'].update(start=207394, end=207394)
        refused(moved_xoe, 'XOE')

        def wrong_basis(m):
            m['bss_owners'][-2]['basis'] = 'link-paragraph-alignment-v1'
        refused(wrong_basis, 'paragraph fill before c_common')


class CommNearFixtureTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.fx = read_json(FIXTURE)

    def test_masm_comm_near_emits_the_msc_comdef_form(self):
        c = self.fx['communals']
        # the same near communal (type index 0, data type 62h, byte length) from both tools
        self.assertIn(['_shared_tab', 'near', 8], c['A'])
        self.assertIn(['_shared_tab', 'near', 8], c['C1'])
        asm = bytes.fromhex(self.fx['comdef_records']['A'][0])
        msc = bytes.fromhex(self.fx['comdef_records']['C1'][0])
        self.assertIn(b'\x0b_shared_tab\x00\x62\x81\x08\x00', asm)    # MASM: always the 81h word leaf
        self.assertIn(b'\x0b_shared_tab\x00\x62\x08', msc)            # MSC: one byte below 80h
        # MASM writes its COMDEF records in symbol-table order, not declaration order
        self.assertEqual([n for n, _, _ in c['A']], ['_asm_comm', '_zeta_buf', '_mid_word', '_one_byte', '_shared_tab'])
        self.assertEqual([n for n, _, _ in c['A'] if n == '_zeta_buf'], ['_zeta_buf'])
        self.assertIn(['_zeta_buf', 'near', 300], c['A'])

    def test_link_places_c_common_on_a_paragraph_and_merges_declarations(self):
        segs = {s['name']: s for s in self.fx['segments']}
        self.assertEqual((segs['_BSS']['start'], segs['_BSS']['stop']), (52, 57))
        self.assertEqual(segs['c_common']['start'], 64)                   # after an unaligned _BSS end
        self.assertEqual(self.fx['publics']['_shared_tab'], 64)          # one allocation for MASM + MSC
        for ref in self.fx['references']:
            self.assertEqual(ref['linked'], self.fx['publics'][ref['name']], ref)

    def test_gate_accepts_the_fixture_unit(self):
        items = _fixture_items(self.fx)
        row = {'id': 'c_common', 'kind': cu.KIND, 'classification': cu.KIND, 'placement': cu.PLACEMENT,
               'segment': 'c_common', 'schema': cu.SCHEMA, 'start': 64, 'end': 394, 'communals': items}
        self.assertEqual(len(cu.check_row_form(row, {'bss_end': 394})), 8)
        problems = cu.check_link(items, _declared(self.fx), self.fx['references'], self.fx['publics'],
                                 self.fx['segments'], b'', self.fx['dgroup_frame'])
        self.assertEqual(problems, [])

    def test_gate_refusals(self):
        fx = self.fx

        def problems(items=None, declared=None, refs=None, pubs=None):
            return cu.check_link(items or _fixture_items(fx), declared or _declared(fx), refs or fx['references'],
                                 pubs or fx['publics'], fx['segments'], b'', fx['dgroup_frame'])
        pubs = dict(fx['publics'], _c_int=378)
        self.assertTrue(any(p.startswith('(b) _c_int linked at 378') for p in problems(pubs=pubs)))
        refs = copy.deepcopy(fx['references'])
        refs[0]['linked'] += 2
        self.assertTrue(any('operand' in p for p in problems(refs=refs)))
        declared = _declared(fx)
        declared['_c_long'] = [('C2', 4, False)]
        self.assertTrue(any('non-accepted' in p for p in problems(declared=declared)))
        declared = _declared(fx)
        declared['_c_arr'] = [('C1', 6, True)]
        self.assertTrue(any('declared size' in p for p in problems(declared=declared)))
        declared = _declared(fx)
        del declared['_c_long']
        self.assertTrue(any('no accepted declaring unit' in p for p in problems(declared=declared)))
        declared = _declared(fx)
        declared['_stray'] = [('C2', 2, True)]
        self.assertTrue(any('outside the communal unit' in p for p in problems(declared=declared)))
        items = _fixture_items(fx)
        next(i for i in items if i['name'] == '_c_int')['size_basis'] = {
            'kind': 'neutral-unreferenced', 'declaration': 'x', 'review_note': 'no reference anywhere (test)'}
        self.assertTrue(any('neutral communal _c_int' in p for p in problems(items=items)))
        # (b) of the tiling: a moved or resized communal breaks the unit form
        row = {'id': 'c_common', 'kind': cu.KIND, 'classification': cu.KIND, 'placement': cu.PLACEMENT,
               'segment': 'c_common', 'schema': cu.SCHEMA, 'start': 64, 'end': 394,
               'communals': _fixture_items(fx)}
        row['communals'][4]['address'] += 1
        with self.assertRaisesRegex(ValueError, 'allocation position'):
            cu.check_row_form(row, {'bss_end': 394})

    def test_pinned_tools_reproduce_the_fixture(self):
        live = cu.comm_near_fixture()
        for key in ('comdef_records', 'segments', 'publics'):
            self.assertEqual(json.loads(json.dumps(live[key])), self.fx[key], key)
        self.assertEqual(json.loads(json.dumps(live['references'])), self.fx['references'])
        self.assertEqual(live['dgroup'], self.fx['dgroup_frame'])


class NamesAndBasisTests(unittest.TestCase):

    def test_placeholders(self):
        for name in ('_slot207412_a', '_word_3F874', '_unk_44D3C', '_buf_4AB12', '__slot3'):
            self.assertTrue(cu.is_placeholder(name), name)
        for name in ('_td10checkrel', '_curtshape', '_trackdata18', '__bufin', '_mat_rot_1', '_video_flag4_is1'):
            self.assertFalse(cu.is_placeholder(name), name)

    def test_registry_and_pinned_names(self):
        manifest = read_json(ROOT / 'layout/manifest.json')

        def item(name, address, kind='declared-type'):
            return {'name': name, 'address': address, 'size': 2, 'declarers': ['x'], 'size_basis': {'kind': kind}}
        self.assertEqual(cu.check_names([item('_td10checkptr', 207408)], manifest), [])
        self.assertTrue(cu.check_names([item('_td10checkrel', 207408)], manifest))
        self.assertTrue(cu.check_names([item('_slot207408_a', 207408)], manifest))
        self.assertEqual(cu.check_names([item('__bufin', 215782, 'pinned-member')], manifest), [])
        self.assertTrue(cu.check_names([item('__bufxx', 215782, 'pinned-member')], manifest))

    def test_size_basis_forms(self):
        src = 'src/seg033_mcgawnd.c'
        declared = {'_a': [('o', 40, src)], '_b': [('o', 6, src)], '_c': [('o', 2, src)]}
        text = 'struct SPRITE far *mcgawnd_window_sprite;'
        items = [
            {'name': '_a', 'address': 1000, 'size': 40, 'declarers': ['o'],
             'size_basis': {'kind': 'absorbed-gap', 'declaration': text, 'element_size': 3,
                            'gap': [1030, 1040], 'indexed_sites': [1]}},
            {'name': '_b', 'address': 1040, 'size': 6, 'declarers': ['o'],
             'size_basis': {'kind': 'neutral-unreferenced', 'declaration': text, 'review_note': 'short'}},
            {'name': '_c', 'address': 1046, 'size': 2, 'declarers': ['o'],
             'size_basis': {'kind': 'declared-type', 'declaration': 'int not_in_source;'}}]
        problems = cu._check_size_basis_form(items, declared)
        self.assertTrue(any('absorbed gap' in p for p in problems))
        self.assertTrue(any('review note' in p for p in problems))
        self.assertTrue(any('not in a declaring source' in p for p in problems))
        items[0]['size_basis']['element_size'] = 4
        items[1]['size_basis']['review_note'] = 'unreferenced storage; element type not recoverable'
        items[2]['size_basis']['declaration'] = text
        self.assertEqual(cu._check_size_basis_form(items, declared), [])
        # during a promotion the declaration is read from the staged candidate bytes
        staged = {src: b'int not_in_source;'}
        self.assertTrue(cu._check_size_basis_form(items, declared, lambda path: staged[path]))
        items[2]['size_basis']['declaration'] = 'int not_in_source;'
        self.assertEqual(sum('not in a declaring source' in p for p in
                             cu._check_size_basis_form(items, declared, lambda path: staged[path])), 2)


class TransactionTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')

    def _row(self):
        return {'id': 'c_common', 'kind': cu.KIND, 'classification': cu.KIND, 'placement': cu.PLACEMENT,
                'segment': 'c_common', 'schema': cu.SCHEMA, 'start': 207408, 'end': 222352,
                'communals': [{'name': '_whole', 'address': 207408, 'size': 222352 - 207408,
                               'declarers': ['x'], 'size_basis': {'kind': 'declared-type', 'declaration': 'x'}}]}

    def _raw_manifest(self):
        manifest = copy.deepcopy(self.manifest)
        evidence_row = next(row for row in read_json(ROOT / bss_link.PARTITION_EVIDENCE)['rows']
                            if row.get('raw_form') == 'communal-unit')
        raw = {key: evidence_row[key] for key in ('id', 'object', 'start', 'end', 'raw_form')}
        raw.update(kind='UNRESOLVED_RAW', classification='UNRESOLVED_MIXED')
        manifest['bss_owners'][-1] = raw
        return manifest

    def test_attach_replaces_the_raw_unit_exactly(self):
        base = self._raw_manifest()
        staged = cu.attach(base, self._row())
        self.assertEqual(staged['bss_owners'][-1]['kind'], cu.KIND)
        rows = bss_link.load_partition(staged)
        self.assertEqual((rows[-1]['kind'], rows[-1]['start']), (cu.KIND, 207408))
        refreshed = cu.attach(staged, self._row())
        self.assertEqual(refreshed['bss_owners'][-1]['id'], 'c_common')
        self.assertEqual(refreshed['bss_owners'][-1]['communals'], self._row()['communals'])
        wrong_owner = self._row()
        wrong_owner['id'] = 'other_communal_unit'
        with self.assertRaisesRegex(ValueError, 'accepted unit owner and extent'):
            cu.attach(staged, wrong_owner)
        bad = self._row()
        bad['start'] = 207396
        with self.assertRaises(ValueError):
            cu.attach(base, bad)

    def test_declarations_need_the_whole_unit(self):
        recipe_of = lambda o: read_json(ROOT / o['recipe'])
        self.assertEqual(cu.check_manifest(self.manifest, recipe_of),
                         {'communal_unit': 'c_common', 'communals': 303, 'bytes': 14944})
        without_unit = copy.deepcopy(self.manifest)
        without_unit['bss_owners'].pop()
        with self.assertRaisesRegex(ValueError, 'only with the whole communal unit'):
            cu.check_manifest(without_unit, recipe_of)

        # Omitting even one declaration from an accepted declaring object leaves a communal
        # without its accepted declarer.
        target = next(o for o in self.manifest['owners']
                      if len(recipe_of(o).get('communal_declarations', [])) > 1)

        def omit_one(owner):
            recipe = recipe_of(owner)
            if owner['id'] == target['id']:
                recipe = dict(recipe, communal_declarations=recipe['communal_declarations'][:-1])
            return recipe
        with self.assertRaisesRegex(ValueError, r'\(a\)'):
            cu.check_manifest(self.manifest, omit_one)

        staged = cu.attach(self._raw_manifest(), self._row())
        with self.assertRaisesRegex(ValueError, r'\(a\)'):
            cu.check_manifest(staged, recipe_of)

    def test_probe_binds_a_tentative_definition_exactly(self):
        from probe_module import probe
        from compiler import compile_source
        recipe = read_json(ROOT / 'recipes/seg033_mcgawnd.json')
        source = (ROOT / recipe['source']).read_bytes()
        names = [row['name'] for row in recipe['communal_declarations']]
        obj, _ = compile_source(source, recipe['profile'], communals=names)
        self.assertEqual(obj.communals[0]['length'], 4)
        self.assertEqual(obj.unreferenced_communals, [])
        payload, receipt = probe(recipe, source_override=source)
        self.assertEqual(len(payload), recipe['end'] - recipe['start'])
        self.assertEqual(receipt['communals'], [{'name': '_mcgawnd_window_sprite', 'size': 4}])
        without_declaration = dict(recipe)
        without_declaration.pop('communal_declarations')
        with self.assertRaisesRegex(Exception, 'COMDEF'):
            probe(without_declaration, source_override=source)
        wrong = dict(recipe, communal_declarations=[{'name': '_mcgawnd_window_sprite', 'size': 2}])
        with self.assertRaisesRegex(Exception, 'communal_declarations'):
            probe(wrong, source_override=source)

    def test_batch_line_and_atomic_refusal(self):
        import tempfile
        import batch_publish
        with tempfile.TemporaryDirectory() as tmp:
            batch = Path(tmp) / 'b.txt'
            batch.write_text('f a.c r.json\nCOMMUNAL unit.json\n')
            self.assertEqual(batch_publish.read_batch(batch)[-1], {'communal': 'unit.json'})
            batch.write_text('COMMUNAL a.json\nCOMMUNAL b.json\n')
            with self.assertRaisesRegex(ValueError, 'one COMMUNAL'):
                batch_publish.read_batch(batch)
        logs = []
        entries = [{'name': 'x'}]
        result = batch_publish._stage_communal(entries, [], {'x': {'stage': 'individual', 'error': 'e'}},
                                               self.manifest, None, None, None, None, logs.append, self._row())
        self.assertEqual(result[:3], ([], None, None))
        self.assertIn('@communal', result[3])

    def test_unreferenced_communal_is_not_a_binding(self):
        from object_probe import declared_externals

        class O:
            externals = ['__acrtused', '_used', '_unused']
            external_scopes = ['external', 'communal', 'communal']
            communals = [{'name': '_used'}, {'name': '_unused'}]
            linker_fixups = [{'target_kind': 'external', 'target': '_used', 'frame_kind': 'target', 'frame': '_used'}]
        self.assertEqual(declared_externals(O()), {'__acrtused', '_used'})


if __name__ == '__main__':
    unittest.main()
