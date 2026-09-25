"""Original-code array evidence must fail closed without a dominating bound."""
import copy
import hashlib
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
from data_extent_generator import derive
from data_symbols import resolve_symbols
from mz import MZ
from oracle import verify


def synthetic(guarded=True):
    # cmp [bp+6],0; jl exit; cmp [bp+6],3; jg exit;
    # mov ax,34; imul [bp+6]; mov bx,ax; mov al,[bx+1000h].
    prelude = bytes.fromhex('558bec')
    scale = bytes.fromhex('b82200f76e068bd88a8700105dcb')
    if guarded:
        guards = bytearray.fromhex('837e06007c00837e06037f00')
        exit_at = len(prelude)+len(guards)+len(scale)-2
        guards[5] = exit_at-(len(prelude)+6)
        guards[11] = exit_at-(len(prelude)+12)
        body = prelude+guards+scale
    else:
        body = prelude+scale
    image = body+bytes(256)
    digest = hashlib.sha256(image).hexdigest()
    inventory = {'functions': [{'name':'synthetic', 'start':0, 'end':len(body),
                                'sha256':hashlib.sha256(body).hexdigest(),
                                'status':'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'}]}
    layout = {'oracle_sha256':digest, 'frame_load_address':0x2000,
              'bss_start':0x2000, 'bss_end':0x4000,
              'symbols':{'_records':{'load_address':0x3000,'storage':'bss'}}}
    return image, layout, inventory


def synthetic_loop():
    code = bytearray.fromhex('558bec83ec02c746fe0000')
    guard_at = len(code)
    code += bytes.fromhex('837efe047d00')
    code += bytes.fromhex('b82200f76efe8bd88a870010ff46feeb00')
    exit_at = len(code)
    code += bytes.fromhex('8be55dcb')
    code[guard_at+5] = exit_at-(guard_at+6)
    jump_at = exit_at-2
    code[jump_at+1] = (guard_at-exit_at) & 0xff
    image = bytes(code)+bytes(256)
    inventory = {'functions': [{'name':'counted_loop', 'start':0, 'end':len(code),
                                'sha256':hashlib.sha256(code).hexdigest(),
                                'status':'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'}]}
    layout = {'oracle_sha256':hashlib.sha256(image).hexdigest(),
              'frame_load_address':0x2000, 'bss_start':0x2000,
              'bss_end':0x4000,
              'symbols':{'_records':{'load_address':0x3000,'storage':'bss'}}}
    return image, layout, inventory


class GeneratedExtents(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        _, unpacked, report, _ = verify(write=False)
        cls.image = MZ.parse(unpacked).load_image(unpacked)
        cls.relocations = report['unpacked_mz']['relocations']

    def test_real_slice_rederived_and_audio_buffer_unbounded(self):
        layout = read_json(ROOT/'layout/data-symbols.json')
        generated, rejected, witnesses = derive(self.image, self.relocations, layout)
        self.assertEqual(set(generated), {'_audiochunks_unk2'})
        self.assertEqual(generated['_audiochunks_unk2']['width'], 8*76)
        self.assertEqual(generated['_audiochunks_unk2']['extent_proof'],
                         layout['symbols']['_audiochunks_unk2']['generated_extent'])
        self.assertEqual(len(witnesses), 4)
        self.assertEqual(rejected, [])
        self.assertEqual(len(resolve_symbols(['_state'], self.image, self.relocations)['_state']['allowed_addends']), 1120)
        self.assertEqual(len(resolve_symbols(['_audiochunks_unk2'], self.image, self.relocations)['_audiochunks_unk2']['allowed_addends']), 608)
        # Give the observed audio buffer an anchored name for this diagnostic.
        # Its post-access wrap check cannot derive a guarded width.
        diagnostic = copy.deepcopy(layout)
        diagnostic['symbols']['_audio_buffer_probe'] = {
            'load_address':layout['frame_load_address']+0x97dc, 'storage':'bss'}
        found, _, _ = derive(self.image, self.relocations, diagnostic)
        self.assertNotIn('_audio_buffer_probe', found)

    def test_synthetic_unbounded_and_conflicting_anchor(self):
        image, layout, inventory = synthetic(False)
        self.assertEqual(derive(image, [], layout, inventory)[0], {})
        image, layout, inventory = synthetic(True)
        found, rejected, _ = derive(image, [], layout, inventory)
        self.assertEqual(found['_records']['width'], 4*34)
        self.assertEqual(rejected, [])
        conflict = copy.deepcopy(layout)
        conflict['symbols']['_other'] = {'load_address':0x3002,'storage':'bss'}
        found, rejected, _ = derive(image, [], conflict, inventory)
        self.assertEqual(found, {})
        self.assertEqual(rejected[0]['reason'], 'independent interior anchor')
        # The same base with incompatible scale evidence cannot be unioned.
        first = image[:inventory['functions'][0]['end']]
        second = first.replace(bytes.fromhex('b82200'), bytes.fromhex('b82400'))
        combined = first+bytes(128-len(first))+second+bytes(128)
        mixed = copy.deepcopy(layout)
        mixed['oracle_sha256'] = hashlib.sha256(combined).hexdigest()
        two = copy.deepcopy(inventory)
        two['functions'].append({'name':'other_scale', 'start':128,
                                 'end':128+len(second),
                                 'sha256':hashlib.sha256(second).hexdigest(),
                                 'status':'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'})
        found, rejected, _ = derive(combined, [], mixed, two)
        self.assertEqual(found, {})
        self.assertEqual(rejected[0]['reason'], 'conflicting bounds or strides')

    def test_initialized_counted_loop(self):
        image, layout, inventory = synthetic_loop()
        found, rejected, _ = derive(image, [], layout, inventory)
        self.assertEqual(found['_records']['width'], 4*34)
        self.assertEqual(rejected, [])

    def test_generated_proof_mutation_rejects_at_binding(self):
        layout = read_json(ROOT/'layout/data-symbols.json')
        bad = copy.deepcopy(layout)
        bad['symbols']['_audiochunks_unk2']['generated_extent']['index_bound'][1] = 24
        with patch('data_symbols.read_json', side_effect=lambda path:
                   bad if str(path).endswith('data-symbols.json') else read_json(path)):
            with self.assertRaisesRegex(ValueError, 'Generated data extent'):
                resolve_symbols(['_audiochunks_unk2'], self.image, self.relocations)


if __name__ == '__main__':
    unittest.main()
