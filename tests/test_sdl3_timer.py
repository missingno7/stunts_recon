"""Compile and exercise timer IRQ delivery and recovered deadline behavior."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

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


PROBE = r'''
#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
volatile uint16_t input_pushed;
static jmp_buf escape;
static int stop_requested, callback_count, nested_once, cancelled;
static uint64_t input_tick;
void port_trace_timer_tick(uint64_t n,uint64_t due,uint64_t host){(void)n;(void)due;(void)host;}
void port_guest_set_exit_status(int status){assert(status==1);}
void port_guest_unwind(const char *s){(void)s;assert(0);}
void port_input_script_pump(uint64_t ns){input_tick=ns/PORT_TIMER_PERIOD_NS;}
void port_input_apply_queued(uint64_t n){assert(input_tick==n);}
#include "TIMER_SOURCE"
void port_guest_check_stop(void){if(stop_requested)longjmp(escape,1);port_timer_pump();}
void port_guest_request_stop(const char *s){(void)s;stop_requested=1;port_timer_cancel();}
static void callback(void){
 assert(input_tick==port_timer_raw_tick()); ++callback_count;
 if(!nested_once){nested_once=1;timedwait(1);}
}
static int guest(void *data){
 (void)data;port_timer_guest_begin();
 if(setjmp(escape)==0){
  timer_setup_interrupt(); addtimer(callback);
  if(cancelled)timedwait(1000);
  else{
   timedwait(2); assert(gettick()==2 && callback_count==1);
   waitbios(1); assert(port_timer_tick_count()==1 && gettick()==5);
   setticks(2); waitticks(); assert(gettick()==7 && testticks());
  }
 }else assert(cancelled);
 port_timer_guest_finish();return 0;
}
int main(void){
 assert(SDL_Init(0));port_timer_start();
 SDL_Thread *g=SDL_CreateThread(guest,"handshake-guest",NULL);assert(g);
 unsigned grants=0;
 while(port_timer_host_wait()){
  assert(port_timer_raw_tick()==grants);
  port_timer_pump(); /* foreign thread cannot dispatch */
  port_guest_notify_activity(); /* input notification alone cannot create time */
  assert(port_timer_raw_tick()==grants);
  assert(port_timer_host_grant()); ++grants;
 }
 SDL_WaitThread(g,NULL); assert(grants==7 && callback_count==6);
 port_timer_stop();
 cancelled=1;stop_requested=0;nested_once=1;port_timer_start();
 g=SDL_CreateThread(guest,"cancel-guest",NULL);assert(g);
 assert(port_timer_host_wait());assert(port_timer_host_grant());
 assert(port_timer_host_wait());port_guest_request_stop("stop at predicate");
 SDL_WaitThread(g,NULL);port_timer_stop();SDL_Quit();
 puts("SDL3 timer adapter checks passed");return 0;
}
'''


class SDL3TimerTests(unittest.TestCase):
    def test_irq_dispatch_and_deadlines(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the SDL3 host checks")
        if not (SDL_ROOT / "include" / "SDL3" / "SDL.h").is_file():
            self.skipTest("SDL3 headers are not provisioned")
        if not (SDL_ROOT / "lib" / "libSDL3.dll.a").is_file():
            self.skipTest("SDL3 import library is not provisioned")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-timer-") as directory:
            executable = Path(directory) / "sdl3-timer-probe.exe"
            source = Path(directory) / "probe.c"
            source.write_text(PROBE.replace("TIMER_SOURCE", (ROOT / "port/timer.c").as_posix()))
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-Wno-unused-parameter",
                "-I", str(SDL_ROOT / "include"), "-I", str(ROOT / "port"),
                str(source), str(ROOT / "port/tick_clock.c"),
                "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(executable),
            ]
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join([
                str(Path(compiler).resolve().parent), str(SDL_ROOT / "bin"),
                environment.get("PATH", ""),
            ])
            result = subprocess.run(command, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False,
                                    timeout=10)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("SDL3 timer adapter checks passed", result.stdout)


if __name__ == "__main__":
    unittest.main()
