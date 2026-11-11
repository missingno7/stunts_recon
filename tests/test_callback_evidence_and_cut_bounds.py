import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))

from common import read_json
from oracle import verify
from mz import MZ
from code_symbols import resolve_code_symbols, resolve_callback_pointer
from cut_simulator import simulate


class Integration15(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        executable = verify(write=False)[1]
        mz = MZ.parse(executable)
        cls.image = mz.load_image(executable)
        cls.relocations = mz.relocations


    def test_callback_one_pointer_proof_in_both_resolvers(self):
        expected = {'kind':'far-code','frame_load_address':72560,'load_address':75158}
        self.assertEqual(resolve_callback_pointer(self.image, self.relocations), expected)
        self.assertEqual(resolve_code_symbols({'_frame_callback'}, self.image, self.relocations),
                         {'_frame_callback':expected})
        layout = read_json(ROOT/'layout/code-symbols.json')
        changed = copy.deepcopy(layout)
        changed['symbols']['_frame_callback']['pointer_anchors'][0]['hex'] = 'b8260abab710'
        import code_symbols
        with patch.object(code_symbols, 'read_json', side_effect=lambda path:
                          changed if str(path).endswith('code-symbols.json') else read_json(path)):
            with self.assertRaises(ValueError):
                resolve_callback_pointer(self.image, self.relocations)
            with self.assertRaises(ValueError):
                resolve_code_symbols({'_frame_callback'}, self.image, self.relocations)


    def test_cut_simulator_rejects_unbounded_input(self):
        with self.assertRaises(ValueError):
            simulate({'schema':'msc510-code-cut-input-v1','members':[
                {'name':'oversized','length':250001,'instruction_ends':[250001]}]})


if __name__ == '__main__':
    unittest.main()
