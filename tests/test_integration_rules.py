"""Positive and negative gates for reviewed aliases, addends, and complete groups."""
import copy
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from binder import bind_data_offsets, bind_mixed_far_data
from code_symbols import resolve_code_symbols
from common import identity, read_json
from compiler import compile_source
from data_symbols import resolve_symbols
from multi_contribution import bind_multi, checked_members
from mz import MZ
from oracle import verify
from promote import replace_group


class IntegrationRules(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle = verify(write=False)
        cls.image = MZ.parse(cls.oracle[1]).load_image(cls.oracle[1])
        cls.relocations = cls.oracle[2]['unpacked_mz']['relocations']

    def test_single_anchor_alias_and_rejections(self):
        for name in ('_file_find_next', '_file_load_shape2d_nofatal_thunk',
                     '_mmgr_get_ofs_diff'):
            self.assertEqual(resolve_code_symbols([name], self.image, self.relocations)[name]['kind'],
                             'far-code')
        import code_symbols
        original = code_symbols.read_json
        for mutation in ('hash', 'call', 'corroboration'):
            def altered(path):
                data = original(path)
                if str(path).endswith('code-symbols.json'):
                    entry = data['symbols']['_file_find_next']
                    if mutation == 'hash':
                        entry['mapped_target']['sha256'] = '0'*64
                    elif mutation == 'call':
                        entry['anchors'][0]['hex'] = '9a00000000'
                    else:
                        entry['distinct_verified_callers'] = 2
                return data
            with self.subTest(mutation=mutation), patch('code_symbols.read_json', side_effect=altered):
                with self.assertRaises(ValueError):
                    resolve_code_symbols(['_file_find_next'], self.image, self.relocations)
        with self.assertRaises(ValueError):
            resolve_code_symbols(['_file_find_next'], self.image, [])

    def test_two_far_calls_mixed_with_data_and_order(self):
        source = (b'extern unsigned char flags[]; long product(long a,long b,long c,long d) '
                  b'{ flags[1]=3; flags[2]=4; return a*b-c*d; }')
        obj, _ = compile_source(source, 'msc510-medium')
        self.assertEqual([f['loc'] for f in obj.linker_fixups],
                         ['pointer32', 'pointer32', 'offset16', 'offset16'])
        start = 0x15000
        relocs = [{'segment':0x1000, 'offset':0x5000+f['offset']+2,
                   'load_offset':start+f['offset']+2}
                  for f in obj.linker_fixups if f['loc']=='pointer32']
        symbols = {'__aFlmul':{'kind':'far-code','frame_load_address':0x1cc50,
                               'load_address':0x1e8d8},
                   '_flags':{'group':'DGROUP','frame_load_address':0x2b770,
                             'load_address':0x34d06,'allowed_addends':[1,2]}}
        declarations = {'segments':obj.segment_defs,'groups':obj.groups,
                        'publics':obj.publics,'externals':obj.externals}
        args = (obj, 'UNIT_TEXT', '_product', obj.segment_length('UNIT_TEXT'),
                obj.linker_fixups, declarations, symbols, start, relocs)
        payload, receipt = bind_mixed_far_data(*args)
        self.assertEqual(receipt['generated_relocations'], relocs)
        self.assertEqual([f['kind'] for f in receipt['fixups']].count('far-call'), 2)
        self.assertEqual(len(payload), obj.segment_length('UNIT_TEXT'))
        with self.assertRaises(ValueError):
            bind_mixed_far_data(*args[:-1], list(reversed(relocs)))
        bad = copy.deepcopy(symbols)
        bad['_flags']['allowed_addends'] = [1]
        with self.assertRaises(ValueError):
            bind_mixed_far_data(*(args[:6]+(bad,)+args[7:]))

    def test_dgroup_object_addend_scope(self):
        symbols = resolve_symbols(['_audioflag2','_clip','_sdgame2ptr'],
                                  self.image, self.relocations)
        self.assertEqual(symbols['_audioflag2']['allowed_addends'], [0])
        self.assertEqual(symbols['_clip']['allowed_addends'], [0,2,4,6])
        self.assertEqual(symbols['_sdgame2ptr']['allowed_addends'], [0,2])
        obj, _ = compile_source(b'extern unsigned char audioflag2[]; '
                                b'int lookup(void) { return audioflag2[0x3c5f]; }',
                                'msc510-medium')
        declarations = {'segments':obj.segment_defs,'groups':obj.groups,
                        'publics':obj.publics,'externals':obj.externals}
        with self.assertRaisesRegex(ValueError, 'addend leaves'):
            bind_data_offsets(obj, 'UNIT_TEXT', '_lookup', obj.segment_length('UNIT_TEXT'),
                              obj.linker_fixups, declarations,
                              {'_audioflag2':symbols['_audioflag2']})

    def test_complete_members_and_subsumption(self):
        inventory = read_json(ROOT/'evidence/functions.json')['functions']
        names = ('audio_enable_flag2','audio_disable_flag2','audio_toggle_flag2')
        members = []
        for name in names:
            f, = [f for f in inventory if f['name']==name]
            members.append({'name':name,'stable_id':f['stable_id'],
                            'start':f['start'],'end':f['end'],
                            'target':{'size':f['size'],'sha256':f['sha256']},
                            'public':'_'+name})
        recipe = {'id':'audio_flag2_group', 'start':members[0]['start'],
                  'end':members[-1]['end'], 'members':members,
                  'target':identity(self.image[members[0]['start']:members[-1]['end']]),
                  'subsumed_owners':['load_273b2']}
        checked_members(recipe, self.image)
        # Reconstruct the pre-group ownership window instead of treating the
        # current manifest (which already contains the group) as its own input.
        manifest = copy.deepcopy(read_json(ROOT/'layout/manifest.json'))
        owners = manifest['owners']
        group_index, = [i for i, owner in enumerate(owners)
                        if owner['id'] == 'audio_flag2_group']
        group_owner, following_raw = owners[group_index:group_index+2]
        self.assertEqual((group_owner['start'], group_owner['end']), (160690, 160766))
        self.assertEqual((following_raw['kind'], following_raw['start'],
                          following_raw['end']), ('UNRESOLVED_RAW', 160766, 161966))
        owners[group_index:group_index+2] = [
            {'classification':'GAME_C', 'end':160696, 'id':'load_273b2',
             'kind':'MATCHING_C', 'name':'audio_enable_flag2',
             'recipe':'tests/fixtures/audio_enable_flag2.json', 'start':160690},
            {'classification':'UNRESOLVED_MIXED', 'end':161966,
             'id':'raw_273b8_278ae', 'kind':'UNRESOLVED_RAW', 'start':160696},
        ]
        staged = replace_group(manifest, recipe, self.oracle)
        self.assertTrue(any(o['id']=='audio_flag2_group' and o['start']==recipe['start']
                            and o['end']==recipe['end'] for o in staged['owners']))
        bad = copy.deepcopy(recipe)
        bad['members'][1]['start'] += 1
        with self.assertRaises(ValueError):
            checked_members(bad, self.image)
        bad = copy.deepcopy(recipe)
        bad['subsumed_owners'] = []
        with self.assertRaises(ValueError):
            replace_group(manifest, bad, self.oracle)

    def test_internal_near_call_exact_object(self):
        obj, _ = compile_source(b'void far beta(void); void far alpha(void) { beta(); } '
                                b'void far beta(void) { }', 'msc510-medium')
        pubs = {p['name']:p['offset'] for p in obj.publics}
        self.assertEqual(pubs, {'_alpha':0,'_beta':6})
        recipe = {'start':0,'end':8,'object_segment':'UNIT_TEXT',
                  'members':[{'public':'_alpha','start':0},
                             {'public':'_beta','start':6}],
                  'object_declarations':{'segments':obj.segment_defs,
                                         'groups':obj.groups,'publics':obj.publics,
                                         'externals':obj.externals},
                  'expected_fixups':obj.linker_fixups,'expected_relocations':[]}
        payload, receipt = bind_multi(obj, recipe, b'', [])
        self.assertEqual(payload, bytes.fromhex('0ee80200cb90cb90'))
        self.assertEqual(receipt['internal_calls'][0]['displacement'], 2)
        bad = copy.deepcopy(obj)
        bad.linker_fixups[0]['encoded_addend'] = '0100'
        bad_recipe = copy.deepcopy(recipe)
        bad_recipe['expected_fixups'] = bad.linker_fixups
        with self.assertRaises(ValueError):
            bind_multi(bad, bad_recipe, b'', [])

    def test_group_internal_and_external_fixups(self):
        obj, _ = compile_source(
            b'extern void far gamma(void); void far beta(void); '
            b'void far alpha(void) { beta(); gamma(); } void far beta(void) { }',
            'msc510-medium')
        start = 0x15000
        far, near = obj.linker_fixups
        self.assertEqual((far['target'], near['target']), ('_gamma','_beta'))
        relocation = {'segment':0x1000, 'offset':0x5000+far['offset']+2,
                      'load_offset':start+far['offset']+2}
        recipe = {'start':start,'end':start+obj.segment_length('UNIT_TEXT'),
                  'object_segment':'UNIT_TEXT',
                  'members':[{'public':'_alpha','start':start},
                             {'public':'_beta','start':start+10}],
                  'object_declarations':{'segments':obj.segment_defs,
                                         'groups':obj.groups,'publics':obj.publics,
                                         'externals':obj.externals},
                  'expected_fixups':obj.linker_fixups,
                  'expected_relocations':[relocation],
                  'external_binding':{'mode':'external-far-call-v1'}}
        symbols = {'_gamma':{'kind':'far-code','frame_load_address':0x1ea20,
                             'load_address':0x202aa}}
        with patch('multi_contribution.resolve_recipe_symbols', return_value=symbols):
            payload, receipt = bind_multi(obj, recipe, b'', [])
            self.assertEqual(payload.hex(), '0ee806009a8a18a21ecbcb90')
            self.assertEqual(receipt['generated_relocations'], [relocation])
            bad = copy.deepcopy(recipe)
            bad['expected_relocations'] = []
            with self.assertRaises(ValueError):
                bind_multi(obj, bad, b'', [])


if __name__ == '__main__':
    unittest.main()
