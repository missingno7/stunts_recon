#!/usr/bin/env python3
"""Re-record the two controlled menu branches used by replay frame fixtures.

Only mouse hold events at occurrences 264 and 270 change. The v14 original
interpreter records a new ArtifactV2 and checks it against its bound base.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess

from replay_oracle_probe import POINT, SCHEMA_V14, choose_runner, resolve_bound_base, sha256


TARGETS = {"track-default": (0.68, 0.50),
           "opponent-clock": (0.125, 0.72)}
SOURCE_NAME = "rec_20260827_013930.pfreplay.json"
ORIGINAL_MOUSE = (0.5302083333333333, 0.7541666666666667)


def make_script(replay: dict, target: str) -> dict:
    if replay["environment"]["base_snapshot"]["canonical_schema"] != SCHEMA_V14:
        raise ValueError("branch source is not canonical v14")
    events = []
    edited = []
    for original in replay["events"]:
        at = original["at"]
        if at["occurrence"] >= 450:
            break
        event = {"occurrence": at["occurrence"], "sequence": len(events),
                 "channel": original["channel"], "payload": original["payload"].copy()}
        if (at["occurrence"] in (264, 270)
                and event["channel"] == "dos.mouse.normalized"):
            payload = event["payload"]
            if (payload["u"], payload["v"]) != ORIGINAL_MOUSE:
                raise ValueError("source mouse event differs from recorded Drive click")
            payload["u"], payload["v"] = TARGETS[target]
            edited.append(at["occurrence"])
        events.append(event)
    if edited != [264, 270]:
        raise ValueError("expected exactly the Drive mouse press and release")
    return {"format": "portforge-dos-input-script-v1", "point_id": POINT,
            "events": events}


def main(argv=None):
    raise RuntimeError("Legacy interpreter recording retired; use original_tick_oracle.py and tick_oracle_scenarios.py")


if __name__ == "__main__":
    raise SystemExit(main())
