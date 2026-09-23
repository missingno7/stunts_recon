"""One fresh end-to-end health check for the supported grinding workflow."""
import argparse
import sys
import unittest
from common import ROOT, read_json, write_json, require, identity
from build_exact import build, inputs
from oracle import verify
from compiler import verify_toolchain
from reconstruction_factory import refresh
from workflow import workflow_inputs, fingerprint


def validate(independent=True):
    destination=ROOT/'docs/current/validation.json'
    destination.unlink(missing_ok=True)
    report={'status':'RUNNING'}
    before=inputs(); workflow_before=workflow_inputs()
    try:
        require(not (ROOT/'build/grind.lock').exists() and not (ROOT/'build/promotion.lock').exists(),
                'Writer lock exists; inspect the active/interrupted writer before validation')
        oracle=verify(write=False)
        profiles=sorted(read_json(ROOT/'layout/toolchain.json')['profiles'])
        for profile in profiles: verify_toolchain(profile)
        out=ROOT/'build/validation';out.mkdir(parents=True,exist_ok=True)
        suite=unittest.defaultTestLoader.discover(str(ROOT/'tests'))
        with (out/'tests.log').open('w',encoding='utf-8') as log:
            result=unittest.TextTestRunner(stream=log,verbosity=2).run(suite)
        report['tests']={'passed':result.testsRun-len(result.errors)-len(result.failures)-len(result.skipped),
                         'failed':len(result.errors)+len(result.failures),'skipped':len(result.skipped),
                         'log':'build/validation/tests.log'}
        require(result.wasSuccessful() and not result.skipped,'Tests failed/skipped; see build/validation/tests.log')
        acceptance=build()
        owners=read_json(ROOT/'layout/manifest.json')['owners']
        active_ids={o['id'] for o in owners}|{o.get('name') for o in owners if o['kind']!='UNRESOLVED_RAW'}
        for path in (ROOT/'recovery/promotions').glob('*.json'):
            promotion=read_json(path)
            require(promotion.get('status')!='PROMOTED' or promotion['task'] in active_ids,
                    'Previously promoted contribution lost ownership: '+str(path))
        if independent:
            from crosscheck_runner import main as crosscheck
            crosscheck()
            parity=read_json(ROOT/'recovery/promoted-runner-parity.json')
            require(parity['inputs']==before,'Independent compiler snapshot differs')
            report['independent_compiler']={'active_functions':len(parity['results']),'status':'PASS'}
        else:
            report['independent_compiler']={'status':'NOT_RUN'}
        status=refresh();queue=read_json(ROOT/'recovery/queue.json')
        require(workflow_inputs()==workflow_before and inputs()==before,'Inputs changed during validation')
        require(acceptance['inputs']==before and status['full_image_status']=='HYBRID_EXACT','Full build is stale')
        for row in queue['tasks']:
            card=read_json(ROOT/row['card'])
            require(card['id']==row['id'] and card['name']==row['name'] and card['tier']==row['tier'],
                    'Queue/card identity mismatch')
            require(card['workflow_snapshot']==workflow_before,'Stale current card')
        report.update(status='PASS', full_image='HYBRID_EXACT', oracle=oracle[2]['load_image'],
                      executable=acceptance['executable'], toolchain_profiles=profiles,
                      acceptance_input_fingerprint=queue['input_fingerprint'],
                      workflow_fingerprint=fingerprint(workflow_before), queue_counts=queue['counts'],
                      grinding_scope='Verified leaf contributions with supported binding; supervisor work remains')
        write_json(destination,report)
        summary=('# Latest validation\n\n'
                 f"Tests: **{report['tests']['passed']} passed**, no failures or skips. Full image: **HYBRID_EXACT**.\n\n"
                 f"Independent compiler: **{report['independent_compiler']['status']}**. Queue: **{queue['counts']}**.\n\n"
                 'This receipt describes its recorded input and workflow fingerprints; rerun `python tools/validate.py` after changes.\n\n'
                 'Read [grinder instructions](grinder-instructions.md) for routine work and [readiness](readiness.md) for remaining supervisor gates.\n')
        (ROOT/'docs/current/validation.md').write_text(summary,encoding='utf-8')
        return report
    except Exception as error:
        report.update(status='FAILED',error=str(error))
        write_json(destination,report)
        (ROOT/'docs/current/validation.md').write_text('# Latest validation\n\nFAILED: '+str(error)+'\n',encoding='utf-8')
        raise


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--no-independent',action='store_true',help='Explicitly skip DOSBox-X parity; not full release validation')
    a=p.parse_args();report=validate(not a.no_independent)
    print('PASS:',report['tests']['passed'],'tests; HYBRID_EXACT; workflow/current cards verified')


if __name__=='__main__':main()
