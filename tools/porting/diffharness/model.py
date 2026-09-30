"""Data types for routine-level historical-vs-port comparisons."""
from __future__ import annotations

from dataclasses import dataclass, field
from typing import Callable, Sequence


@dataclass(frozen=True)
class MemoryWrite:
    segment: int
    offset: int
    data: bytes


@dataclass(frozen=True)
class MemoryRegion:
    name: str
    segment: int
    offset: int
    size: int


@dataclass(frozen=True)
class RoutineCase:
    """One deterministic call contract. Stack words are listed arg0 first."""
    name: str
    routine: str
    args: Sequence[int] = ()
    call: str = "far"
    registers: dict[str, int] = field(default_factory=dict)
    memory: Sequence[MemoryWrite] = ()
    compare: Sequence[MemoryRegion] = ()
    result_registers: Sequence[str] = ("ax", "dx")
    compare_flags: bool = False
    port_call: Callable | None = None


@dataclass
class CallResult:
    registers: dict[str, int]
    flags: int
    memory: dict[str, bytes]
    instructions: int
    stop: str


class HarnessError(RuntimeError):
    pass


class TrapError(HarnessError):
    pass


class MismatchError(AssertionError):
    def __init__(self, case: RoutineCase, details: Sequence[str]):
        self.case = case
        self.details = list(details)
        super().__init__(f"{case.name}: differential mismatch\n" + "\n".join(details))
