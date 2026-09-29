import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "porting"))

import normalize_portforge_trace  # noqa: E402


class RuntimeTraceNormalizerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scratch = ROOT / "build" / "workers" / "integ55"
        cls.scratch.mkdir(parents=True, exist_ok=True)

    def test_fixture_stream_normalizes_ticks_phases_and_frame_deltas(self):
        source = ROOT / "tests" / "fixtures" / "runtime_trace_excerpt.jsonl"
        with tempfile.TemporaryDirectory(dir=self.scratch) as temp_dir:
            output = Path(temp_dir) / "arbitrary" / "normalized.jsonl"
            count = normalize_portforge_trace.normalize_file(source, output)
            rows = [json.loads(line) for line in output.read_text(encoding="utf-8").splitlines()]

        self.assertEqual(count, 5)
        self.assertEqual(rows[0]["tick_id"], 1)
        self.assertEqual(rows[0]["machine_tick"], 1000)
        self.assertEqual(rows[1]["timer_interval_ticks"], 10_000_153)
        self.assertEqual(rows[1]["game_frame_delta_since_prev_timer"], 0)
        self.assertEqual(rows[1]["rate_target_hz"], 20)
        self.assertEqual(rows[1]["game_mode_pair_raw"], "input1_replay0")

        self.assertEqual(rows[2]["frame_id"], 1)
        self.assertEqual(rows[2]["video_phase"], "frame_start")
        self.assertEqual(rows[2]["machine_tick"], 10_001_154)
        self.assertEqual(rows[3]["game_frame_delta_since_prev_same_phase"], 1)
        self.assertEqual(rows[3]["game_mode_pair_raw"], "input1_replay0")

        self.assertEqual(rows[4]["event_type"], "audio_publication")
        self.assertEqual(rows[4]["audio_frame_count"], 321)
        self.assertTrue(all(row["raw_player_tail_byte"] == 173 for row in rows))
        self.assertFalse(any("sim_step_id" in row for row in rows))

    def test_rejects_in_place_output(self):
        source = ROOT / "tests" / "fixtures" / "runtime_trace_excerpt.jsonl"
        with self.assertRaisesRegex(ValueError, "paths must differ"):
            normalize_portforge_trace.normalize_file(source, source)

    def test_reports_malformed_json_line(self):
        with tempfile.TemporaryDirectory(dir=self.scratch) as temp_dir:
            source = Path(temp_dir) / "bad.jsonl"
            source.write_text('{"trace_schema":"stunts-runtime-trace-v1","event_type":"header"}\nnot-json\n',
                              encoding="utf-8")
            with self.assertRaisesRegex(ValueError, r"bad.jsonl:2: invalid JSON"):
                with (Path(temp_dir) / "out").open("w", encoding="utf-8") as output:
                    normalize_portforge_trace.normalize_stream(source, output)


if __name__ == "__main__":
    unittest.main()
