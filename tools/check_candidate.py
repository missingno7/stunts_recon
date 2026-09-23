"""FAST -> fresh entire hybrid build -> serial MATCHING_C promotion."""
import argparse
import copy
import os
import json
from common import ROOT, read_json, write_json, require, project_path, identity, sha, json_bytes
from probe_module import probe
from build_exact import build, inputs
from workflow import check_workflow_scope, task_name, control_inputs, load_snapshot


def check_scope(card, recipe):
    current = inputs()
    allowed = {recipe['source']}
    expected = load_snapshot(card['scope_snapshot'])
    require({k:v for k,v in current.items() if k not in allowed} ==
            {k:v for k,v in expected.items() if k not in allowed},
            'Changes outside candidate source scope; supervisor must refresh reviewed queue')


def replace_raw(manifest, recipe):
    result = copy.deepcopy(manifest)
    start, end = recipe['start'], recipe['end']
    overlapping = [o for o in result['owners'] if o['start'] < end and o['end'] > start]
    require(len(overlapping) == 1 and overlapping[0]['kind'] == 'UNRESOLVED_RAW', 'Candidate is not wholly raw-owned')
    owner = overlapping[0]
    require(owner['start'] <= start < end <= owner['end'], 'Candidate crosses ownership')
    split = []
    if owner['start'] < start:
        split.append({**owner, 'end': start, 'id': f"raw_{owner['start']:05x}_{start:05x}"})
    split.append({'id':recipe['stable_id'], 'name':recipe['id'], 'start':start, 'end':end,
                  'kind':'MATCHING_C','classification':'GAME_C', 'recipe':'recipes/'+recipe['id']+'.json'})
    if end < owner['end']:
        split.append({**owner, 'start':end, 'id':f"raw_{end:05x}_{owner['end']:05x}"})
    index = result['owners'].index(owner)
    result['owners'][index:index+1] = split
    return result


def check(task, promote=False, scope=True):
    task_name(task)
    path = ROOT / 'recipes' / (task + '.json')
    recipe = read_json(path)
    if scope:
        card=read_json(ROOT/'recovery/cards'/(task+'.json'))
        check_scope(card, recipe)
        check_workflow_scope(card,recipe)
        require(card['tier']=='CHEAP' and not card.get('capability_blockers'),'Candidate needs supervisor capability review')
    control_before=control_inputs() if scope else None
    before = inputs()
    payload, fast = probe(recipe)
    require(inputs() == before, 'Inputs changed during FAST')
    if scope: require(control_inputs()==control_before, 'Workflow controls changed during FAST')
    if not promote:
        return {'status':'FAST_PASS_ONLY', 'task':task, 'bytes':len(payload), 'receipt':fast}
    lock = ROOT/'build/promotion.lock'
    fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    os.close(fd)
    destination = ROOT/'src'/(task+'.c')
    original_recipe = original_manifest = original_plan = None
    canonical_changed = False
    committed = False
    created = False
    try:
        require(inputs()==before, 'Inputs changed after FAST before promotion lock')
        if scope:
            check_scope(read_json(ROOT/'recovery/cards'/(task+'.json')),recipe)
            check_workflow_scope(read_json(ROOT/'recovery/cards'/(task+'.json')),recipe)
        require(not destination.exists(), 'Promotion destination already exists')
        original_recipe = path.read_bytes()
        original_manifest = (ROOT/'layout/manifest.json').read_bytes()
        original_plan = (ROOT/'layout/production-plan.json').read_bytes()
        source = project_path(recipe['source']).read_bytes()
        require(identity(source) == fast['source'], 'Candidate changed since FAST')
        destination.write_bytes(source)
        created = True
        expected_staged={**before,destination.relative_to(ROOT).as_posix():sha(source)}
        require(inputs()==expected_staged,'Unexpected edits while staging candidate')
        staged = {**recipe, 'source':destination.relative_to(ROOT).as_posix()}
        manifest = replace_raw(read_json(ROOT/'layout/manifest.json'), staged)
        fresh = build(manifest, {'recipes/'+task+'.json':staged}, publish=False)
        require(fresh['inputs']==expected_staged and inputs()==expected_staged,'Out-of-scope edit since FAST')
        if scope:require(control_inputs()==control_before,'Workflow controls changed during staged promotion')
        require(project_path(recipe['source']).read_bytes() == source, 'Recovery source changed during full build')
        plan = json.loads(original_plan)
        plan['modules'] = [{'owner':o['id'],'recipe':o['recipe'], 'source':(staged if o['recipe']=='recipes/'+task+'.json' else read_json(ROOT/o['recipe']))['source']} if o['kind']=='MATCHING_C' else {'owner':o['id'],'library':o['library'],'module':o['module']}
                           for o in manifest['owners'] if o['kind']!='UNRESOLVED_RAW']
        expected_canonical = {**expected_staged, 'recipes/'+task+'.json':sha(json_bytes(staged)),
                              'layout/manifest.json':sha(json_bytes(manifest)),
                              'layout/production-plan.json':sha(json_bytes(plan))}
        canonical_changed = True
        write_json(path, staged)
        write_json(ROOT/'layout/manifest.json', manifest)
        write_json(ROOT/'layout/production-plan.json', plan)
        require(inputs()==expected_canonical,'Unexpected edits during canonical C staging')
        if scope:require(control_inputs()==control_before,'Workflow controls changed during canonical staging')
        acceptance = build()
        require(acceptance['inputs']==expected_canonical and inputs()==expected_canonical,
                'Canonical C build absorbed out-of-scope edits')
        if scope:require(control_inputs()==control_before,'Workflow controls changed during canonical build')
        record = {'status':'PROMOTED', 'task':task, 'bytes':len(payload), 'fast':fast,
                  'staged_full_build':fresh['executable'], 'canonical_full_build':acceptance['executable'],
                  'candidate_compiled_fresh_at_least':3}
        write_json(ROOT/'recovery/promotions'/(task+'.json'), record)
        committed = True
        return record
    finally:
        if not committed:
            if canonical_changed:
                path.write_bytes(original_recipe)
                (ROOT/'layout/manifest.json').write_bytes(original_manifest)
                (ROOT/'layout/production-plan.json').write_bytes(original_plan)
            if created:
                destination.unlink(missing_ok=True)
            (ROOT/'build/exact/acceptance.json').unlink(missing_ok=True)
        lock.unlink(missing_ok=True)


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('task')
    p.add_argument('--promote', action='store_true')
    p.add_argument('--hypothesis',default='Source hypothesis supplied through check_candidate.py')
    args=p.parse_args()
    try:
        from grind import run
        result=run(args.task,args.hypothesis,args.promote)
        print(result['status'],args.task,result['report_path'])
        if result['status'] in ['FAILED','ERROR']:raise SystemExit(1)
    except Exception as error:
        write_json(ROOT/'build/last-candidate-failure.json', {'task':args.task,'error':str(error)})
        raise

if __name__=='__main__':
    main()
