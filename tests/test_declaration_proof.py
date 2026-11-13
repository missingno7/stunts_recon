"""Declaration proof shares the strict rename comparator and real journal."""
import copy
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import declaration_proof as declarations
import object_equivalence
import preprocessor
import rename
import transaction
from common import identity, json_bytes
from object_probe import read_object, DEBUG_SEGMENT_POLICY
from omf import OmfReader
sys.path.insert(0, str(Path(__file__).resolve().parent))
from test_rename_transaction import (record, name, object_fixture, external_fixture,
                                           fixture_files, communal_fixture)


def debug_fixture(symbols=b'abc', types=b'xyz', line=10, payload=b'\x9a\0\0\0\0\xcb'):
    records = []
    for kind, body in OmfReader.records(object_fixture(payload=payload)[1]):
        if kind == 0x96:
            body += b''.join(name(n) for n in ('$$SYMBOLS', 'DEBSYM', '$$TYPES', 'DEBTYP'))
        if kind == 0xA0:
            records.extend([record(0xA0, b'\x02\0\0' + symbols),
                            record(0xA0, b'\x03\0\0' + types),
                            record(0x94, b'\0\x01' + struct.pack('<HH', line, 0))])
        records.append(record(kind, body))
        if kind == 0x98:
            records.extend([record(0x98, b'\x20' + struct.pack('<H', len(symbols)) + b'\x03\x04\0'),
                            record(0x98, b'\x20' + struct.pack('<H', len(types)) + b'\x05\x06\0')])
    raw = b''.join(records)
    return read_object(raw, debug_segments=DEBUG_SEGMENT_POLICY), raw


class DeclarationComparisonTests(unittest.TestCase):
    def test_shared_implementation(self):
        self.assertIs(rename.compare_objects, object_equivalence.compare_objects)
        self.assertIs(rename.refresh_expectations, object_equivalence.refresh_expectations)

    def test_ordered_named_fixups_and_extents(self):
        before = external_fixture(['_target', '_unused'], 1)
        self.assertIsNone(declarations.declaration_compare(before, external_fixture(['_unused', '_target'], 2), {}))
        self.assertIsNotNone(declarations.declaration_compare(before, external_fixture(['_unused', '_target'], 1), {}))
        changed = copy.deepcopy(before)
        changed[0].segment_lengths['CODE'] += 1
        self.assertIsNotNone(declarations.declaration_compare(before, changed, {}))

    def test_unused_api_imports_may_change_only_in_declaration_mode(self):
        before = external_fixture(['_target'], 1)
        after = external_fixture(['_unused', '_target'], 2)
        self.assertIsNotNone(rename.compare_objects(before, after, {}))
        self.assertIsNone(declarations.declaration_compare(before, after, {}))
        self.assertIsNotNone(declarations.declaration_compare(before, external_fixture(['_unused'], 1), {}))

    def test_strict_option_keeps_whole_object_policy_for_macro_changes(self):
        before = external_fixture(['_target', '_unused'], 1)
        after = external_fixture(['_unused', '_target'], 2)
        self.assertIsNone(declarations.declaration_compare(before, after, {}))
        self.assertIsNotNone(declarations.strict_object_compare(before, after, {}))
        self.assertIsNone(declarations.strict_object_compare(before, before, {}))

    def test_external_frame_change_is_still_refused(self):
        before = external_fixture(['_target', '_unused'], 1)
        after = external_fixture(['_unused', '_target'], 2)
        before[0].linker_fixups[0].update(frame_kind='external', frame='_target', frame_index=1)
        after[0].linker_fixups[0].update(frame_kind='external', frame='_unused', frame_index=1)
        self.assertIsNotNone(declarations.declaration_compare(before, after, {}))

    def test_ordinary_line_records_remain_exact(self):
        before = object_fixture()
        raw = b''.join(record(k, b) + (record(0x94, b'\0\x01\1\0\0\0') if k == 0xA0 else b'')
                       for k, b in OmfReader.records(before[1]))
        self.assertIsNotNone(declarations.declaration_compare(before, (read_object(raw), raw), {}))

    def test_debug_contents_lengths_and_lines_may_change_only_explicitly(self):
        before, after = debug_fixture(), debug_fixture(b'changed symbols', b'new types', 99)
        self.assertIsNotNone(rename.compare_objects(before, after, {}))
        self.assertIsNone(declarations.declaration_compare(before, after, {}))

    def test_debug_does_not_exempt_code_or_linked_fixups(self):
        before = debug_fixture()
        self.assertIsNotNone(declarations.declaration_compare(before, debug_fixture(payload=b'\x9a\0\0\0\0\xc3'), {}))
        after = debug_fixture()
        after[0].linker_fixups[0]['offset'] += 1
        self.assertIsNotNone(declarations.declaration_compare(before, after, {}))

    def test_debug_policy_requires_both_decoded_segments(self):
        with self.assertRaisesRegex(ValueError, 'both reviewed'):
            object_equivalence.compare_objects(object_fixture(), object_fixture(), {}, debug_policy=DEBUG_SEGMENT_POLICY)

    def test_communal_order_deferred_but_size_remains_exact(self):
        before = communal_fixture([('_a', 2), ('_b', 4)])
        self.assertIsNone(declarations.declaration_compare(before, communal_fixture([('_b', 4), ('_a', 2)]), {}))
        self.assertIsNotNone(declarations.declaration_compare(before, communal_fixture([('_b', 4), ('_a', 3)]), {}))


