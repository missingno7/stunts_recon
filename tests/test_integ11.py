"""Reviewed alias and bounded rotation-table integration checks."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json
from code_symbols import resolve_code_symbols
from data_symbols import resolve_symbols
from function_evidence import current_inventory, reviewed_functions
from mz import MZ
from oracle import verify


class Integration11(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle=verify(write=False)
        cls.image=MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations=oracle[2]['unpacked_mz']['relocations']

    def test_runtime_and_data_aliases_resolve(self):
        self.assertEqual(set(resolve_code_symbols({'_strcat','_itoa','_abs'},
                                                  self.image,self.relocations)),
                         {'_strcat','_itoa','_abs'})
        names={'_trackcenterpos','_voicefileptr','_planptr'}
        self.assertEqual(set(resolve_symbols(names,self.image,self.relocations)),names)
        symbols=read_json(ROOT/'layout/data-symbols.json')['symbols']
        # integ11's proposals placed these inside _state/the audio ring and were
        # refused.  integ22 grounds them at their pinned reference placements;
        # their extents must stay clear of the independently proved objects.
        resolved=resolve_symbols({'_trkObjectList','_td02_penalty_related','_state'},
                                 self.image,self.relocations)
        state=(resolved['_state']['load_address'],resolved['_state']['load_address']+1120)
        for name,proposed in (('_trkObjectList',214478),('_td02_penalty_related',214506)):
            row=resolved[name]; end=row['load_address']+len(row['allowed_addends'])
            self.assertNotEqual(row['load_address'],proposed)
            self.assertFalse(row['load_address']<state[1] and state[0]<end)
            self.assertFalse(row['load_address']<218268 and 216908<end)
        self.assertEqual(resolved['_trkObjectList']['load_address']-178032,0x2098)
        self.assertEqual(resolved['_td02_penalty_related']['load_address']-178032,0x8178)

    def test_rotation_switch_and_select_boundaries_are_reviewed(self):
        inventory=current_inventory(self.image)
        rows={r['name']:r for r in inventory['functions']}
        self.assertEqual((rows['mat_rot_zxy']['start'],rows['mat_rot_zxy']['end']),
                         (90618,91002))
        self.assertEqual((rows['select_cliprect_rotate']['start'],
                          rows['select_cliprect_rotate']['end']),(85510,85662))
        self.assertEqual(rows['mat_rot_zxy']['status'],
                         'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')
        doc=copy.deepcopy(read_json(ROOT/'layout/function-evidence.json'))
        row=next(r for r in doc['functions'] if r['name']=='mat_rot_zxy')
        row['table_proofs'][0]['targets'][0]+=1
        original=read_json
        def tampered(path):
            return doc if path==ROOT/'layout/function-evidence.json' else original(path)
        with patch('function_evidence.read_json',side_effect=tampered):
            with self.assertRaises(ValueError):
                reviewed_functions(self.image)


if __name__=='__main__':
    unittest.main()
