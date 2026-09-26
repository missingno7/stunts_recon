import copy
import json
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
from data_symbols import resolve_symbols
from function_evidence import current_inventory
from cut_simulator import simulate
from diagnostics import _decode


class Integration15(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        executable = verify(write=False)[1]
        mz = MZ.parse(executable)
        cls.image = mz.load_image(executable)
        cls.relocations = mz.relocations

    def test_aliasgap_reviewed_bindings(self):
        proposal = read_json(ROOT/'build/workers/aliasgap3/proposal.json')
        data = set(proposal['data_symbol_additions']) | set(proposal['data_symbol_extent_additions'])
        bound = resolve_symbols(data, self.image, self.relocations)
        layout = read_json(ROOT/'layout/data-symbols.json')['symbols']
        for name in data:
            self.assertEqual(bound[name]['load_address'], layout[name]['load_address'])
            self.assertEqual(max(bound[name]['allowed_addends']), layout[name]['width']-1)
        code = resolve_code_symbols(set(proposal['code_symbol_additions']),
                                    self.image, self.relocations)
        for name, row in proposal['code_symbol_additions'].items():
            self.assertEqual(code[name]['load_address'], row['mapped_target']['start'])
        lost = [r for r in self.relocations if r['load_offset'] !=
                proposal['code_symbol_additions']['_sprite_1_unk']['anchors'][0]['relocation']['load_offset']]
        with self.assertRaises(ValueError):
            resolve_code_symbols({'_sprite_1_unk'}, self.image, lost)

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

    def test_rotation_boundary_is_reviewed(self):
        row = next(f for f in current_inventory(self.image)['functions']
                   if f['name']=='mat_rot_zxy')
        self.assertEqual((row['start'],row['end'],row['status']),
                         (90618,91002,'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'))

    def test_accepted_group_record_cuts_and_relocations(self):
        expected = {'init_main':[(0,946),(946,1542)],
                    'rectlist_core_group':[(0,946),(946,1496)],
                    'mouse_set_pixratio':[(0,386)]}
        for name, cuts in expected.items():
            with self.subTest(group=name):
                recipe = read_json(ROOT/'recipes'/f'{name}.json')
                members=[]
                for row in recipe['members']:
                    lo,hi=row['start'],row['end']
                    instruction_ends = sorted({ins['load_offset'] + len(bytes.fromhex(ins['bytes'])) - lo
                        for ins in _decode(self.image[lo:hi],lo) if
                        0 < ins['load_offset'] + len(bytes.fromhex(ins['bytes'])) - lo <= hi-lo})
                    if not instruction_ends or instruction_ends[-1] != hi-lo:
                        instruction_ends.append(hi-lo)
                    member = {'name':row['name'],'length':hi-lo,
                              'instruction_ends':instruction_ends,'fixups':[],'relocations':[]}
                    base=lo-recipe['start']
                    member['fixups']=[{'offset':f['offset']-base,'width':f['width'],'kind':f['loc']}
                        for f in recipe['expected_fixups'] if base <= f['offset'] < base+hi-lo]
                    member['relocations']=[{'site':r['load_offset']-lo,**r}
                        for r in recipe['expected_relocations'] if lo <= r['load_offset'] < hi]
                    members.append(member)
                result=simulate({'schema':'msc510-code-cut-input-v1','members':members})
                self.assertEqual([(r['start'],r['end']) for r in result['records']], cuts)
                self.assertEqual([r['load_offset'] for r in result['exepack_order_relocations']],
                                 [r['load_offset'] for r in recipe['expected_relocations']])

    def test_cut_simulator_rejects_unbounded_input(self):
        with self.assertRaises(ValueError):
            simulate({'schema':'msc510-code-cut-input-v1','members':[
                {'name':'oversized','length':250001,'instruction_ends':[250001]}]})


if __name__ == '__main__':
    unittest.main()
