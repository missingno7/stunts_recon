"""Routine-level differential execution against the pristine DOS image."""

from .model import (CallResult, HarnessError, MemoryRegion, MemoryWrite,
                    MismatchError, RoutineCase, TrapError)
from .runner import DiffHarness, differences

__all__ = ["CallResult", "DiffHarness", "HarnessError", "MemoryRegion",
           "MemoryWrite", "MismatchError", "RoutineCase", "TrapError",
           "differences"]
