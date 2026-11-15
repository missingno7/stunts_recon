"""Manual clock contracts, including the actual gameint countdown, without SDL."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_timer import host_compiler, ROOT
from tests.test_sdl3_audio import extract_generated_function


PROBE = r'''
#include "tick_clock.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
static PortTickClock clock_state;
static volatile uint16_t pushed;
static char events[64];
static unsigned event_count;
static void a(void) { events[event_count++]='A'; }
static void b(void) { events[event_count++]='B'; }
static void c(void) { events[event_count++]='C'; }
static void remove_next(void) { events[event_count++]='R'; port_clock_remove(&clock_state,b); }
static void add_next(void) { events[event_count++]='+'; assert(port_clock_add(&clock_state,c)); }
static void nested(void) {
    events[event_count++]='N';
    port_clock_advance(&clock_state,1);
    port_clock_dispatch(&clock_state,&pushed);
    assert(clock_state.counter==2);
}
static void step(unsigned n) {
    port_clock_advance(&clock_state,n);
    port_clock_dispatch(&clock_state,&pushed);
}
static void fresh(void) {
    port_clock_init(&clock_state); port_clock_setup(&clock_state);
    pushed=0; event_count=0;
}
/* Host the accepted gameint body, not a copied countdown implementation. */
static int16_t call_proc_flag, snd_tick_clock, frmcs_time, g_audio_frms_ix,
    sndposrecord, sigframe, g_rplbfask, inrepflg, gm_playmode,
    timeraud, g_clocks, g_rplmodui, slomodiv8;
static struct { int16_t game_end_delay_elapsed,game_end_delay_limit; } core={0,1000};
static uint8_t audio_frmarr[0x22*0x28];
static unsigned simulation_calls;
static int sseqds(void) { return 1; }
static void myupdatesfx1(void *p,int ticks) { (void)p;(void)ticks; }
static void myupdatesfx(void) {}
static void moveeverything(int mode) { assert(mode==0); ++simulation_calls; }
#define AUDIO_FRAME_BYTES 0x22
#define AUDIO_FRAME_CAPACITY 0x28
GAMEINT
int main(void) {
    fresh();
    port_clock_advance(&clock_state,4);
    assert(atomic_load(&clock_state.raw_tick)==4 && atomic_load(&clock_state.elapsed)==0);
    assert(clock_state.counter==0); /* publication never runs guest code */
    port_clock_dispatch(&clock_state,&pushed);
    assert(clock_state.counter==4 && port_clock_delta(&clock_state)==4);
    assert(port_clock_delta(&clock_state)==0);
    step(1); assert(atomic_load(&clock_state.elapsed)==1);
    step(3); port_clock_setup(&clock_state);
    assert(clock_state.counter==0 && port_clock_delta(&clock_state)==0);
    assert(atomic_load(&clock_state.raw_tick)==8 && atomic_load(&clock_state.elapsed)==1);
    step(4); assert(atomic_load(&clock_state.elapsed)==1);
    step(1); assert(atomic_load(&clock_state.elapsed)==2);
    atomic_store(&clock_state.enabled,0); step(7);
    assert(clock_state.counter==5 && atomic_load(&clock_state.elapsed)==2);
    atomic_store(&clock_state.enabled,1);
    pushed=0x0100; step(5); /* locked CMP BYTE, not native word truthiness */
    assert(clock_state.counter==10 && atomic_load(&clock_state.elapsed)==3);
    pushed=0x0101; step(5);
    assert(clock_state.counter==10 && atomic_load(&clock_state.elapsed)==4);
    pushed=0; port_clock_seed_counter(&clock_state,0xfffffffeu);
    port_clock_set_copy(&clock_state,4);
    assert(port_clock_test_copy(&clock_state)); /* unsigned wrapped deadline */
    step(1); assert(clock_state.counter==0xffffffffu);
    step(1); assert(clock_state.counter==0 && port_clock_delta(&clock_state)==2);
    atomic_store(&clock_state.elapsed,0xffffffffu); step(3);
    assert(atomic_load(&clock_state.elapsed)==0);
    atomic_store(&clock_state.elapsed,0x20000); port_clock_set_input(&clock_state,0x100);
    atomic_store(&clock_state.elapsed,0x30000); assert(!port_clock_test_input(&clock_state));
    atomic_store(&clock_state.elapsed,0x30100); assert(port_clock_test_input(&clock_state));
    fresh(); port_clock_add(&clock_state,remove_next); port_clock_add(&clock_state,b);
    port_clock_add(&clock_state,c); step(1);
    assert(event_count==2 && events[0]=='R' && events[1]=='C');
    fresh(); port_clock_add(&clock_state,add_next); step(1);
    assert(event_count==2 && events[0]=='+' && events[1]=='C');
    fresh(); port_clock_add(&clock_state,nested); step(1);
    assert(event_count==1 && clock_state.counter==2 && clock_state.dispatched_tick==2);
    fresh(); for (unsigned i=0;i<5;++i) assert(port_clock_add(&clock_state,a));
    assert(!port_clock_add(&clock_state,b));
    fresh(); port_clock_advance(&clock_state,7); port_clock_setup(&clock_state);
    port_clock_dispatch(&clock_state,&pushed);
    assert(clock_state.counter==0 && clock_state.dispatched_tick==7);
    assert(atomic_load(&clock_state.elapsed)==1);
    port_clock_set_copy(&clock_state,5);
    uint32_t private_deadline=port_clock_counter_deadline(&clock_state,2);
    step(2);
    assert(port_clock_counter_reached(&clock_state,private_deadline));
    assert(!port_clock_test_copy(&clock_state));
    fresh(); atomic_store(&clock_state.raw_tick,UINT64_C(0xfffffffe));
    clock_state.dispatched_tick=UINT64_C(0xfffffffe);
    step(3);
    assert(atomic_load(&clock_state.raw_tick)==UINT64_C(0x100000001));
    assert(clock_state.dispatched_tick==UINT64_C(0x100000001) && clock_state.counter==3);
    for (unsigned rate=10;rate<=20;rate+=10) {
        fresh(); simulation_calls=0; g_clocks=0; timeraud=frmcs_time=(int16_t)(100/rate);
        port_clock_add(&clock_state,gameint);
        for (unsigned tick=1;tick<=100;++tick) {
            step(1); assert(simulation_calls==tick/(100/rate));
        }
        assert(g_clocks==(int)rate);
        inrepflg=1; gm_playmode=2; step(10); assert(simulation_calls==rate);
        inrepflg=0; gm_playmode=0;
    }
    puts("manual clock and accepted gameint schedules passed");
    return 0;
}
'''


class ManualTickClockTests(unittest.TestCase):
    def test_manual_clock_and_original_gameint_schedules(self):
        compiler = host_compiler()
        self.assertIsNotNone(compiler)
        gameint = extract_generated_function((ROOT / "game/race.c").read_text(),
                                             r"^void far gameint\(void\)")
        environment = os.environ.copy()
        environment["PATH"] = str(Path(compiler).parent) + os.pathsep + environment.get("PATH", "")
        with tempfile.TemporaryDirectory(prefix="manual-clock-") as temp:
            source, exe = Path(temp) / "probe.c", Path(temp) / "probe.exe"
            source.write_text(PROBE.replace("GAMEINT", gameint.replace(" far ", " ")))
            run = subprocess.run([compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-I", str(ROOT / "port"), str(source), str(ROOT / "port/tick_clock.c"),
                "-o", str(exe)], env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
            run = subprocess.run([str(exe)], env=environment, capture_output=True, text=True, timeout=5)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)


if __name__ == "__main__":
    unittest.main()
