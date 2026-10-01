"""Oracle checks for unsigned speed handling in crash-state transitions."""
from __future__ import annotations

import json
import os
from pathlib import Path
import struct
import sys
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "build" / "python"))

try:
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import (
        DEFAULT_LOAD_SEGMENT, OracleImage, dgroup_segment,
    )
except ImportError as error:
    RealModeRunner = MemoryRegion = MemoryWrite = RoutineCase = OracleImage = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None


ASSETS = Path(os.environ.get("STUNTS_ASSET_DIR", ROOT / "assets"))
DATA_SYMBOLS = ROOT / "layout" / "data-symbols.json"

# The original update_crash_state instructions select the player at DS:8E76
# and the opponent at DS:8F46. The player base plus the CARSTATE field offsets
# comes from the preserved DOS layout: car_speed +42, car_speed2 +44, crash
# flag +0xC9. GAMESTATE's game_impactSpeed is six bytes before the player base.
PLAYER_STATE_DS = 0x8E76
OPPONENT_STATE_DS = 0x8F46
IMPACT_SPEED_DS = PLAYER_STATE_DS - 6
# The original player's crash branch passes the setup at DS:A6EA.
CAR_SETUP_DS = 0xA6EA
RATE_FRAME_DS = 0x9260
CAR_SPEED_OFFSET = 42
CAR_SPEED2_OFFSET = 44
CAR_RPM_OFFSET = 34
CAR_REAR_SURFACES_OFFSET = 192
CAR_ALL_SURFACES_OFFSET = 193
CRASH_FLAG_OFFSET = 0xC9

BOUNDARY_SPEEDS = (0, 0x7FFF, 0x8000, 0x807B, 0xFFFF)


