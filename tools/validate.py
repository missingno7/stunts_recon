"""The acceptance gate: exactness proof (whole image, relocations, independent compiles, BSS link)
plus the tool and port test groups.  Each part is reported on its own."""
import os
import argparse
import unittest
from common import ROOT, read_json, write_json, require, identity
from build_exact import build, inputs
from oracle import verify
from compiler import verify_toolchain
from transaction import exclusive, ensure_consistent
import preflight

# Port tests need the i686 toolchain and SDL3; everything else exercises the reconstruction tools.
PORT_TESTS = ('test_sdl3_', 'test_port_', 'test_porting_', 'test_diffharness_', 'test_host_probe',
              'test_matrix_machine_semantics', 'test_asm_migration_completeness',
              'test_replay_oracle_probe', 'test_runtime_trace_normalizer', 'test_extent_evidence')
TEST_GROUPS = ('all', 'tools', 'port', 'none')


def compare_baseline(path):
    baseline = read_json(path)
    manifest = read_json(ROOT/'layout/manifest.json')
    require(manifest == baseline['manifest'], 'Migration changed accepted ownership')
    require(read_json(ROOT/'layout/oracle.lock.json') == baseline['oracle'], 'Migration changed oracle lock')
    for name, saved in baseline['sources'].items():
        recipe = read_json(ROOT/('recipes/'+saved['recipe']['id']+'.json'))
        # Descriptive evidence references can move; all binding/build facts stay exact.
        require({k:v for k,v in recipe.items() if k != 'evidence'} ==
                {k:v for k,v in saved['recipe'].items() if k != 'evidence'}, 'Migration changed accepted recipe: '+name)
        require(identity((ROOT/recipe['source']).read_bytes()) == saved['identity'], 'Migration changed accepted C: '+name)
    return {'functions':len(baseline['sources']), 'owners':len(manifest['owners']), 'status':'PRESERVED'}


def select_tests(group):
    """Discovered tests of one group, in discovery order (a failed import keeps its module's group)."""
    chosen = unittest.TestSuite()
    def walk(suite):
        for test in suite:
            if isinstance(test, unittest.TestSuite):
                walk(test); continue
            module = type(test).__module__.split('.')[-1]
            if type(test).__module__ == 'unittest.loader': module = test._testMethodName
            if group == 'all' or (group == 'port') == module.startswith(PORT_TESTS):
                chosen.addTest(test)
    walk(unittest.defaultTestLoader.discover(str(ROOT/'tests')))
    return chosen


def prove(report, before, original, independent, baseline):
    """The exactness proof: whole image, ordered relocations, independent compiles, BSS real link."""
    with exclusive():
        ensure_consistent()
        acceptance = build()
        if independent:
            from crosscheck_runner import main as crosscheck
            import memo
            with memo.session():
                crosscheck()
            parity = read_json(ROOT/'build/validation/independent.json')
            require(parity['inputs'] == before, 'Independent compiler snapshot differs')
            report['independent_compiler'] = {'functions':len(parity['results']), 'status':'PASS'}
        else:
            report['independent_compiler'] = {'status':'NOT_RUN'}
        if baseline: report['migration_baseline'] = compare_baseline(baseline)
        # Accepted BSS storage and every linked pinned runtime storage row are
        # re-proven by a fresh real link.
        import bss_link
        report['bss_real_link'] = bss_link.canonical_gate()
        # BSS ownership per object (accepted storage, raw per-object placeholders,
        # pinned-runtime raw, fill and the raw communal unit).
        report['bss_ownership'] = bss_link.ownership_report(bss_link.load_partition())
        owners = read_json(ROOT/'layout/manifest.json')['owners']
        require(inputs() == before and acceptance['inputs'] == before, 'Inputs changed during the proof')
        report.update(full_image=acceptance['status'], executable=acceptance['executable'],
                      oracle=original[2]['load_image'], relocation_count=acceptance['relocation_count'],
                      accepted_functions=[o['name'] for o in owners if o['kind']=='MATCHING_C'],
                      accepted_asm=[o['name'] for o in owners if o['kind']=='MATCHING_ASM'],
                      ownership={k:sum(o['end']-o['start'] for o in owners if o['kind']==k)
                                 for k in ('MATCHING_C','MATCHING_C_DATA','MATCHING_ASM',
                                           'MATCHING_ASM_DATA','KNOWN_TOOLCHAIN_LIBRARY',
                                           'KNOWN_TOOLCHAIN_LIBRARY_DATA','LINK_FILL',
                                           'BSS_IN_IMAGE','UNRESOLVED_RAW')},
                      matching_asm_bytes=acceptance['matching_asm_bytes'], inputs=before)
        # Re-derived record-closed prefix owners (RECORD_CLOSED_EXACT, ACCEPTED);
        # their proofs came from this fresh build, never from a stored receipt.
        prefixes=[r['binding']['prefix'] for r in acceptance['compiler_receipts']
                  if isinstance(r.get('binding'),dict) and
                  r['binding'].get('mode')=='record-closed-prefix-v1']
        report['record_closed_prefixes']={'owners':prefixes,
                                          'bytes':sum(x['owned_end_offset'] for x in prefixes)}


