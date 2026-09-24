"""Shared-base constraints remain diagnostics distinct from native acceptance."""
import hashlib
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
from common import ROOT, read_json
from shared_base_probe import solve_shared_base


class SharedBaseProbeTests(unittest.TestCase):
    def test_pair_witness_and_insufficient_frame_refuse(self):
        first = {'symbol': 'first', 'frame_paragraph': 0x2a9d,
                 'inferred_base': 174588}
        second = {'symbol': 'second', 'frame_paragraph': 0x2a9d,
                  'inferred_base': 174564}
        self.assertEqual(solve_shared_base([first])['state'],
                         'INSUFFICIENT_INDEPENDENT_ANCHORS')
        conflict = solve_shared_base([first, second])
        self.assertEqual(conflict['state'], 'INCONSISTENT_BASE')
        self.assertEqual(conflict['witness'], ['first', 'second'])
        self.assertEqual(abs(conflict['base_difference']), 24)
        self.assertEqual(solve_shared_base([first, {**second, 'inferred_base': 174588}])['base'],
                         174588)
        self.assertEqual(solve_shared_base([first, {**second, 'frame_paragraph': None}])['state'],
                         'UNKNOWN_FRAME')
        self.assertEqual(solve_shared_base([first, {**second, 'frame_paragraph': 0x2a9e}])['state'],
                         'INCONSISTENT_FRAME')

    def test_current_multi_public_and_private_data_results_remain_research_only(self):
        report = read_json(ROOT/'recovery/shared-base-census.json')
        self.assertEqual(report['authority'], 'RESEARCH_ONLY_PLACEMENT_DIAGNOSTIC')
        for name, digest in report['input_sha256'].items():
            self.assertEqual(hashlib.sha256((ROOT/name).read_bytes()).hexdigest(), digest)
        shape = [row for row in report['public_placements'] if row['task_id'] == 'load_2a9ea']
        self.assertEqual({row['placement']['state'] for row in shape},
                         {'CONSISTENT_BASE', 'INCONSISTENT_BASE'})
        consistent = next(row for row in shape if row['placement']['state'] == 'CONSISTENT_BASE')
        self.assertEqual(consistent['candidate_span_if_placed'], [174570, 174594])
        self.assertEqual(consistent['oracle_anchored_function_span'], [174570, 175280])
        self.assertFalse(consistent['extent_equal_to_anchored_span'])
        self.assertTrue(all(row['acceptance'] == 'NOT_EVALUATED' for row in shape))
        private = [row for row in report['private_data_placements'] if row['task_id'] == 'load_2167c']
        self.assertTrue(private)
        self.assertTrue(all(row['placement']['state'] == 'INSUFFICIENT_INDEPENDENT_ANCHORS'
                            and row['content_state'] == 'DIFFERS'
                            and row['matching_prefix_bytes'] == 32
                            and row['first_mismatch_offset'] == 32
                            and row['candidate_byte_at_first_mismatch'] == 0
                            and row['oracle_byte_at_first_mismatch'] == 32
                            and row['acceptance'] == 'NOT_EVALUATED' for row in private))


if __name__ == '__main__':
    unittest.main()
