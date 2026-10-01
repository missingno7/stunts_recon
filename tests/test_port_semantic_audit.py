"""The semantic report and hard contracts use freshly prepared SDL3 views."""
import json
import os
from pathlib import Path
import subprocess
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools/porting"))
from semantic_audit import check_contracts, registry_checks, scoped_claims
from semantic_detectors import (scan_spans, function_rows, source_globals,
                                generalized_span_candidates)


class PortSemanticAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.work = ROOT / "build/porting/semantic-audit"
        env = os.environ.copy()
        ran = subprocess.run([sys.executable, str(ROOT / "tools/porting/semantic_audit.py")],
                             env=env, capture_output=True, text=True, timeout=240)
        if ran.returncode:
            raise AssertionError(ran.stdout + ran.stderr)
        cls.report = json.loads((cls.work / "report.json").read_text())
        cls.rows = json.loads((cls.work / "compile-report.json").read_text())["sources"]

    def test_fresh_complete_inventory_and_scoped_evidence(self):
        report = self.report
        self.assertEqual(report["status"], "PASS", report["errors"])
        self.assertEqual(report["summary"]["historical_c_sources"], 38)
        self.assertEqual(report["summary"]["historical_functions"], 622)
        self.assertGreaterEqual(report["summary"]["portable_c_sources"], 69)
        self.assertTrue(all(row["status"] == "pass" for row in report["compiler_contracts"]))
        self.assertGreater(report["summary"]["compiler_assertions"], 600)
        self.assertIn("compile-only", report["summary"]["coverage"])
        self.assertIn("unverified/high-risk", report["summary"]["coverage"])
        matrix = next(r for r in report["functions"] if r["id"] == "port/matrix.c::mat_vec")
        self.assertIn("matrix-mat-vec", matrix["evidence"])
        self.assertIn("differential-oracle", matrix["coverage"])
        self.assertTrue(all(c["scope_level"] == "direct-routine-cases" for c in matrix["evidence_claims"]))
        transform = next(r for r in report["functions"] if r.get("historical_name") == "trans_op")
        self.assertNotIn("differential-oracle", transform["coverage"])
        self.assertEqual({c["scope_level"] for c in transform["evidence_claims"]},
                         {"composite-path-only", "expression-window-only"})
        wheel = [f for f in report["span_findings"] if f["class"] == "cross_global_pointer_span"]
        self.assertEqual(len(wheel), 4)
        self.assertEqual({f["required_bytes"] for f in wheel}, {144, 12})
        self.assertTrue(all(f["status"] == "contract_covered" for f in wheel))

    def test_old_cross_global_representation_is_rejected(self):
        row = next(r for r in self.rows if r["source"] == "src/obj_seg004.c")
        file = Path(row["overlay"])
        saved = file.read_bytes()
        try:
            file.write_bytes(saved.replace(b"source = port_wheel_source;", b"source = pts_set;"))
            findings = scan_spans(ROOT, self.rows)
            self.assertTrue(any(f.get("status") == "fail" and
                                "wheel_update" in f.get("message", "") for f in findings))
        finally:
            file.write_bytes(saved)

    def test_changed_scalar_signedness_and_sentinel_extent_fail_compiler_contracts(self):
        gcc = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        row = next(r for r in self.rows if r["source"] == "src/obj_seg006.c")
        file = Path(row["overlay"])
        saved = file.read_bytes()
        try:
            file.write_bytes(saved.replace(b"poly_link_list[POLYINFO_CAPACITY + 1]",
                                           b"poly_link_list[POLYINFO_CAPACITY]"))
            result = check_contracts([row], self.work, gcc)
            self.assertEqual(result[0]["status"], "fail")
            self.assertTrue(any("polygon sentinel" in e for e in result[0]["errors"]))
            file.write_bytes(saved + b"\n#undef I16\n#define I16 uint16_t\n")
            result = check_contracts([row], self.work, gcc)
            self.assertEqual(result[0]["status"], "fail")
            self.assertTrue(any("signed DOS scalars" in e for e in result[0]["errors"]))
        finally:
            file.write_bytes(saved)
            check_contracts([row], self.work, gcc)

    def test_unknown_global_span_is_reported_without_being_called_proven(self):
        text = '''struct VECTOR { short x, y, z; };
struct VECTOR points[6];
void consume(struct VECTOR *p) { int i; for (i=0;i<24;++i) p[i].x=0; }
void caller(void) { consume(points); }
'''
        item = {"text": text, "functions": function_rows("probe.c", text),
                "globals": source_globals("probe.c", text)}
        findings = generalized_span_candidates({"probe.c": item})
        span = next(f for f in findings if f["class"] == "caller_pointer_span_exceeds_named_extent")
        self.assertEqual((span["remaining_object_bytes"], span["required_bytes"]), (36, 144))
        self.assertEqual(span["status"], "heuristic_candidate")
        self.assertIsNone(span["registered_contract"])
        reused = '''short scratch[20000], names[82];
void reuse(void) { int i;
for (i=0;i<20000;++i) scratch[i]=0;
for (i=0;i<82;++i) names[i]=0;
}'''
        item = {"text": reused, "functions": function_rows("reuse.c", reused),
                "globals": source_globals("reuse.c", reused)}
        self.assertFalse(any(f.get("global") == "names" for f in
                             generalized_span_candidates({"reuse.c": item})))

    def test_registry_rejects_dangling_evidence_and_duplicate_contracts(self):
        fake = {"id": "missing", "scope": "bounded", "observables": ["memory"],
                "tests": ["tests/missing_test.py"]}
        errors = registry_checks({"contracts": [fake, fake]})
        self.assertTrue(any("duplicate" in e for e in errors))
        self.assertTrue(any("missing evidence" in e for e in errors))
        fake["tests"] = ["tests/test_port_semantic_audit.py::missing_method"]
        self.assertTrue(any("missing evidence method" in e for e in registry_checks({"contracts": [fake]})))
        self.assertEqual(scoped_claims({"same_name": [{"source_scope": ["src/obj_seg003.c"]}]},
            "same_name", ["src/obj_seg003_prefix.c"]), [])


if __name__ == "__main__":
    unittest.main()
