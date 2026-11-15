"""Explicit startup policy, entropy replay and preserved recorded-rate precedence."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from tests.test_sdl3_driving import ROOT,EXECUTABLE,ASSETS,assert_build_ready,write_driving_input
from tests.test_sdl3_diagnostics import compiler_path,make_test_environment,SDL_ROOT

class StartupSettingsTests(unittest.TestCase):
    def setUp(self):
        assert_build_ready()
        work=ROOT / "build/workers/portclock";work.mkdir(parents=True,exist_ok=True)
        self.temp=tempfile.TemporaryDirectory(prefix="startup-settings-",dir=work)
        self.addCleanup(self.temp.cleanup);self.root=Path(self.temp.name)
        self.env=make_test_environment(compiler_path())
    def session(self,label,extra=(),config=None,ticks=900):
        folder=self.root / label;folder.mkdir()
        settings=folder / "config.json"
        if config is not None:settings.write_text(json.dumps(config))
        script=folder / "input.json";write_driving_input(script)
        p=subprocess.run([str(EXECUTABLE),f"--assets={ASSETS}",f"--config={settings}",
            f"--input-script={script}",f"--trace={folder / 'trace.jsonl'}","--test-auto-protection",
            "--clock=manual",f"--run-ticks={ticks}","--audio-output=none",*extra],
            env=self.env,cwd=ROOT,capture_output=True,text=True,timeout=45)
        self.assertEqual(p.returncode,0,p.stdout+p.stderr)
        events=list(map(json.loads,(folder / "trace.jsonl").read_text().splitlines()))
        return next(e for e in events if e.get("event_type")=="startup_settings"),events
    def test_json_cli_seed_repeat_and_live_10_hz(self):
        config={"frame_rate":20,"detail_level":3,"drawing_path":"full"}
        args=("--seed=0x12345678","--frame-rate=10","--detail-level=0","--drawing-path=dirty_rects")
        header,a=self.session("a",args,config)
        _,b=self.session("b",args,config)
        self.assertEqual((header["seed"],header["seed_source"],header["frame_rate"],header["detail_level"],header["drawing_path"]),
            ("0000000012345678","explicit",10,0,"dirty_rects"))
        states=lambda es:[(e["raw_tick_id"],e["bytes_hex"]) for e in es if e.get("event_type")=="state_snapshot"]
        self.assertTrue(states(a));self.assertEqual(states(a),states(b))
        live=[e for e in a if e.get("event_type")=="simulation_step" and e.get("game_mode")==0 and e.get("game_inputmode")==1 and e["raw_tick_id"]>=600]
        self.assertGreater(len(live),20);self.assertEqual({e["rate_target_hz"] for e in live},{10})
        self.assertTrue(all(y["raw_tick_id"]-x["raw_tick_id"]==10 for x,y in zip(live,live[1:])))
        _,c=self.session("different",("--seed=987654321","--frame-rate=10"),ticks=900)
        self.assertNotEqual(states(a),states(c))
        h,_=self.session("json",("--seed=1",),config,ticks=0)
        self.assertEqual((h["frame_rate"],h["detail_level"],h["drawing_path"]),(20,3,"full"))
    def test_default_seed_uses_os_entropy(self):
        a,_=self.session("os-a",ticks=0);b,_=self.session("os-b",ticks=0)
        self.assertEqual(a["seed_source"],"os");self.assertEqual(b["seed_source"],"os")
        self.assertNotEqual(a["seed"],b["seed"])
    def test_config_validation_and_preservation(self):
        probe=self.root / "config.c";exe=self.root / "config.exe"
        probe.write_text('#include "port_runtime.h"\n#include <stdio.h>\nint main(int n,char **v){int ok=port_config_load(v[1]);printf("%d %d %d %d",ok,port_config_frame_rate(),port_config_detail_level(),port_config_dirty_rects());return 0;}')
        compiler=compiler_path()
        p=subprocess.run([compiler,"-std=c11","-I",str(ROOT / "port"),"-I",str(SDL_ROOT / "include"),str(probe),str(ROOT / "port/config.c"),"-L",str(SDL_ROOT / "lib"),"-lSDL3","-o",str(exe)],env=self.env,capture_output=True,text=True,timeout=30)
        self.assertEqual(p.returncode,0,p.stdout+p.stderr)
        for data,expected in [('{"frame_rate":10,"detail_level":4,"drawing_path":"full","future":true}',"1 10 4 0"),
            ('{}',"1 20 0 1"),('{"frame_rate":12}',"0 20 0 1"),('{"detail_level":5}',"0 20 0 1"),
            ('{"detail_level":1.5}',"0 20 0 1"),('{"drawing_path":"bad"}',"0 20 0 1"),
            ('{"frame_rate":10,"frame_rate":20}',"0 20 0 1"),('{"nested":{"frame_rate":10}}',"1 20 0 1")]:
            path=self.root / "settings.json";path.write_text(data)
            p=subprocess.run([str(exe),str(path)],env=self.env,capture_output=True,text=True,timeout=5)
            self.assertEqual(p.returncode,0,p.stderr);self.assertEqual(p.stdout,expected)
            self.assertEqual(path.read_text(),data)
    def test_recorded_replay_rate_keeps_precedence(self):
        # Open Options -> Load Replay -> shipped DEFAULT.RPL explicitly. Keep
        # startup at 10 Hz, while the independently supplied file records 20 Hz.
        folder=self.root / "replay";folder.mkdir();script=folder / "input.json"
        events=[{"visible_tick":ms*1000000,"channel":"dos.keyboard.scancodes","payload":codes}
            # A real original INT9 has one pending key word. Three menu moves
            # must allow three polls; a batch of three makes retains only one.
            for ms,codes in [(1500,[28,156]),(4500,[80,208]),(4600,[80,208]),(4700,[80,208]),
                (5500,[28,156]),(6500,[28,156])]]
        events += [{"visible_tick":ms*1000000,"channel":"dos.mouse.normalized",
            "payload":{"u":x/320,"v":y/200,"buttons":buttons}}
            for ms,x,y,buttons in [(2500,285,140,1),(2650,285,140,0),(2800,0,0,0),
                (3500,285,140,1),(3650,285,140,0),(3800,0,0,0)]]
        events.sort(key=lambda e:e["visible_tick"])
        script.write_text(json.dumps({"format":"stunts-exact-input-script-v1","anchor_tick":0,"events":events}))
        trace=folder / "trace.jsonl"
        p=subprocess.run([str(EXECUTABLE),f"--assets={ASSETS}",f"--input-script={script}",
            f"--trace={trace}",f"--config={folder / 'config.json'}","--seed=1","--frame-rate=10",
            "--audio=ad15","--audio-output=none","--clock=manual","--run-ticks=1600"],env=self.env,cwd=ROOT,capture_output=True,text=True,timeout=45)
        self.assertEqual(p.returncode,0,p.stdout+p.stderr)
        events=list(map(json.loads,trace.read_text().splitlines()))
        replay=[e for e in events if e.get("event_type")=="simulation_step" and e.get("game_mode")==2]
        self.assertTrue(replay,"file dialog must load the shipped recorded replay")
        self.assertEqual({e["rate_target_hz"] for e in replay},{20})

if __name__=="__main__":unittest.main()
