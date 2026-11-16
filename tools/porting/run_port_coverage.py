"""Run port tests in two coverage phases and update the port-layer coverage report."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_BUILD = ROOT / "build" / "coverage" / "sdl3"
ORIGINAL_MODULES = {
    "test_sdl3_framebuffer",
    "test_sdl3_original_steps",
    "test_sdl3_original_ticks",
    "test_sdl3_renderer_equivalence",
    "test_sdl3_scenarios",
    "test_sdl3_selection_frames",
    "test_sdl3_startup_seed",
}
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "porting"))
from port_layer_coverage import annotate, collect_gcov


def flatten(suite):
    for item in suite:
        if isinstance(item, unittest.TestSuite):
            yield from flatten(item)
        else:
            yield item


def module_name(test) -> str:
    return test.__class__.__module__.rsplit(".", 1)[-1]


def clear_gcda(build_dir: Path) -> None:
    for data in build_dir.rglob("*.gcda"):
        data.unlink()


def run_phase(name: str, tests: list, build_dir: Path, profile_path: Path,
              executable: Path, gcov: Path) -> None:
    if not tests:
        raise RuntimeError(f"No tests selected for coverage phase {name}")
    clear_gcda(build_dir)
    suite = unittest.TestSuite(tests)
    print(f"\n=== Coverage phase: {name} ({len(tests)} tests) ===")
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    if not result.wasSuccessful():
        raise RuntimeError(f"Coverage phase {name} failed: {len(result.failures)} failures, "
                           f"{len(result.errors)} errors")
    profile = collect_gcov(build_dir, profile_path, gcov)
    profile["scenario_tests_passed"] = True
    profile["phase"] = name
    profile["tests_run"] = result.testsRun
    profile["modules"] = sorted({module_name(test) for test in tests})
    profile_path.write_text(json.dumps(profile, indent=2) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD)
    parser.add_argument("--executable", type=Path)
    parser.add_argument("--gcov", type=Path, default=Path(r"C:\msys64\mingw32\bin\gcov.exe"))
    parser.add_argument("--layer", type=Path, default=ROOT / "tools" / "porting" / "port-layer.json")
    parser.add_argument("--docs", type=Path, default=ROOT / "docs" / "porting" / "port-layer.md")
    args = parser.parse_args()
    build_dir = args.build_dir.resolve()
    executable = (args.executable or build_dir / "stunts.exe").resolve()
    if not executable.is_file():
        parser.error(f"instrumented executable is missing: {executable}; build with port/build.py build --coverage")
    report_path = build_dir / "build-report.json"
    if not report_path.is_file() or not json.loads(report_path.read_text()).get("coverage_instrumented"):
        parser.error(f"{report_path} does not describe a GCC --coverage build")
    if not args.gcov.is_file():
        parser.error(f"gcov is missing: {args.gcov}")

    os.environ["STUNTS_TEST_EXECUTABLE"] = str(executable)
    from tools import validate
    tests = list(flatten(validate.select_tests("port")))
    original_tests = [test for test in tests if module_name(test) in ORIGINAL_MODULES]
    other_tests = [test for test in tests if module_name(test) not in ORIGINAL_MODULES]
    if not original_tests:
        parser.error("original-reference scenario modules were not discovered")

    original_profile = build_dir.parent / "original-functions.json"
    other_profile = build_dir.parent / "other-functions.json"
    run_phase("original_compared", original_tests, build_dir, original_profile,
              executable, args.gcov.resolve())
    run_phase("other_port_tests", other_tests, build_dir, other_profile,
              executable, args.gcov.resolve())
    annotate(args.layer.resolve(), original_profile, other_profile, args.docs.resolve())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
