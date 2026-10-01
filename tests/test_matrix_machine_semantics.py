"""Differentially verify mapped matrix helpers against locked 16-bit routines.

`mat_invert` is excluded because the reviewed symbol map has no verified
machine extent for it; its semantic C lead is not oracle evidence.
"""
from __future__ import annotations

import os
from pathlib import Path
import random
import shutil
import struct
import subprocess
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "build" / "python"))
sys.path.insert(0, str(ROOT / "tools" / "porting"))
sys.path.insert(0, str(ROOT))

try:
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import OracleImage, SymbolMap, dgroup_segment
except ImportError as error:  # Unicorn is an optional local oracle dependency.
    RealModeRunner = MemoryRegion = MemoryWrite = RoutineCase = None
    OracleImage = SymbolMap = dgroup_segment = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None

WORD_COUNT = 64
BASE_OFFSET = 0xA000
MEMORY_BYTES = WORD_COUNT * 2


def words_bytes(words: list[int]) -> bytes:
    return struct.pack("<64H", *(word & 0xFFFF for word in words))


def signed_word(bits: int) -> int:
    return bits - 0x10000 if bits & 0x8000 else bits


def set_words(target: list[int], start: int, values: list[int]) -> None:
    target[start:start + len(values)] = [value & 0xFFFF for value in values]


def compiler_path() -> Path:
    configured = os.environ.get("DIFFHARNESS_GCC")
    candidates = [Path(configured)] if configured else []
    candidates.extend((Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                       Path(r"C:\msys64\mingw64\bin\gcc.exe")))
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    found = shutil.which("gcc") or shutil.which("i686-w64-mingw32-gcc")
    if found:
        return Path(found).resolve()
    raise unittest.SkipTest("matrix semantic differential requires GCC")


def compiler_environment(compiler: Path) -> dict[str, str]:
    environment = os.environ.copy()
    sdl_bin = Path(r"C:\tools\sdl3-3.4.16-i686\bin")
    environment["PATH"] = os.pathsep.join(
        [str(compiler.parent), str(sdl_bin), environment.get("PATH", "")])
    return environment


def build_host_probe(compiler: Path) -> tuple[Path, dict[str, str]]:
    worker = ROOT / "build" / "workers" / "matrix_machine_semantics"
    worker.mkdir(parents=True, exist_ok=True)
    output = worker / "matrix_probe.exe"
    sdl = Path(r"C:\tools\sdl3-3.4.16-i686")
    command = [
        str(compiler), "-std=gnu11", "-O0", "-ffunction-sections",
        "-fdata-sections", "-DPORT_BUILD=1",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(sdl / "include"),
        str(ROOT / "port" / "matrix.c"),
        str(ROOT / "tests" / "matrix_machine_probe.c"),
        "-Wl,--gc-sections", "-o", str(output),
    ]
    environment = compiler_environment(compiler)
    result = subprocess.run(command, cwd=ROOT, env=environment,
                            capture_output=True, text=True, timeout=60,
                            check=False)
    if result.returncode:
        raise AssertionError("matrix host probe compile failed:\n" +
                             result.stdout + result.stderr)
    return output, environment


def machine_words(runner: RealModeRunner, routine: str, mode: int,
                  first: int, second: int, output: int,
                  words: list[int]) -> list[int]:
    segment = dgroup_segment()
    case = RoutineCase(
        f"{routine} pointers={first},{second},{output}", routine,
        args=[BASE_OFFSET + 2 * first, BASE_OFFSET + 2 * second,
              BASE_OFFSET + 2 * output],
        call="far",
        memory=[MemoryWrite(segment, BASE_OFFSET, words_bytes(words))],
        compare=[MemoryRegion("all_words", segment, BASE_OFFSET, MEMORY_BYTES)],
        result_registers=(),
    )
    actual = runner.call(case).memory["all_words"]
    return list(struct.unpack("<64H", actual))


def host_words(probe: Path, environment: dict[str, str], cases
               ) -> list[list[int]]:
    payload = "".join(
        " ".join(str(value) for value in
                 (mode, first, second, output,
                  *(word & 0xFFFF for word in words))) + "\n"
        for mode, first, second, output, words in cases)
    result = subprocess.run([str(probe)], input=payload, text=True,
                            env=environment, capture_output=True, timeout=30,
                            check=False)
    if result.returncode:
        raise AssertionError(f"matrix host probe failed: {result.stderr}")
    rows = [[int(word, 16) for word in line.split()]
            for line in result.stdout.splitlines()]
    if len(rows) != len(cases) or any(len(row) != WORD_COUNT for row in rows):
        raise AssertionError("matrix host probe returned an incomplete batch")
    return rows


