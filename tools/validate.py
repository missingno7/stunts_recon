"""Fresh oracle, technical tests, whole hybrid image and independent compiler parity."""
import argparse
import unittest
from common import ROOT, read_json, write_json, require, identity
from build_exact import build, inputs
from oracle import verify
from compiler import verify_toolchain
from transaction import exclusive, ensure_consistent


def compare_baseline(path):
    baseline = read_json(path)
    manifest = read_json(ROOT/'layout/manifest.json')
    require(manifest == baseline['manifest'], 'Migration changed accepted ownership')
    require(read_json(ROOT/'layout/oracle.lock.json') == baseline['oracle'], 'Migration changed oracle lock')
    for name, saved in baseline['sources'].items():
        recipe = read_json(ROOT/saved['recipe']['source'].replace('src/', 'recipes/').replace('.c', '.json'))
        # Descriptive evidence references can move; all binding/build facts stay exact.
        require({k:v for k,v in recipe.items() if k != 'evidence'} ==
                {k:v for k,v in saved['recipe'].items() if k != 'evidence'}, 'Migration changed accepted recipe: '+name)
        require(identity((ROOT/recipe['source']).read_bytes()) == saved['identity'], 'Migration changed accepted C: '+name)
    return {'functions':len(baseline['sources']), 'owners':len(manifest['owners']), 'status':'PRESERVED'}


def validate(independent=True, baseline=None):
    destination = ROOT/'build/validation/report.json'
    destination.unlink(missing_ok=True)
    report = {'status':'RUNNING'}
    try:
        ensure_consistent()
        before = inputs()
        original = verify(write=False)
        profiles = sorted(read_json(ROOT/'layout/toolchain.json')['profiles'])
        for profile in profiles: verify_toolchain(profile)
        out = ROOT/'build/validation'; out.mkdir(parents=True, exist_ok=True)
        suite = unittest.defaultTestLoader.discover(str(ROOT/'tests'))
        with (out/'tests.log').open('w', encoding='utf-8') as log:
            result = unittest.TextTestRunner(stream=log, verbosity=2).run(suite)
        report['tests'] = {'passed':result.testsRun-len(result.errors)-len(result.failures)-len(result.skipped),
                           'failed':len(result.errors)+len(result.failures), 'skipped':len(result.skipped),
                           'log':'build/validation/tests.log'}
        require(result.wasSuccessful() and not result.skipped, 'Tests failed/skipped; see build/validation/tests.log')
        # Tests exercise locks themselves; serialize fresh production proof afterwards.
        with exclusive():
            ensure_consistent()
            require(inputs() == before, 'Inputs changed during technical tests')
            acceptance = build()
            if independent:
                from crosscheck_runner import main as crosscheck
                crosscheck()
                parity = read_json(ROOT/'build/validation/independent.json')
                require(parity['inputs'] == before, 'Independent compiler snapshot differs')
                report['independent_compiler'] = {'functions':len(parity['results']), 'status':'PASS'}
            else:
                report['independent_compiler'] = {'status':'NOT_RUN'}
            if baseline: report['migration_baseline'] = compare_baseline(baseline)
            owners = read_json(ROOT/'layout/manifest.json')['owners']
            require(inputs() == before and acceptance['inputs'] == before, 'Inputs changed during validation')
            report.update(status='PASS', full_image=acceptance['status'], executable=acceptance['executable'],
                          oracle=original[2]['load_image'], relocation_count=acceptance['relocation_count'],
                          accepted_functions=[o['name'] for o in owners if o['kind']=='MATCHING_C'],
                          ownership={k:sum(o['end']-o['start'] for o in owners if o['kind']==k)
                                     for k in ('MATCHING_C','KNOWN_TOOLCHAIN_LIBRARY','UNRESOLVED_RAW')},
                          matching_asm_bytes=acceptance['matching_asm_bytes'], inputs=before)
            write_json(destination, report)
        return report
    except Exception as error:
        report.update(status='FAILED', error=str(error)); write_json(destination, report); raise


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--no-independent', action='store_true', help='Explicitly omit DOSBox-X parity; incomplete release verification')
    p.add_argument('--baseline', help='Optional recorded migration baseline to compare exact ownership and source identities')
    a = p.parse_args(); report = validate(not a.no_independent, a.baseline)
    print('PASS:', report['tests']['passed'], 'tests;', report['full_image'], '; independent compiler', report['independent_compiler']['status'])
    print('Ownership bytes:', report['ownership'], '; matching ASM', report['matching_asm_bytes'])


if __name__ == '__main__': main()
