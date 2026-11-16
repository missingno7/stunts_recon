"""Review index completeness and honest finite coverage classifications."""
import hashlib
import json
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]

class PortLayerInventoryTests(unittest.TestCase):
    def test_join_keeps_all_existing_entries_and_unexercised_debt(self):
        inventory=json.loads((ROOT/'tools/porting/port-layer.json').read_text());rows=inventory['rows']
        self.assertEqual(len({r['id'] for r in rows}),len(rows))
        asm=json.loads((ROOT/'docs/porting/asm-migration.json').read_text())['routines']
        host=json.loads((ROOT/'docs/porting/platform-boundary.json').read_text())['entries']
        contracts=json.loads((ROOT/'port/semantic-contracts.json').read_text())['contracts']
        self.assertEqual({r['id'] for r in rows if r['kind']=='asm'},
            {'asm:'+r['owner_id']+'::'+r['public_symbol'] for r in asm})
        self.assertEqual(len([r for r in rows if r['kind']=='host']),len(host))
        self.assertEqual({r['name'] for r in rows if r['kind']=='semantic_contract'},{c['id'] for c in contracts})
        for path,expected in inventory['inputs'].items():
            self.assertEqual(hashlib.sha256((ROOT/path).read_bytes()).hexdigest(),expected,path)
        for row in rows:
            self.assertIn(row['status'],('not exercised','exercised and agrees','exercised and differs'))
            if row['status']!='not exercised':
                self.assertTrue(row['evidence'] or row['prior_original_tests'],row['id'])
            else:
                self.assertIn('runtime_entry',row['reachability'],row['id'])
                self.assertTrue(row['reachability']['scenario'],row['id'])
                self.assertTrue(row['reachability']['reason'],row['id'])
        # The race is a bounded first slice, not fabricated whole-port coverage.
        self.assertTrue(any(r['status']=='not exercised' and r['kind']=='asm' for r in rows))
        self.assertTrue(any(r['status']=='exercised and differs' for r in rows))
        self.assertEqual(len(inventory['summary']['reconstructed_asm_modules']),40)

if __name__=='__main__':unittest.main()
