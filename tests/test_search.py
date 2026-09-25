"""Isolation and evidence tests for standalone historical C exploration."""
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import context
import search
from common import read_json, write_json
from omf import ObjectModule


def make_environment(root):
    (root / 'layout').mkdir(parents=True, exist_ok=True)
    (root / 'assets').mkdir(parents=True, exist_ok=True)
    (root / 'tools').mkdir(parents=True, exist_ok=True)
    for name in ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD'):
        (root / 'assets' / name).write_bytes(name.encode())
    (root / 'layout/oracle.lock.json').write_text('{}')
    config = {'files': [], 'flags': [], 'directory': 'tools/compiler'}
    runner = {'path': 'tools/runner.exe'}
    write_json(root / 'layout/toolchain.json', {'profiles': {'msc510-medium': config}, 'runner': runner})
    (root / 'tools/runner.exe').write_bytes(b'runner')
    for name in ('search.py', 'compiler.py', 'preprocessor.py', 'common.py', 'object_probe.py', 'omf.py',
                 'oracle.py', 'mz.py', 'dsi.py', 'exepack.py', 'diagnostics.py',
                 'probe_module.py', 'binder.py', 'code_symbols.py'):
        (root / 'tools' / name).write_text(name)
    return config, runner


class ContextTests(unittest.TestCase):
    def test_reads_direct_inventory_manifest_recipe_and_search_history(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            row = {'name': 'fixture', 'stable_id': 'load_1234', 'start': 10, 'end': 12,
                   'size': 2, 'sha256': 'a' * 64, 'status': 'verified_instruction_boundaries',
                   'confidence': 'high', 'calls': []}
            write_json(root / 'evidence/functions.json', {'functions': [row], 'anchors': [], 'globals': []})
            write_json(root / 'layout/manifest.json', {'owners': [{'id': 'load_1234', 'kind': 'MATCHING_C',
                       'name': 'fixture', 'start': 10, 'end': 12, 'recipe': 'recipes/fixture.json'}]})
            write_json(root / 'recipes/fixture.json', {'name': 'fixture', 'source': 'src/active.c',
                       'profile': 'msc510-medium', 'object_segment': 'UNIT_TEXT', 'public': '_fixture',
                       'target': {'size': 2, 'sha256': 'a' * 64}, 'expected_fixups': [], 'expected_relocations': []})
            write_json(root / 'build/search/one/report.json', {'run_id': 'one', 'created_utc': '2026-01-01',
                       'function': {'name': 'fixture'}, 'candidate': {'name': 'scratch.c', 'sha256': 'b' * 64},
                       'compiler': {'status': 'COMPILED', 'profile': 'msc510-medium'},
                       'observed_output': {'identity': 'c' * 64}, 'comparison': {'summary': {'counts': {}}}})
            with patch.object(context, 'ROOT', root):
                packet = context.packet('load_1234', ['history', 'symbols'])
                listing = context.listing('fixture')
            self.assertEqual(packet['name'], 'fixture')
            self.assertEqual(packet['recipe']['path'], 'recipes/fixture.json')
            self.assertEqual(packet['search_history'][0]['run_id'], 'one')
            self.assertEqual(packet['symbols']['recipe_symbols'], [])
            self.assertEqual(listing['functions'][0]['owner_kind'], 'MATCHING_C')
            self.assertFalse((root / 'recovery').exists())

    def test_context_history_is_bounded_without_dropping_total(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            write_json(root / 'evidence/functions.json', {'functions': [{'name': 'fixture', 'stable_id': 'x'}]})
            write_json(root / 'layout/manifest.json', {'owners': []})
            for i in range(4):
                write_json(root / f'build/search/{i}/report.json', {'run_id': str(i), 'created_utc': str(i),
                           'function': {'name': 'fixture'}})
            with patch.object(context, 'ROOT', root):
                packet = context.packet('fixture')
            self.assertEqual(len(packet['search_history']), 3)
            self.assertEqual(packet['omitted_search_runs'], 1)


class SearchTests(unittest.TestCase):
    def test_candidate_is_frozen_and_search_does_not_change_canonical_inputs(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            config, runner = make_environment(root)
            (root / 'recipes').mkdir()
            (root / 'layout/manifest.json').write_text('{"owners": []}')
            candidate = root / 'elsewhere/scratch.c'
            candidate.parent.mkdir()
            candidate.write_text('int fixture(void) { return 1; }\n')
            work = root / 'build/probes/fake'
            work.mkdir(parents=True)
            (work / 'compiler.log').write_text('compiler completed')
            (work / 'UNIT.OBJ').write_bytes(b'fake-object')
            obj = ObjectModule({'UNIT_TEXT': b'\x55\xc3'}, [], [], [], 'UNIT',
                               {'UNIT_TEXT': 2}, [{'index': 1, 'name': 'UNIT_TEXT', 'length': 2}], [], [])
            receipt = {'profile': 'msc510-medium', 'work_directory': str(work), 'object': {'size': 11}}
            with patch.object(search, 'ROOT', root), \
                 patch('compiler.ROOT', root), \
                 patch('compiler.verify_toolchain', return_value=(config, runner)), \
                 patch('compiler.compile_source', return_value=(obj, receipt)) as compile_call:
                before = candidate.read_bytes()
                first = search.run(candidate)
                compile_call.assert_called_once_with(before, 'msc510-medium')
                report_path = root / 'build/search' / first['run_id'] / 'report.json'
                report = read_json(report_path)
                self.assertEqual(report['compiler']['status'], 'COMPILED')
                self.assertEqual(report['context_hypothesis'].split('.')[0], 'Candidate is a standalone scratch translation unit')
                self.assertIn('UNIT_TEXT', report['observed_output']['segment_names'])
                frozen = root / report['candidate']['frozen_path']
                self.assertEqual(frozen.read_bytes(), before)
                self.assertTrue((root / report['observed_output']['path']).is_file())
                self.assertEqual(candidate.read_bytes(), before)
                self.assertFalse((root / 'src').exists())

    def test_environment_identity_ignores_unrelated_search_history(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            config, runner = make_environment(root)
            with patch.object(search, 'ROOT', root), patch('compiler.ROOT', root), \
                 patch('compiler.verify_toolchain', return_value=(config, runner)):
                first = search._environment('msc510-medium', None, None, None)[1]['sha256']
                write_json(root / 'build/search/unrelated/report.json', {'run_id': 'unrelated', 'observed_output': {'identity': 'x'}})
                second = search._environment('msc510-medium', None, None, None)[1]['sha256']
            self.assertEqual(first, second)

    def test_compile_error_retains_actual_log_and_category(self):
        from compiler import CompileFailure
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            config, runner = make_environment(root)
            candidate = root / 'scratch.c'
            candidate.write_text('int broken(')
            work = root / 'build/probes/failure'
            work.mkdir(parents=True)
            (work / 'compiler.log').write_text('UNIT.C(1): error C1001: syntax error')
            (work / 'UNIT.OBJ').write_bytes(b'partial')
            error = CompileFailure('Compiler failed', {'work_directory': str(work)}, 'COMPILER_ERROR')
            with patch.object(search, 'ROOT', root), patch('compiler.ROOT', root), \
                 patch('compiler.verify_toolchain', return_value=(config, runner)), \
                 patch('compiler.compile_source', side_effect=error):
                report = search.run(candidate)
            self.assertEqual(report['compiler']['status'], 'FAILED')
            self.assertEqual(report['compiler']['category'], 'COMPILER_ERROR')
            self.assertIn('error C1001', report['compiler']['log_excerpt'])
            self.assertTrue((root / report['compiler']['artifacts']['object_path']).is_file())

    def test_null_history_records_and_failed_cli_summary_do_not_crash(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            write_json(root / 'build/search/empty/report.json', {'run_id': 'empty', 'function': None,
                       'candidate': None, 'compiler': None, 'observed_output': None})
            with patch.object(search, 'ROOT', root):
                rows = search.history()
            self.assertEqual(rows['total_runs'], 1)
        failed = {'run_id': 'failed', 'candidate': {'name': 'bad.c'},
                  'compiler': {'status': 'FAILED', 'category': 'COMPILER_ERROR', 'message': 'syntax error'},
                  'observed_output': None, 'comparison': {'status': 'NOT_REQUESTED'},
                  'recipe_check': None, 'equivalent_runs': []}
        output = StringIO()
        with patch.object(search, 'run', return_value=failed), patch.object(sys, 'argv', ['search.py', 'bad.c']), \
                redirect_stdout(output):
            search.main()
        shown = json.loads(output.getvalue())
        self.assertEqual(shown['compiler']['category'], 'COMPILER_ERROR')
        self.assertIsNone(shown['emitted_segments'])

    def test_object_document_encodes_raw_byte_fields(self):
        obj = ObjectModule({'UNIT_TEXT': b'\x90'}, [], [], [], 'UNIT',
                           {'UNIT_TEXT': 1}, [], [], [{'raw': b'\x01\x02'}])
        observed = search._object_document(obj)
        self.assertEqual(observed['comments'][0]['raw']['bytes_hex'], '0102')

    def test_unrelated_manifest_owner_does_not_change_selected_binding_context(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            (root / 'layout').mkdir()
            write_json(root / 'layout/manifest.json', {'owners': [{'id': 'load_selected', 'kind': 'MATCHING_C',
                       'name': 'selected', 'start': 1, 'end': 2}]})
            write_json(root / 'layout/code-symbols.json', {'symbols': {}})
            write_json(root / 'layout/data-symbols.json', {'symbols': {}})
            recipe = {'stable_id': 'load_selected', 'expected_fixups': []}
            oracle_result = (None, b'ignored', {'unpacked_mz': {'relocations': []}})
            with patch.object(search, 'ROOT', root), patch('mz.MZ.parse') as parse:
                parse.return_value.load_image.return_value = b'\x00' * 8
                before = search._recipe_binding_context(recipe, oracle_result)
                write_json(root / 'layout/manifest.json', {'owners': [
                    {'id': 'load_selected', 'kind': 'MATCHING_C', 'name': 'selected', 'start': 1, 'end': 2},
                    {'id': 'raw_unrelated', 'kind': 'UNRESOLVED_RAW', 'start': 4, 'end': 6}]})
                after = search._recipe_binding_context(recipe, oracle_result)
            self.assertEqual(before, after)

    def test_unsupported_object_fallback_keeps_rejection_explicit(self):
        from compiler import CompileFailure
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temp:
            root = Path(temp)
            config, runner = make_environment(root)
            candidate = root / 'scratch.c'
            candidate.write_text('int scratch(void) { return 0; }')
            work = root / 'build/probes/unsupported'
            work.mkdir(parents=True)
            (work / 'compiler.log').write_text('compiled with local symbols')
            (work / 'UNIT.OBJ').write_bytes(b'unsupported-omf')
            error = CompileFailure('Unsupported OMF record BAKPAT', {'work_directory': str(work)}, 'UNSUPPORTED_OBJECT')
            with patch.object(search, 'ROOT', root), patch('compiler.ROOT', root), \
                 patch('compiler.verify_toolchain', return_value=(config, runner)), \
                 patch('compiler.compile_source', side_effect=error), \
                 patch('object_probe.read_object', side_effect=ValueError('BAKPAT unsupported')):
                report = search.run(candidate)
            self.assertEqual(report['compiler']['status'], 'FAILED')
            self.assertEqual(report['compiler']['research_fallback']['status'], 'UNAVAILABLE')
            self.assertIn('BAKPAT unsupported', report['compiler']['research_fallback']['error'])
            self.assertTrue((root / report['compiler']['artifacts']['object_path']).is_file())


if __name__ == '__main__':
    unittest.main()