def run_tests(report, group):
    out = ROOT/'build/validation'; out.mkdir(parents=True, exist_ok=True)
    with (out/'tests.log').open('w', encoding='utf-8') as log:
        result = unittest.TextTestRunner(stream=log, verbosity=2).run(select_tests(group))
    report['tests'] = {'group':group,
                       'passed':result.testsRun-len(result.errors)-len(result.failures)-len(result.skipped),
                       'failed':len(result.errors)+len(result.failures), 'skipped':len(result.skipped),
                       'log':'build/validation/tests.log'}
    require(result.wasSuccessful() and not result.skipped, 'Tests failed/skipped; see build/validation/tests.log')
    if group in ('all', 'port'):
        semantic = read_json(ROOT/'build/porting/semantic-audit/report.json')
        require(semantic['status'] == 'PASS', 'SDL3 semantic contracts failed')
        report['port_semantic_audit'] = {
            'status': semantic['status'], 'summary': semantic['summary'],
            'report': 'build/porting/semantic-audit/report.json',
            'evidence_execution': 'Referenced tests passed in this no-skip suite; claims remain bounded to their registered observables/cases',
        }


def validate(independent=True, baseline=None, tests='all', proof=True):
    """Run the selected parts and report each on its own.

    The exactness proof and the tests are independent results: a failure in
    one never hides the other.  `complete` is true only for the full gate.
    """
    require(tests in TEST_GROUPS, 'Unknown test group: '+str(tests))
    preflight.check(port=tests in ('all', 'port'))
    (ROOT/'build').mkdir(parents=True, exist_ok=True)
    destination = ROOT/'build/validation/report.json'
    destination.unlink(missing_ok=True)
    report = {'status':'RUNNING', 'scope':{'proof':proof, 'independent':proof and independent, 'tests':tests}}
    failures = []
    try:
        ensure_consistent()
        before = inputs()
        # Tests and real-link helpers consume the derived load image from
        # build/oracle. Materialize it first in a fresh clone.
        original = verify(write=True)
        profiles = sorted(read_json(ROOT/'layout/toolchain.json')['profiles'])
        for profile in profiles: verify_toolchain(profile)
        if proof:
            try:
                prove(report, before, original, independent, baseline)
                report['proof'] = {'status':'PASS'}
            except Exception as error:
                report['proof'] = {'status':'FAILED', 'error':str(error)}
                failures.append('exactness proof: '+str(error))
            print('Exactness proof:', report['proof']['status'], report.get('full_image', ''), flush=True)
        if tests != 'none':
            try:
                run_tests(report, tests)
            except Exception as error:
                failures.append('tests: '+str(error))
            # Tests exercise locks and scratch builds themselves; canonical inputs must not move.
            if inputs() != before: failures.append('Inputs changed during technical tests')
        report['complete'] = bool(proof and independent and tests == 'all' and not failures)
        report.update(status='FAILED' if failures else 'PASS', failures=failures)
        write_json(destination, report)
        return report
    except Exception as error:
        report.update(status='FAILED', error=str(error)); write_json(destination, report); raise


