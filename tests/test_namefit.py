"""integ28: diagnostic names-registry tools (tools/namefit.py, tools/typeinfer.py)."""
import json
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import namefit


class NamefitRenameTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sym = namefit.Symbols()
        cls.address = cls.sym.name_to_addr['gameconfig']

    def test_rename_is_address_bound_and_token_exact(self):
        text = ('extern char gameconfig[];\nint f(void) { struct { int gameconfig; } s; '
                'return gameconfig[1] + s.gameconfig; /* gameconfig */ }\n')
        new, info = namefit.apply_map(text, self.sym, {self.address: 'game_cfg'})
        self.assertIn('extern char game_cfg[];', new)
        self.assertIn('game_cfg[1]', new)
        self.assertIn('s.gameconfig', new)        # member access keeps its field name
        self.assertIn('/* gameconfig */', new)    # comments are not rewritten
        self.assertEqual(info['collisions'], [])

    def test_rename_onto_an_existing_identifier_is_a_collision(self):
        text = 'extern char gameconfig[];\nint f(void) { int detail; return gameconfig[0] + detail; }\n'
        _, info = namefit.apply_map(text, self.sym, {self.address: 'detail'})
        self.assertEqual(info['collisions'], ['detail'])

    def test_aliases_of_one_address_merge_under_one_name(self):
        other = [n for n in self.sym.addr_names[self.address] if n != 'gameconfig'][0]
        text = ('extern char gameconfig[];\nextern char %s[];\n'
                'int f(void) { return gameconfig[0] + %s[1]; }\n' % (other, other))
        new, info = namefit.apply_map(text, self.sym, {self.address: 'game_cfg'})
        self.assertNotIn(other + '[', new)
        self.assertEqual(new.count('extern char game_cfg[];'), 1)


class NamefitCheckTests(unittest.TestCase):
    def test_name_neutral_accepted_tu_stays_exact(self):
        sym = namefit.Symbols()
        tus = [t for t in namefit.load_tus('accepted') if t['id'] == 'obj_seg031']
        self.assertEqual(len(tus), 1)
        text = (ROOT / tus[0]['source']).read_text(encoding='latin-1')
        refs = namefit.referenced_globals(text, sym)
        self.assertTrue(refs)
        name, address = refs[0]
        rows, summary = namefit.run_check(tus, sym, {address: name + 'x'}, [], jobs=1)
        self.assertEqual(rows[0]['result'], 'EXACT', rows[0])


class NamefitCacheTests(unittest.TestCase):
    def test_failed_diagnostic_cache_is_retried_and_not_written(self):
        from compiler import CompileFailure
        tu = next(t for t in namefit.load_tus('accepted') if t['id'] == 'obj_seg031')
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        stale = {'error': 'old preprocessor permission failure'}
        failure = CompileFailure('current compiler unavailable', {}, 'compiler_error')
        with patch.object(namefit.CACHE, 'get', return_value=stale), \
             patch.object(namefit.CACHE, 'put') as store, \
             patch('compiler.compile_source', side_effect=failure) as compile_call:
            result = namefit.compile_summary(tu, text)
        compile_call.assert_called_once()
        store.assert_not_called()
        self.assertEqual(result['error'], 'current compiler unavailable')


class TypeinferTests(unittest.TestCase):
    def test_show_reports_address_and_all_binding_aliases(self):
        out = subprocess.run([sys.executable, str(ROOT / 'tools/typeinfer.py'), '--show', 'gameconfig'],
                             capture_output=True, text=True, timeout=300, cwd=str(ROOT / 'tools'))
        self.assertEqual(out.returncode, 0, out.stderr)
        start = out.stdout.index('{')
        row = json.JSONDecoder().raw_decode(out.stdout[start:])[0]
        self.assertEqual(row['address'], namefit.Symbols().name_to_addr['gameconfig'])
        self.assertIn('_gameconfig', row['names'])


if __name__ == '__main__':
    unittest.main()
