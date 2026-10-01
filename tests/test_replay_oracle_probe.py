"""Synthetic checks included in the repository's unittest discovery gate."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest
import zlib

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools/porting"))
from replay_oracle_probe import SCHEMA_V13, SCHEMA_V14, choose_runner, resolve_bound_base
from replay_menu_branch import make_script
from replay_snapshot import MEMORY_BYTES, PAGE_BYTES, VGA_OFFSET, load_snapshot, png_preview


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def snapshot(folder: Path, mode: int, *, corrupt: bool = False) -> Path:
    path = folder / "sample.pfsnapshot"
    (path / "memory").mkdir(parents=True)
    page = bytes([7]) * PAGE_BYTES
    memory = bytearray(MEMORY_BYTES)
    memory[VGA_OFFSET:VGA_OFFSET + PAGE_BYTES] = page
    pages = {"page_size": PAGE_BYTES, "total_bytes": MEMORY_BYTES,
             "pages": [{"index": VGA_OFFSET // PAGE_BYTES, "offset": 0,
                        "sha256": digest(page)}]}
    palette = bytes([0, 0, 0]) * 7 + bytes([255, 0, 0]) + bytes([0, 0, 0]) * 248
    meta = {"fingerprints": {"memory_sha256": digest(memory)},
            "devices": {"video_mode": mode, "palette_hex": palette.hex()}}
    (path / "snapshot.json").write_text(json.dumps(meta))
    (path / "memory/pages.json").write_text(json.dumps(pages))
    (path / "memory/pages.bin").write_bytes(bytes(PAGE_BYTES) if corrupt else page)
    return path


class ReplayOracleProbeTest(unittest.TestCase):
    def test_mode13_extracts_guest_indices_and_dac6(self) -> None:
        with TemporaryDirectory() as tmp:
            frame = load_snapshot(snapshot(Path(tmp), 0x13))
        self.assertEqual(frame.indices, bytes([7]) * PAGE_BYTES + bytes(64000 - PAGE_BYTES))
        self.assertEqual(frame.dac6[21:24], bytes([63, 0, 0]))
        self.assertEqual(len(frame.payload), 64768)
        self.assertTrue(png_preview(frame).startswith(b"\x89PNG\r\n\x1a\n"))

    def test_text_mode_is_state_only(self) -> None:
        with TemporaryDirectory() as tmp:
            frame = load_snapshot(snapshot(Path(tmp), 3))
        self.assertIsNone(frame.indices)
        self.assertIsNone(frame.dac6)
        self.assertIsNone(frame.payload)
        with self.assertRaisesRegex(ValueError, "no mode 13h"):
            png_preview(frame)

    def test_snapshot_rejects_tampered_page_and_duplicate_index(self) -> None:
        with TemporaryDirectory() as tmp:
            path = snapshot(Path(tmp), 0x13, corrupt=True)
            with self.assertRaisesRegex(ValueError, "page .* SHA-256"):
                load_snapshot(path)
            (path / "memory/pages.bin").write_bytes(bytes([7]) * PAGE_BYTES)
            pages_file = path / "memory/pages.json"
            pages = json.loads(pages_file.read_text())
            pages["pages"].append(pages["pages"][0])
            pages_file.write_text(json.dumps(pages))
            with self.assertRaisesRegex(ValueError, "duplicate or overlapping"):
                load_snapshot(path)

    def test_runner_selection_uses_canonical_schema(self) -> None:
        with TemporaryDirectory() as tmp:
            root = Path(tmp) / "source"
            v13_tree = root / "port_forge/build/history/pf-391f1dd"
            v14_tree = root / "port_forge"
            for tree in (v13_tree, v14_tree):
                (tree / "config").mkdir(parents=True, exist_ok=True)
                (tree / "config/canonical-projections-v1.json").write_text("{}")
            v13 = Path(tmp) / "v13.exe"
            v14 = Path(tmp) / "v14.exe"
            v13.write_bytes(b"v13")
            v14.write_bytes(b"v14")
            self.assertEqual(choose_runner(SCHEMA_V13, root, Path(tmp), v13_runner=v13)[0], v13)
            self.assertEqual(choose_runner(SCHEMA_V14, root, Path(tmp), v14_runner=v14)[0], v14)
            with self.assertRaisesRegex(ValueError, "v13 replay needs"):
                choose_runner(SCHEMA_V13, root, Path(tmp))
            with self.assertRaisesRegex(ValueError, "unsupported canonical schema"):
                choose_runner("pf-canonical-rm-v15", root, Path(tmp))

    def test_bound_base_is_exact_named_sibling(self) -> None:
        with TemporaryDirectory() as tmp:
            root = Path(tmp) / "source"
            corpus = root / "artifacts/replays-v2"
            corpus.mkdir(parents=True)
            replay_file = corpus / "sample.pfreplay.json"
            replay_file.write_text("{}")
            bound = corpus / "sample.pfreplay.json.base.pfsnapshot"
            bound.mkdir()
            replay = {"environment": {"base_snapshot": {"file": bound.name}}}
            self.assertEqual(resolve_bound_base(root, replay_file, replay), bound)
            replay["environment"]["base_snapshot"]["file"] = "../wrong.pfsnapshot"
            with self.assertRaisesRegex(ValueError, "filename"):
                resolve_bound_base(root, replay_file, replay)

    def test_controlled_branch_changes_only_two_mouse_events(self) -> None:
        original = {"environment": {"base_snapshot": {"canonical_schema": SCHEMA_V14}},
                    "events": [{"at": {"occurrence": occurrence},
                                "channel": "dos.mouse.normalized",
                                "payload": {"buttons": button, "u": 0.5302083333333333,
                                            "v": 0.7541666666666667}}
                               for occurrence, button in ((264, 1), (270, 0), (450, 0))]}
        script = make_script(original, "track-default")
        self.assertEqual(len(script["events"]), 2)
        self.assertEqual([event["payload"]["buttons"] for event in script["events"]], [1, 0])
        self.assertTrue(all((event["payload"]["u"], event["payload"]["v"]) == (0.68, 0.5)
                            for event in script["events"]))
        self.assertEqual(original["events"][0]["payload"]["u"], 0.5302083333333333)

    def test_checked_in_original_replay_payloads_match_provenance(self) -> None:
        folder = Path(__file__).resolve().parent / "fixtures/sdl3/original_replay"
        rows = json.loads((folder / "manifest.json").read_text())
        self.assertEqual({row["label"] for row in rows}, {
            "main-menu", "car-countach", "car-lancia", "track-default",
            "opponent-clock", "race-driving", "race-result"})
        for row in rows:
            self.assertEqual(json.loads((folder / f"{row['label']}.json").read_text()), row)
            payload = zlib.decompress((folder / row["payload"]).read_bytes())
            self.assertEqual(len(payload), row["uncompressed_bytes"])
            self.assertEqual(len(payload), 64768)
            self.assertEqual(digest(payload[:64000]), row["indices_sha256"])
            self.assertEqual(digest(payload[64000:]), row["palette_rgb6_sha256"])
            self.assertEqual(digest(payload), row["payload_sha256"])
            self.assertEqual(row["sparse_mismatches"], 0)


if __name__ == "__main__":
    unittest.main()
