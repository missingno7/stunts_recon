"""Run one unittest module in an isolated process for validate.py."""
from __future__ import annotations

import json
import sys
import time
import traceback
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / "tools"), str(ROOT), str(ROOT / "build/python")]


def flatten(suite):
    for test in suite:
        if isinstance(test, unittest.TestSuite):
            yield from flatten(test)
        else:
            yield test


class RecordingResult(unittest.TextTestResult):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.statuses = {}

    @staticmethod
    def _id(test):
        try:
            return test.id()
        except (AttributeError, TypeError):
            return str(test)

    def addSuccess(self, test):
        self.statuses[self._id(test)] = "passed"
        super().addSuccess(test)

    def addFailure(self, test, err):
        self.statuses[self._id(test)] = "failed"
        super().addFailure(test, err)

    def addError(self, test, err):
        self.statuses[self._id(test)] = "error"
        super().addError(test, err)

    def addSkip(self, test, reason):
        self.statuses[self._id(test)] = "skipped"
        super().addSkip(test, reason)

    def addSubTest(self, test, subtest, err):
        if err is not None:
            self.statuses[self._id(test)] = "failed"
        super().addSubTest(test, subtest, err)


def run_module(module: str, run_dir: Path) -> int:
    log_path = run_dir / (module + ".log")
    result_path = run_dir / (module + ".json")
    started = time.perf_counter()
    try:
        loader = unittest.defaultTestLoader
        discovered = loader.discover(str(ROOT / "tests"), pattern=module + ".py")
        tests = list(flatten(discovered))
        if not tests:
            raise RuntimeError("module discovery produced no tests")
        # select_tests() deliberately flattens discovery suites, so module-level
        # fixtures remain outside the gate just as they were in the serial runner.
        suite = unittest.TestSuite(tests)
        from test_artifact_cache import install_compiler_cache
        install_compiler_cache(module)
        with log_path.open("w", encoding="utf-8", newline="") as log:
            with redirect_stdout(log), redirect_stderr(log):
                result = unittest.TextTestRunner(stream=log, verbosity=2,
                                                 resultclass=RecordingResult).run(suite)
        payload = {
            "module": module,
            "seconds": time.perf_counter() - started,
            "tests_run": result.testsRun,
            "passed": result.testsRun - len(result.errors) - len(result.failures) - len(result.skipped),
            "failed": len(result.errors) + len(result.failures),
            "skipped": len(result.skipped),
            "statuses": result.statuses,
            "failures": [{"test": test.id(), "traceback": detail}
                         for test, detail in result.failures],
            "errors": [{"test": test.id(), "traceback": detail}
                       for test, detail in result.errors],
            "skips": [{"test": test.id(), "reason": reason}
                      for test, reason in result.skipped],
        }
        result_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
        return 0
    except BaseException:
        detail = traceback.format_exc()
        log_path.write_text(detail, encoding="utf-8")
        result_path.write_text(json.dumps({
            "module": module, "seconds": time.perf_counter() - started,
            "tests_run": 0, "passed": 0, "failed": 1, "skipped": 0,
            "statuses": {}, "failures": [], "errors": [], "skips": [],
            "worker_error": detail,
        }, indent=2), encoding="utf-8")
        return 1


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit("usage: test_worker.py MODULE RUN_DIRECTORY")
    destination = Path(sys.argv[2])
    destination.mkdir(parents=True, exist_ok=True)
    raise SystemExit(run_module(sys.argv[1], destination))
