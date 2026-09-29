"""Smoke coverage for generated state-model artifacts."""
import importlib.util
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("porting_state_model", ROOT / "tools/porting/state_model.py")
state_model = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(state_model)


def fixture_model():
    return {
        "schema": "stunts-global-state-model-v1",
        "generated_by": "test",
        "authority": {},
        "access_analysis": {},
        "coverage": {
            "registry_data_symbols": 1,
            "complete_data_symbol_type_and_size": 1,
            "unresolved_data_symbols": 0,
            "symbols_with_unknown_unit": 0,
            "symbols_with_unknown_replay_class": 0,
            "symbols_marked_bit_exact_replay_state": 0,
            "symbols_marked_presentation_or_process_local": 1,
            "struct_tags": 1,
            "struct_variants": 1,
            "struct_variants_with_compiler_size": 1,
            "struct_types_with_global_or_embedded_addresses": 1,
            "struct_types_with_instance_dependent_replay_class": 0,
            "struct_types_without_registered_instances": 0,
            "active_source_files": 1,
            "supplemental_target_object_extents": 0,
        },
        "state_policy": {
            "replay_determinism": "fixture policy",
            "reconstructed_names": "fixture alias policy",
            "process_pointers": "fixture pointer policy",
            "unknown_units": "fixture unit policy",
            "scale_ledger": [{"fact": "one tick", "evidence": ["src/probe.c:1"]}],
            "subsystem_notes": [{"subsystem": "timer_platform", "summary": "timer fixture", "evidence": ["src/probe.c:1"]}],
        },
        "data_symbols": [{
            "name": "clock_ticks", "address": 256, "address_hex": "0x00100", "storage": "bss",
            "type": "unsigned short", "size_bytes": 2, "owner_subsystem": "timer_platform",
            "owner_objects": ["timer_probe"], "units": {"value": "ticks", "confidence": "exact"},
            "lifetime": {"value": "process", "confidence": "exact"},
            "replay_determinism": {"class": "timer_state", "preserve_bit_exactly": False},
            "readers": [], "writers": [], "evidence": [{"citation": "src/probe.c:1"}],
            "coverage": "complete",
        }],
        "supplemental_target_object_extents": [],
        "struct_types": [{
            "tag": "TIMERPROBE", "variants": [{
                "translation_unit": "timer_probe", "source": "src/probe.c", "line": 1,
                "size_bytes": 2, "size_status": "compiler-measured",
                "evidence": [{"citation": "src/probe.c:1"}, {"citation": "build/search/probe/report.json"}],
            }],
            "instances": [{"name": "clock_ticks", "address": 256}],
            "owner_subsystem": "timer_platform", "readers": [], "writers": [],
            "units": "ticks", "lifetime": "process",
            "replay_determinism": {"class": "timer_state", "preserve_bit_exactly": False},
            "field_semantics": [{"field": "tick", "unit": "ticks", "meaning": "callback count",
                                 "evidence": [{"citation": "src/probe.c:1"}]}],
        }],
    }


class StateModelSmokeTests(unittest.TestCase):
    def test_full_artifacts_emit_under_build(self):
        model = fixture_model()
        output = ROOT / "build/workers/integ55/state-model-smoke"
        md_path, json_path = state_model.emit_model(model, output)
        self.assertEqual(json.loads(json_path.read_text(encoding="utf-8")), model)
        markdown = md_path.read_text(encoding="utf-8")
        self.assertIn("clock_ticks", markdown)
        self.assertIn("TIMERPROBE", markdown)
        self.assertIn("Evidence-backed field units", markdown)

    def test_duplicate_registry_symbol_is_rejected(self):
        model = fixture_model()
        model["data_symbols"].append(dict(model["data_symbols"][0]))
        model["coverage"]["registry_data_symbols"] = 2
        with self.assertRaisesRegex(ValueError, "duplicate registered data symbols"):
            state_model.validate_model(model)


if __name__ == "__main__":
    unittest.main()
