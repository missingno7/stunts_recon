"""Run the differential harness' three positive and one negative example."""
import argparse
from pathlib import Path

from .examples import (clear_rect_case, draw_filled_rect_case,
                       icon_combine_case, mulscl_case, negative_mismatch,
                       shape2d_runs_case, sprite_1_unk3_case,
                       sprite_1_unk_case)
from .runner import DiffHarness


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", type=Path,
                        help="read-only source asset directory (defaults to ROOT/assets)")
    parser.add_argument("--gcc", type=Path, help="i686 MinGW GCC used for port DLL")
    parser.add_argument("--instruction-budget", type=int, default=1_000_000)
    args = parser.parse_args()
    harness = DiffHarness(asset_dir=args.assets, gcc=args.gcc,
                          instruction_budget=args.instruction_budget)
    cases = (sprite_1_unk3_case(), draw_filled_rect_case(),
             sprite_1_unk_case(), icon_combine_case(False),
             icon_combine_case(True), icon_combine_case(True, width=1),
             shape2d_runs_case(False),
             shape2d_runs_case(True), clear_rect_case(), mulscl_case())
    for case in cases:
        original, _ = harness.run(case)
        print(f"PASS {case.name}: {original.instructions} original instructions")
    mismatch = negative_mismatch(harness)
    print("PASS negative mismatch detection: " + mismatch.details[0])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
