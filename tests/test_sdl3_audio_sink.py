"""Game observables are independent of output device and callback size."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import unittest
from tests.test_sdl3_driving import ROOT,EXECUTABLE,ASSETS,STARTUP_SEED,assert_build_ready,write_driving_input

WORK=ROOT / "build/workers/portclock/sink-comparison"

class AudioSinkTests(unittest.TestCase):
    def test_ad15_and_pc15_match_real_dummy_null_and_two_buffers(self):
        assert_build_ready(); WORK.mkdir(parents=True,exist_ok=True)
        script=WORK / "input.json";write_driving_input(script)
        environment=os.environ.copy();environment["SDL_VIDEODRIVER"]="dummy"
        environment.pop("SDL_AUDIODRIVER",None)
        environment["PATH"]=os.pathsep.join((r"C:/msys64/mingw32/bin",r"C:/tools/sdl3-3.4.16-i686/bin",environment.get("PATH","")))
        report={}
        for driver in ("ad15","pc15"):
            results=[];devices={"real":[],"dummy":[]}
            for mode,buffer in [(m,b) for b in (512,2048) for m in ("null","real","dummy")]:
                folder=WORK / f"{driver}-{mode}-{buffer}";folder.mkdir(exist_ok=True)
                for old in folder.glob("frame-*.fbr"):old.unlink()
                env=environment.copy()
                # WASAPI fixes its physical period at 480 here. DirectSound permits
                # both requested real-device sizes; default WASAPI is also exercised
                # by the repeated sound test and the 30-second interactive checks.
                if mode!="null":env["SDL_AUDIODRIVER"]="dummy" if mode=="dummy" else "directsound"
                p=subprocess.run([str(EXECUTABLE),f"--assets={ASSETS}",f"--input-script={script}",
                    f"--test-startup-seed={STARTUP_SEED}","--seed=1","--test-auto-protection",
                    "--clock=manual","--run-ticks=1800",f"--audio={driver}",
                    "--audio-output="+("none" if mode=="null" else "device"),
                    f"--audio-buffer-frames={buffer}",f"--sound-requests={folder / 'sound.jsonl'}",
                    f"--trace={folder / 'trace.jsonl'}",f"--capture-dir={folder}",
                    f"--config={folder / 'config.json'}",f"--diagnostics-dir={folder / 'diagnostics'}"],
                    cwd=ROOT,env=env,capture_output=True,text=True,timeout=45)
                self.assertEqual(p.returncode,0,p.stdout+p.stderr)
                events=list(map(json.loads,(folder / "trace.jsonl").read_text().splitlines()))
                state=[(e["raw_tick_id"],e["bytes_hex"]) for e in events if e.get("event_type")=="state_snapshot"]
                self.assertTrue(state)
                frames=[]
                for e in events:
                    if e.get("event_type")!="video_publication":continue
                    path=folder / ("frame-%06d.fbr"%e["frame_id"])
                    if path.exists():frames.append((e["raw_tick_id"],e["frame_id"],hashlib.sha256(path.read_bytes()[20:]).hexdigest()))
                self.assertGreater(len(frames),100)
                sounds=(folder / "sound.jsonl").read_bytes()
                result=(hashlib.sha256(sounds).hexdigest(),state,frames)
                if results:self.assertEqual(result,results[0],f"{driver} {mode} buffer={buffer}")
                results.append(result)
                stats=[e for e in events if e.get("event_type")=="audio_sink"][-1]
                if mode!="null":
                    self.assertIn("output opened through SDL3",p.stderr)
                    self.assertGreater(stats["consumed_frames"],0)
                    devices[mode].append(stats["device_buffer_frames"])
                else:self.assertEqual(stats["consumed_frames"],0)
                report[f"{driver}-{mode}-{buffer}"]={"sound_sha256":result[0],"events":len(sounds.splitlines())-1,
                    "states":len(state),"frames":len(frames),"sink":stats}
            for mode,sizes in devices.items():self.assertNotEqual(sizes[0],sizes[1],f"{mode} ignored buffer requests")
        (WORK / "comparison.json").write_text(json.dumps(report,indent=2)+"\n")

if __name__=="__main__":unittest.main()
