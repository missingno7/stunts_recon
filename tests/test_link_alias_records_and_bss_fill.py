"""integ42: link-only public alias binding inside accepted OMF records."""
import sys
import types
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from omf import OmfReader  # noqa: E402
from reallink import RawObject, _bind_accepted_code_aliases  # noqa: E402
import bss_link  # noqa: E402


class AcceptedObjectAliasTests(unittest.TestCase):

    def test_alias_extends_existing_pubdef_without_changing_contribution(self):
        target = RawObject('OWNER', 'S027_TEXT', 'CODE', 'word',
                           b'\x00\x00\xc3\x90\x90')
        target.publics = [('_readable_target', 2)]
        target.fixups = [(0, '_external_dep')]
        raw = target.build()
        original = OmfReader().read(raw)
        unit = types.SimpleNamespace(kind='c', accepted=True, id='c_obj_seg027', objs=[{
            'bytes': raw, 'obj': original, 'piece': {'start': 0x10000}}])
        bindings = _bind_accepted_code_aliases(
            [unit], {'_historical_alias': ('code', 0x10002, None, 'code-symbols')})

        self.assertEqual(bindings['_historical_alias']['owner'], 'c_obj_seg027')
        self.assertEqual(bindings['_historical_alias']['offset'], 2)
        linked = OmfReader().read(unit.objs[0]['bytes'])
        self.assertEqual(linked.segment_defs, original.segment_defs)
        self.assertEqual(linked.segment_lengths, original.segment_lengths)
        self.assertEqual(linked.segments, original.segments)
        self.assertEqual(linked.linker_fixups, original.linker_fixups)
        self.assertEqual(linked.externals, original.externals)
        self.assertEqual([(p['name'], p['segment'], p['offset']) for p in linked.publics],
                         [('_readable_target', 'S027_TEXT', 2),
                          ('_historical_alias', 'S027_TEXT', 2)])
        self.assertEqual(unit.objs[0]['link_public_aliases'], ['_historical_alias'])

    def test_non_code_or_non_reviewed_alias_is_untouched(self):
        target = RawObject('OWNER', 'S027_TEXT', 'CODE', 'word', b'\xc3')
        target.publics = [('_readable_target', 0)]
        raw = target.build()
        obj = OmfReader().read(raw)
        unit = types.SimpleNamespace(kind='c', accepted=True, id='c_obj_seg027', objs=[{
            'bytes': raw, 'obj': obj, 'piece': {'start': 0x10000}}])
        self.assertEqual(_bind_accepted_code_aliases(
            [unit], {'_data_name': ('data', 0x10000, None, 'code-symbols'),
                    '_operand_name': ('code', 0x10000, None, 'operand-derived')}), {})
        self.assertEqual(unit.objs[0]['bytes'], raw)


class BssWordFillTests(unittest.TestCase):

    def fixture(self):
        objects = [{'id': 'obj_seg007', 'start': 0, 'end': 10},
                   {'id': 'obj_seg008', 'start': 10, 'end': 20}]
        manifest = {
            'owners': [
                {'id': 'owner007', 'kind': 'MATCHING_C', 'name': 'obj_seg007',
                 'start': 1, 'end': 2},
                {'id': 'owner008', 'kind': 'MATCHING_C', 'name': 'obj_seg008',
                 'start': 11, 'end': 12},
            ],
            'bss_owners': [
                {'id': 'owner007:_BSS', 'kind': 'MATCHING_C_DATA', 'parent': 'owner007',
                 'segment': '_BSS', 'start': 1000, 'end': 1005,
                 'target': {'sha256': 'a', 'size': 5}, 'placement': bss_link.STATIC},
                {'id': 'fill_003ed_003ee', 'kind': 'LINK_FILL',
                 'basis': bss_link.BSS_WORD_FILL, 'start': 1005, 'end': 1006,
                 'object': 'obj_seg007'},
                {'id': 'owner008:_BSS', 'kind': 'MATCHING_C_DATA', 'parent': 'owner008',
                 'segment': '_BSS', 'start': 1006, 'end': 1011,
                 'target': {'sha256': 'b', 'size': 5}, 'placement': bss_link.STATIC},
            ],
        }
        evidence = {'schema': 'bss-raw-placeholders-v1', 'rows': [
            {'id': 'raw_obj7', 'object': 'obj_seg007', 'raw_form': bss_link.OBJECT_BSS,
             'start': 1000, 'end': 1005},
            {'id': 'raw_fill', 'object': None, 'raw_form': bss_link.WORD_FILL,
             'start': 1005, 'end': 1006},
            {'id': 'raw_obj8', 'object': 'obj_seg008', 'raw_form': bss_link.OBJECT_BSS,
             'start': 1006, 'end': 1011},
        ]}
        return objects, manifest, evidence

    def test_word_fill_replaces_placeholder_only_between_accepted_bss(self):
        objects, manifest, evidence = self.fixture()
        rows = bss_link.check_partition(manifest, objects, evidence, 1000, 1011)
        self.assertEqual(rows[1], {'id': 'fill_003ed_003ee', 'kind': 'LINK_FILL',
                                   'form': bss_link.BSS_WORD_FILL, 'object': 'obj_seg007',
                                   'start': 1005, 'end': 1006})
        report = bss_link.ownership_report(rows)
        self.assertEqual(report['raw_fill'], 0)
        self.assertEqual(report['link_fill'], 1)

    def test_word_fill_refuses_unaligned_gap(self):
        objects, manifest, evidence = self.fixture()
        manifest['bss_owners'][0]['end'] = 1004
        manifest['bss_owners'][1]['start'] = 1004
        manifest['bss_owners'][1]['end'] = 1005
        manifest['bss_owners'][1]['id'] = 'fill_003ec_003ed'
        manifest['bss_owners'][2]['start'] = 1005
        evidence['rows'][0]['end'] = 1004
        evidence['rows'][1]['start'] = 1004
        evidence['rows'][1]['end'] = 1005
        evidence['rows'][2]['start'] = 1005
        with self.assertRaisesRegex(ValueError, 'one-byte word gap'):
            bss_link.check_partition(manifest, objects, evidence, 1000, 1011)


if __name__ == '__main__':
    unittest.main()
