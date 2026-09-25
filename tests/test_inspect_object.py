import importlib.util
import ast
import struct
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = next(parent for parent in Path(__file__).resolve().parents
            if (parent / "tools/common.py").is_file())
sys.path.insert(0, str(ROOT / "tools"))
MODULE_PATH = Path(__file__).with_name("inspect_object.py")
if not MODULE_PATH.is_file():
    MODULE_PATH = ROOT / "tools/inspect_object.py"
spec = importlib.util.spec_from_file_location("inspect_object", MODULE_PATH)
inspect_object = importlib.util.module_from_spec(spec)
spec.loader.exec_module(inspect_object)
from object_probe import read_object


def record(kind, body):
    prefix = bytes([kind]) + struct.pack("<H", len(body) + 1) + body
    return prefix + bytes([-sum(prefix) & 0xFF])


def research_fixture(include_b4=True, include_b6=True, include_fixup=True,
                     duplicate_segdef=False, declared_length=3):
    code = b"\x00\x00\xc3"
    # LNAMES: UNIT_TEXT and CODE; SEGDEF index 1, length 3.
    names = record(0x96, b"\x09UNIT_TEXT\x04CODE")
    segdef = record(0x98, b"\x48" + struct.pack("<H", declared_length) + b"\x01\x02\x00")
    # A local external, plus an external-target offset16 fixup at code offset 0.
    lextdef = record(0xB4, b"\x07_helper\x00")
    fixupp = record(0x9C, b"\x84\x00\x46\x01")
    # A local public at offset zero, with type index zero.
    lpubdef = record(0xB6, b"\x00\x01\x06_local\x00\x00\x00")
    data = record(0xA0, b"\x01\x00\x00" + code)
    parts = [record(0x80, b"\x01x"), names, segdef]
    if duplicate_segdef:
        parts.append(record(0x98, b"\x48" + struct.pack("<H", declared_length) + b"\x01\x02\x00"))
    if include_b4:
        parts.append(lextdef)
    if include_b6:
        parts.append(lpubdef)
    parts.append(data)
    if include_fixup:
        parts.append(fixupp)
    parts.append(record(0x8A, b"\x00"))
    return b"".join(parts), code


class InspectObjectTests(unittest.TestCase):
    def test_unresolved_b4_is_rejected_but_research_evidence_is_preserved(self):
        raw, target = research_fixture()
        with self.assertRaisesRegex(ValueError, "Unresolved local OMF external"):
            read_object(raw)
        b6_only, _ = research_fixture(include_b4=False, include_fixup=False)
        self.assertEqual(read_object(b6_only).local_publics,
                         [{'name':'_local','segment':'UNIT_TEXT','offset':0}])
        rows = inspect_object.inventory_records(raw)
        self.assertEqual([r["name"] for r in rows if r["name"] in ("LEXTDEF", "LPUBDEF")],
                         ["LEXTDEF", "LPUBDEF"])
        self.assertTrue(all(r["body_hex"] for r in rows if r["name"] in ("LEXTDEF", "LPUBDEF")))
        private = ROOT / "build/private"
        private.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=private) as out:
            report = inspect_object.inspect_bytes(raw, target, {"work_directory": "unused"},
                                                  Path(out), "UNIT_TEXT")
        self.assertEqual(report["authority"], "RESEARCH_ONLY")
        self.assertEqual(report["strict_object_probe"]["status"], "REJECTED")
        self.assertEqual(report["segment_lengths"], {"UNIT_TEXT": 3})
        self.assertEqual(report["complete_contribution"]["size"], 3)
        self.assertEqual(len(report["fixups"]), 1)
        self.assertEqual(report["fixups"][0]["target"], "_helper")
        self.assertFalse(report["bindable_payload_available"])
        self.assertNotIn("payload", report)
        self.assertEqual(report["acceptance_status"], "NOT_EVALUATED")
        self.assertEqual(report["promotion_status"], "NOT_PROMOTED")

    def test_malformed_checksum_and_unknown_record_fail_closed(self):
        raw, _ = research_fixture()
        malformed = raw[:-1] + bytes([raw[-1] ^ 1])
        with self.assertRaisesRegex(ValueError, "checksum"):
            inspect_object.inventory_records(malformed)
        unknown = record(0x99, b"x")
        data = record(0x80, b"\x01x") + unknown + record(0x8A, b"\x00")
        with self.assertRaisesRegex(ValueError, "Unsupported OMF record"):
            inspect_object.inventory_records(data)

    def test_duplicate_segdef_name_is_rejected(self):
        raw, target = research_fixture(duplicate_segdef=True)
        with tempfile.TemporaryDirectory(dir=ROOT / "build/private") as out:
            with self.assertRaisesRegex(ValueError, "Duplicate SEGDEF names"):
                inspect_object.inspect_bytes(raw, target, {}, Path(out), "UNIT_TEXT")

    def test_incomplete_initialized_extent_is_rejected(self):
        raw, target = research_fixture(declared_length=4, include_fixup=False)
        with tempfile.TemporaryDirectory(dir=ROOT / "build/private") as out:
            with self.assertRaisesRegex(ValueError, "does not equal its declared"):
                inspect_object.inspect_bytes(raw, target, {}, Path(out), "UNIT_TEXT")

    def test_output_must_be_private_and_no_acceptance_hook_is_imported(self):
        with self.assertRaisesRegex(ValueError, "build/private"):
            inspect_object.inspect_bytes(b"", b"", {}, ROOT / "build")
        tree = ast.parse(MODULE_PATH.read_text(encoding="utf-8"))
        imported = {alias.name for node in ast.walk(tree)
                    if isinstance(node, ast.Import) for alias in node.names}
        imported.update(node.module for node in ast.walk(tree)
                        if isinstance(node, ast.ImportFrom) and node.module)
        self.assertFalse(imported & {"build_exact", "promote", "transaction"})


if __name__ == "__main__":
    unittest.main()
