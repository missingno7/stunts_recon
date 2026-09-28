"""integ27: pinned DOS pass environment (pass directory, TEMP, pass memory) on
both hosts, the register-gated MSC 6.00A profile (readable `_asm`, optimize
pragma regions, CodeView debug segments), and the numeric program-address lint."""
import copy
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json
import compiler
import object_flags
import pass_environment as PE
from compiler import compile_source, CompileFailure, check_inline_asm, verify_toolchain
from object_probe import read_object, DEBUG_SEGMENT_POLICY
from preprocessor import prepare

C6 = 'msc600a-medium-zi'
C6_FLAGS = ['/AM', '/Os', '/Oe', '/Og', '/Gs', '/Zi']
FIXTURE = ROOT/'tests/fixtures/integ27/c6_zi_unit.c'


def patched_lock(mutate):
    original = compiler.read_json

    def reader(path):
        data = original(path)
        if Path(path) == ROOT/'layout/toolchain.json':
            data = copy.deepcopy(data)
            mutate(data)
        return data
    return reader


class PassEnvironment(unittest.TestCase):
    def test_pinned_pass_directory_is_the_runner_view(self):
        config, _ = verify_toolchain('msc510-medium')
        self.assertEqual(config['pass_environment']['dos_directory'], 'C:\\TOOLS\\MSC-5.10')
        self.assertEqual(PE.dos_visible((ROOT/config['directory']).resolve()),
                         config['pass_environment']['dos_directory'])

    def test_moved_or_renamed_pass_directory_is_refused(self):
        def mutate(data):
            data['profiles']['msc510-medium']['pass_environment']['dos_directory'] = 'C:\\MSC'
        with patch('compiler.read_json', side_effect=patched_lock(mutate)):
            with self.assertRaisesRegex(ValueError, 'pass strings are part of the compiler profile'):
                verify_toolchain('msc510-medium')

    def test_c_profile_without_pass_environment_is_refused(self):
        def mutate(data):
            del data['profiles']['msc510-medium']['pass_environment']
        with patch('compiler.read_json', side_effect=patched_lock(mutate)):
            with self.assertRaisesRegex(ValueError, 'lacks its pinned DOS pass environment'):
                verify_toolchain('msc510-medium')

    def test_runner_options_are_pinned(self):
        def mutate(data):
            data['runner']['argv_options'] = ['-v5.00']
        with patch('compiler.read_json', side_effect=patched_lock(mutate)):
            with self.assertRaisesRegex(ValueError, 'Pinned runner options differ'):
                verify_toolchain('msc510-medium')

    def test_independent_runner_hash_is_pinned(self):
        lock = read_json(ROOT/'layout/toolchain.json')
        bad = copy.deepcopy(lock)
        bad['independent_runner']['sha256'] = '0' * 64
        with patch('pass_environment.lock', return_value=bad):
            with self.assertRaisesRegex(ValueError, 'Independent DOSBox-X runner hash mismatch'):
                PE.independent_runner()

    def test_independent_mirror_refuses_unrepresentable_directory(self):
        config = copy.deepcopy(verify_toolchain('msc510-medium')[0])
        config['pass_environment']['dos_directory'] = 'C:\\TOOLS\\MSC-6.00A-SIMANTW\\BIN'
        with self.assertRaisesRegex(ValueError, 'not representable by the independent DOS host'):
            PE.mirror('msc510-medium', config)

    def test_mirror_is_hash_identical_at_the_pinned_path(self):
        config, _ = verify_toolchain('msc510-medium')
        root, drive = PE.mirror('msc510-medium', config)
        self.assertEqual(drive, 'C')
        cl = root/'TOOLS'/'MSC-5.10'/'CL.EXE'
        self.assertEqual((ROOT/config['directory']/'CL.EXE').read_bytes(), cl.read_bytes())

    def test_pass_memory_is_pinned_and_reproduced_on_both_hosts(self):
        probe = PE.build_probe()
        config, _ = verify_toolchain('msc510-medium')
        pinned = config['pass_environment']['pass_memory_paragraphs']
        player = PE.measure_player('msc510-medium', probe)
        dosbox = PE.measure_dosbox('msc510-medium', probe)
        self.assertEqual({k: v['pass_paragraphs'] for k, v in player.items()}, pinned['runner'])
        self.assertEqual({k: v['pass_paragraphs'] for k, v in dosbox.items()}, pinned['independent_runner'])
        # C2 (the pass whose output depends on the host strings) is identical;
        # C1/C3 differ only by the copied environment block size.
        self.assertEqual(player['C2']['pass_paragraphs'], dosbox['C2']['pass_paragraphs'])
        self.assertEqual(player['C2']['psp'], dosbox['C2']['psp'])
        for name in ('C1', 'C3'):
            self.assertEqual(player[name]['arena_end'], dosbox[name]['arena_end'])
            self.assertLessEqual(player[name]['pass_paragraphs'] - dosbox[name]['pass_paragraphs'], 4)
            self.assertEqual(dosbox[name]['psp'] - dosbox['C2']['psp'],
                             dosbox[name]['env_paragraphs'] - dosbox['C2']['env_paragraphs'])


