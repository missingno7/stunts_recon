import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MIGRATION = ROOT / "docs" / "porting" / "asm-migration.json"
MANIFEST = ROOT / "layout" / "manifest.json"
ASM_OWNER_KINDS = {"MATCHING_ASM", "KNOWN_TOOLCHAIN_LIBRARY"}
ASM_DATA_KINDS = {"MATCHING_ASM_DATA", "KNOWN_TOOLCHAIN_LIBRARY_DATA"}


class AsmMigrationCompletenessTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.migration = json.loads(MIGRATION.read_text(encoding="utf-8"))
        cls.manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))

    def test_each_manifest_asm_owner_and_pinned_runtime_member_has_a_row(self):
        code_rows = {row["owner_id"] for row in self.migration["owners"]}
        data_rows = {row["owner_id"] for row in self.migration["data_owners"]}
        missing = []
        for row in self.manifest["owners"]:
            if row["kind"] in ASM_OWNER_KINDS and row["id"] not in code_rows:
                missing.append(row["id"])
            if row["kind"] in ASM_DATA_KINDS and row["id"] not in data_rows:
                missing.append(row["id"])
        runtime_members = {row["member_id"] for row in self.migration["runtime_data_members"]}
        for row in self.manifest.get("runtime_data_members", []):
            if row["id"] not in runtime_members:
                missing.append(row["id"])
        self.assertEqual(missing, [])

    def test_public_routine_rows_are_owned_and_categorized(self):
        owners = {row["owner_id"] for row in self.migration["owners"]}
        allowed = set(self.migration["category_values"])
        keys = [(row["owner_id"], row["public_symbol"]) for row in self.migration["routines"]]
        self.assertEqual(len(keys), len(set(keys)), "duplicate owner/public-symbol row")
        for row in self.migration["routines"]:
            self.assertIn(row["owner_id"], owners)
            self.assertIn(row["category"], allowed)
            for field in ("semantics", "platform_dependency", "faithful_port_replacement",
                          "translation_before_first_link", "confidence", "evidence"):
                self.assertTrue(row.get(field), f"{row['public_symbol']} lacks {field}")


if __name__ == "__main__":
    unittest.main()
