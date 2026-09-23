"""Grinder control state, distinct from immutable construction inputs."""
import re
from common import ROOT, read_json, sha, json_bytes, require
from build_exact import inputs


def task_name(task):
    require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', task) is not None, 'Invalid task name')
    return task


def control_inputs():
    result = {}
    for name in ['recovery/blockers.json', 'recovery/restunts-inventory.json',
                 'recovery/library-evidence.json', 'recovery/task-control.json']:
        path = ROOT/name
        result[name] = sha(path.read_bytes()) if path.exists() else None
    for path in sorted((ROOT/'recovery/attempts').rglob('*')):
        if path.is_file(): result[path.relative_to(ROOT).as_posix()] = sha(path.read_bytes())
    return result


def workflow_inputs():
    return {**inputs(), **control_inputs()}


def fingerprint(snapshot):
    return sha(json_bytes(snapshot))


def attempts(task):
    task_name(task)
    return [read_json(p) for p in sorted((ROOT/'recovery/attempts'/task).glob('*/report.json'))]


def attempt_ledger():
    return [read_json(p) for p in sorted((ROOT/'recovery/attempts').glob('*/*/report.json'))]


def state(task, interval=None, ledger=None):
    task_name(task)
    path = ROOT/'recovery/task-control.json'
    controls = read_json(path).get('tasks', {}) if path.exists() else {}
    control = controls.get(task,{})
    epoch = control.get('epoch', 0)
    ledger = attempt_ledger() if ledger is None else ledger
    history = [r for r in ledger if r['task']==task and r['epoch'] == epoch]
    blockers = read_json(ROOT/'recovery/blockers.json')['attempts']
    relevant = [b for b in blockers if b['name'] == task or
                (interval and b.get('interval') and b['interval'][0] < interval[1] and interval[0] < b['interval'][1])]
    released = control.get('released_blockers', [])
    relevant = [b for b in relevant if sha(json_bytes(b)) not in released]
    failures = sum(r.get('budget_charge', False) for r in history)
    escalated = next((r for r in reversed(history) if r.get('escalate')), None)
    reason = (relevant[0]['reason'] if relevant else
              escalated['error'] if escalated else
              'Three distinct failed hypotheses; supervisor review required' if failures >= 3 else None)
    if interval and not reason:
        for other in sorted({r['task'] for r in ledger}-{task}):
            rows=[r for r in ledger if r['task']==other and r['epoch']==controls.get(other,{}).get('epoch',0)]
            overlapping=[r for r in rows if r['recipe']['start'] < interval[1] and interval[0] < r['recipe']['end']]
            if any(r.get('escalate') for r in overlapping) or sum(r.get('budget_charge',False) for r in overlapping)>=3:
                reason='Overlapping failed interval from '+other+'; supervisor must reopen that original task'
                break
    return {'epoch':epoch, 'failures':failures, 'remaining':max(0, 3-failures),
            'blocked':bool(reason), 'reason':reason, 'history':history}


def require_eligible(recipe):
    status = state(recipe['id'], [recipe['start'], recipe['end']])
    require(not status['blocked'], 'Task is blocked: '+str(status['reason']))
    return status


def check_workflow_scope(card, recipe):
    expected = card.get('workflow_snapshot')
    require(expected is not None, 'Legacy card; refresh reviewed queue')
    allowed = {recipe['source']}
    require({k:v for k,v in workflow_inputs().items() if k not in allowed} ==
            {k:v for k,v in expected.items() if k not in allowed},
            'Workflow evidence/attempt state changed; refresh reviewed queue')
    require_eligible(recipe)


def hypothesis_key(recipe, source):
    # Changes to another candidate or unrelated production ownership do not make
    # an unchanged failed source worth recompiling. Compiler/binder revisions do.
    context = {k:v for k,v in inputs().items() if k.startswith('tools/') or
               k in ['layout/toolchain.json', 'layout/data-symbols.json', 'layout/oracle.lock.json']}
    return fingerprint({'recipe':{k:v for k,v in recipe.items() if k != 'source'},
                        'source':sha(source), 'capability_context':context})
