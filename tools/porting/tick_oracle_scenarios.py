"""Write shared input-only fixtures for original/manual-port tick comparisons.

Uses existing screen/driving/replay scripts, with an explicit protection answer
for the captured startup-seed fixture instead of either runtime's test injector.
"""
import argparse
import json
from pathlib import Path
import sys

ROOT=Path(__file__).resolve().parents[2]
LIMITS={"startup":1000,"selection":2200,"race":1800,"replay":2550}


def write_scenario(name,path):
    path.parent.mkdir(parents=True,exist_ok=True)
    sys.path.insert(0,str(ROOT))
    from tests.sdl3_screen_regression import write_startup_input
    from tests.test_sdl3_selection_frames import write_input
    from tests.test_sdl3_driving import write_driving_input
    from tests.test_sdl3_replay_playback import write_replay_input
    {"startup":write_startup_input,"selection":write_input,"race":write_driving_input,
     "replay":write_replay_input}[name](path)
    script=json.loads(path.read_text(encoding="utf-8"))
    script['format']='stunts-exact-input-script-v1'
    if name in ("race","replay"):
        # The independent startup-seed capture derives question a03: previewing.
        # Pace characters after the original editor's buffer clear; both runs
        # receive these exact make/break bytes, without adaptive input injection.
        for i,scan in enumerate((25,19,18,47,23,18,17,23,49,34,28)):
            script["events"].append(dict(visible_tick=(500+2*i)*10000154,
                channel="dos.keyboard.scancodes",payload=[scan,scan|128]))
    script["events"]=[e for e in script["events"] if e["visible_tick"]<=LIMITS[name]*10000154]
    script["events"].sort(key=lambda e:e["visible_tick"])
    # Preserve the exact script bytes in the sealed DOSBox capture receipts.
    path.write_bytes((json.dumps(script,indent=2)+"\n").replace("\n","\r\n").encode('utf-8'))
    return LIMITS[name]


if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("scenario",choices=LIMITS)
    p.add_argument("output",type=Path)
    a=p.parse_args()
    print(write_scenario(a.scenario,a.output))
