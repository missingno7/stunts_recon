"""Original-machine service fault inputs and six-class audit integrity."""
from __future__ import annotations

import json
import os
from pathlib import Path
import subprocess
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / 'build/python'), str(ROOT / 'tools/porting')]
from diffharness.emulator import RealModeRunner
from diffharness.model import RoutineCase, MemoryWrite, MemoryRegion
from diffharness.oracle import OracleImage, dgroup_segment
from unicorn.x86_const import UC_X86_REG_AX, UC_X86_REG_CX, UC_X86_REG_EFLAGS
from semantic_audit import functions
from semantic_mapping import public_extent, build_mapping
from semantic_detectors import AUDIT_CLASSES, machine_signals, six_class_audit
from tests.test_sdl3_projection_math import host_compiler


def original_save(image, *, close_error=False, short_write=False, create_error=0):
    calls = []

    def dos(machine):
        service = machine.reg_read(UC_X86_REG_AX) >> 8
        calls.append(service)
        error = 0
        if service == 0x3c:
            value, error = (create_error, 1) if create_error else (7, 0)
        elif service == 0x40:
            value = machine.reg_read(UC_X86_REG_CX) - int(short_write)
        elif service == 0x3e:
            value, error = (6, 1) if close_error else (0, 0)
        elif service == 0x41:
            value = 0
        else:
            raise AssertionError(f'unexpected original DOS service {service:02x}')
        machine.reg_write(UC_X86_REG_AX, value)
        flags = machine.reg_read(UC_X86_REG_EFLAGS)
        machine.reg_write(UC_X86_REG_EFLAGS, (flags & ~1) | error)

    result = RealModeRunner(image, interrupt_handlers={0x21: dos}).call(RoutineCase(
        'savefilez explicit DOS results', 140555,
        args=(0x8000, 0x100, 0x6000, 3, 0),
        memory=(MemoryWrite(dgroup_segment(), 0x8000, b'AUDIT.TRK\0'),
                MemoryWrite(0x6000, 0x100, b'abc')), result_registers=('ax',)))
    return result.registers['ax'], calls


class StaticAuditOriginalTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = OracleImage.load()

    def test_save_close_status_matches_original_including_failure(self):
        # Compile the production routine body with explicit host-service results.
        # No expected value is derived from this body.
        body = functions((ROOT / 'port/file.c').read_text())['savefilez']['body']
        probe = r'''
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#define PORT_FULL_PATH_BYTES 3072u
#define PORT_FILE_CHUNK_BYTES 0x4000u
static int fail_close, short_write, fail_create, removed;
static int port_memory_extent(const void *p,size_t *s){return 0;}
static FILE *open_save_write(const char *p,char *a,size_t n){
 if(fail_create){errno=fail_create==5?EACCES:ENOENT;return NULL;}
 a[0]='X';a[1]=0;return tmpfile();}
static int SDL_RemovePath(const char *p){++removed;return 1;}
static size_t audit_write(const void *p,size_t s,size_t n,FILE *f){
 return fwrite(p,s,n-short_write,f);}
static int audit_close(FILE *f){int r=fclose(f);return fail_close?EOF:r;}
#define fwrite audit_write
#define fclose audit_close
''' + body + r'''
int main(int argc,char **argv){fail_close=atoi(argv[1]);short_write=atoi(argv[2]);
 fail_create=atoi(argv[3]);int r=savefilez("AUDIT.TRK","abc",3);
 printf("%d %d\n",r,removed);return 0;}
'''
        cc = host_compiler()
        self.assertIsNotNone(cc, 'GCC is required')
        env = os.environ.copy()
        env['PATH'] = str(Path(cc).parent) + os.pathsep + env.get('PATH', '')
        with tempfile.TemporaryDirectory(prefix='stunts-static-save-') as tmp:
            source, exe = Path(tmp) / 'probe.c', Path(tmp) / 'probe.exe'
            source.write_text(probe)
            built = subprocess.run([cc, '-std=c11', '-O2', str(source), '-o', str(exe)],
                                   env=env, capture_output=True, text=True, timeout=30)
            self.assertEqual(built.returncode, 0, built.stderr)
            for close, short, create in ((0, 0, 0), (1, 0, 0), (0, 1, 0),
                                         (1, 1, 0), (0, 0, 2), (0, 0, 5)):
                with self.subTest(close_error=close, short_write=short, create_error=create):
                    expected, calls = original_save(self.image, close_error=close,
                        short_write=short, create_error=create)
                    ran = subprocess.run([str(exe), str(close), str(short), str(create)],
                        env=env, capture_output=True, text=True, timeout=5)
                    self.assertEqual(ran.returncode, 0, ran.stderr)
                    status, removed = map(int, ran.stdout.split())
                    self.assertEqual(status, expected, f'original services {calls}')
                    if not create:
                        self.assertEqual(bool(removed), 0x41 in calls)
                    self.assertEqual(calls[-1], 0x41 if short or create else 0x3e)

    def test_public_interval_uses_pinned_declarations_and_rejects_unknowns(self):
        manifest = json.loads((ROOT / 'layout/manifest.json').read_text())
        owners = {r['id']: r for r in manifest['owners']}
        asm = json.loads((ROOT / 'docs/porting/asm-migration.json').read_text())
        row = next(r for r in asm['routines'] if r['public_symbol'] == '_rand')
        extent = public_extent(row, owners)
        self.assertEqual((extent['start'], extent['end']), (124494, 124534))
        self.assertIsNone(public_extent(dict(row, public_symbol='_invented'), owners))
        self.assertIsNone(public_extent(dict(row, module_entry_offset=None), owners))

    def test_missing_addresses_never_join_to_an_unrelated_native_function(self):
        from port_layer import build
        from port_layer_coverage import never_note
        report = build(dict(historical_mapping=[dict(address=None,
            portable_names=[dict(source='port/sprite.c', name='poison_null_join')],
            mapping_status='unresolved_address')], functions=[]))
        for row in report['rows']:
            self.assertNotIn('poison_null_join', [p['name'] for p in row.get('portable', [])])
        rand = next(r for r in report['rows'] if r['id'] == 'asm:library_rand_124476::_rand')
        self.assertEqual(rand['original_address'], 124494)
        note = never_note(rand, 'unmapped', set())
        self.assertIn('does not prove guest unreachability', note)
        self.assertNotIn('No game caller reaches', note)

    def test_all_six_checks_retain_unmapped_rows_and_original_operand_widths(self):
        facts = machine_signals(self.image.load_image, 133074, 133091)
        self.assertEqual(facts['undecoded_tail_hex'], '')
        load = next(f for f in facts['signals']['alias-layout'] if f['address'] == 133083)
        self.assertEqual(load['memory'][0]['bytes'], 1)
        self.assertTrue(any(f['instruction'] == 'xor ah, ah' for f in
                            facts['signals']['width-signedness']))
        # Join an unknown helper and a mapped never-executed routine. Neither
        # absence of evidence nor an all-zero source-signal count is a PASS.
        layer = dict(rows=[dict(id='native_helper:unknown', kind='native_helper',
                               name='unknown', portable=[]),
            dict(id='asm:kbjoy::_convertjoy', kind='asm', name='convertjoy', portable=[],
                 execution_coverage=dict(classification='never_executed'))])
        result = six_class_audit(ROOT, layer, dict(historical_mapping=[]), image=self.image.load_image)
        self.assertEqual(result['rows'][0]['id'], 'asm:kbjoy::_convertjoy')
        self.assertEqual(result['summary']['rows'], 2)
        self.assertEqual(result['summary']['never_executed_first'], 1)
        for row in result['rows']:
            self.assertEqual(set(row['checks']), set(AUDIT_CLASSES))
            self.assertTrue(all(c['status'] in ('requires-review', 'original-unmapped')
                                for c in row['checks'].values()))

    def test_every_inventory_row_retains_six_obligations(self):
        layer = json.loads((ROOT / 'tools/porting/port-layer.json').read_text())
        # Exercise the entire original-coordinate join without rerunning the
        # production overlay/compiler audit already tested by the port gate.
        layer['rows'] = [dict(r, portable=[]) for r in layer['rows']]
        report = six_class_audit(ROOT, layer, dict(historical_mapping=build_mapping(
            ROOT, definitions={})), image=self.image.load_image)
        self.assertEqual({r['id'] for r in report['rows']}, {r['id'] for r in layer['rows']})
        self.assertEqual(report['summary']['rows'], len(layer['rows']))
        self.assertGreaterEqual(report['summary']['original_intervals'], 400)
        phases = [r['phase'] for r in report['rows']]
        self.assertEqual(phases, sorted(phases))
        for row in report['rows']:
            self.assertEqual(set(row['checks']), set(AUDIT_CLASSES))
            self.assertTrue(all(check['obligation'] for check in row['checks'].values()))
            self.assertFalse(any(check['status'] == 'PASS' for check in row['checks'].values()))
        # Data tables must not accidentally become x86 instruction evidence.
        ctype = next(r for r in report['rows'] if r['kind'] == 'semantic_data')
        self.assertEqual(ctype['original'], [])
        self.assertEqual(ctype['original_data']['bytes'], 257)

    def test_direction_table_and_msc_random_match_original_machine_cases(self):
        bodies = functions((ROOT / 'port/input.c').read_text())['convertjoy']['body']
        randoms = functions((ROOT / 'port/random.c').read_text())
        bodies += '\n' + '\n'.join(randoms[n]['body'] for n in
                                    ('port_random_test_set_state', 'port_random_test_rand'))
        probe = '#include <stdint.h>\n#include <stdlib.h>\n#include <stdio.h>\n'
        probe += 'static uint32_t s_test_rand_state; static int s_test_rand_enabled;\n' + bodies
        probe += r'''
int main(int argc,char **argv){uint32_t seed=(uint32_t)strtoul(argv[1],0,0);
 port_random_test_set_state(seed);
 for(int i=0;i<12;++i)printf("%u ",(uint16_t)port_random_test_rand());
 for(int i=0;i<16;++i)printf("%u ",(uint16_t)convertjoy((int16_t)(seed|i)));
 return 0;}
'''
        cc = host_compiler()
        self.assertIsNotNone(cc)
        env = os.environ.copy()
        env['PATH'] = str(Path(cc).parent) + os.pathsep + env.get('PATH', '')
        with tempfile.TemporaryDirectory(prefix='stunts-static-runtime-') as tmp:
            src, exe = Path(tmp) / 'probe.c', Path(tmp) / 'probe.exe'
            src.write_text(probe)
            built = subprocess.run([cc, '-std=c11', '-O2', str(src), '-o', str(exe)],
                                   env=env, capture_output=True, text=True, timeout=30)
            self.assertEqual(built.returncode, 0, built.stderr)
            for seed in (0, 0x8000, 0xfffffff0):
                original, state = [], seed
                for _ in range(12):
                    case = RoutineCase('MSC rand state', '_rand',
                        memory=(MemoryWrite(dgroup_segment(), 0x3930, state.to_bytes(4, 'little')),),
                        compare=(MemoryRegion('state', dgroup_segment(), 0x3930, 4),))
                    result = RealModeRunner(self.image).call(case)
                    original.append(result.registers['ax'])
                    state = int.from_bytes(result.memory['state'], 'little')
                for flags in range(16):
                    original.append(RealModeRunner(self.image).call(RoutineCase(
                        'convertjoy high/low bits', 133074, args=((seed | flags) & 0xffff,),
                        result_registers=('ax',))).registers['ax'])
                ran = subprocess.run([str(exe), str(seed)], env=env,
                                     capture_output=True, text=True, timeout=5)
                self.assertEqual(ran.returncode, 0, ran.stderr)
                self.assertEqual(list(map(int, ran.stdout.split())), original)

    def test_matrix_transpose_preserves_original_partial_overlap_order(self):
        body = functions((ROOT / 'port/matrix.c').read_text())['mat_invert']['body']
        cc = host_compiler()
        self.assertIsNotNone(cc)
        env = os.environ.copy()
        env['PATH'] = str(Path(cc).parent) + os.pathsep + env.get('PATH', '')
        probe = '#include <stdint.h>\n#include <stdio.h>\n#include <stdlib.h>\n' + body + r'''
int main(int argc,char **argv){uint16_t words[64];
 for(unsigned i=0;i<64;++i)words[i]=(uint16_t)(i*997u+0x8021u);
 mat_invert((int16_t *)(words+24),(int16_t *)(words+24+atoi(argv[1])));
 FILE *f=fopen(argv[2],"wb");if(!f)return 1;
 int ok=fwrite(words,1,sizeof words,f)==sizeof words;return fclose(f)||!ok;}
'''
        initial = struct.pack('<64H', *((i * 997 + 0x8021) & 0xffff for i in range(64)))
        with tempfile.TemporaryDirectory(prefix='stunts-static-transpose-') as tmp:
            src, exe, out = (Path(tmp) / n for n in ('probe.c', 'probe.exe', 'out.bin'))
            src.write_text(probe)
            built = subprocess.run([cc, '-std=c11', '-O2', str(src), '-o', str(exe)],
                                   env=env, capture_output=True, text=True, timeout=30)
            self.assertEqual(built.returncode, 0, built.stderr)
            for delta in range(-9, 10):
                with self.subTest(overlap_words=delta):
                    result = RealModeRunner(self.image).call(RoutineCase(
                        'mat_invert overlap', 'mat_invert', args=(0x9030, 0x9030 + 2 * delta),
                        memory=(MemoryWrite(dgroup_segment(), 0x9000, initial),),
                        compare=(MemoryRegion('words', dgroup_segment(), 0x9000, len(initial)),),
                        result_registers=()))
                    ran = subprocess.run([str(exe), str(delta), str(out)], env=env,
                                         capture_output=True, text=True, timeout=5)
                    self.assertEqual(ran.returncode, 0, ran.stderr)
                    self.assertEqual(out.read_bytes(), result.memory['words'])


if __name__ == '__main__':
    unittest.main()
