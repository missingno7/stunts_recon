"""Literal source emissions remain separate from instruction and production proof."""
import hashlib
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from common import ROOT, read_json
from mz import MZ
from oracle import verify
from source_emission import numeric_literal_bytes
sys.path.insert(0, str(ROOT/'build/python'))
from capstone import Cs, CS_ARCH_X86, CS_MODE_16


class NumericLiteralEmissionTests(unittest.TestCase):
    def test_exact_single_value_width_and_endian(self):
        self.assertEqual(numeric_literal_bytes('byte_19F07 db 30'), b'\x1e')
        self.assertEqual(numeric_literal_bytes('incnums db 0FFh'), b'\xff')
        self.assertEqual(numeric_literal_bytes('word_2ecf8 dw 1234h'), b'\x34\x12')
        self.assertEqual(numeric_literal_bytes('dw 0'), b'\x00\x00')

    def test_expressions_lists_overflow_and_other_directives_refuse(self):
        for source in ('db 256', 'dw 65536', 'db -1', 'db 1,2',
                       'dw offset loc_12345', 'db symbol', 'db 1+2', 'dd 1',
                       'db 0 dup (?)', 'db ?', 'word_2ecf8 dw 0,0'):
            with self.subTest(source=source):
                self.assertIsNone(numeric_literal_bytes(source))

    def test_imported_literal_trailers_are_exact_but_not_ready(self):
        _, unpacked, _, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        inventory = {f['name']: f for f in read_json(ROOT/'recovery/restunts-inventory.json')['functions']}
        queue = {task['name']: task for task in read_json(ROOT/'recovery/queue.json')['tasks']}
        for name in ('init_div0', '_rand', 'sprite_1_unk', 'sub_35C4E'):
            with self.subTest(name=name):
                function = inventory[name]
                self.assertEqual(function['status'], 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')
                self.assertEqual(hashlib.sha256(image[function['start']:function['end']]).hexdigest(),
                                 function['sha256'])
                self.assertEqual(queue[name]['tier'], 'SUPERVISOR')
        seg002 = (ROOT/'build/references/restunts/src/restunts/asmorig/seg002.asm').read_text(encoding='latin1').splitlines()
        trailer = b''.join(numeric_literal_bytes(line.split(';', 1)[0])
                           for line in seg002[321:332])
        self.assertEqual(len(trailer), 11)
        self.assertEqual(image[0x19f07-0x10000:0x19f07-0x10000+11], trailer)
        seg012 = (ROOT/'build/references/restunts/src/restunts/asmorig/seg012.asm').read_text(encoding='latin1').splitlines()
        embedded = b''.join(numeric_literal_bytes(line.split(';', 1)[0])
                            for line in seg012[1513:1517])
        self.assertEqual(embedded, b'\x00' * 8)
        self.assertEqual(image[0x2f354-0x10000:0x2f354-0x10000+8], embedded)
        self.assertEqual(inventory['parse_shape2d_helper2']['status'],
                         'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')
        self.assertEqual(queue['parse_shape2d_helper2']['tier'], 'SUPERVISOR')

    def test_raw_literal_can_also_be_a_code_entry(self):
        _, unpacked, _, _ = verify(write=False)
        image = MZ.parse(unpacked).load_image(unpacked)
        lines = (ROOT/'build/references/restunts/src/restunts/asmorig/seg028.asm').read_text(encoding='latin1').splitlines()
        declared = b''.join(numeric_literal_bytes(line.split(';', 1)[0])
                            for line in lines[1654:1657])
        address = 0x3930e - 0x10000
        self.assertEqual(declared, bytes.fromhex('837ef0'))
        self.assertEqual(image[address:address+len(declared)], declared)
        self.assertIn('byte_3930E', lines[1629])  # direct conditional branch target
        decoder = Cs(CS_ARCH_X86, CS_MODE_16)
        entered = next(decoder.disasm(image[address:address+5], address, count=1))
        self.assertEqual(entered.mnemonic, 'cmp')
        inventory = {f['name']: f for f in read_json(ROOT/'recovery/restunts-inventory.json')['functions']}
        queue = {task['name']: task for task in read_json(ROOT/'recovery/queue.json')['tasks']}
        self.assertEqual(inventory['sub_39088']['status'], 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')
        self.assertEqual(queue['sub_39088']['tier'], 'SUPERVISOR')

    def test_newly_mapped_multiple_entry_body_has_explicit_cfg_blocker(self):
        census = read_json(ROOT/'recovery/blocker-census.json')
        row = next(row for row in census['tasks'] if row['name'] == 'font_set_unk')
        self.assertIn('SINGLE_ENTRY_CFG_UNREACHABLE_CANDIDATE', row['categories'])
        self.assertEqual(row['primary_category'], 'SINGLE_ENTRY_CFG_UNREACHABLE_CANDIDATE')
        self.assertGreater(row['single_entry_cfg_finding']['byte_count'], 0)
        self.assertFalse(row['automatic_reopen'])
        queue = {task['name']: task for task in read_json(ROOT/'recovery/queue.json')['tasks']}
        self.assertEqual(queue['font_set_unk']['tier'], 'SUPERVISOR')


if __name__ == '__main__':
    unittest.main()
