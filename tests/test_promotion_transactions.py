"""Promotion must preserve the FAST baseline across canonical metadata writes."""
import sys
import tempfile
import unittest
from contextlib import ExitStack
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import identity, sha, write_json
import check_candidate
import check_library


class PromotionTransactionTests(unittest.TestCase):
    def exercise_failure(self, library, inject_edit):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as directory:
            root = Path(directory)
            for folder in ['layout', 'recipes', 'src', 'recovery/candidates', 'build/exact']:
                (root/folder).mkdir(parents=True, exist_ok=True)
            manifest = {'owners':[{'id':'raw', 'kind':'UNRESOLVED_RAW', 'start':0, 'end':10}]}
            write_json(root/'layout/manifest.json', manifest)
            write_json(root/'layout/production-plan.json', {'modules':[]})
            (root/'build/exact/acceptance.json').write_text('stale PASS')
            candidate = {'id':'new', 'name':'new', 'stable_id':'new', 'start':1, 'end':2,
                         'kind':'KNOWN_TOOLCHAIN_LIBRARY', 'library':'pinned.lib', 'module':'m'}
            if library:
                write_json(root/'layout/library-candidates.json', {'new':candidate})
            else:
                candidate['source'] = 'recovery/candidates/new.c'
                (root/candidate['source']).write_bytes(b'int f(void) { return 0; }\n')
                write_json(root/'recipes/new.json', candidate)
            original = {p:p.read_bytes() for p in [root/'layout/manifest.json', root/'layout/production-plan.json']}
            if not library: original[root/'recipes/new.json'] = (root/'recipes/new.json').read_bytes()
            def snapshot():
                return {p.relative_to(root).as_posix():sha(p.read_bytes())
                        for folder in ['layout', 'recipes', 'src', 'recovery/candidates']
                        for p in (root/folder).rglob('*') if p.is_file()}
            canonical_calls = []
            def build(manifest=None, *args, **kwargs):
                if manifest is None:
                    canonical_calls.append(True)
                    raise ValueError('canonical failure')
                return {'inputs':snapshot(), 'executable':identity(b'fixture')}
            def write(path, value):
                write_json(path, value)
                if inject_edit and Path(path) == root/'layout/production-plan.json':
                    (root/'src/unrelated.c').write_text('unexpected change')
            module = check_library if library else check_candidate
            with ExitStack() as stack:
                for key, value in [('ROOT', root), ('inputs', snapshot), ('build', build), ('write_json', write)]:
                    stack.enter_context(patch.object(module, key, value))
                if library:
                    stack.enter_context(patch.object(module, 'verify', return_value=(b'', b'', {})))
                    stack.enter_context(patch.object(module.MZ, 'parse', return_value=SimpleNamespace(load_image=lambda _:bytes(10), relocations=[])))
                    stack.enter_context(patch.object(module, 'bind_library', return_value=(b'X', {})))
                else:
                    stack.enter_context(patch.object(module, 'project_path', side_effect=lambda value:root/value))
                    stack.enter_context(patch.object(module, 'probe', return_value=(b'X', {'source':identity((root/candidate['source']).read_bytes())})))
                with self.assertRaisesRegex(ValueError, 'canonical|Unexpected'):
                    if library: module.check('new', promote=True)
                    else: module.check('new', promote=True, scope=False)
            self.assertEqual(bool(canonical_calls), not inject_edit)
            for path, data in original.items(): self.assertEqual(path.read_bytes(), data)
            self.assertFalse((root/'build/promotion.lock').exists())
            self.assertFalse((root/'build/exact/acceptance.json').exists())
            self.assertFalse((root/'src/new.c').exists())
            # Unrelated user changes are detected but never erased by rollback.
            self.assertEqual((root/'src/unrelated.c').exists(), inject_edit)

    def test_library_rejects_edit_during_canonical_writes(self): self.exercise_failure(True, True)
    def test_c_rejects_edit_during_canonical_writes(self): self.exercise_failure(False, True)
    def test_library_rolls_back_canonical_build_failure(self): self.exercise_failure(True, False)
    def test_c_rolls_back_canonical_build_failure(self): self.exercise_failure(False, False)


if __name__ == '__main__': unittest.main()
