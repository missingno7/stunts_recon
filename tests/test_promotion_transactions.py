"""Publication locking, crash recovery, and no-write acceptance checks."""
import subprocess
import sys
import tempfile
import unittest
from contextlib import ExitStack, contextmanager
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / 'tools'
sys.path.insert(0, str(TOOLS))

import promote
import transaction


class InterruptedWrite(RuntimeError):
    pass


class PromotionTransactionTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT / 'build')
        self.root = Path(self.temp.name)
        (self.root / 'build').mkdir(parents=True, exist_ok=True)
        (self.root / 'src').mkdir(parents=True)
        (self.root / 'recipes').mkdir()
        (self.root / 'layout').mkdir()

    def tearDown(self):
        self.temp.cleanup()

    @contextmanager
    def transaction_root(self):
        with patch.object(transaction, 'ROOT', self.root):
            yield

    @contextmanager
    def mock_promoter(self, input_values, probe_action=None):
        manifest = {'owners': [{'id': 'raw', 'kind': 'UNRESOLVED_RAW',
                                'start': 0, 'end': 10}]}
        manifest_path = self.root / 'layout/manifest.json'
        manifest_path.write_text('fixture manifest')
        candidate = self.root / 'candidate.c'
        source = b'int new_function(void) { return 1; }\n'
        candidate.write_bytes(source)
        recipe = {'id': 'new_function', 'stable_id': 'new_function_id',
                  'start': 1, 'end': 2, 'source': 'src/new_function.c',
                  'profile': 'msc510-medium', 'object_segment': 'UNIT_TEXT',
                  'public': '_new_function', 'target': {'size': 1, 'sha256': '0' * 64},
                  'expected_fixups': [], 'expected_relocations': []}
        oracle = (b'', b'', {'unpacked_mz': {'relocations': []}})
        built = {'inputs': input_values[0], 'executable': {'sha256': 'fixture'},
                 'relocation_count': 0}

        def fake_probe(*args, **kwargs):
            if probe_action is not None:
                probe_action(candidate)
            return b'X', {'fixture': True}

        with ExitStack() as stack:
            stack.enter_context(patch.object(transaction, 'ROOT', self.root))
            stack.enter_context(patch.object(promote, 'ROOT', self.root))
            stack.enter_context(patch.object(promote, 'inputs', side_effect=input_values))
            stack.enter_context(patch.object(promote, 'read_json', return_value=manifest))
            stack.enter_context(patch.object(promote, 'verify', return_value=oracle))
            stack.enter_context(patch.object(
                promote.MZ, 'parse',
                return_value=SimpleNamespace(load_image=lambda _data: bytes(10))))
            stack.enter_context(patch.object(promote, 'checked_function',
                                              return_value={'start': 1, 'end': 2}))
            stack.enter_context(patch.object(promote, 'default_recipe', return_value=recipe))
            stack.enter_context(patch.object(promote, 'probe', side_effect=fake_probe))
            stack.enter_context(patch.object(promote, 'build', return_value=built))
            yield candidate, source, manifest_path

    def test_os_lock_excludes_another_process(self):
        script = (
            "import sys\n"
            "from pathlib import Path\n"
            "sys.path.insert(0, sys.argv[1])\n"
            "import transaction\n"
            "transaction.ROOT = Path(sys.argv[2])\n"
            "try:\n"
            "    with transaction.exclusive():\n"
            "        pass\n"
            "except ValueError:\n"
            "    raise SystemExit(7)\n"
            "raise SystemExit(0)\n"
        )
        with self.transaction_root(), transaction.exclusive():
            blocked = subprocess.run([sys.executable, '-c', script, str(TOOLS), str(self.root)],
                                     capture_output=True, text=True, check=False)
            self.assertEqual(blocked.returncode, 7, blocked.stderr)
        available = subprocess.run([sys.executable, '-c', script, str(TOOLS), str(self.root)],
                                   capture_output=True, text=True, check=False)
        self.assertEqual(available.returncode, 0, available.stderr)
        self.assertTrue((self.root / 'build/promotion.lock').exists())

    def test_interrupted_multi_file_apply_recovers_from_journal(self):
        source = self.root / 'src/one.c'
        manifest = self.root / 'layout/manifest.json'
        source.write_bytes(b'old source')
        manifest.write_bytes(b'old manifest')
        changes = {'src/one.c': b'new source',
                   'recipes/one.json': b'new recipe',
                   'layout/manifest.json': b'new manifest'}
        with self.transaction_root():
            rows = transaction.prepare(changes)
            real_atomic_bytes = transaction.atomic_bytes
            writes = 0

            def crash_after_first_write(path, data):
                nonlocal writes
                real_atomic_bytes(path, data)
                writes += 1
                if writes == 1:
                    raise InterruptedWrite('simulated process interruption')

            with patch.object(transaction, 'atomic_bytes', side_effect=crash_after_first_write):
                with self.assertRaises(InterruptedWrite):
                    transaction.apply(rows)
            self.assertEqual(source.read_bytes(), b'new source')
            self.assertEqual(manifest.read_bytes(), b'old manifest')
            self.assertTrue(transaction.journal_path().exists())
            transaction.recover()
        self.assertEqual(source.read_bytes(), b'old source')
        self.assertEqual(manifest.read_bytes(), b'old manifest')
        self.assertFalse((self.root / 'recipes/one.json').exists())
        self.assertFalse((self.root / 'build/publication.json').exists())

    def test_recovery_refuses_to_clobber_a_conflicting_user_edit(self):
        source = self.root / 'src/one.c'
        manifest = self.root / 'layout/manifest.json'
        source.write_bytes(b'old source')
        manifest.write_bytes(b'old manifest')
        changes = {'src/one.c': b'new source',
                   'layout/manifest.json': b'new manifest'}
        with self.transaction_root():
            rows = transaction.prepare(changes)
            transaction.apply(rows)
            manifest.write_bytes(b'user edit')
            with self.assertRaisesRegex(ValueError, 'conflicting edit'):
                transaction.rollback()
            self.assertEqual(source.read_bytes(), b'new source')
            self.assertEqual(manifest.read_bytes(), b'user edit')
            self.assertTrue(transaction.journal_path().exists())
            manifest.write_bytes(b'new manifest')
            transaction.recover()
        self.assertEqual(source.read_bytes(), b'old source')
        self.assertEqual(manifest.read_bytes(), b'old manifest')

    def test_state_race_stops_before_any_canonical_write(self):
        original = {'state': 'before'}
        changed = {'state': 'changed'}
        with self.mock_promoter([original, changed]) as (candidate, source, manifest_path):
            manifest_before = manifest_path.read_bytes()
            with self.assertRaisesRegex(ValueError, 'Canonical inputs changed'):
                promote.promote('new_function', candidate, verify_only=True)
            self.assertEqual(manifest_path.read_bytes(), manifest_before)
            self.assertFalse((self.root / 'src/new_function.c').exists())
            self.assertFalse((self.root / 'recipes/new_function.json').exists())
            self.assertFalse((self.root / 'build/acceptance/new_function/report.json').exists())
            self.assertEqual(candidate.read_bytes(), source)

    def test_candidate_source_race_stops_before_any_canonical_write(self):
        baseline = {'state': 'stable'}

        def change_candidate(path):
            path.write_bytes(b'concurrent candidate edit')

        with self.mock_promoter([baseline, baseline], change_candidate) as (
                candidate, source, manifest_path):
            manifest_before = manifest_path.read_bytes()
            with self.assertRaisesRegex(ValueError, 'Candidate source changed'):
                promote.promote('new_function', candidate, verify_only=True)
            self.assertEqual(manifest_path.read_bytes(), manifest_before)
            self.assertFalse((self.root / 'src/new_function.c').exists())
            self.assertFalse((self.root / 'recipes/new_function.json').exists())
            self.assertFalse((self.root / 'build/acceptance/new_function/report.json').exists())
            self.assertEqual(candidate.read_bytes(), b'concurrent candidate edit')
            self.assertNotEqual(candidate.read_bytes(), source)

    def test_verify_only_writes_a_report_without_changing_canonical_state(self):
        baseline = {'state': 'stable'}
        with self.mock_promoter([baseline, baseline]) as (candidate, source, manifest_path):
            manifest_before = manifest_path.read_bytes()
            result = promote.promote('new_function', candidate, verify_only=True)
            self.assertEqual(result['status'], 'VERIFIED_ONLY')
            self.assertEqual(manifest_path.read_bytes(), manifest_before)
            self.assertEqual(candidate.read_bytes(), source)
            self.assertFalse((self.root / 'src/new_function.c').exists())
            self.assertFalse((self.root / 'recipes/new_function.json').exists())
            self.assertTrue((self.root / 'build/acceptance/new_function/report.json').exists())


if __name__ == '__main__':
    unittest.main()
