"""Replay tool contracts; deliberately does not run the deferred corpus test."""
import json
from pathlib import Path
import tempfile
import unittest

from tools.porting.replay_corpus import ROOT, metadata, replay_script, snapshots, state_fields, pack, discard_asset_copy
from tools.porting.format_reference import parse_replay


class ReplayCorpusToolTests(unittest.TestCase):
    def test_default_metadata_uses_embedded_track_and_exact_input_extent(self):
        raw=(ROOT/'assets/DEFAULT.RPL').read_bytes()
        parsed=parse_replay(raw);info=metadata(ROOT/'assets/DEFAULT.RPL')
        self.assertEqual(info['frames'],parsed['frame_count'])
        self.assertEqual(info['rate'],parsed['fps'])
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'bad.rpl';path.write_bytes(raw[:-1])
            with self.assertRaisesRegex(ValueError,'extent'):metadata(path)
            path.write_bytes(raw+b'\0')
            with self.assertRaisesRegex(ValueError,'extent'):metadata(path)

    def test_step_identity_preserves_replay_epochs_without_content_alignment(self):
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'trace.jsonl';records=[]
            for n in (1,2,3,1,2):
                raw=bytearray(1120);raw[320:322]=n.to_bytes(2,'little')
                records.append(dict(event_type='state_snapshot',bytes_hex=raw.hex()))
            path.write_text('\n'.join(map(json.dumps,records)))
            self.assertEqual([(e,n) for e,n,_ in snapshots(path)],[(1,1),(1,2),(1,3),(2,1),(2,2)])

    def test_menu_script_loads_then_rewinds_and_plays(self):
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'input.json';replay_script(path)
            events=json.loads(path.read_text())['events']
        enter_visits=[r['visit'] for r in events if r['phase']==22 and r['channel']=='dos.keyboard.scancodes']
        self.assertEqual(enter_visits,[1,2])
        self.assertTrue(any(r['phase']==11 for r in events))

    def test_all_state_bytes_have_a_diagnostic_field_label(self):
        covered=[i for _,at,size in state_fields() for i in range(at,at+size)]
        self.assertEqual(sorted(covered),list(range(1120)))

    def test_packing_never_overwrites_an_expectation(self):
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);(root/'reference.json.gz').write_bytes(b'existing')
            with self.assertRaisesRegex(ValueError,'overwrite'):pack(root,{})

    def test_cleanup_protects_original_assets(self):
        with self.assertRaisesRegex(ValueError,'outside'):discard_asset_copy(ROOT)
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);(root/'assets').mkdir();(root/'assets/copy').write_bytes(b'copy')
            discard_asset_copy(root)
            self.assertFalse((root/'assets').exists())


if __name__=='__main__':unittest.main()