class RegisterGatedProfile(unittest.TestCase):
    @staticmethod
    def seg_row(segment):
        functions = read_json(ROOT/'evidence/functions.json')['functions']
        return next(f for f in functions if f.get('segment') == segment and
                    type(f.get('start')) is int and f.get('name'))

    def recipe(self, segment, **extra):
        row = self.seg_row(segment)
        recipe = {'id': row['name'], 'kind': 'c', 'profile': C6, 'start': row['start'], 'end': row['end'],
                  'compiler_flags': list(C6_FLAGS), 'compiler_flags_register': 'CC-MSC600A-seg007'}
        recipe.update(extra)
        return recipe

    def test_profile_is_pinned_and_gated(self):
        config, _ = verify_toolchain(C6)
        self.assertEqual(config['flags'], C6_FLAGS)
        self.assertEqual(config['register_gated'], 'CC-MSC600A-*')
        self.assertEqual(object_flags.registered_profile_objects()['seg007'],
                         ('CC-MSC600A-seg007', C6, C6_FLAGS))

    def test_seg007_recipe_uses_the_ruled_flags(self):
        self.assertEqual(object_flags.recipe_flags(self.recipe('seg007')), C6_FLAGS)
        with self.assertRaisesRegex(ValueError, 'differs from the ruled profile/flag set'):
            object_flags.recipe_flags(self.recipe('seg007', compiler_flags=['/AM', '/Os', '/Gs', '/Zi']))
        with self.assertRaisesRegex(ValueError, 'differs from the ruled profile/flag set'):
            object_flags.recipe_flags({k: v for k, v in self.recipe('seg007').items()
                                       if k != 'compiler_flags_register'})

    def test_profile_refused_without_register_entry(self):
        # Another object has no CC-MSC600A entry.
        with self.assertRaisesRegex(ValueError, 'requires one object with a SUPPORTED register entry'):
            object_flags.recipe_flags(self.recipe('seg008'))
        # seg007 itself is refused once its entry is not SUPPORTED.
        register = read_json(ROOT/'evidence/toolchain-hypotheses.json')
        for row in register['hypotheses']:
            if row['id'] == 'CC-MSC600A-seg007':
                row['status'] = 'PLAUSIBLE'
        original = object_flags.read_json

        def reader(path):
            return register if Path(path) == ROOT/'evidence/toolchain-hypotheses.json' else original(path)
        with patch('object_flags.read_json', side_effect=reader):
            with self.assertRaisesRegex(ValueError, 'requires one object with a SUPPORTED register entry'):
                object_flags.recipe_flags(self.recipe('seg007'))

    def test_asm_refused_under_msc510(self):
        source = b'int f(int a) { _asm { mov ax, a } return a; }\n'
        with self.assertRaisesRegex(CompileFailure, 'Inline assembly/raw emission forbidden'):
            compile_source(source, 'msc510-medium')
        with self.assertRaisesRegex(ValueError, 'Inline assembly/raw emission forbidden'):
            check_inline_asm(source.decode(), verify_toolchain('msc510-medium')[0])

    def test_raw_emission_refused_under_c6(self):
        config, _ = verify_toolchain(C6)
        readable = 'int f(int a) { _asm {\n mov ax, a\n add ax, 5\n mov a, ax\n } return a; }\n'
        self.assertEqual(check_inline_asm(readable, config)['statements'], 3)
        for raw in ('void f(void) { _asm _emit 0x90 }\n',
                    'void f(void) { __emit(0x90); }\n',
                    'void f(void) { _asm {\n db 90h\n } }\n',
                    'void f(void) { _asm {\n dw 9090h\n } }\n',
                    'void f(void) { _asm {\n 0x90\n } }\n',
                    'void f(void) { _asm {\n 90h, 0CBh\n } }\n',
                    'void f(void) { asm { nop } }\n'):
            with self.assertRaises(ValueError, msg=raw):
                check_inline_asm(raw, config)
        with self.assertRaisesRegex(CompileFailure, 'Raw byte emission forbidden'):
            compile_source(b'void f(void) { _asm _emit 0x90 }\n', C6)

    def test_optimize_pragma_only_under_c6_and_recorded(self):
        source = FIXTURE.read_bytes()
        with self.assertRaisesRegex(ValueError, 'Unsupported historical pragma'):
            prepare(source, 'msc510-medium')
        _, closure = prepare(source, C6)
        self.assertEqual([(p['pragma'], p['letters'], p['state']) for p in closure],
                         [('optimize', 'tl', 'on'), ('optimize', 'tl', 'off')])

    def test_c6_zi_object_debug_segments(self):
        obj, receipt = compile_source(FIXTURE.read_bytes(), C6)
        self.assertEqual(receipt['inline_asm']['statements'], 3)
        self.assertEqual([s['name'] for s in obj.segment_defs], ['UNIT_TEXT', '_DATA', 'CONST', '_BSS'])
        self.assertEqual([d['name'] for d in obj.debug_segments], ['$$SYMBOLS', '$$TYPES'])
        self.assertTrue(all(f['segment'] == 'UNIT_TEXT' for f in obj.linker_fixups))
        raw = Path(receipt['work_directory'], 'UNIT.OBJ').read_bytes()
        # Without the reviewed policy the CodeView rewrite is refused.
        with self.assertRaisesRegex(ValueError, 'Overlapping LEDATA'):
            read_object(raw)
        with self.assertRaisesRegex(ValueError, 'Unknown debug segment policy'):
            read_object(raw, debug_segments='any-debug')
        # CODE record cuts and FIXUPP order are the candidate's own.
        from prefix_proof import candidate_records
        from cut_simulator import observed_code_records
        records = candidate_records(obj, 'UNIT_TEXT')
        observed = observed_code_records(raw, 1)
        self.assertEqual([(r['start'], r['end']) for r in records],
                         [(r['offset'], r['offset'] + r['size']) for r in observed])
        self.assertEqual([f for r in records for f in r['fixups']], obj.linker_fixups)

    def test_debug_segment_policy_rejects_image_references(self):
        obj, receipt = compile_source(FIXTURE.read_bytes(), C6)
        raw = bytearray(Path(receipt['work_directory'], 'UNIT.OBJ').read_bytes())
        # A debug-class name outside the reviewed pair is refused (the LNAMES
        # record checksum is recomputed so only the name differs).
        at = raw.find(b'DEBSYM')
        raw[at:at+6] = b'DEBSYX'
        start = 0
        while start < len(raw):
            length = raw[start+1] | raw[start+2] << 8
            if start < at < start + 3 + length:
                raw[start+2+length] = (-sum(raw[start:start+2+length])) & 255
                break
            start += 3 + length
        with self.assertRaisesRegex(ValueError, 'Overlapping LEDATA|Unexpected debug segment'):
            read_object(bytes(raw), debug_segments=DEBUG_SEGMENT_POLICY)


