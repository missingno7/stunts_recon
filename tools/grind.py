"""Bounded source experiments with durable diagnostics and serial promotion."""
import argparse
import os
from datetime import datetime, timezone
from common import ROOT, read_json, write_json, require, project_path, identity, sha, json_bytes
from build_exact import inputs
from check_candidate import check
from probe_module import ProbeFailure
from workflow import attempts, state, task_name, require_eligible, hypothesis_key, fingerprint


def run(task, hypothesis, promote=False):
    task_name(task)
    require(hypothesis.strip(), 'Explain the source hypothesis being tested')
    recipe = read_json(ROOT/'recipes'/(task+'.json'))
    require(recipe['id'] == task, 'Task/recipe mismatch')
    require(recipe['source'].startswith('recovery/candidates/'), 'Grinder requires an unpromoted recovery source')
    status = require_eligible(recipe)
    source = project_path(recipe['source']).read_bytes()
    key = hypothesis_key(recipe, source)
    previous = [r for r in status['history'] if r['hypothesis_key'] == key and r['status'] != 'ERROR']
    require(not previous or (promote and previous[-1]['status'] == 'FAST_PASS_ONLY'),
            'Unchanged hypothesis already tested; inspect its report or request supervisor reopen')
    lock = ROOT/'build/grind.lock'; lock.parent.mkdir(parents=True, exist_ok=True)
    fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    os.write(fd, json_bytes({'task':task,'pid':os.getpid()})); os.close(fd)
    try:
        # Recheck after acquiring the writer lock; do not race another attempt.
        status=require_eligible(recipe)
        previous=[r for r in status['history'] if r['hypothesis_key']==key and r['status']!='ERROR']
        require(not previous or (promote and previous[-1]['status']=='FAST_PASS_ONLY'),
                'Unchanged hypothesis already tested after acquiring writer lock')
        require(project_path(recipe['source']).read_bytes() == source, 'Source changed before experiment')
        before = inputs()
        report = {'schema':1, 'task':task, 'epoch':status['epoch'], 'hypothesis':hypothesis,
                  'hypothesis_key':key, 'source':identity(source), 'recipe':recipe,
                  'started_utc':datetime.now(timezone.utc).isoformat(),
                  'input_fingerprint':fingerprint(before), 'budget_charge':False, 'escalate':False}
        try:
            result = check(task, promote=promote)
            report.update(status=result['status'], result=result)
        except ProbeFailure as error:
            report.update(status='FAILED', error=str(error), diagnostics=error.details,
                          budget_charge=True,
                          escalate=error.details['category'] in ['UNSUPPORTED_SOURCE','UNSUPPORTED_OBJECT','BINDING_REVIEW_REQUIRED'])
        except Exception as error:
            report.update(status='ERROR', error=str(error), error_type=type(error).__name__)
        if report['status'] in ['FAILED', 'ERROR'] and inputs() != before:
            report.update(status='ERROR', error='Inputs changed during failed experiment; inspect concurrent edits',
                          budget_charge=False, escalate=False)
        directory = ROOT/'recovery/attempts'/task
        existing = [int(p.name) for p in directory.iterdir() if p.is_dir() and p.name.isdigit()] if directory.exists() else []
        report['sequence'] = max(existing, default=0)+1
        destination = directory/f"{report['sequence']:04d}"
        destination.mkdir(parents=True, exist_ok=False)
        (destination/'source.c').write_bytes(source)
        receipt = report.get('diagnostics',{}).get('receipt') or report.get('result',{}).get('receipt') or report.get('result',{}).get('fast')
        if receipt and receipt.get('work_directory'):
            log = project_path(receipt['work_directory'])/'compiler.log'
            if log.exists(): (destination/'compiler.log').write_bytes(log.read_bytes())
        if receipt:
            if receipt.get('effective_code'):
                report['effective_key']=fingerprint({'code':receipt['effective_code'],
                    'context':hypothesis_key(recipe,b'')})
                report['same_effective_as']=[r['sequence'] for r in status['history']
                    if r.get('effective_key')==report['effective_key']]
            obj = receipt.get('object')
            if obj:
                report['same_object_as'] = [r['sequence'] for r in status['history']
                    if (r.get('diagnostics',{}).get('receipt',{}).get('object') == obj)]
        write_json(destination/'report.json', report)
        from reconstruction_factory import refresh
        refresh()
        report['report_path'] = (destination/'report.json').relative_to(ROOT).as_posix()
        return report
    finally:
        lock.unlink(missing_ok=True)


def reopen(task, reason):
    task_name(task); require(reason.strip(), 'Supervisor reopen needs a concrete new-evidence reason')
    require(not (ROOT/'build/grind.lock').exists() and not (ROOT/'build/promotion.lock').exists(), 'Active writer lock')
    inventory=read_json(ROOT/'recovery/restunts-inventory.json')
    found=[f for f in inventory['functions'] if f['name']==task]
    require(len(found)==1,'Unknown/ambiguous task')
    f=found[0]; interval=[f.get('start'),f.get('end')]
    path=ROOT/'recovery/task-control.json'
    control=read_json(path) if path.exists() else {'schema':1,'tasks':{}}
    old=control['tasks'].get(task,{'epoch':0})
    relevant=[b for b in read_json(ROOT/'recovery/blockers.json')['attempts'] if b['name']==task or
              (None not in interval and b.get('interval') and b['interval'][0]<interval[1] and interval[0]<b['interval'][1])]
    entry={'epoch':old['epoch']+1, 'reason':reason, 'released_blockers':[sha(json_bytes(b)) for b in relevant],
           'utc':datetime.now(timezone.utc).isoformat(), 'previous':old}
    control['tasks'][task]=entry
    write_json(path,control)
    from reconstruction_factory import refresh
    refresh()
    return entry


def main():
    p=argparse.ArgumentParser(description=__doc__); sub=p.add_subparsers(dest='command',required=True)
    for command in ['attempt','promote']:
        c=sub.add_parser(command); c.add_argument('task'); c.add_argument('--hypothesis',required=True)
    c=sub.add_parser('reopen'); c.add_argument('task'); c.add_argument('--reason',required=True)
    a=p.parse_args()
    if a.command=='reopen':
        print('Supervisor epoch:',reopen(a.task,a.reason)['epoch']);return
    result=run(a.task,a.hypothesis,a.command=='promote')
    print(result['status'], result['report_path'])
    if result['status'] in ['FAILED','ERROR']:
        print(result['error']);raise SystemExit(1)


if __name__=='__main__':main()
