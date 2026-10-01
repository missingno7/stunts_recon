"""Exercise host audio backends and SDL3 delivery."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
import re


ROOT = Path(__file__).resolve().parents[1]
SDL_ROOT = Path(os.environ.get("SDL3_ROOT", r"C:\tools\sdl3-3.4.16-i686"))


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                      Path(r"C:\msys64\mingw64\bin\gcc.exe")):
        if candidate.is_file():
            return str(candidate)
    return shutil.which("gcc")


def extract_generated_function(source: str, signature: str) -> str:
    """Return one complete generated C function by its line-start signature."""
    match = None
    for candidate in re.finditer(signature, source, re.MULTILINE):
        open_brace = source.find("{", candidate.end())
        semicolon = source.find(";", candidate.end())
        if open_brace >= 0 and (semicolon < 0 or open_brace < semicolon):
            match = candidate
            break
    if match is None:
        raise AssertionError(f"generated overlay lacks {signature!r}")
    open_brace = source.find("{", match.start())
    if open_brace < 0:
        raise AssertionError(f"generated overlay has no body for {signature!r}")
    depth = 0
    for index in range(open_brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[match.start():index + 1]
    raise AssertionError(f"unterminated generated function {signature!r}")


def extract_generated_struct(source: str, name: str) -> str:
    match = re.search(r"(?m)^struct " + re.escape(name) + r"\s*\{", source)
    if match is None:
        raise AssertionError(f"generated overlay lacks struct {name}")
    open_brace = source.find("{", match.start())
    depth = 0
    for index in range(open_brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                end = source.find(";", index)
                return source[match.start():end + 1]
    raise AssertionError(f"unterminated generated struct {name}")


class SDL3AudioTests(unittest.TestCase):
    def test_ad15_driver_matches_locked_machine_register_traces(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the AD15 driver checks")
        import sys
        sys.path.insert(0, str(ROOT / "tools" / "porting"))
        import audio_bank

        skid_path = ROOT / "assets" / "ADSKIDMS.VCE"
        engine_path = ROOT / "assets" / "ADENG1.VCE"
        skid_bytes = skid_path.read_bytes()
        engine_bytes = engine_path.read_bytes()
        skid = audio_bank.parse_archive(skid_bytes)
        engine = audio_bank.parse_archive(engine_bytes)
        records = {
            chunk.name.upper(): skid_bytes[chunk.start:chunk.end]
            for chunk in skid.chunks
        }
        records.update({
            chunk.name.upper(): engine_bytes[chunk.start:chunk.end]
            for chunk in engine.chunks
        })
        for name in ("ELPI", "SNAR", "STAR"):
            self.assertIn(name, records)

        compiler_env = os.environ.copy()
        compiler_env["PATH"] = os.pathsep.join(
            [str(Path(compiler).resolve().parent), compiler_env.get("PATH", "")])
        with tempfile.TemporaryDirectory(prefix="stunts-ad15-driver-") as directory:
            temp = Path(directory)
            fixture_paths = [temp / "AD15.DRV"]
            fixture_paths[0].write_bytes((ROOT / "assets" / "AD15.DRV").read_bytes())
            for name in ("ELPI", "SNAR", "STAR"):
                path = temp / f"{name}.VCE"
                path.write_bytes(records[name])
                fixture_paths.append(path)
            executable = temp / "ad15-driver-probe.exe"
            command = [
                compiler, "-std=c11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-I", str(ROOT / "port"),
                str(ROOT / "tests" / "sdl3" / "ad15_driver_probe.c"),
                str(ROOT / "port" / "ad15_driver.c"), "-o", str(executable),
            ]
            result = subprocess.run(command, cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable), *(str(path) for path in fixture_paths)],
                                    cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            if result.returncode == 77:
                self.skipTest("AD15 voice pointers require the i686 target layout")
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("AD15 register and voice-state traces match", result.stdout)

    def test_nuked_opl3_wrapper_matches_reference_pcm(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the OPL3 core check")
        import sys
        sys.path.insert(0, str(ROOT / "port"))
        from dependencies import nuked_opl3_root

        dependency_root = nuked_opl3_root(fetch=False)
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = os.pathsep.join(
            [str(Path(compiler).resolve().parent), compiler_env.get("PATH", "")])
        with tempfile.TemporaryDirectory(prefix="stunts-opl3-core-") as directory:
            executable = Path(directory) / "opl3-probe.exe"
            command = [
                compiler, "-std=c11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-I", str(ROOT / "port"),
                "-I", str(dependency_root),
                str(ROOT / "tests" / "sdl3" / "opl3_probe.c"),
                str(ROOT / "port" / "port_opl3.c"),
                str(dependency_root / "opl3.c"), "-o", str(executable),
            ]
            result = subprocess.run(command, cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("wrapper matches direct Nuked", result.stdout)

    def test_shipped_ad15_instrument_renders_bipolar_pcm(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the AD15 PCM check")
        import sys
        sys.path.insert(0, str(ROOT / "port"))
        from dependencies import nuked_opl3_root

        dependency_root = nuked_opl3_root(fetch=False)
        import sys
        sys.path.insert(0, str(ROOT / "tools" / "porting"))
        import audio_bank

        bank_path = ROOT / "assets" / "ADSKIDMS.VCE"
        bank_bytes = bank_path.read_bytes()
        bank = audio_bank.parse_archive(bank_bytes)
        record = next(chunk for chunk in bank.chunks if chunk.name.upper() == "ELPI")
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = os.pathsep.join(
            [str(Path(compiler).resolve().parent), compiler_env.get("PATH", "")])
        with tempfile.TemporaryDirectory(prefix="stunts-ad15-pcm-") as directory:
            temp = Path(directory)
            driver_path = temp / "AD15.DRV"
            record_path = temp / "ELPI.VCE"
            driver_path.write_bytes((ROOT / "assets" / "AD15.DRV").read_bytes())
            record_path.write_bytes(bank_bytes[record.start:record.end])
            executable = temp / "ad15-pcm-probe.exe"
            command = [
                compiler, "-std=c11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-I", str(ROOT / "port"),
                "-I", str(dependency_root),
                str(ROOT / "tests" / "sdl3" / "ad15_pcm_probe.c"),
                str(ROOT / "port" / "ad15_driver.c"),
                str(ROOT / "port" / "port_opl3.c"),
                str(dependency_root / "opl3.c"), "-o", str(executable),
            ]
            result = subprocess.run(command, cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable), str(driver_path), str(record_path)],
                cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                check=False)
            if result.returncode == 77:
                self.skipTest("AD15 voice pointers require the i686 target layout")
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("ELPI note rendered", result.stdout)

    def test_missing_skid_over_instrument_is_rejected_without_voice_mutation(self) -> None:
        # The crash is caused by a real shipped bank pairing: SKIDOVER names
        # KEYS as instrument slot 2, while PCSKIDMS has no KEYS record.
        import sys
        sys.path.insert(0, str(ROOT / "tools" / "porting"))
        import audio_bank

        kms_path = ROOT / "assets" / "SKIDOVER.KMS"
        vce_path = ROOT / "assets" / "PCSKIDMS.VCE"
        self.assertTrue(kms_path.is_file(), "SKIDOVER.KMS test asset is required")
        self.assertTrue(vce_path.is_file(), "PCSKIDMS.VCE test asset is required")
        kms = kms_path.read_bytes()
        outer = audio_bank.parse_archive(kms)
        over_chunk = next(chunk for chunk in outer.chunks if chunk.name.lower() == "over")
        song = audio_bank.parse_archive(kms, over_chunk.start, over_chunk.end)
        header_chunk = next(chunk for chunk in song.chunks if chunk.name.upper() == "HDR1")
        instruments = audio_bank.parse_header_chunk(kms, header_chunk)["instrument_names"]
        self.assertGreaterEqual(len(instruments), 3)
        self.assertEqual(instruments[2], "KEYS")
        voices = audio_bank.parse_archive(vce_path.read_bytes())
        self.assertNotIn("KEYS", {chunk.name.upper() for chunk in voices.chunks})

        overlay_dir = ROOT / "build" / "sdl3" / "host-build" / "overlay" / "src"
        overlay_027 = overlay_dir / "obj_seg027.c"
        overlay_028 = overlay_dir / "obj_seg028.c"
        overlay_029 = overlay_dir / "obj_seg029.c"
        if not all(path.is_file() for path in (overlay_027, overlay_028, overlay_029)):
            self.skipTest("build the SDL3 host overlay to execute its generated audio functions")
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the generated-overlay regression")

        src027 = overlay_027.read_text(encoding="latin-1")
        src028 = overlay_028.read_text(encoding="latin-1")
        src029 = overlay_029.read_text(encoding="latin-1")
        structs = []
        for name, packing in (("AudioEvent", 2), ("AudioChunk", 1), ("AudioVoice", 1)):
            structs.append(f"#pragma pack(push, {packing})\n" +
                           extract_generated_struct(src028, name) +
                           "\n#pragma pack(pop)\n")
        functions = [
            extract_generated_function(src029,
                r"^I16 FAR audioresource_compare_chunknames\s*\("),
            extract_generated_function(src029,
                r"^I16 FAR audioresource_get_chunk_index\s*\("),
            extract_generated_function(src029,
                r"^I8 FAR \* FAR audioresource_find\s*\("),
            extract_generated_function(src027,
                r"^void FAR audioresource_copy_4_bytes\s*\("),
            extract_generated_function(src027,
                r"^void FAR audio_map_song_instruments\s*\("),
            extract_generated_function(src028,
                r"^I16 FAR _loadds process_audio_event\s*\("),
        ]
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = os.pathsep.join(
            [str(Path(compiler).resolve().parent), compiler_env.get("PATH", "")])
        with tempfile.TemporaryDirectory(prefix="stunts-missing-audio-instrument-") as tmp:
            tmp_path = Path(tmp)
            probe_template = (ROOT / "tests" / "sdl3" /
                              "audio_missing_instrument_probe.c").read_text(encoding="latin-1")
            probe_source = probe_template.replace(
                "/* GENERATED_AUDIO_TYPES */", "\n\n".join(structs)).replace(
                "/* GENERATED_AUDIO_FUNCTIONS */", "\n\n".join(functions))
            probe = tmp_path / "audio_missing_instrument_probe.c"
            probe.write_text(probe_source, encoding="latin-1")
            executable = tmp_path / "audio-missing-instrument-probe.exe"
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wno-unused-parameter", "-Wno-incompatible-pointer-types",
                "-Wno-implicit-function-declaration", "-DPORT_BUILD=1",
                "-DPORT_AUDIO_PROBE_STRUCTS=1",
                "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
                "-I", str(ROOT / "tools" / "porting" / "port_include"),
                "-I", str(ROOT / "port"), "-I", str(tmp_path),
                str(probe), "-o", str(executable),
            ]
            result = subprocess.run(command, cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable), str(kms_path), str(vce_path)],
                                    cwd=ROOT, env=compiler_env,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("missing KEYS instrument safely rejected", result.stdout)

    def test_pc15_sample_isr_and_pit2_modes_one_and_three(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the SDL3 host checks")
        import sys
        sys.path.insert(0, str(ROOT / "port"))
        from dependencies import nuked_opl3_root

        dependency_root = nuked_opl3_root(fetch=False)

        with tempfile.TemporaryDirectory(prefix="stunts-pc15-audio-") as directory:
            executable = Path(directory) / "pc15-audio-probe.exe"
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-Wno-unused-parameter",
                "-I", str(ROOT / "port"), "-I", str(dependency_root),
                str(ROOT / "tests" / "sdl3" / "pc15_audio_probe.c"),
                str(ROOT / "port" / "audio.c"),
                str(ROOT / "port" / "pc_speaker.c"),
                str(ROOT / "port" / "ad15_driver.c"),
                str(ROOT / "port" / "port_opl3.c"),
                str(dependency_root / "opl3.c"),
                "-o", str(executable),
            ]
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join(
                [str(Path(compiler).resolve().parent), environment.get("PATH", "")])
            result = subprocess.run(command, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("PC15 sample ISR and PIT2 mode-1/mode-3 checks passed",
                          result.stdout)

    def test_sdl3_dummy_audio_stream_requests_samples(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the SDL3 host checks")
        if not (SDL_ROOT / "include" / "SDL3" / "SDL.h").is_file():
            self.skipTest("SDL3 headers are not provisioned")
        if not (SDL_ROOT / "lib" / "libSDL3.dll.a").is_file():
            self.skipTest("SDL3 import library is not provisioned")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-audio-") as directory:
            executable = Path(directory) / "sdl3-audio-probe.exe"
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-Wno-unused-parameter",
                "-I", str(SDL_ROOT / "include"), "-I", str(ROOT / "port"),
                str(ROOT / "tests" / "sdl3" / "audio_sdl_probe.c"),
                str(ROOT / "port" / "audio_sdl.c"),
                "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(executable),
            ]
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join(
                [str(Path(compiler).resolve().parent), str(SDL_ROOT / "bin"),
                 environment.get("PATH", "")])
            result = subprocess.run(command, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("SDL3 dummy playback requested samples", result.stdout)


if __name__ == "__main__":
    unittest.main()