class CandidateAndHeaderTests(unittest.TestCase):
    def test_candidate_scope_duplicate_and_dos_names(self):
        files = fixture_files()
        with tempfile.TemporaryDirectory() as d:
            candidate = Path(d) / 'unit.c'; candidate.write_bytes(b'int old_function(void) { return 1; }\n')
            header = Path(d) / 'api.h'; header.write_bytes(b'extern int service(void);\n')
            source, changes = declarations.candidate_changes('owner', candidate, ['include/api.h='+str(header)], files)
            self.assertEqual(source, 'game/owner.c')
            self.assertEqual(changes, {'include/api.h': header.read_bytes()})
            for spec in ('include/longfilename.h=', 'include/../api.h=', 'game/api.h='):
                with self.assertRaises(ValueError):
                    declarations.candidate_changes('owner', candidate, [spec+str(header)], files)
            with self.assertRaisesRegex(ValueError, 'Duplicate'):
                declarations.candidate_changes('owner', candidate, ['include/api.h='+str(header)]*2, files)

    def test_new_header_authorization_is_exact_and_scoped_and_closure_persists(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d); (root/'include').mkdir(); (root/'recipes').mkdir(); (root/'layout').mkdir()
            (root/'layout/toolchain.json').write_bytes(json_bytes({'profiles': {'fixture': {'directory': '.', 'files': []}}}))
            raw = b'extern int service(void);\n'
            header = root/'include/api.h'; header.write_bytes(raw)
            source = b'#include "api.h"\n'
            with patch.object(preprocessor, 'ROOT', root), patch.object(preprocessor, 'git_tracked', return_value=False):
                with self.assertRaisesRegex(ValueError, 'not tracked'):
                    preprocessor.prepare(source, 'fixture')
                with preprocessor.staged_headers({'include/api.h': raw}):
                    expanded, closure = preprocessor.prepare(source, 'fixture')
                    self.assertIn(raw, expanded)
                    header.write_bytes(raw+b'bad')
                    with self.assertRaisesRegex(ValueError, 'not tracked'):
                        preprocessor.prepare(source, 'fixture')
                    header.write_bytes(raw)
                with self.assertRaisesRegex(ValueError, 'not tracked'):
                    preprocessor.prepare(source, 'fixture')
                (root/'recipes/owner.json').write_bytes(json_bytes({'preprocessor_closure': closure}))
                self.assertEqual(preprocessor.prepare(source, 'fixture')[1], closure)
                header.write_bytes(raw+b'bad')
                with self.assertRaisesRegex(ValueError, 'not tracked'):
                    preprocessor.prepare(source, 'fixture')

    def test_new_header_and_recipe_share_rollback(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            with patch.object(transaction, 'ROOT', root):
                rows = transaction.prepare({'include/api.h': b'extern int f(void);\n', 'recipes/unit.json': b'{}\n'})
                transaction.apply(rows)
                self.assertTrue((root/'include/api.h').is_file())
                transaction.rollback()
                self.assertFalse((root/'include/api.h').exists())
                self.assertFalse((root/'recipes/unit.json').exists())
                self.assertFalse(transaction.journal_path().exists())

    def test_verify_only_and_failed_publication_leave_canonical_tree_unchanged(self):
        import base64
        from common import sha
        for verify_only in (True, False):
            with self.subTest(verify_only=verify_only), tempfile.TemporaryDirectory() as d:
                root = Path(d)
                files = fixture_files()
                for n, raw in files.items():
                    (root/n).parent.mkdir(parents=True, exist_ok=True); (root/n).write_bytes(raw)
                (root/'build/python').mkdir(parents=True)
                (root/'assets').mkdir()
                for n in ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD'):
                    (root/'assets'/n).write_bytes(b'fixture')
                candidate = root/'build/candidate.c'
                candidate.write_bytes(b'#include "api.h"\n' + files['game/owner.c'])
                header = root/'build/api.h'; header.write_bytes(b'extern int service(void);\n')
                before = rename.canonical_files(root)
                def run(argv, *, cwd, env, log, timeout):
                    self.assertEqual(env['STUNTS_BUILD_WORKERS'], '1')
                    self.assertGreater(timeout, 0)
                    if '--canonical-proof' in argv:
                        return 1  # Inject a proof failure after the journal is applied.
                    request = json.loads(Path(argv[-1]).read_bytes())
                    changes = {n: base64.b64decode(raw) for n, raw in request['changes'].items()}
                    for n, raw in changes.items():
                        (Path(cwd)/n).parent.mkdir(parents=True, exist_ok=True)
                        (Path(cwd)/n).write_bytes(raw)
                    Path(request['output']).write_bytes(json_bytes({'changes': list(changes),
                        'whole_image': {'historical_baseline': {'loaded_library_modules': [], 'explicit_objects': []}}}))
                    return 0
                with patch.object(declarations, 'ROOT', root), patch.object(rename, 'ROOT', root), \
                     patch.object(transaction, 'ROOT', root), patch.object(rename, 'tracked_snapshot', return_value=files), \
                     patch('build_exact.inputs', side_effect=lambda: {n:sha(raw) for n,raw in rename.canonical_files(root).items()}), \
                     patch.object(declarations, 'bounded_run', side_effect=run):
                    if verify_only:
                        result = declarations.promote_declarations('owner', candidate, ['include/api.h='+str(header)], True)
                        self.assertEqual(result['status'], 'DECLARATIONS_VERIFIED_ONLY')
                    else:
                        with self.assertRaisesRegex(ValueError, 'Canonical declaration proof failed'):
                            declarations.promote_declarations('owner', candidate, ['include/api.h='+str(header)])
                    self.assertEqual(rename.canonical_files(root), before)
                    self.assertFalse(transaction.journal_path().exists())
                    self.assertFalse((root/'include/api.h').exists())

    def test_header_change_refreshes_all_consumers(self):
        from promote import _cosmetic_closure
        files = fixture_files()
        files['include/api.h'] = b'extern int f(void);\n'
        recipe = json.loads(files['recipes/owner.json'])
        recipe['preprocessor_closure'] = [{'path': 'include/api.h', **identity(files['include/api.h'])}]
        files['recipes/owner.json'] = json_bytes(recipe)
        changes = {'include/api.h': b'extern int f(void); /* shared API */\n'}
        compiled = object_fixture()
        def compiler(recipe, source):
            return *compiled, {}
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            for n, raw in files.items():
                (root/n).parent.mkdir(parents=True, exist_ok=True); (root/n).write_bytes(raw)
            engine = rename.ProofEngine(root, files, compiler=compiler, image=lambda: {'status': 'HYBRID_EXACT'},
                                        changes_factory=lambda f,r: dict(changes), comparator=declarations.declaration_compare)
            def closure(recipe, source):
                return 'preprocessor_closure', [{'path': 'include/api.h', **identity((root/'include/api.h').read_bytes())}]
            with patch('promote._cosmetic_closure', side_effect=closure):
                result = engine.trial([], whole=True)
            self.assertFalse(result['failures'])
            self.assertEqual(json.loads(engine.current['recipes/owner.json'])['preprocessor_closure'],
                             closure(recipe, b'')[1])
            self.assertEqual(files['include/api.h'], b'extern int f(void);\n')


class SourceCitationTests(unittest.TestCase):
    def test_only_the_current_declaration_citation_changes(self):
        from types import SimpleNamespace
        from declaration_sources import refresh_source_bases
        manifest = {'bss_owners': [{'id': 'c_common', 'communals': [{
            'name': '_buffer', 'address': 100, 'size': 128, 'declarers': ['unit'],
            'size_basis': {'kind': 'declared-type', 'declaration': 'char buffer[128];'}}]}]}
        files = {'layout/manifest.json': json_bytes(manifest), 'game/unit.c': b'char buffer[128];\n'}
        engine = SimpleNamespace(files=files, recipes={'recipes/unit.json': {
            'owners': ['unit'], 'recipe': {'source': 'game/unit.c'}}})
        changes = {'game/unit.c': b'#define CAPACITY 128\nchar buffer[CAPACITY];\n'}
        refresh_source_bases(engine, changes, ['recipes/unit.json'])
        after = json.loads(changes['layout/manifest.json'])
        self.assertEqual(after['bss_owners'][0]['communals'][0]['size_basis']['declaration'], 'char buffer[CAPACITY];')
        after['bss_owners'][0]['communals'][0]['size_basis']['declaration'] = 'char buffer[128];'
        self.assertEqual(after, manifest)
        self.assertEqual(json.loads(files['layout/manifest.json']), manifest)

    def test_locator_excludes_function_locals_and_externs(self):
        from declaration_sources import statements
        import cdecls
        text = 'char buffer[CAP];\nvoid f(void) { char local[128]; }\nextern int imported;\n'
        self.assertEqual(list(statements(text)), ['char buffer[CAP];', 'extern int imported;'])

    def test_a_communal_size_mismatch_never_reaches_the_citation_updater(self):
        from unittest.mock import Mock
        files = fixture_files()
        calls = []
        def compiler(recipe, source):
            calls.append(source)
            obj, raw = communal_fixture([('_buffer', 2 if len(calls) == 1 else 4)])
            return obj, raw, {}
        hook = Mock()
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            for name, raw in files.items():
                (root/name).parent.mkdir(parents=True, exist_ok=True); (root/name).write_bytes(raw)
            engine = rename.ProofEngine(root, files, compiler=compiler, image=lambda: {},
                changes_factory=lambda f,r: {'game/owner.c': b'int changed;\n'},
                comparator=declarations.declaration_compare, post_compare=hook)
            with patch('promote._cosmetic_closure', return_value=('preprocessor_closure', [])):
                result = engine.trial([], whole=True)
            self.assertTrue(result['failures'])
            hook.assert_not_called()


class DeclarationScopeTests(unittest.TestCase):
    def reasons(self, old, new, headers=None, changed_headers=None):
        from declaration_scope import requires_raw_objects
        files = {'game/unit.c': old.encode(), **(headers or {})}
        return requires_raw_objects(files, {'game/unit.c': new.encode(), **(changed_headers or {})})

    def test_imports_records_comments_and_block_externs_are_declarations(self):
        old = 'extern int target(int);\nint f(int x) { return target(x); }'
        new = 'struct V { int x,y; };\nextern int unused(void);\nint f(int x) { extern int target(int); /* move */ return target(x); }'
        self.assertEqual(self.reasons(old, new), [])

    def test_body_literals_labels_register_and_signatures_require_raw_objects(self):
        old = 'int f(int x) { register int y; label: y=x+16; return y; }'
        for new in (old.replace('+16', '+VALUE'), old.replace('register ', ''),
                    old.replace('label:', ''), old.replace('int x)', 'unsigned x)')):
            self.assertTrue(self.reasons(old, new))

    def test_included_body_and_transitive_macro_changes_require_raw_objects(self):
        source = '#include "api.h"\nint f(void) { return VALUE; }'
        headers = {'include/api.h': b'#define VALUE BASE\n#define BASE 16\n'}
        changed = {'include/api.h': b'#define VALUE BASE\n#define BASE 32\n'}
        self.assertTrue(self.reasons(source, source, headers, changed))
        self.assertTrue(self.reasons(source, source, headers,
            {'include/api.h': headers['include/api.h'] + b'int g(void) { return 0; }\n'}))

    def test_unused_header_growth_and_relocated_equal_macros_stay_declarations(self):
        old = '#define VALUE 16\nint f(void) { return VALUE; }'
        new = '#include "api.h"\nint f(void) { return VALUE; }'
        self.assertEqual(self.reasons(old, new, {},
            {'include/api.h': b'#ifndef API_H\n#define API_H\n#define VALUE 16\nextern int unused(void);\n#endif\n'}), [])

    def test_existing_conditional_dependency_and_body_directives_require_raw(self):
        old = '#define SELECT 1\n#if SELECT\nint f(void) { return 1; }\n#endif'
        self.assertTrue(self.reasons(old, old.replace('SELECT 1', 'SELECT 0')))
        old = 'int f(void) {\n#if SELECT\nreturn 1;\n#else\nreturn 0;\n#endif\n}'
        self.assertTrue(self.reasons(old, old.replace('#if SELECT', '#if OTHER')))

    def test_uncertain_syntax_never_selects_the_declaration_comparator(self):
        self.assertTrue(self.reasons('int f(void) {', 'int f(void) { /* changed */'))
        self.assertTrue(self.reasons('int f(a)\nint a;\n{ return a; }',
                                    'int f(a)\nint a;\n{ return a+0; }'))

    def test_new_outer_conditions_and_macro_definition_order_require_raw(self):
        old = 'int f(void) { return 1; }'
        self.assertTrue(self.reasons(old, '#if 1\n'+old+'\n#endif'))
        self.assertTrue(self.reasons(old, '#define WRAP 1\n#if WRAP\n'+old+'\n#endif'))
        old = '#define VALUE 1\nint f(void) { return VALUE; }\n#undef VALUE\n#define VALUE 2\n'
        new = old.replace('VALUE 1', 'VALUE_TEMP').replace('VALUE 2', 'VALUE 1').replace('VALUE_TEMP', 'VALUE 2')
        self.assertTrue(self.reasons(old, new))

    def test_generated_functions_and_unexpanded_pinned_headers_require_raw(self):
        old = '#define FN(n) int f(void) { return n; }\nFN(1)\n'
        self.assertTrue(self.reasons(old, old.replace('FN(1)', 'FN(1+0)')))
        self.assertTrue(self.reasons(old, old.replace('FN(1)', '#if 1\nFN(1)\n#endif')))
        self.assertTrue(self.reasons('#include <api.h>\nFN(1)\n', '#include <api.h>\nFN(1+0)\n'))
        self.assertTrue(self.reasons('_asm nop;', '_asm xchg ax,ax;'))


class AssemblyHeaderTests(unittest.TestCase):
    def test_video_geometry_agrees_between_c_and_masm(self):
        import re
        root = Path(__file__).resolve().parents[1]
        c = (root/'include/input.h').read_text()
        asm = (root/'include/video.inc').read_text()
        for name in ('PLATFORM_SCREEN_WIDTH_PIXELS', 'PLATFORM_SCREEN_HEIGHT_PIXELS'):
            c_value = re.search(r'(?m)^#define '+name+r'\s+(\d+)\b', c)
            asm_value = re.search(r'(?mi)^'+name+r'\s+EQU\s+(\d+)\b', asm)
            self.assertEqual(int(c_value[1]), int(asm_value[1]))

    def test_asm_header_candidate_and_frozen_closure(self):
        import assembler
        with tempfile.TemporaryDirectory() as d:
            root = Path(d); (root/'include').mkdir(); (root/'recipes').mkdir()
            header = root/'include/video.inc'; header.write_bytes(b'SCREEN_W EQU 320\n')
            candidate = root/'unit.asm'; candidate.write_bytes(b'INCLUDE include/video.inc\nEND\n')
            files = {'lib/unit.ASM': b'END\n'}
            active = {'recipes/unit.json': {'owners': ['unit'], 'recipe': {'source': 'lib/unit.ASM'}}}
            with patch('rename.active_recipes', return_value=active):
                source, changes = declarations.candidate_changes('unit', candidate,
                    ['include/video.inc='+str(header)], files)
            self.assertEqual(source, 'lib/unit.ASM')
            with patch.object(assembler, 'ROOT', root), patch.object(assembler, 'git_tracked', return_value=False):
                with self.assertRaisesRegex(ValueError, 'not tracked'):
                    assembler.prepare_asm(candidate.read_bytes())
                with preprocessor.staged_headers({'include/video.inc': header.read_bytes()}):
                    expanded, closure = assembler.prepare_asm(candidate.read_bytes())
                self.assertIn(b'SCREEN_W EQU 320', expanded)
                (root/'recipes/unit.json').write_bytes(json_bytes({'include_closure': closure}))
                self.assertEqual(assembler.prepare_asm(candidate.read_bytes())[1], closure)
                header.write_bytes(b'SCREEN_W EQU 321\n')
                with self.assertRaisesRegex(ValueError, 'not tracked'):
                    assembler.prepare_asm(candidate.read_bytes())


class ProcessBoundTests(unittest.TestCase):
    def test_windows_assignment_failure_never_starts_the_child(self):
        import os
        import bounded_process
        if os.name != 'nt':
            # Exercise the successful POSIX path too, without a skipped test.
            with tempfile.TemporaryDirectory() as d:
                self.assertEqual(bounded_process.bounded_run([sys.executable, '-c', 'pass'],
                    cwd=d, env=dict(os.environ), log=Path(d)/'log', timeout=5), 0)
            return
        with tempfile.TemporaryDirectory() as d:
            root = Path(d); marker = root/'started'
            class RefusedJob:
                def AssignProcessToJobObject(self, job, process): return False
                def CloseHandle(self, job): return True
            with patch('bounded_process._windows_job', return_value=(RefusedJob(), 1)):
                with self.assertRaises(OSError):
                    bounded_process.bounded_run([sys.executable, '-c',
                        'from pathlib import Path; Path(' + repr(str(marker)) + ').write_text("started")'],
                        cwd=root, env=dict(os.environ), log=root/'log', timeout=5)
            self.assertFalse(marker.exists())

    def test_timeout_stops_the_owned_process_tree(self):
        import os
        import subprocess
        from bounded_process import bounded_run
        with tempfile.TemporaryDirectory() as d:
            root = Path(d); ids = root/'pids.txt'
            if os.name == 'nt':
                script = ('import os,sys,subprocess,time; from pathlib import Path; '
                          'child=subprocess.Popen([sys.executable,"-c","import time; time.sleep(30)"]); '
                          f'Path({str(ids)!r}).write_text(str(os.getpid())+" "+str(child.pid)); time.sleep(30)')
                with self.assertRaises(subprocess.TimeoutExpired):
                    bounded_run([sys.executable, '-c', script], cwd=root, env=dict(os.environ), log=root/'log', timeout=5)
                self.assertTrue(ids.exists(), 'The process-tree fixture did not start within its bound')
                import ctypes as C
                from ctypes import wintypes as W
                kernel=C.WinDLL('kernel32', use_last_error=True)
                kernel.OpenProcess.argtypes=[W.DWORD,W.BOOL,W.DWORD];kernel.OpenProcess.restype=W.HANDLE
                kernel.WaitForSingleObject.argtypes=[W.HANDLE,W.DWORD];kernel.WaitForSingleObject.restype=W.DWORD
                kernel.CloseHandle.argtypes=[W.HANDLE]
                for pid in map(int,ids.read_text().split()):
                    handle=kernel.OpenProcess(0x00100000,False,pid)
                    if handle:
                        try:self.assertEqual(kernel.WaitForSingleObject(handle,1000),0,'A bounded child survived')
                        finally:kernel.CloseHandle(handle)
                    else:self.assertEqual(C.get_last_error(),87)
            else:
                with self.assertRaises(subprocess.TimeoutExpired):
                    bounded_run([sys.executable, '-c', 'import time; time.sleep(30)'], cwd=root,
                                env=dict(os.environ), log=root/'log', timeout=0.1)


if __name__ == '__main__':
    unittest.main()
