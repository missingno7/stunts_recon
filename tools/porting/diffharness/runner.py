"""Execute one original call and compare it with a PORT_BUILD host call."""
from __future__ import annotations

from .emulator import RealModeRunner
from .model import CallResult, MismatchError, RoutineCase
from .oracle import OracleImage
from .port import PortLibrary


def differences(case: RoutineCase, original: CallResult,
                port: CallResult) -> list[str]:
    issues = []
    for name in case.result_registers:
        before = original.registers.get(name.lower())
        after = port.registers.get(name.lower())
        if before != after:
            issues.append(f"{name.upper()}: original={before!r}, port={after!r}")
    if case.compare_flags:
        mask = case.flag_mask & 0xFFFF
        before, after = original.flags & mask, port.flags & mask
        if before != after:
            issues.append(f"FLAGS&{mask:04X}: original={before:04X}, port={after:04X}")
    for region in case.compare:
        before = original.memory.get(region.name, b"")
        after = port.memory.get(region.name, b"")
        if len(before) != len(after):
            issues.append(f"{region.name}: original length={len(before)}, "
                          f"port length={len(after)}")
            continue
        if before != after:
            at = next(i for i, (left, right) in enumerate(zip(before, after))
                      if left != right)
            issues.append(f"{region.name}[{at:#x}]: original={before[at]:02X}, "
                          f"port={after[at]:02X} ("+
                          f"{sum(a != b for a, b in zip(before, after))} differing bytes)")
    return issues


class DiffHarness:
    def __init__(self, *, asset_dir=None, gcc=None, load_segment: int = 0x1000,
                 instruction_budget: int = 1_000_000):
        self.oracle = OracleImage.load(asset_dir)
        self.machine = RealModeRunner(self.oracle, load_segment=load_segment,
                                       instruction_budget=instruction_budget)
        self.port = PortLibrary(gcc)

    def run(self, case: RoutineCase) -> tuple[CallResult, CallResult]:
        original = self.machine.call(case)
        port = self.port.call(case)
        issues = differences(case, original, port)
        if issues:
            raise MismatchError(case, issues)
        return original, port
