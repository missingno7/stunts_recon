"""Tick stamping, same-tick order, delayed live input and input suspension."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_input import ROOT, SDL_ROOT, host_compiler

PROBE = r"""
#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <assert.h>
#include <stdlib.h>
static uint64_t tick, last_tick, last_sequence;
static unsigned count, yields;
extern void flush_stdin(void);
void port_guest_notify_activity(void) {}
void port_guest_check_stop(void) {}
void port_guest_set_exit_status(int n) { (void)n; abort(); }
void port_guest_request_stop(const char *s) { (void)s; abort(); }
void port_guest_unwind(const char *s) { (void)s; abort(); }
uint64_t port_timer_raw_tick(void) { return tick; }
uint64_t port_timer_machine_time_ns(void) { return tick * PORT_TIMER_PERIOD_NS; }
void port_trace_key_consumed(uint16_t word) { (void)word; }
void port_trace_input_keyboard(uint64_t seq, uint64_t due, uint64_t now,
                               const uint8_t *codes, size_t n) {
    (void)codes; assert(n); assert(due == now && now == tick * PORT_TIMER_PERIOD_NS);
    if(count) assert(tick > last_tick || (tick == last_tick && seq > last_sequence));
    last_tick=tick; last_sequence=seq; ++count;
}
void port_trace_input_mouse(uint64_t seq,uint64_t due,uint64_t now,
                            double u,double v,uint16_t b) {
    (void)seq;(void)due;(void)now;(void)u;(void)v;(void)b; abort();
}
static void grant(void) {
    ++tick;
    port_input_script_pump(tick * PORT_TIMER_PERIOD_NS);
    port_input_apply_queued(tick);
}
void port_guest_yield(void) {
    assert(++yields <= 3); grant();
}
int main(int argc,char **argv) {
    assert(argc==2); assert(SDL_Init(SDL_INIT_EVENTS)); port_input_init();
    assert(port_input_script_load(argv[1]));
    /* Live input observed while the host is late: cannot affect old ticks. */
    port_input_set_queue_tick(4);
    port_input_queue_event(SDL_EVENT_KEY_DOWN,SDL_SCANCODE_C,0);
    port_input_queue_event(SDL_EVENT_KEY_UP,SDL_SCANCODE_C,0);
    assert(!port_input_key_state(0x2e));
    grant(); assert(tick==1 && count==2); /* legacy ns 0 and 1 both round to tick one */
    /* Locked INT9 queue capacity is one word; the later make overwrites a. */
    assert(port_input_key_state(0x1e)==0 && port_input_key_state(0x30)==0);
    assert(testkey()=='b'); assert(!testkey());
    grant(); assert(tick==2 && count==3);
    assert(testkey()=='d'); assert(!testkey());
    assert(!port_input_key_state(0x2e));
    /* Empty BIOS queue suspends; only explicit grants can make input visible. */
    flush_stdin(); assert(tick==4 && yields==2 && count==5);
    assert(!port_input_key_state(0x2e)); assert(!testkey());
    SDL_Quit(); return 0;
}
"""


class TickInputTests(unittest.TestCase):
    def test_stamped_input_order_and_wait_predicate(self):
        compiler = host_compiler()
        self.assertIsNotNone(compiler)
        scratch = ROOT / "build/workers/portclock"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="tick-input-", dir=scratch) as temporary:
            folder = Path(temporary)
            source, exe, script = folder / "probe.c", folder / "probe.exe", folder / "input.json"
            source.write_text(PROBE)
            script.write_text(json.dumps({"format":"stunts-exact-input-script-v1",
                "anchor_tick":0,"events":[{"visible_tick":ns,
                "channel":"dos.keyboard.scancodes","payload":[scan,scan|0x80]}
                for ns,scan in [(0,0x1e),(1,0x30),(10000155,0x20)]]}))
            env = os.environ.copy()
            env["PATH"] = os.pathsep.join((str(Path(compiler).parent),str(SDL_ROOT / "bin"),env.get("PATH","")))
            result = subprocess.run([compiler,"-std=c11","-Wall","-Wextra","-Werror",
                "-DPORT_TICK_HANDSHAKE=1","-I",str(ROOT / "port"),"-I",str(SDL_ROOT / "include"),
                str(source),str(ROOT / "port/input.c"),str(ROOT / "port/input_script.c"),
                "-L",str(SDL_ROOT / "lib"),"-lSDL3","-o",str(exe)],
                env=env,capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            result = subprocess.run([str(exe),str(script)],env=env,capture_output=True,text=True,timeout=5)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)


if __name__ == "__main__":
    unittest.main()