def differential_cases(vector_name: str, multiply_name: str):
    cases = []
    vectors = []

    words = [0] * WORD_COUNT
    set_words(words, 8, [16384, 0, 0, 0, 15736, -4563, 0, 4563, 15736])
    set_words(words, 0, [0, -840, 2880])
    vectors.append(("car selection rotation", 0, 0, 8, 24, words))

    words = [0] * WORD_COUNT
    set_words(words, 8, [12000, -3100, 2750, 4900, 8700, -6200,
                         -1800, 7300, 11100])
    set_words(words, 0, [-16384, 901, 32767])
    vectors.append(("asymmetric nonalias", 0, 0, 8, 24, words))

    words = [0] * WORD_COUNT
    set_words(words, 8, [16000, 900, -700, -1200, 15000, 2500,
                         800, -3000, 14000])
    set_words(words, 0, [4000, -7000, 9000])
    vectors.append(("output aliases input vector", 0, 0, 8, 0, words))

    words = [0] * WORD_COUNT
    set_words(words, 8, [16384, 0, 0, 0, 16384, 0, 0, 0, 16384])
    set_words(words, 0, [-123, 456, -789])
    vectors.append(("identity aliases input vector", 0, 0, 8, 0, words))

    words = [0] * WORD_COUNT
    set_words(words, 8, [8192, 0, 0, 4096, 0, 0, 2048, 0, 0])
    set_words(words, 0, [1000, 2000, 3000])
    vectors.append(("zero coefficient skips", 0, 0, 8, 24, words))

    rng = random.Random(0x6D61745F766563)
    for index in range(24):
        words = [0] * WORD_COUNT
        vector = [signed_word(rng.randrange(0x10000)) for _ in range(3)]
        matrix = [signed_word(rng.randrange(0x10000)) for _ in range(9)]
        if index % 3 == 0:
            matrix[index % 9] = 0
        if index % 4 == 0:
            vector[index % 3] = 0
        set_words(words, 0, vector)
        set_words(words, 8, matrix)
        vectors.append((f"seeded mat_vec {index}", 0, 0, 8,
                        0 if index % 2 else 24, words))

    for label, mode, first, second, output, words in vectors:
        cases.append((label, vector_name, mode, first, second, output, words))

    for label, output in (
        ("mat_multiply distinct", 32),
        ("mat_multiply output aliases right", 0),
        ("mat_multiply output aliases left", 16),
        ("mat_multiply overwrites right term 2", 1),
        ("mat_multiply overwrites right term 3", 2),
        ("mat_multiply overwrites left term 2", 19),
        ("mat_multiply overwrites left term 3", 22),
    ):
        words = [0] * WORD_COUNT
        set_words(words, 0, [11000, -2300, 4100, 6700, 9200, -3700,
                             -5100, 2800, 14300])
        set_words(words, 16, [9800, 3300, -7100, -4200, 12500, 6100,
                              2500, -8900, 10100])
        set_words(words, 32, [0x5555] * 9)
        cases.append((label, multiply_name, 1, 0, 16, output, words))
    return cases


@unittest.skipIf(ORACLE_IMPORT_ERROR is not None,
                 f"pinned real-mode oracle dependencies unavailable: {ORACLE_IMPORT_ERROR}")
class MatrixMachineSemanticsTest(unittest.TestCase):
    def test_vector_and_matrix_memory_effects_match_locked_machine(self):
        compiler = compiler_path()
        probe, environment = build_host_probe(compiler)
        oracle = OracleImage.load()
        symbols = SymbolMap()
        vector_name = symbols.resolve("mat_vec").name
        multiply_name = symbols.resolve("mat_multiply").name
        runner = RealModeRunner(oracle, instruction_budget=1_000_000)
        cases = differential_cases(vector_name, multiply_name)

        expected = []
        for label, routine, mode, first, second, output, words in cases:
            with self.subTest(case=label):
                expected.append(machine_words(
                    runner, routine, mode, first, second, output, words))
        actual = host_words(probe, environment, [
            (mode, first, second, output, words)
            for _, _, mode, first, second, output, words in cases])

        self.assertEqual(len(cases), 36)
        for (label, *_), locked, port in zip(cases, expected, actual):
            if locked != port:
                mismatch = next(i for i, (left, right) in enumerate(zip(locked, port))
                                if left != right)
                self.fail(f"{label}: word[{mismatch}] locked={locked[mismatch]:04x} "
                          f"port={port[mismatch]:04x}")


if __name__ == "__main__":
    unittest.main()