class ShortNameAliases(unittest.TestCase):
    """seg003 short names: address-bound clones of grounded aliases (item 2)."""

    @classmethod
    def setUpClass(cls):
        from oracle import verify
        from mz import MZ
        result = verify(write=False)
        cls.image = MZ.parse(result[1]).load_image(result[1])
        cls.relocations = result[2]['unpacked_mz']['relocations']

    def test_every_short_name_is_a_clone_with_provenance(self):
        symbols = read_json(ROOT/'layout/data-symbols.json')['symbols']
        clones = {n: s for n, s in symbols.items() if 'clone_of' in s}
        self.assertGreaterEqual(len(clones), 49)
        for name, symbol in clones.items():
            # integ31: a clone may also carry the spelling of the accepted
            # defining object's public at that address (names registry).
            # integ33: reviewed readable reconstruction names (L7-naming).
            # integ40: communal public spellings are constrained by LINK's
            # first-sight hash allocation order.
            self.assertTrue('pressure-constrained reconstruction name' in symbol['name_provenance'] or
                            symbol['name_provenance'].startswith('readable reconstruction name (integ33') or
                            symbol['name_provenance'].startswith('spelling of the accepted') or
                            symbol['name_provenance'].startswith('link-hash-constrained reconstruction name (integ40'),
                            symbol['name_provenance'])
            source = symbols[symbol['clone_of']]
            self.assertEqual((symbol['load_address'], symbol['storage'], symbol.get('width')),
                             (source['load_address'], source['storage'], source.get('width')))

    def test_clone_resolves_exactly_as_its_source(self):
        from data_symbols import resolve_symbols
        both = resolve_symbols(['_simd_opp', '_simd_opponent', '_trkpos', '_trackpos'],
                               self.image, self.relocations)
        self.assertEqual(both['_simd_opp'], both['_simd_opponent'])
        self.assertEqual(both['_trkpos'], both['_trackpos'])

    def test_clone_cannot_widen_move_or_chain(self):
        import data_symbols
        symbols = read_json(ROOT/'layout/data-symbols.json')['symbols']
        for change in ({'width': 61}, {'load_address': symbols['_trkpos']['load_address'] + 2},
                       {'clone_of': '_trkpos2'}, {'references': []}, {'clone_of': '_nonexistent'}):
            bad = copy.deepcopy(symbols)
            bad['_trkpos'].update(change)
            with self.assertRaisesRegex(ValueError, 'Clone data alias differs', msg=str(change)):
                data_symbols.clone_sources(bad, ['_trkpos'])
        chained = copy.deepcopy(symbols)
        chained['_trackpos'] = dict(chained['_trkpos'], clone_of='_trkpos2')
        with self.assertRaisesRegex(ValueError, 'Clone data alias differs'):
            data_symbols.clone_sources(chained, ['_trkpos'])


