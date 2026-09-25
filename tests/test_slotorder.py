"""Accepted local homes and small collision/scope probes."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from slotorder import bucket, locals_from_source, predict, suggest_names, suggest_registers


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
        self.assertEqual([(r['name'], r['type']) for r in rows],
                         [('destination', 'char *'), ('source', 'char far *'),
                          ('current', 'char far *')])
        self.assertEqual(next(r['bp_offset'] for r in predict(rows)['locals']
                              if r['name']=='current'), -4)
        facing = locals_from_source((ROOT/'src/is_facing_camera.c').read_text(), 'is_facing_camera')
        self.assertEqual({r['name']:r['bp_offset'] for r in predict(facing)['locals']
                          if r['storage']!='parameter'},
                         {'dx0':-4, 'dx1':-12, 'dy0':-8, 'dy1':-16})
        names = suggest_names(['first','second','third'])
        self.assertEqual(names['declaration_order'], ['third','second','first'])

    def test_register_declaration_order_independent_of_bp_homes(self):
        rows = [{'name':'y','type':'int','storage':'register','referenced':True},
                {'name':'a','type':'int','storage':'register','referenced':True}]
        result = predict(rows)
        self.assertEqual([(r['name'],r['register']) for r in result['register_assignments']],
                         [('y','si'),('a','di')])
        self.assertEqual(result['prologue_push_sequence'], ['di','si'])
        self.assertEqual(result['epilogue_pop_sequence'], ['si','di'])
        self.assertEqual(result['locals'][0]['bp_offset'], -4)
        self.assertEqual(result['locals'][1]['bp_offset'], -2)
        suggestion = suggest_registers({'si':'index','di':'source'})
        self.assertEqual(suggestion['declaration_order'], ['index','source'])

    def test_twenty_accepted_save_orders(self):
        cases = json.loads((ROOT/'tests/fixtures/slotorder_register_saves.json').read_text())
        self.assertEqual(len(cases),20)
        for case in cases:
            with self.subTest(function=case['function']):
                result=predict([],other_register_uses=case['used'])
                self.assertEqual(result['prologue_push_sequence'],case['pushes'])
                if case['expected_epilogue']:
                    self.assertEqual(result['epilogue_pop_sequence'],case['expected_epilogue'])


if __name__ == '__main__':
    unittest.main()
