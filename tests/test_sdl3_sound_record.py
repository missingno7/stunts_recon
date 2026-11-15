"""Exact manual driver fixtures and observational wall-paced session comparisons."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_timer import host_compiler, ROOT
from tests.test_sdl3_driving import assert_build_ready, ASSETS, STARTUP_SEED, EXECUTABLE
from tests.sdl3_screen_regression import write_startup_input

WORK = ROOT / "build/workers/portclock/sound-comparison"

MANUAL = r'''
#include "port_runtime.h"
#include "audio_backend.h"
#include "sound_record.h"
#include "tick_clock.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void *audiodriverbinary;
unsigned char g_audiodrvvoices_count, audio_driver_mode, audio_driver_extension_mode,
 audio_pause_in_progress, audioflag2, audio_song_ready, audioflag6,
 g_musicvolumesetting, sfx_audio_vol;
char g_drvaudiocode[14];
static PortTickClock clock_state;
static volatile uint16_t pushed;
static const char *driver_path;
static uint8_t voices[16][0x2e], chunks[24][0x4c];
static uint8_t *sample;
static size_t sample_size;
static void observe(uint64_t *tick,int *slot) {
 *tick=clock_state.dispatched_tick;
 *slot=clock_state.dispatch_depth ? clock_state.callback_slot : -1;
}
static void *read_file(const char *path,size_t *length) {
 FILE *f=fopen(path,"rb"); assert(f);
 assert(!fseek(f,0,SEEK_END)); long n=ftell(f); assert(n>0);
 rewind(f); void *p=malloc((size_t)n); assert(p);
 assert(fread(p,1,(size_t)n,f)==(size_t)n); fclose(f); *length=(size_t)n; return p;
}
void *port_fs_load(const char *path,size_t *length,PortFarPtr *address) {
 (void)path;(void)address; void *p=read_file(driver_path,length);
 port_sound_record_resource("AD15.DRV",p,*length); return p;
}
void purgememory(void *p) { port_sound_record_forget(p); free(p); }
int port_memory_extent(const void *p,size_t *remaining) { (void)p;(void)remaining;return 0; }
void addtimer(void (*f)(void)) { assert(port_clock_add(&clock_state,f)); }
void deltimer(void (*f)(void)) { port_clock_remove(&clock_state,f); }
void audiodriver_timer(void) {}
void reset_audio_driver_state(void) {}
int port_audio_sdl_start(void) { return 0; }
void port_audio_sdl_stop(void) {}
static void script_tick(void) {
 uint8_t *v=voices[1], *c=chunks[16];
 switch(clock_state.dispatched_tick) {
 case 1:
   port_sound_record_logical("chunk_init",16,-1,sample,(uint32_t[]){127,64,16},3);
   port_sound_record_event(16,sample,0xe6,1,(uint32_t)(uintptr_t)sample,0,6,2);
   port_sound_record_event(16,sample,0xdd,100,0xdeadbeef,0,2,0);
   /* Existing AD15 ELPI fixture: bind, note 60/96, volume, scalar pitch. */
   port_audio_driver_21(1,v,c,sample); port_audio_driver_09(1,v,c,60,96,sample); break;
 case 2: port_audio_driver_12(1,v,100); port_audio_driver_24(1,v,0x1234); break;
 case 3: port_audio_driver_27(1,v,c,sample); port_audio_driver_0c(1,v); break;
 case 4: port_audio_driver_0f(1,v); port_audio_driver_18();
   port_audio_driver_1b(c,-1,1); port_audio_driver_1e(1); port_audio_driver_30(voices); break;
 case 5:
   port_audio_driver_15(1,v,0x40,-2); (void)port_audio_driver_36(1);
   port_audio_driver_39(2,(const uint8_t[]){0x10,0xff});
   (void)port_audio_driver_3c(); port_audio_driver_3f(2,(const uint8_t[]){0x20,0xfe});
   port_audio_driver_42(sample);
   port_sound_record_logical("chunk_complete",16,1,sample,(uint32_t[]){0xff},1); break;
 }
}
int main(int argc,char **argv) {
 assert(argc==4); driver_path=argv[2];
 port_clock_init(&clock_state); port_clock_setup(&clock_state);
 assert(port_sound_record_open(argv[1],observe));
 port_sound_record_tables(chunks,sizeof(chunks[0]),voices,sizeof(voices[0]));
 sample=read_file(argv[3],&sample_size);
 port_sound_record_resource("ELPI.VCE",sample,sample_size);
 assert(port_audio_backend_load("ad15"));
 assert(port_clock_add(&clock_state,script_tick));
 for(unsigned n=0;n<6;++n) {
   port_clock_advance(&clock_state,1); port_clock_dispatch(&clock_state,&pushed);
 }
 port_audio_driver_03(); port_audio_driver_06(); port_audio_shutdown();
 assert(port_sound_record_close()); free(sample); return 0;
}
'''


class SoundRequestTests(unittest.TestCase):
    def test_manual_ad15_fixture_repeats_byte_for_byte(self):
        """Reuse the instrument/note fixture from the existing AD15 regression."""
        from tools.porting.audio_bank import parse_archive
        from tools.porting.project_headers import expand_constant_literals
        from port.sound_adapter import adapt_sound_observers
        observed = adapt_sound_observers("lib/audiores.c",
            (ROOT / "lib/audiores.c").read_text(),
            (ROOT / "port/sound_record.h").as_posix())
        self.assertEqual(expand_constant_literals("AUDIO_CHUNK_CAPACITY", observed, ROOT), "24")
        compiler = host_compiler()
        self.assertIsNotNone(compiler)
        archive = (ROOT / "assets/ADSKIDMS.VCE").read_bytes()
        chunk = next(c for c in parse_archive(archive).chunks if c.name.upper() == "ELPI")
        environment = os.environ.copy()
        environment["PATH"] = str(Path(compiler).parent) + os.pathsep + environment.get("PATH", "")
        with tempfile.TemporaryDirectory(prefix="sound-fixture-") as temporary:
            directory = Path(temporary)
            source, exe = directory / "probe.c", directory / "probe.exe"
            sample = directory / "elpi.vce"
            source.write_text(MANUAL)
            sample.write_bytes(archive[chunk.start:chunk.end])
            # No SDL or upstream synthesizer is needed to observe driver calls.
            synth = directory / "synth.c"
            synth.write_text('#include "port_opl3.h"\n'
                'void port_opl3_reset(unsigned rate){(void)rate;}\n'
                'void port_opl3_write(uint8_t r,uint8_t v){(void)r;(void)v;}\n'
                'size_t port_opl3_render_s16(int16_t *p,size_t n,unsigned r)'
                '{(void)p;(void)r;return n;}\n')
            result = subprocess.run([compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-DPORT_SOUND_RECORD=1", "-I", str(ROOT / "port"), str(source), str(synth),
                *[str(ROOT / "port" / f) for f in
                  ("audio.c", "ad15_driver.c", "pc_speaker.c", "sound_record.c", "tick_clock.c")],
                "-o", str(exe)], env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            streams = []
            for n in range(2):
                path = directory / f"requests-{n}.jsonl"
                result = subprocess.run([str(exe), str(path), str(ROOT / "assets/AD15.DRV"),
                    str(sample)], env=environment, capture_output=True, text=True, timeout=5)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                streams.append(path.read_bytes())
            self.assertEqual(*streams)
            WORK.mkdir(parents=True, exist_ok=True)
            for label, stream in zip(("manual-a", "manual-b"), streams):
                (WORK / f"{label}.jsonl").write_bytes(stream)
            rows = list(map(json.loads, streams[0].splitlines()))[1:]
            driver = [r for r in rows if r["kind"] == "driver"]
            self.assertEqual({r["entry"] for r in driver},
                {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 48, 54, 57, 60, 63, 66})
            start = next(r for r in driver if r["entry"] == 9)
            self.assertEqual((start["chunk"], start["voice"], start["voice_generation"]), (16, 1, 1))
            self.assertEqual(start["arguments_u16"], [1, 60, 96])
            self.assertEqual(start["resource"]["name"], "ELPI.VCE")
            self.assertEqual(start["resource"]["offset"], 0)
            self.assertEqual(start["source"], "callback")
            self.assertEqual(next(r for r in driver if r["entry"] == 0)["return_u16"], 10)
            self.assertEqual(next(r for r in driver if r["entry"] == 57)["payload_hex"], "10ff")
            call = next(r for r in rows if r.get("command") == 0xe6)
            self.assertIsNone(call["value_u32"])
            self.assertEqual(call["target_resource"]["name"], "ELPI.VCE")
            self.assertNotIn(b"3735928559", streams[0])  # unused parser value stays absent
            for a, b in zip(rows, rows[1:]):
                self.assertGreater((b["raw_tick"], b["sequence"]), (a["raw_tick"], a["sequence"]))

    def test_manual_session_repeat_loaded_and_output_comparisons(self):
        from concurrent.futures import ThreadPoolExecutor
        from tests.test_sdl3_driving import write_driving_input
        assert_build_ready()
        WORK.mkdir(parents=True, exist_ok=True)
        environment = os.environ.copy()
        environment["SDL_VIDEODRIVER"] = "dummy"
        environment["PATH"] = os.pathsep.join((r"C:\msys64\mingw32\bin",
            r"C:\tools\sdl3-3.4.16-i686\bin", environment.get("PATH", "")))
        environment.pop("SDL_AUDIODRIVER", None)
        script = WORK / "manual-driving-input.json"
        write_driving_input(script)

        def session(label):
            folder = WORK / label
            folder.mkdir(exist_ok=True)
            # Clear only this test's previous frame artifacts, never inputs.
            for old in folder.glob("frame-*"): old.unlink()
            env = environment.copy()
            if label == "dummy": env["SDL_AUDIODRIVER"] = "dummy"
            result = subprocess.run([str(EXECUTABLE), f"--assets={ASSETS}",
                f"--input-script={script}", f"--test-startup-seed={STARTUP_SEED}",
                "--test-auto-protection", "--clock=manual", "--run-ticks=1800", "--audio=ad15",
                f"--sound-requests={folder / 'sound.jsonl'}", f"--trace={folder / 'trace.jsonl'}",
                f"--capture-dir={folder}", f"--diagnostics-dir={folder / 'diagnostics'}",
                f"--config={folder / 'config.json'}"], cwd=ROOT, env=env,
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            events = list(map(json.loads,(folder / "trace.jsonl").read_text().splitlines()))
            sounds = (folder / "sound.jsonl").read_bytes()
            states = [(e["raw_tick_id"],e["bytes_hex"]) for e in events if e.get("event_type")=="state_snapshot"]
            self.assertTrue(states, "session must reach GAMESTATE, not only startup screens")
            pictures = []
            for e in events:
                if e.get("event_type") != "video_publication": continue
                f = folder / ("frame-%06d.fbr" % e["frame_id"])
                if f.exists(): pictures.append((e["machine_tick"],e["frame_id"],f.read_bytes()[20:]))
            self.assertGreater(len(pictures),100)
            stop = next(e for e in events if e.get("event_type")=="host_stop")
            self.assertEqual(stop["machine_tick"],1800*10000154)
            return sounds,states,pictures,result.stderr

        baseline = session("repeat-a")
        repeat = session("repeat-b")
        with ThreadPoolExecutor(max_workers=2) as pool:
            loaded = list(pool.map(session,("loaded-a","loaded-b")))
        for result in [repeat,*loaded]:
            self.assertEqual(result[0],baseline[0],"sound requests differ at manual ticks")
            self.assertEqual(result[1],baseline[1],"GAMESTATE sequence differs")
            self.assertEqual(result[2],baseline[2],"indexed pixels/palette differ at the same ticks")
        dummy = session("dummy")
        self.assertEqual(dummy[1],baseline[1],"device must not affect GAMESTATE")
        self.assertEqual(dummy[2],baseline[2],"device must not affect indexed frames")
        report={"repeat":True,"loaded_repeat":True,"states_and_indexed_frames":True,
                "output_vs_dummy":dummy[0]==baseline[0],"ticks":1800,
                "state_count":len(baseline[1]),"indexed_frame_count":len(baseline[2]),
                "device_status":[[s for s in r[3].splitlines() if "PORT audio:" in s]
                                  for r in [baseline,repeat,*loaded,dummy]]}
        (WORK / "manual-comparison.json").write_text(json.dumps(report,indent=2)+"\n")
        print("Manual sound/state/frame comparison:",json.dumps(report))


if __name__ == "__main__":
    unittest.main()