class AddressLint(unittest.TestCase):
    """Numeric program addresses without a FIXUPP (tools/address_lint.py)."""

    def test_hard_coded_cs_island_offset_is_flagged(self):
        import address_lint
        from assembler import assemble_source
        source = (b".8086\r\nextrn _incnums:byte\r\n_TEXT segment word public 'CODE'\r\n"
                  b"assume cs:_TEXT\r\npublic _f\r\n_f proc far\r\n mov di, 72A8h\r\n"
                  b" mov di, offset _incnums\r\n mov al, cs:[72A8h]\r\n mov al, cs:_incnums[bx]\r\n"
                  b" mov ax, word ptr ds:[9234h]\r\n retf\r\n_f endp\r\n_TEXT ends\r\nend\r\n")
        obj, _ = assemble_source(source, 'masm510-game')
        findings = address_lint.lint_object(obj, '_TEXT', 125472)
        self.assertEqual([(f['offset'], f['level']) for f in findings],
                         [(0, 'HIGH'), (6, 'HIGH'), (15, 'HIGH')])
        self.assertTrue(all('_incnums' in f['symbols'] for f in findings[:2]))
        self.assertIn('_gameconfig', findings[2]['symbols'])

    def test_hard_coded_dgroup_offset_is_flagged_in_c(self):
        import address_lint
        layout = read_json(ROOT/'layout/data-symbols.json')
        offset = layout['symbols']['_gameconfig']['load_address'] - layout['frame_load_address']
        literal = f'int f(void) {{ return *(int *)0x{offset:x}; }}\n'.encode()
        symbolic = b'extern int gameconfig; int f(void) { return gameconfig; }\n'
        bad, _ = compile_source(literal, 'msc510-medium')
        good, _ = compile_source(symbolic, 'msc510-medium')
        findings = address_lint.lint_object(bad, 'UNIT_TEXT')
        # MSC loads the constant pointer as `mov bx,imm`: reported for review.
        self.assertEqual([f['level'] for f in findings], ['REVIEW'])
        self.assertIn('_gameconfig', findings[0]['symbols'])
        self.assertEqual(address_lint.lint_object(good, 'UNIT_TEXT'), [])


if __name__ == '__main__':
    unittest.main()
