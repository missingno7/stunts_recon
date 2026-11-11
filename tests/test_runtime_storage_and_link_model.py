"""integ36: the pinned runtime startup group, placed by the real link.

Runtime DGROUP model (reallink links pinned member OBJs and carves their DGROUP
segments out of raw debt), `linked` runtime storage owned as pinned runtime
data, zero-length BSS-class sections anchored only by the real link, the
runtime-owned `_BSS` row of output.c, reviewed in-member code tables, the
bounded `__cfltcvt_tab` and accepted zero-fixup group providers."""
import copy
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
from oracle import verify  # noqa: E402
from mz import MZ  # noqa: E402
from library import bind_library  # noqa: E402
from build_exact import validate_layout  # noqa: E402
import bss_link  # noqa: E402
import link_fill  # noqa: E402
import reallink  # noqa: E402
import runtime_binding  # noqa: E402


class RuntimeGroupTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        exe = verify(write=False)[1]
        mz = MZ.parse(exe)
        cls.image, cls.relocs = mz.load_image(exe), mz.relocations
        cls.manifest = read_json(ROOT / 'layout/manifest.json')

    def owner(self, owner_id, manifest=None):
        return copy.deepcopy(next(o for o in (manifest or self.manifest)['owners'] if o['id'] == owner_id))

    def bind(self, owner, manifest=None):
        return bind_library(owner, self.image, self.relocs, manifest=manifest or self.manifest)

    def refused(self, owner, pattern, manifest=None):
        with self.assertRaisesRegex(ValueError, pattern):
            self.bind(owner, manifest)


    def test_zero_length_sections_need_the_real_link_anchor(self):
        base = self.owner('library_dos_crt0dat_118056')
        bad = copy.deepcopy(base)
        bad['binding']['storage']['XOB']['anchor'] = {'kind': 'data-alias', 'symbol': '_unk_42A24', 'offset': 0}
        self.refused(bad, 'needs the real-link anchor')
        bad = copy.deepcopy(base)
        bad['binding']['storage'].pop('XOB')
        bad['binding']['zero_section_aliases'] = {'XOB': {'kind': 'data-alias', 'symbol': '_unk_42A24', 'offset': 0}}
        self.refused(bad, 'lacks independently grounded placement: XOB')
        bad = copy.deepcopy(base)
        bad['binding']['storage']['XOB'].update(start=207398, end=207398)
        self.refused(bad, 'does not follow the complete _BSS class')
        bad = copy.deepcopy(base)
        bad['binding']['storage']['XIFB'] = {'start': 199722, 'end': 199722, 'ownership': 'linked',
                                             'anchor': {'kind': runtime_binding.REAL_LINK_ANCHOR}}
        self.refused(bad, 'only for a zero-length DGROUP BSS-class section')
        bad = copy.deepcopy(base)
        bad['binding']['storage']['XO']['ownership'] = 'proven-raw'
        self.refused(bad, 'needs the real-link anchor')

    def test_linked_storage_needs_exactly_its_owned_rows(self):
        owner = self.owner('library_chkstk_118452')
        manifest = copy.deepcopy(self.manifest)
        row = next(o for o in manifest['owners'] if o['id'] == 'library_chkstk_118452:_DATA')
        manifest['owners'][manifest['owners'].index(row)] = {'id': 'raw_x', 'kind': 'UNRESOLVED_RAW',
                                                            'start': row['start'], 'end': row['end']}
        self.refused(owner, 'lacks exactly one owned data row', manifest)
        with self.assertRaisesRegex(ValueError, 'lacks exactly one owned row'):
            validate_layout(manifest, len(self.image))
        proven = copy.deepcopy(owner)
        proven['binding']['storage']['_DATA']['ownership'] = 'proven-raw'
        self.refused(proven, 'no longer wholly raw-owned')
        manifest = copy.deepcopy(self.manifest)
        next(o for o in manifest['owners'] if o['id'] == 'library_chkstk_118452:_DATA')['segment'] = 'CONST'
        with self.assertRaisesRegex(ValueError, 'Orphaned or unlisted pinned runtime data owner'):
            validate_layout(manifest, len(self.image))
        # integ37: abort.asm's PAD is the owned final MSG COMMON overlay; an
        # earlier, overwritten 2-byte contribution can never be owned.
        common = self.owner('library_dos_crt0msg_118410')
        common['binding']['storage']['PAD']['ownership'] = 'linked'
        self.refused(common, 'public or sole COMMON')

    def test_runtime_bss_row_is_grounded_by_its_own_operands(self):
        owner = self.owner('library_output_120254')
        bad = copy.deepcopy(owner)
        bad['binding']['storage']['_BSS']['anchor'] = {'kind': 'data-alias', 'symbol': '_off_4289A', 'offset': 2}
        self.refused(bad, 'grounded by its own code operands')
        moved = copy.deepcopy(owner)
        moved['binding']['storage']['_BSS'].update(start=207002, end=207040)
        self.refused(moved, 'not grounded by its own code operands')
        manifest = copy.deepcopy(self.manifest)
        next(o for o in manifest['bss_owners'] if o['id'] == 'library_output_120254:_BSS').pop('placement')
        with self.assertRaisesRegex(ValueError, 'real-link placement kind'):
            validate_layout(manifest, len(self.image))

    def test_reviewed_code_tables(self):
        output = self.owner('library_output_120254')
        self.assertEqual(output['binding']['code_tables'],
                         [{'dispatch': 382, 'table': 728, 'entries': 22,
                           'bound': {'kind': 'unsigned-guard-v1', 'site': 368}}])
        for mutate, pattern in [
                (lambda t: t.update(entries=21), 'guard differs'),
                (lambda t: t.update(entries=23), 'not an own code offset'),
                (lambda t: t['bound'].update(site=366), 'guard differs'),
                (lambda t: t.update(table=730), 'dispatch differs')]:
            bad = copy.deepcopy(output)
            mutate(bad['binding']['code_tables'][0])
            self.refused(bad, pattern)
        bad = copy.deepcopy(output)
        bad['binding'].pop('code_tables')
        self.refused(bad, 'needs explicit table proof')
        bad = copy.deepcopy(output)
        bad['binding']['code_table_fixup_offsets'] = [382] + list(range(728, 772, 2))
        bad['binding'].pop('code_tables')
        self.refused(bad, 'needs explicit table proof')
        raise_ = self.owner('library_dos_raise_124318')
        self.assertEqual(raise_['binding']['code_tables'][0]['bound'], {'kind': 'member-prefix-v1'})
        bad = copy.deepcopy(raise_)
        bad['binding']['code_tables'][0]['entries'] = 5
        self.refused(bad, 'member prefix')

    def test_cfltcvt_table_is_bounded(self):
        output = self.owner('library_output_120254')
        self.assertEqual(output['binding']['data_object_proofs']['__cfltcvt_tab'], runtime_binding.FPTRAP_TABLE)
        bad = copy.deepcopy(output)
        bad['binding']['data_object_proofs']['__cfltcvt_tab']['allowed_addends'].append(20)
        self.refused(bad, 'Unreviewed runtime data-object proof')

    def test_accepted_zero_fixup_provider(self):
        self.assertEqual(runtime_binding.group_public('_strlen', self.manifest)['address'], 123780)
        forged = copy.deepcopy(self.manifest)
        next(o for o in forged['owners'] if o['id'] == 'library_strlen')['name'] = 'forged'
        with self.assertRaisesRegex(ValueError, 'zero-fixup group provider'):
            runtime_binding.group_public('_strlen', forged)

    def test_fills_after_runtime_neighbours(self):
        rows = {o['start']: o for o in self.manifest['owners'] if o['kind'] == 'LINK_FILL'}
        for start in (118055, 124475, 191859):
            receipt = link_fill.checked_fill(rows[start], self.manifest, self.image)
            self.assertEqual(receipt['fill'], [start, start + 1])
        self.assertEqual(rows[191859]['basis'], link_fill.WORD_BASIS)
        manifest = copy.deepcopy(self.manifest)
        row = next(o for o in manifest['owners'] if o['id'] == 'library_dos_crt0_117858:_DATA')
        row['kind'] = 'UNRESOLVED_RAW'
        with self.assertRaisesRegex(ValueError, 'not followed by an accepted DGROUP contribution'):
            link_fill.checked_fill(next(o for o in manifest['owners'] if o['start'] == 191859), manifest, self.image)


