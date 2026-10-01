"""Read-only parser coverage for the optional original asset corpus."""
from contextlib import redirect_stdout
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'/'porting'))

import audio_bank  # noqa: E402
import format_reference  # noqa: E402


class PortingAssetInventoryTests(unittest.TestCase):
    def test_diagnostics_and_save_directories_are_not_counted_as_assets(self):
        with tempfile.TemporaryDirectory(prefix='stunts-asset-inventory-') as temporary:
            root = Path(temporary) / 'assets'
            root.mkdir()
            (root / 'NOTICE.TXT').write_bytes(b'opaque local installation note')
            (root / 'diagnostics').mkdir()
            (root / 'diagnostics' / 'trace.jsonl').write_bytes(b'{}\n')
            (root / 'saves').mkdir()
            report = Path(temporary) / 'report.json'
            with patch('sys.argv', ['format_reference.py', str(root), '--report', str(report)]), \
                 redirect_stdout(io.StringIO()):
                self.assertEqual(format_reference.main(), 0)
            result = json.loads(report.read_text(encoding='utf-8'))
            self.assertEqual(result['asset_count'], 1)
            self.assertEqual(result['catalogued_count'], 1)
            self.assertEqual(result['extension_counts'], {'.TXT': 1})
            self.assertEqual(result['error_count'], 0)


@unittest.skipUnless((ROOT/'assets').is_dir(), 'original assets are not provisioned')
class PortingAssetParserTests(unittest.TestCase):
    def test_format_parser_reads_assets_and_writes_report_under_build(self):
        report = ROOT/'build/porting/test-asset-validation.json'
        before = sorted((p.name, p.stat().st_size, p.stat().st_mtime_ns)
                        for p in (ROOT/'assets').iterdir() if p.is_file())
        with patch('sys.argv', ['format_reference.py', str(ROOT/'assets'), '--report', str(report)]), \
             redirect_stdout(io.StringIO()):
            self.assertEqual(format_reference.main(), 0)
        result = json.loads(report.read_text(encoding='utf-8'))
        self.assertEqual(result['error_count'], 0)
        self.assertEqual(result['asset_count'], len(before))
        self.assertTrue(result['roundtrip_checks']['all_identical'])
        after = sorted((p.name, p.stat().st_size, p.stat().st_mtime_ns)
                       for p in (ROOT/'assets').iterdir() if p.is_file())
        self.assertEqual(before, after)

    def test_audio_bank_parser_reads_all_supplied_banks_and_writes_under_build(self):
        report = ROOT/'build/porting/test-audio-bank-validation.json'
        before = sorted((p.name, p.stat().st_size, p.stat().st_mtime_ns)
                        for p in (ROOT/'assets').iterdir() if p.is_file())
        with patch('sys.argv', ['audio_bank.py', '--verify-all', '--assets', str(ROOT/'assets'),
                                '--json', str(report)]), \
             redirect_stdout(io.StringIO()):
            self.assertEqual(audio_bank.main(), 0)
        result = json.loads(report.read_text(encoding='utf-8'))
        self.assertEqual(audio_bank.summarize(result)['files'], 13)
        self.assertEqual(audio_bank.summarize(result)['voice_records'], 105)
        after = sorted((p.name, p.stat().st_size, p.stat().st_mtime_ns)
                       for p in (ROOT/'assets').iterdir() if p.is_file())
        self.assertEqual(before, after)


if __name__ == '__main__':
    unittest.main()
