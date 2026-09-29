import json
import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "porting_extent_evidence", ROOT / "tools" / "porting" / "extent_evidence.py"
)
extent_evidence = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(extent_evidence)
array_dimension = extent_evidence.array_dimension
minimum_observed_access_bytes = extent_evidence.minimum_observed_access_bytes


class ExtentEvidenceTests(unittest.TestCase):
    def test_label_gap_and_access_lower_bound_do_not_prove_allocation(self):
        next_label_gap = 16
        observed = minimum_observed_access_bytes([(0, 2), (14, 2), (16, 2)])
        allocated_extent = None
        self.assertEqual(next_label_gap, 16)
        self.assertEqual(observed, 18)
        self.assertIsNone(array_dimension(allocated_extent, 2))

    def test_independent_boundary_allows_array_dimension(self):
        observed = minimum_observed_access_bytes([(0, 2), (14, 2), (16, 2)])
        self.assertEqual(observed, 18)
        self.assertEqual(array_dimension(18, 2), 9)

    def test_non_divisible_boundary_does_not_make_fixed_array(self):
        self.assertIsNone(array_dimension(17, 2))

    def test_camera_object_extents_have_locked_target_and_source_evidence(self):
        path = ROOT / "tools" / "porting" / "object-extent-evidence.json"
        data = json.loads(path.read_text(encoding="utf-8"))
        arrays = [row for row in data["objects"] if row.get("source_view", "").startswith("game_camera_buttons_")]
        self.assertEqual(len(arrays), 4)
        for row in arrays:
            with self.subTest(name=row["source_view"]):
                self.assertEqual(row["allocated_extent_bytes"], 18)
                self.assertEqual(len(row["target_values"]), 9)
                self.assertEqual(len(row["target_payload_hex"].split()), 18)
                self.assertTrue(any("layout/oracle.lock.json" in cite for cite in row["evidence"]))
                self.assertFalse(any("build/workers/" in cite for cite in row["evidence"]))


if __name__ == "__main__":
    unittest.main()
