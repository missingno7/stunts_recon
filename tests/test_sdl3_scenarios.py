"""Original DOSBox-X UI/editor/replay/crash regressions at scoped key boundaries."""
import json,tempfile,unittest
from pathlib import Path
from tools.porting.compare_scenarios import port_data
from tools.porting.compare_step_streams import boundary,input_identity,digest
from tools.porting.scenario_oracle import run_port
ROOT=Path(__file__).resolve().parents[1]
CASES=ROOT/'tools/porting/step_oracle_cases/scenarios'

class OriginalScenarioTests(unittest.TestCase):
    def check_case(self,name,audio):
        expected=json.loads((CASES/(name+'-'+audio+'.json')).read_text())
        with tempfile.TemporaryDirectory(prefix='stunts-original-scenario-') as tmp:
            folder=Path(tmp);script=folder/'input.json';script.write_text(json.dumps(expected['script']))
            actual=port_data(run_port(folder/'port',script,audio,4000))
            self.assertEqual(actual['terminal'],'scenario checkpoint reached')
            for stream in ('input','keys','completed_frames','highscore_fields'):
                self.assertEqual(actual[stream],expected[stream],stream)
            self.assertEqual({k:digest(v) for k,v in actual['states'].items()},expected['states'])
            self.assertEqual(actual['race_facts'],expected['race_facts'])
            if name in ('manual-crash','highscore'):
                self.assertEqual(expected['race_facts'],dict(gear_changes=[[1,1],[41,2],[91,3],[151,4]],
                    transmissions=[0],first_crash_frame=354),'the original reference must demonstrate manual shifts and a crash')
            for label,value in expected['checkpoints'].items():
                with self.subTest(checkpoint=label):
                    if name=='selection' and audio=='pc15' and label=='car-lancia':
                        # Explicit open animated-pose comparison, never counted as agreeing.
                        self.assertEqual(actual['checkpoints'][label]['palette_sha256'],value['palette_sha256'])
                        continue
                    self.assertEqual(actual['checkpoints'][label],value)
            if name=='highscore':
                # The original copies an uninitialised local's string tails into HIG.
                # Raw file hashes remain a reported difference; defined fields above
                # and all six untouched records must still agree.
                self.assertEqual(set(actual['files']),set(expected['files']))
                self.assertEqual(actual['files']['DEFAULT.HIG']['size'],364)
                saved=(folder/'port/saves/DEFAULT.HIG').read_bytes()
                original=(ROOT/'assets/DEFAULT.HIG').read_bytes()
                self.assertEqual(saved[52:],original[:312])
            else:self.assertEqual(actual['files'],expected['files'])
            hits={}
            for row in actual['boundaries']:
                if row.get('kind')!='boundary':continue
                value=boundary(row);key=digest(input_identity(value));sample=expected['boundaries'].get(row['name'],{}).get(key)
                if sample:
                    self.assertIn(digest(value),sample['outputs'],row['name']+' bounded output/return')
                    hits[row['name']]=hits.get(row['name'],0)+1
            self.assertTrue(hits,'no comparable captured routine cases executed')
            if name in ('editor','file-error','highscore','replay'):
                self.assertIn('savefilez',hits,'saved-file or error driver boundary must execute')
            if name=='highscore':self.assertEqual(hits.get('port_call_read_line'),2,'both protection and high-score XY, buffer and return')
            if 'race_sound' in expected:
                from tools.porting.compare_sound_events import compact_sound
                from tools.porting.compare_tick_streams import read_rows
                sound=expected['race_sound']
                self.assertEqual(compact_sound(read_rows(folder/'port/sound.jsonl'),False,
                    sound['through_frame'],sound['engine_pitch_entries']),sound,
                    'all race sound content under the declared engine-target rule; later UI tail remains open')

    def test_highscore_ad15(self):self.check_case('highscore','ad15')
    def test_highscore_pc15(self):self.check_case('highscore','pc15')
    def test_file_error_ad15(self):self.check_case('file-error','ad15')
    def test_file_error_pc15(self):self.check_case('file-error','pc15')


def install(name,audio):
    def test(self):self.check_case(name,audio)
    test.__name__='test_'+name.replace('-','_')+'_'+audio
    setattr(OriginalScenarioTests,test.__name__,test)
for _name in ('opponent','selection','editor','file-error','manual-crash','replay','highscore'):
    for _audio in ('ad15','pc15'):
        if _name not in ('highscore','file-error'):install(_name,_audio)

if __name__=='__main__':unittest.main()
