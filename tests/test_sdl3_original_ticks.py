"""Narrow agreements with ORIGINAL DOSBox-X observations; strict comparison checks.

No complete scenario currently agrees in absolute IRQ ticks. Assertions cover
input delivery, bounded driver setup and the initial indexed frame explicitly.
"""
import hashlib
import json
from pathlib import Path
import tempfile
import unittest
import zlib

from tests.test_sdl3_driving import ROOT,EXECUTABLE,assert_build_ready
from tools.porting.original_tick_oracle import input_ticks,run_port,bindings
from tools.porting.compare_tick_streams import first_difference,resynchronised_differences,fnv

FIXTURES=ROOT/'tools/porting/tick_oracle_cases'
WORK=ROOT/'build/workers/portclock/original-tests'


class OriginalTickComparisonTests(unittest.TestCase):
    def test_agreeing_original_boundaries(self):
        assert_build_ready()
        fixtures=sorted(FIXTURES.glob('*.json'))
        self.assertEqual(len(fixtures),8,'both original drivers in all four scenarios')
        WORK.mkdir(parents=True,exist_ok=True)
        for path in fixtures:
            with self.subTest(case=path.stem),tempfile.TemporaryDirectory(dir=WORK) as temporary:
                fixture=json.loads(path.read_text(encoding='utf-8'));provenance=fixture['provenance']
                self.assertEqual(fixture['schema'],'stunts-dosbox-tick-fixture-v1')
                for source,field in [(ROOT/'layout/oracle.lock.json','oracle_lock_sha256'),
                    (ROOT/f"assets/{fixture['audio'].upper()}.DRV",'driver_sha256')]:
                    self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(),provenance[field])
                folder=Path(temporary);script=folder/'input.json'
                script.write_bytes((FIXTURES/fixture['script']).read_bytes())
                self.assertEqual(hashlib.sha256(script.read_bytes()).hexdigest(),provenance['script_sha256'])
                actual=run_port(EXECUTABLE,folder,script,fixture['ticks'],fixture['audio'],compare_original=False)
                prefix=fixture['driver_initialisation']
                self.assertEqual(actual['sound'][:len(prefix)],prefix)
                checkpoint=fixture['frame_checkpoint']
                self.assertEqual(actual['frames'][fixture['frame_checkpoint_ordinal']],checkpoint)
                indexed=zlib.decompress(bytes.fromhex(fixture['frame_checkpoint_zlib_hex']))
                self.assertEqual(len(indexed),64768)
                self.assertEqual(fnv(indexed[:64000]),checkpoint['indices'])
                self.assertEqual(fnv(indexed[64000:]),checkpoint['palette'])
                if fixture['audio']=='ad15':
                    self.assertEqual(actual['frames'][0],fixture['first_frame'])
                payload=zlib.decompress(bytes.fromhex(fixture['first_frame_zlib_hex']))
                self.assertEqual(len(payload),64768)
                self.assertEqual(fnv(payload[:64000]),fixture['first_frame']['indices'])
                self.assertEqual(fnv(payload[64000:]),fixture['first_frame']['palette'])
                self.assertEqual(actual['scans'],fixture['scans'])
                if fixture['scenario'] in ('startup','selection'):
                    self.assertEqual(actual['keys'],fixture['keys'])
                else:
                    # Through the explicit protection answer. Later consumption
                    # ticks diverge and remain visible in the full comparison.
                    self.assertEqual(actual['keys'][:14],fixture['keys'][:14])
                if fixture['scenario']=='replay':
                    states=[r for r in actual['states'] if 1850<=r['raw_tick']<2100]
                    self.assertEqual(len(states),50)
                    self.assertEqual(states,fixture['replay_play_states'])
                    seek=[r for r in actual['states'] if r['raw_tick']==2240][:25]
                    self.assertEqual(len(seek),25)
                    self.assertEqual(seek,fixture['seek_tick_prefix'])

    def test_comparison_does_not_realign_ticks(self):
        original=[dict(raw_tick=8,entry=9),dict(raw_tick=9,entry=12)]
        shifted=[dict(raw_tick=7,entry=9),dict(raw_tick=8,entry=12)]
        difference=first_difference(original,shifted)
        self.assertEqual(difference['index'],0)
        self.assertEqual(difference['original_context'],original)
        self.assertEqual(difference['port_context'],shifted)
        diagnostic=resynchronised_differences(original,shifted)
        self.assertEqual(diagnostic['content_matches'],2)
        self.assertEqual(diagnostic['port_minus_original_tick_histogram'],{'-1':2})
        self.assertIsNone(first_difference(original,list(original)))

    def test_resynchronisation_reports_more_than_one_divergence(self):
        original=[dict(raw_tick=i,value=v) for i,v in enumerate('abcde')]
        actual=[dict(raw_tick=i,value=v) for i,v in enumerate('axcye')]
        result=resynchronised_differences(original,actual)
        self.assertEqual(result['content_matches'],3)
        self.assertEqual(len(result['differences']),2)
        self.assertTrue(all(r['resynchronised'] for r in result['differences']))

    def test_script_uses_grants_and_stable_same_tick_order(self):
        events=[dict(visible_tick=n,channel='dos.keyboard.scancodes',payload=[scan])
                for n,scan in ((0,28),(10_000_154,156),(10_000_155,72))]
        actual=input_ticks(dict(format='stunts-exact-input-script-v1',anchor_tick=0,events=events))['events']
        self.assertEqual([(e['raw_tick'],e['payload']) for e in actual],[(1,[28]),(1,[156]),(2,[72])])

    def test_observer_bindings_are_grounded_in_locked_image(self):
        points=bindings()
        self.assertEqual(points['timer'],0x2eb21909)
        self.assertTrue(all(h.get('entry_bytes') for h in points['hooks'] if 'original_load_offset' in h))
        self.assertIn('mousecursor',{h['name'] for h in points['hooks']})


if __name__=='__main__':unittest.main()
