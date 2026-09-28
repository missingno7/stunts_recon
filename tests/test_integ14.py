"""Focused reviewed boundary and CRT0 entry binding guards."""
import copy
import sys
import unittest
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json
from oracle import verify
from mz import MZ
from function_evidence import reviewed_functions
from code_symbols import resolve_code_symbols
from runtime_binding import reviewed_main_entry


class Integ14Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        exe=verify(write=False)[1]
        mz=MZ.parse(exe)
        cls.image=mz.load_image(exe)
        cls.relocations=mz.relocations

    def test_ready_boundaries_and_conditional_holds(self):
        rows=reviewed_functions(self.image)
        proposal=read_json(ROOT/'build/workers/bounds/proposal.json')
        for row in proposal['function_evidence_overlay_additions']:
            self.assertEqual((rows[row['name']]['start'], rows[row['name']]['end']),
                             (row['start'],row['end']))
        # integ26 reviewed audio_map_song_tracks with its structural post-table
        # jump; the other conditional rows stay outside the overlay.
        for row in proposal['conditional_row_proposals']:
            # integ31 reviewed sub_38702 (extent 934 B, 18-word dispatch).
            if row['name'] in ('audio_map_song_tracks','sub_38702'):
                continue
            self.assertNotIn(row['name'],rows)
        self.assertIn('audio_map_song_tracks',rows)

    def test_generic_table_word_and_domain_mutations_rejected(self):
        original=read_json(ROOT/'layout/function-evidence.json')
        for name, alteration in (
                ('build_track_object', lambda f: f['dispatch_tables'][0]['targets'].__setitem__(0,0)),
                ('draw_line_related', lambda f: f['dispatch_tables'][1]['valid_indices'].insert(0,1)),
                ('loop_game', lambda f: f['table_proofs'][0]['entry_offsets'].__setitem__(0,0))):
            with self.subTest(name=name):
                altered=copy.deepcopy(original)
                row=next(f for f in altered['functions'] if f['name']==name)
                alteration(row)
                def fake_read(path):
                    return altered if path==ROOT/'layout/function-evidence.json' else read_json(path)
                with patch('function_evidence.read_json',side_effect=fake_read):
                    with self.assertRaises(ValueError):
                        reviewed_functions(self.image)

    def test_crt0_main_call_and_source_site(self):
        owner={'module':'dos\\crt0.asm','start':117858}
        fix={'segment':'_TEXT','offset':152,'target_kind':'external','target':'_main',
             'loc':'pointer32','displacement':0,'self_relative':False}
        self.assertEqual(reviewed_main_entry(self.image,self.relocations,owner,fix),0)
        self.assertEqual(resolve_code_symbols({'_main'},self.image,self.relocations)['_main'],
                         {'kind':'far-code','frame_load_address':0,'load_address':0})
        with self.assertRaises(ValueError):
            reviewed_main_entry(self.image,[r for r in self.relocations
                                            if r['load_offset']!=118012],owner,fix)
        with self.assertRaises(ValueError):
            reviewed_main_entry(self.image,self.relocations,owner,{**fix,'target':'_other'})

    def test_audio_timer_pointer_alias(self):
        result=resolve_code_symbols({'_audiodriver_timer'},self.image,self.relocations)
        self.assertEqual(result['_audiodriver_timer'],
                         {'kind':'far-code','frame_load_address':165424,
                          'load_address':165436})
        without=[r for r in self.relocations if r['load_offset']!=162432]
        with self.assertRaises(ValueError):
            resolve_code_symbols({'_audiodriver_timer'},self.image,without)


if __name__=='__main__':
    unittest.main()