@unittest.skipUnless(ASSETS.is_dir(), "original MCGA assets are not provisioned")
class Sdl3CrashOracleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        if ORACLE_IMPORT_ERROR is not None:
            raise unittest.SkipTest(
                f"locked machine oracle dependency is unavailable: {ORACLE_IMPORT_ERROR}")
        cls.oracle = OracleImage.load(ASSETS)
        cls.runner = RealModeRunner(cls.oracle)
        cls.dgroup = dgroup_segment(DEFAULT_LOAD_SEGMENT)

    def call_original(self, crash_mode: int, speed: int) -> tuple[int, int, int, int]:
        """Run the locked DOS routine and return flag, speeds, and impact speed."""
        player = bytearray(0xD0)
        struct.pack_into("<H", player, CAR_SPEED_OFFSET, speed)
        struct.pack_into("<H", player, CAR_SPEED2_OFFSET, speed)
        case = RoutineCase(
            name=f"original update_crash_state({crash_mode}, 0), speed={speed:#06x}",
            routine="update_crash_state",
            args=(crash_mode, 0),
            call="far",
            memory=[MemoryWrite(self.dgroup, PLAYER_STATE_DS, bytes(player))],
            compare=[
                MemoryRegion("player", self.dgroup, PLAYER_STATE_DS, len(player)),
                MemoryRegion("impact", self.dgroup, IMPACT_SPEED_DS, 2),
            ],
            result_registers=(),
        )
        result = self.runner.call(case)
        player_after = result.memory["player"]
        return (
            player_after[CRASH_FLAG_OFFSET],
            struct.unpack_from("<H", player_after, CAR_SPEED_OFFSET)[0],
            struct.unpack_from("<H", player_after, CAR_SPEED2_OFFSET)[0],
            struct.unpack_from("<H", result.memory["impact"])[0],
        )

    def call_original_car_speed(self, speed: int, speed2: int) -> tuple[int, int]:
        """Call the original braking update with a synthetic, explicit setup."""
        player = bytearray(0xD0)
        struct.pack_into("<H", player, CAR_RPM_OFFSET, 1000)
        struct.pack_into("<H", player, CAR_SPEED_OFFSET, speed)
        struct.pack_into("<H", player, CAR_SPEED2_OFFSET, speed2)
        player[CAR_REAR_SURFACES_OFFSET] = 4
        player[CAR_ALL_SURFACES_OFFSET] = 4

        # Only the fields read on this path are populated. With the far
        # aerorest pointer left at 0000:0000, the original's zero-initialized
        # conventional memory supplies a zero table entry; pseudo-gravity is
        # also zero. Therefore input 2 at 20 fps subtracts exactly 260.
        setup = bytearray(0x400)
        struct.pack_into("<H", setup, 4, 260)   # SIMD.braking_eff
        struct.pack_into("<H", setup, 6, 1000)  # SIMD.idle_rpm
        struct.pack_into("<H", setup, 12, 8000) # SIMD.max_rpm
        case = RoutineCase(
            name=f"original update_car_speed(2, 0), speed={speed}, speed2={speed2}",
            routine="update_car_speed",
            args=(2, 0, PLAYER_STATE_DS, CAR_SETUP_DS),
            call="far",
            memory=[
                MemoryWrite(self.dgroup, PLAYER_STATE_DS, bytes(player)),
                MemoryWrite(self.dgroup, CAR_SETUP_DS, bytes(setup)),
                MemoryWrite(self.dgroup, RATE_FRAME_DS, struct.pack("<H", 20)),
            ],
            compare=[MemoryRegion("player", self.dgroup, PLAYER_STATE_DS, len(player))],
            result_registers=(),
        )
        result = self.runner.call(case)
        player_after = result.memory["player"]
        return (
            struct.unpack_from("<H", player_after, CAR_SPEED_OFFSET)[0],
            struct.unpack_from("<H", player_after, CAR_SPEED2_OFFSET)[0],
        )

    def test_dgroup_and_crashpath_pointers_match_layout(self) -> None:
        """Ground DS and the original player-crash path's pointers."""
        symbols = json.loads(DATA_SYMBOLS.read_text(encoding="utf-8"))
        site = symbols["frame_relocation"]["load_offset"]
        self.assertEqual(self.oracle.load_image[site - 1], 0xBF)  # MOV DI, DGROUP
        dgroup_paragraph = struct.unpack_from("<H", self.oracle.load_image, site)[0]
        self.assertEqual(dgroup_paragraph * 16, symbols["frame_load_address"])
        self.assertEqual(dgroup_segment(DEFAULT_LOAD_SEGMENT),
                         (DEFAULT_LOAD_SEGMENT + dgroup_paragraph) & 0xFFFF)

        # data-symbols.json independently anchors this BSS word at the load
        # address reached by DGROUP:8E76. The code bytes then show the same
        # player/opponent DS displacements in update_crash_state's selection.
        player_anchor = symbols["symbols"]["_word_345E6"]["load_address"]
        self.assertEqual(symbols["frame_load_address"] + PLAYER_STATE_DS,
                         player_anchor)
        self.assertEqual(self.oracle.load_image[38396:38401],
                         bytes.fromhex("c746fc768e"))
        self.assertEqual(self.oracle.load_image[38418:38423],
                         bytes.fromhex("c746fc468f"))
        self.assertEqual(OPPONENT_STATE_DS, 0x8F46)

        # player_op tests crash flag, speed2 and speed at these exact DS
        # operands. With crash set, speed2 zero and speed nonzero, its path
        # passes DS:A6EA + DS:8E76 into update_car_speed with input 2.
        self.assertEqual(self.oracle.load_image[29184:29189],
                         bytes.fromhex("803e3f8f00"))
        self.assertEqual(self.oracle.load_image[29200:29205],
                         bytes.fromhex("833ea28e00"))
        self.assertEqual(self.oracle.load_image[29212:29217],
                         bytes.fromhex("833ea08e00"))
        self.assertEqual(self.oracle.load_image[29250:29253],
                         bytes.fromhex("b8eaa6"))
        self.assertEqual(self.oracle.load_image[29254:29257],
                         bytes.fromhex("b8768e"))
        self.assertEqual(self.oracle.load_image[29267:29270],
                         bytes.fromhex("e82208"))  # near call to load offset 31352

    def test_mode_one_captures_but_retains_unsigned_impact_speed(self) -> None:
        for speed in BOUNDARY_SPEEDS:
            with self.subTest(speed=f"{speed:#06x}"):
                crash_flag, car_speed, car_speed2, impact_speed = self.call_original(1, speed)
                self.assertEqual(crash_flag, 1)
                self.assertEqual(car_speed, speed)
                self.assertEqual(car_speed2, speed)
                self.assertEqual(impact_speed, speed)

    def test_suppression_modes_capture_impact_before_zeroing_speed(self) -> None:
        for crash_mode, expected_flag in ((2, 2), (5, 1)):
            for speed in BOUNDARY_SPEEDS:
                with self.subTest(crash_mode=crash_mode, speed=f"{speed:#06x}"):
                    crash_flag, car_speed, car_speed2, impact_speed = self.call_original(
                        crash_mode, speed)
                    self.assertEqual(crash_flag, expected_flag)
                    self.assertEqual(car_speed, 0)
                    self.assertEqual(car_speed2, 0)
                    self.assertEqual(impact_speed, speed)

    def test_crash_braking_can_restore_speed2_from_retained_car_speed(self) -> None:
        # The live trace's 588/0 -> 328/328 transition follows this original
        # code path. update_car_speed starts from car_speed, applies brake,
        # then synchronizes both words when rear wheels exist and their gap is
        # within 20 mph (0x1400). Thus speed2 is not globally monotonic through
        # the final crash frames even with throttle overridden to braking.
        vectors = (
            (0, 0, 0),
            (259, 0, 0),
            (260, 0, 0),
            (261, 0, 1),
            (588, 0, 328),
            (328, 328, 68),
            (68, 68, 0),
        )
        for speed, speed2, expected in vectors:
            with self.subTest(speed=speed, speed2=speed2):
                car_speed, updated_speed2 = self.call_original_car_speed(speed, speed2)
                self.assertEqual(car_speed, expected)
                self.assertEqual(updated_speed2, expected)


if __name__ == "__main__":
    unittest.main()
