"""Ground the differential runner's DS in the locked executable, independently."""
import json
from pathlib import Path
import struct
import unittest

from tools.porting.diffharness.oracle import OracleImage, dgroup_segment

ROOT = Path(__file__).resolve().parents[1]


class DiffHarnessAnchorTests(unittest.TestCase):
    def test_default_stack_preserves_original_small_model_abi(self):
        from tools.porting.diffharness.emulator import RealModeRunner
        from tools.porting.diffharness.model import RoutineCase
        oracle = OracleImage.load()
        # The locked compare_ds_ss routine directly observes the selectors.
        self.assertEqual(oracle.load_image[131869:131881].hex(),
                         "33c08cd38cda3bda750140cb")
        runner = RealModeRunner(oracle)
        result = runner.call(RoutineCase("shared stack", "compare_ds_ss"))
        self.assertEqual(result.registers["ax"], 1)
        self.assertEqual(result.registers["ss"], result.registers["ds"])
        # Arguments/return sentinel must be placed at an overridden SS:SP,
        # rather than requiring a second manual stack write by the caller.
        result = runner.call(RoutineCase("explicit stack", "compare_ds_ss",
                                         registers={"ss": 0x8F00, "sp": 0xE000}))
        self.assertEqual(result.registers["ax"], 0)
        self.assertEqual(result.registers["sp"], 0xE004)

    def test_dgroup_agrees_with_startup_relocation_and_data_operand(self):
        symbols = json.loads((ROOT / "layout/data-symbols.json").read_text())
        frame = symbols["frame_load_address"]
        site = symbols["frame_relocation"]["load_offset"]
        original = OracleImage.load()
        self.assertEqual(original.load_image[site - 1], 0xBF)  # MOV DI, DGROUP
        paragraph = struct.unpack_from("<H", original.load_image, site)[0]
        self.assertEqual(paragraph * 16, frame)
        self.assertTrue(any(record["load_offset"] == site for record in original.relocations))
        imagefunc = symbols["symbols"]["_imagefunc"]["load_address"]
        self.assertEqual(imagefunc - frame, 0x4BB6)
        for load in (0, 0x1000, 0xE000):
            self.assertEqual(dgroup_segment(load), (load + paragraph) & 0xFFFF)
            relocated = original.relocated(load)
            self.assertEqual(struct.unpack_from("<H", relocated, site)[0], dgroup_segment(load))
        self.assertEqual(dgroup_segment(0x1000), 0x3B77)


if __name__ == "__main__":
    unittest.main()
