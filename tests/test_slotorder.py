"""Accepted local homes and small collision/scope probes."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from slotorder import bucket, locals_from_source, predict, suggest_names


class SlotOrderTests(unittest.TestCase):
    def test_nineteen_accepted_addressable_homes(self):
        cases = json.loads((ROOT/'tests/fixtures/slotorder_accepted.json').read_text())
        checked = 0
        for case in cases:
            with self.subTest(function=case['function']):
                homes = {row['key']:row['bp_offset'] for row in predict(case['locals'])['locals']}
                for key, measured in case['measured'].items():
                    self.assertEqual(homes[key], measured)
                    checked += 1
        self.assertEqual(checked, 19)

    def test_collisions_reverse_and_nested_home_reuse(self):
        self.assertEqual(bucket('q'), bucket('a'))
        self.assertEqual([r['bp_offset'] for r in predict([{'name':'q'}, {'name':'a'}])['locals']],
                         [-4, -2])
        rows = [{'name':'outer'}, {'name':'first', 'block':['one']},
                {'name':'second', 'block':['two']}]
        homes = {r['name']:r['bp_offset'] for r in predict(rows)['locals']}
        self.assertEqual(homes['first'], homes['second'])
        self.assertNotEqual(homes['outer'], homes['first'])

    def test_source_cli_parser_and_names(self):
        rows = locals_from_source((ROOT/'src/copy_string.c').read_text(), 'copy_string')
        self.assertEqual([(r['name'], r['type']) for r in rows], [('current', 'char far *')])
        self.assertEqual(predict(rows)['locals'][0]['bp_offset'], -4)
        facing = locals_from_source((ROOT/'src/is_facing_camera.c').read_text(), 'is_facing_camera')
        self.assertEqual({r['name']:r['bp_offset'] for r in predict(facing)['locals']},
                         {'dx0':-4, 'dx1':-12, 'dy0':-8, 'dy1':-16})
        names = suggest_names(['first','second','third'])
        self.assertEqual(names['declaration_order'], ['third','second','first'])


if __name__ == '__main__':
    unittest.main()
