"""Fail-closed controls for exact DW OFFSET table research."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from table_offset_probe import evaluate, table_groups


class TableOffsetProbeTests(unittest.TestCase):
    def setUp(self):
        lines = ["off_10020 dw offset loc_10030",
                 "    dw offset loc_10034",
                 "loc_10024:",
                 "loc_10030:",
                 "loc_10034:"]
        self.groups = table_groups(lines, 1, len(lines))
        self.group = self.groups[0]
        self.definitions = {"off_10020": [1], "loc_10030": [4], "loc_10034": [5]}
        self.image = bytearray(0x40)
        self.image[0x20:0x24] = bytes.fromhex("30003400")

    def probe(self, **changes):
        args = dict(group=self.group, frame_paragraph=0,
                    definitions=self.definitions, image=self.image,
                    relocations=set(), verified_anchors={0x24},
                    function_start=0x10, function_end=0x30)
        args.update(changes)
        return evaluate(**args)

    def test_complete_continuation_and_verified_end_anchor(self):
        self.assertEqual(len(self.groups), 1)
        self.assertEqual(self.group["targets"], ["loc_10030", "loc_10034"])
        row = self.probe()
        self.assertEqual(row["state"], "EXACT_BRACKETED_TABLE_BYTES")
        self.assertEqual(row["predicted_hex"], "30003400")

    def test_byte_relocation_and_interval_controls_fail_closed(self):
        image = bytearray(self.image)
        image[0x22] = 0x35
        self.assertEqual(self.probe(image=image)["state"], "WORD_BYTES_DIFFER")
        self.assertEqual(self.probe(relocations={0x21})["state"],
                         "MZ_RELOCATION_INSIDE_TABLE")
        self.assertEqual(self.probe(function_end=0x23)["state"],
                         "TABLE_OUTSIDE_CANDIDATE_INTERVAL")

    def test_unresolved_target_and_unverified_bracket_do_not_prove_table(self):
        definitions = {**self.definitions, "loc_10030": [4, 99]}
        self.assertEqual(self.probe(definitions=definitions)["state"], "UNRESOLVED_TARGET")
        self.assertEqual(self.probe(verified_anchors=set())["state"],
                         "EXACT_UNBRACKETED_TABLE_BYTES")
        self.assertEqual(self.probe(frame_paragraph=None)["state"],
                         "UNPROVEN_SEGMENT_FRAME")


if __name__ == '__main__': unittest.main()
