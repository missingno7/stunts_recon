"""Compact DGROUP aliases must keep original-instruction and extent gates."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
from binder import bind_mixed_far_data, _checked_data_addend
from binder import bind_data_offsets
from code_symbols import resolve_code_symbols
from compiler import compile_source
from data_symbols import resolve_symbols, check_folded_recipe
from mz import MZ
from oracle import verify


class DataAliases(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']

    def test_compact_aliases_rederive_original_instructions(self):
        layout = read_json(ROOT/'layout/data-symbols.json')
        names = [name for name, symbol in layout['symbols'].items()
                 if 'references' not in symbol and symbol['storage']!='code_island']
        self.assertGreater(len(names), 1100)
        self.assertEqual(set(resolve_symbols(names, self.image, self.relocations)), set(names))

    def test_wrong_address_and_relocated_operand_reject(self):
        layout = read_json(ROOT/'layout/data-symbols.json')
        bad = copy.deepcopy(layout)
        bad['symbols']['_word_4eca']['load_address'] = layout['frame_load_address'] + 1
        original = read_json
        with patch('data_symbols.read_json', side_effect=lambda path: bad if str(path).endswith('data-symbols.json') else original(path)), self.assertRaises(ValueError):
            resolve_symbols(['_word_4eca'], self.image, self.relocations)
        # This independently verified operand is required to remain unrelocated.
        anchor = 77152
        extra = {'segment': anchor // 16, 'offset': anchor % 16, 'load_offset': anchor}
        with self.assertRaises(ValueError):
            resolve_symbols(['_byte_2E916'], self.image, self.relocations + [extra])

    def test_reference_label_width_and_cross_object_addend(self):
        resolved = resolve_symbols(['_wndsprite', '_audioflag2', '_gameconfig'],
                                   self.image, self.relocations)
        self.assertEqual(resolved['_wndsprite']['allowed_addends'], [0, 1, 2, 3])
        self.assertEqual(resolved['_audioflag2']['allowed_addends'], [0])
        self.assertEqual(resolved['_gameconfig']['allowed_addends'], list(range(26)))
        layout = read_json(ROOT/'layout/data-symbols.json')
        bad = copy.deepcopy(layout)
        bad['symbols']['_wndsprite']['width'] = 6
        with patch('data_symbols.read_json', side_effect=lambda path: bad if str(path).endswith('data-symbols.json') else read_json(path)), self.assertRaises(ValueError):
            resolve_symbols(['_wndsprite'], self.image, self.relocations)

    def test_state_extent_and_mutations(self):
        state = resolve_symbols(['_state'], self.image, self.relocations)['_state']
        self.assertEqual(len(state['allowed_addends']), 1120)
        self.assertEqual(_checked_data_addend(state, (1119).to_bytes(2, 'little')), 1119)
        with self.assertRaisesRegex(ValueError, 'addend leaves'):
            _checked_data_addend(state, (1120).to_bytes(2, 'little'))
        layout = read_json(ROOT/'layout/data-symbols.json')
        for mutation in ('width', 'overlap', 'endpoint'):
            bad = copy.deepcopy(layout)
            if mutation == 'width':
                bad['symbols']['_state']['width'] += 1
            elif mutation == 'overlap':
                bad['symbols']['_word_34496']['width'] = 2
            else:
                bad['symbols']['_state']['extent_proof']['next_line'] += 1
            with patch('data_symbols.read_json', side_effect=lambda path: bad if str(path).endswith('data-symbols.json') else read_json(path)), self.assertRaises(ValueError):
                resolve_symbols(['_state'], self.image, self.relocations)

    def test_guarded_folded_index_binding_and_recipe(self):
        source = (b'extern struct AudioChunk { unsigned int first, second; '
                  b'char rest[72]; } audiochunks_unk2[]; '
                  b'unsigned int probe(int i) { return audiochunks_unk2[i-16].first; }')
        obj, _ = compile_source(source, 'msc510-medium')
        fixes = [fix for fix in obj.linker_fixups if fix['target'] == '_audiochunks_unk2']
        self.assertEqual(len(fixes), 1)
        self.assertEqual(fixes[0]['encoded_addend'], '40fb')
        symbol = resolve_symbols(['_audiochunks_unk2'], self.image, self.relocations)['_audiochunks_unk2']
        self.assertEqual(_checked_data_addend(symbol, bytes.fromhex('40fb')), -1216)
        declarations = {'segments':obj.segment_defs, 'groups':obj.groups,
                        'publics':obj.publics, 'externals':obj.externals}
        payload, _ = bind_data_offsets(obj, 'UNIT_TEXT', '_probe',
                                       obj.segment_length('UNIT_TEXT'), obj.linker_fixups,
                                       declarations, {'_audiochunks_unk2':symbol})
        at = fixes[0]['offset']
        self.assertEqual(payload[at:at+2], bytes.fromhex('fc81'))
        # A strict recipe must connect the fixup to the same original guarded
        # instruction, not merely name the array and its negative addend.
        recipe = {'start':161566, 'expected_fixups':[
            {'target':'_audiochunks_unk2','loc':'offset16','width':2,
             'displacement':0,'encoded_addend':'40fb','offset':38}],
            'folded_index_bindings':[{'offset':38,'target':'_audiochunks_unk2',
                                      'field_offset':0,'witness':'sub_3771E'}]}
        check_folded_recipe(recipe, {'_audiochunks_unk2':symbol}, self.image)
        for mutation in ('missing', 'site', 'field'):
            bad = copy.deepcopy(recipe)
            if mutation == 'missing': del bad['folded_index_bindings']
            elif mutation == 'site': bad['expected_fixups'][0]['offset'] += 1
            else: bad['folded_index_bindings'][0]['field_offset'] = 2
            with self.assertRaises(ValueError):
                check_folded_recipe(bad, {'_audiochunks_unk2':symbol}, self.image)

    def test_guarded_folded_index_mutations_reject(self):
        layout = read_json(ROOT/'layout/data-symbols.json')
        for mutation in ('stride', 'bound', 'missing_guard', 'width'):
            bad = copy.deepcopy(layout)
            proof = bad['symbols']['_audiochunks_unk2']['extent_proof']
            if mutation == 'stride': proof['stride'] = 74
            elif mutation == 'bound': proof['index_bound'][1] = 24
            elif mutation == 'missing_guard': proof['witnesses'][1]['anchors'].pop(1)
            else: bad['symbols']['_audiochunks_unk2']['width'] += 2
            with patch('data_symbols.read_json', side_effect=lambda path: bad if str(path).endswith('data-symbols.json') else read_json(path)), self.assertRaises(ValueError):
                resolve_symbols(['_audiochunks_unk2'], self.image, self.relocations)
        symbol = resolve_symbols(['_audiochunks_unk2'], self.image, self.relocations)['_audiochunks_unk2']
        for bad_addend in (-1212, -1292, 608, 0x3c5f):
            with self.assertRaises(ValueError):
                _checked_data_addend(symbol, (bad_addend & 0xffff).to_bytes(2,'little'))

    def test_actual_audio_cross_object_candidate_rejected(self):
        # Same source shape as the worker candidate, kept inline so validation
        # never depends on an ignored research workspace.
        source = (b'extern unsigned char audioflag2[]; '
                  b'extern void audio_driver_func1E(unsigned int,unsigned int); '
                  b'extern void sub_39700(void); '
                  b'void audio_disable_flag2(void) { audioflag2[0]=0; '
                  b'*((unsigned int *)(audioflag2+9))=1; '
                  b'if (audioflag2[0x3C5F]) audio_driver_func1E(0,(unsigned int)audioflag2[0x3C5F]-1); '
                  b'sub_39700(); *((unsigned int *)(audioflag2+9))=0; }')
        obj, _ = compile_source(source, 'msc510-medium')
        symbols = {**resolve_symbols(['_audioflag2'], self.image, self.relocations),
                   **resolve_code_symbols(['_sub_39700','_audio_driver_func1E'],
                                          self.image, self.relocations)}
        start = 0x10000
        relocs = [{'segment': 0x1000, 'offset': fix['offset']+2,
                   'load_offset': start+fix['offset']+2}
                  for fix in obj.linker_fixups if fix['loc']=='pointer32']
        declarations = {'segments':obj.segment_defs,'groups':obj.groups,
                        'publics':obj.publics,'externals':obj.externals}
        with self.assertRaisesRegex(ValueError, 'addend leaves'):
            bind_mixed_far_data(obj,'UNIT_TEXT','_audio_disable_flag2',
                                obj.segment_length('UNIT_TEXT'),obj.linker_fixups,
                                declarations,symbols,start,relocs)

    def test_pinned_ldiv_alias_requires_original_relocation(self):
        symbol = resolve_code_symbols(['__aFldiv'], self.image, self.relocations)['__aFldiv']
        self.assertEqual(symbol['load_address'], 124988)
        without = [row for row in self.relocations if row['load_offset'] != 173170]
        with self.assertRaises(ValueError):
            resolve_code_symbols(['__aFldiv'], self.image, without)


if __name__ == '__main__': unittest.main()
