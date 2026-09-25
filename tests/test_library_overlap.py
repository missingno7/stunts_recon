import copy
import struct
import sys
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from object_probe import read_object
from runtime_link_probe import selected_rows, original_module, reviewed_policy, experiment
from test_pipeline import object_fixture, record


class LibraryOverlapTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rows = selected_rows()
        cls.objects = [original_module(row) for row in cls.rows]

    def test_default_still_rejects_each_archive_overlap(self):
        for data in self.objects:
            with self.assertRaisesRegex(ValueError, 'Overlapping LEDATA'): read_object(data)

    def test_explicit_trace_full_extent_and_return_replacement(self):
        for row, data in zip(self.rows, self.objects):
            policy = reviewed_policy(data); obj = read_object(data, ledata_policy=policy)
            self.assertEqual(len(obj.segment_bytes('_TEXT')), row['size'])
            self.assertEqual(obj.segment_bytes('_TEXT')[-3:], b'\xca\x08\x00')
            self.assertFalse(obj.linker_fixups)
            self.assertGreater(sum(len(r['overlap_offsets']) for r in policy['records']), 0)

    def test_policy_hash_and_order_are_mandatory(self):
        data = self.objects[0]; policy = reviewed_policy(data)
        bad = copy.deepcopy(policy); bad['module_sha256'] = '0'*64
        with self.assertRaises(ValueError): read_object(data, ledata_policy=bad)
        bad = copy.deepcopy(policy); bad['records'].reverse()
        with self.assertRaises(ValueError): read_object(data, ledata_policy=bad)
        bad = copy.deepcopy(policy); bad['records'][-1]['overlap_offsets'] = []
        with self.assertRaises(ValueError): read_object(data, ledata_policy=bad)

    def test_policy_cannot_disable_checksum_validation(self):
        data = bytearray(self.objects[0]); data[4] ^= 1
        policy = reviewed_policy(data)
        with self.assertRaisesRegex(ValueError, 'checksum'): read_object(data, ledata_policy=policy)

    def test_policy_cannot_enable_unknown_records(self):
        data = object_fixture(record(0xb2, b'\x01\x01\0\0\x01\0'))
        with self.assertRaises(ValueError): read_object(data, ledata_policy=reviewed_policy(data))

    def test_policy_requires_an_actual_overlap(self):
        data = object_fixture()
        with self.assertRaises(ValueError): read_object(data, ledata_policy=reviewed_policy(data))

    def test_overlap_with_fixups_is_rejected(self):
        data = self.objects[0]; at = 0
        while data[at] != 0x8a: at += 3 + struct.unpack_from('<H', data, at+1)[0]
        data = data[:at] + record(0x9c, b'\xc4\x00\x54\x01') + data[at:]
        with self.assertRaisesRegex(ValueError, 'with fixups'): read_object(data, ledata_policy=reviewed_policy(data))

    def test_historical_link_complete_combined_members_and_aliases(self):
        for profile in ['msc510-medium', 'msc500-medium']:
            row = experiment(self.rows, profile)
            self.assertEqual(row['status'], 'HISTORICAL_LINK_MATCH')
            self.assertEqual(row['text']['size'], 305)
            self.assertEqual(len(row['publics']), 4)
            self.assertFalse(row['relocations'])


if __name__ == '__main__': unittest.main()
