"""Routine-level differential execution against the pristine DOS image."""

from .model import (CallResult, HarnessError, MemoryRegion, MemoryWrite,
                    MismatchError, RoutineCase, TrapError)

def __getattr__(name):
    # Oracle metadata and address checks do not require the optional emulator.
    if name == "RealModeRunner":
        from .emulator import RealModeRunner
        return RealModeRunner
    if name in {"DiffHarness", "differences"}:
        from .runner import DiffHarness, differences
        return {"DiffHarness": DiffHarness, "differences": differences}[name]
    raise AttributeError(name)

__all__ = ["CallResult", "DiffHarness", "HarnessError", "MemoryRegion",
           "MemoryWrite", "MismatchError", "RoutineCase", "TrapError",
           "RealModeRunner", "differences"]