def real_link_summary():
    """Real-link divergence summary.  Diagnostic: grants and gates nothing; its
    processing order is oracle-derived and raw debt is linked as explicit raw
    objects (see tools/reallink.py)."""
    try:
        import reallink
        summary = reallink.summary(reallink.run(tag='validate', log=lambda *args: None))
    except Exception as error:
        summary = {'status': 'DIAGNOSTIC_ERROR', 'error': str(error)}
    write_json(ROOT/'build/validation/real-link.json', summary)
    print('Real link (diagnostic):', {k: summary.get(k) for k in (
        'status', 'link_returncode', 'image_equal', 'image_mismatch_bytes', 'relocation_set_equal',
        'bank_order_equal', 'packed_equal', 'alias_shims', 'error') if k in summary})
    return summary


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--proof-only', action='store_true',
                   help='Run only the exactness proof (same as --tests none); the fast check after a source change')
    p.add_argument('--tests', choices=TEST_GROUPS, default='all',
                   help='Test group to run: tools (reconstruction tooling), port (SDL3 port), all (default) or none')
    p.add_argument('--no-proof', action='store_true', help='Skip the exactness proof; tests only')
    p.add_argument('--no-independent', action='store_true', help='Explicitly omit DOSBox-X parity; incomplete release verification')
    p.add_argument('--baseline', help='Optional recorded migration baseline to compare exact ownership and source identities')
    p.add_argument('--image', action='store_true',
                   help='Diagnostic only (not a gate): after validation, rebuild the executable with one real '
                        'LINK 3.65 run + EXEPACK (tools/reallink.py) and report the divergence summary')
    p.add_argument('--fresh', action='store_true',
                   help='Run every compiler and assembler again instead of reusing identical earlier runs '
                        '(tools/toolcache.py); always the case for the complete gate')
    a = p.parse_args()
    require(not (a.proof_only and a.no_proof), '--proof-only and --no-proof exclude each other')
    complete_scope = not (a.proof_only or a.no_proof or a.no_independent) and a.tests == 'all'
    if a.fresh or complete_scope:
        os.environ['STUNTS_FRESH'] = '1'
    report = validate(not a.no_independent, a.baseline, 'none' if a.proof_only else a.tests, not a.no_proof)
    proved = report.get('proof', {}).get('status') == 'PASS'
    if a.image and proved:
        report['real_link'] = real_link_summary()
    if 'proof' in report:
        print('Proof:', report['proof']['status'], report.get('full_image', ''), '; independent compiler',
              report.get('independent_compiler', {}).get('status', 'NOT_RUN'))
    if proved:
        print('Ownership bytes:', report['ownership'], '; matching ASM', report['matching_asm_bytes'])
        print('BSS/runtime real-link gate:', report['bss_real_link']['status'], report['bss_real_link']['owners'])
        bss = report['bss_ownership']
        print('BSS ownership bytes:', bss['bytes'])
        print('BSS raw per object:', bss['raw_objects'], '; pinned runtime raw:', bss['raw_runtime'])
        prefixes = report['record_closed_prefixes']
        print('Record-closed prefix owners:', len(prefixes['owners']), 'owning', prefixes['bytes'], 'bytes',
              [(x['owner'], x['owned_records'], x['derived_records'], x['candidate_record_count'])
               for x in prefixes['owners']])
    if 'tests' in report:
        t = report['tests']
        print('Tests (%s):' % t['group'], t['passed'], 'passed,', t['failed'], 'failed,', t['skipped'], 'skipped')
    if report['failures']:
        print('FAILED:')
        for line in report['failures']: print('  -', line)
        raise SystemExit(1)
    print('PASS:', 'complete gate' if report['complete'] else 'partial run (not the complete gate)')


if __name__ == '__main__': main()