class _Obj:
    def __init__(self, segs, publics=(), fixups=(), data=None):
        self.segment_defs = [{'name': n, 'class': k, 'combine': c, 'length': ln} for n, k, c, ln in segs]
        self.publics = [{'name': n, 'segment': s, 'offset': o} for n, s, o in publics]
        self.local_publics = []
        self.linker_fixups = list(fixups)
        self.segments = dict(data or {})

    def segment_bytes(self, name):
        return self.segments[name]


class RealLinkModelTests(unittest.TestCase):

    def test_carve(self):
        self.assertEqual(reallink.carve([(0, 10), (20, 30)], [(2, 4), (8, 22), (29, 40)]),
                         [(0, 2), (4, 8), (22, 29)])

    def plan(self, owners):
        return reallink.runtime_link_plan(SimpleNamespace(manifest={'owners': owners}))

    @staticmethod
    def member(owner_id, segments, storage):
        return {'id': owner_id, 'kind': 'KNOWN_TOOLCHAIN_LIBRARY',
                'binding': {'declarations': {'segments': [{'name': n, 'class': k, 'combine': c}
                                                          for n, k, c in segments]},
                            'storage': {n: {'start': a, 'end': b} for n, (a, b) in storage.items()}}}

    def test_runtime_link_plan(self):
        msg = [('MSG', 'MSG', 'public'), ('PAD', 'MSG', 'common')]
        short = self.member('a', msg, {'MSG': (199734, 199864), 'PAD': (199973, 199975)})
        full = self.member('b', msg, {'MSG': (199940, 199973), 'PAD': (199973, 199992)})
        plan = self.plan([short, full])
        self.assertTrue(plan['a']['linked'] and plan['b']['linked'])
        self.assertEqual(plan['b']['data'], [('MSG', 199940, 199973), ('PAD', 199973, 199992)])
        plan = self.plan([short])
        self.assertFalse(plan['a']['linked'])
        self.assertIn('not wholly supplied', plan['a']['reason'])
        # integ37: BEGDATA is linked (the reconstruction DSEG is linked as BEGDATA too)
        null = self.member('c', [('NULL', 'BEGDATA', 'public')], {'NULL': (178032, 178098)})
        self.assertTrue(self.plan([null])['c']['linked'])

    def test_runtime_placement_readings(self):
        dgroup = 178032
        image = bytearray(200000)
        fix = {'segment': '_TEXT', 'target_kind': 'segment', 'target': '_DATA', 'loc': 'offset16',
               'self_relative': False, 'frame_method': 1, 'offset': 1, 'encoded_addend': '0200', 'displacement': 0}
        image[100 + 1:100 + 3] = (191000 + 2 - dgroup).to_bytes(2, 'little')
        image[191000:191004] = b'WXYZ'
        obj = _Obj([('_TEXT', 'CODE', 'public', 4), ('_DATA', 'DATA', 'public', 4)],
                   publics=[('_f', '_TEXT', 0), ('_d', '_DATA', 0)], fixups=[fix], data={'_DATA': b'WXYZ'})
        owner = {'id': 'm', 'binding': {'storage': {'_DATA': {'start': 191000, 'end': 191004, 'ownership': 'linked'}}}}
        unit = reallink.Unit('rt_m', 'rt', '_TEXT', 100, 104)
        unit.objs = [{'obj': obj, 'piece': owner}]
        mapping = ' Origin   Group\n 2B77:0   DGROUP\n'
        segs = []
        ctx = SimpleNamespace()
        result = reallink.runtime_placement(ctx, [unit], bytes(image), segs, {'_f': 100, '_d': 191000}, mapping)
        self.assertTrue(result['placed'], result)
        self.assertEqual(set(result['rows'][0]['readings']), {'map-public', 'linked-fixups', 'linked-unique-bytes'})
        result = reallink.runtime_placement(ctx, [unit], bytes(image), segs, {'_f': 100, '_d': 191002}, mapping)
        self.assertFalse(result['placed'])
        owner['binding']['storage']['_DATA']['start'] = 191002
        result = reallink.runtime_placement(ctx, [unit], bytes(image), segs, {}, mapping)
        self.assertFalse(result['placed'])

    def test_linked_storage_of_an_unlinked_member_is_unproven(self):
        owner = {'id': 'm', 'kind': 'KNOWN_TOOLCHAIN_LIBRARY',
                 'binding': {'storage': {'MSG': {'start': 1, 'end': 2, 'ownership': 'linked'}}}}
        ctx = SimpleNamespace(manifest={'owners': [owner]})
        result = reallink.runtime_placement(ctx, [], b'', [], {}, '')
        self.assertFalse(result['placed'])
        self.assertIn('not linked from its pinned OBJ', result['problems'][0])


    def test_gate_refuses_unplaced_runtime_storage(self):
        summary = {'image_equal': True, 'relocation_set_equal': True, 'bank_order_equal': True,
                   'packed_equal': True, 'header': {'linked': {'cs': 1}, 'oracle': {'cs': 1}},
                   'bss': {'owners': [], 'problems': []},
                   'runtime': {'rows': [], 'problems': ['m _DATA: real link placed it at [1], row 2']}}
        decision = bss_link.gate(summary)
        self.assertEqual(decision['status'], 'REFUSED')
        self.assertTrue(any('real link placed it' in p for p in decision['problems']))


if __name__ == '__main__':
    unittest.main()
