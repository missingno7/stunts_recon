"""Compact searchable view of immutable attempts, never an acceptance authority.

Routine controls still hash all archived attempts conservatively. This index saves
context reads, not correctness checks. Full validation rebuilds/checks the view.
"""
from common import ROOT, read_json, write_json, identity, json_bytes


def lost_anchors(previous,current):
    """Only comparable engine/target/mode evidence; omissions make this a lower bound."""
    if any(previous.get(k)!=current.get(k) for k in ('engine_sha256','target','comparison')):return None
    if current.get('omitted_anchors'):return None
    lost=[]
    for anchor in previous['anchors']:
        pending=[anchor['target']['bytes']]
        for kept in current['anchors']:
            a,z=kept['target']['bytes'];next_pending=[]
            for lo,hi in pending:
                if hi<=a or lo>=z:next_pending.append([lo,hi])
                else:
                    if lo<a:next_pending.append([lo,a])
                    if z<hi:next_pending.append([z,hi])
            pending=next_pending
        lost.extend(pending)
    return {'lost_target_byte_ranges':lost[:12],'lost_bytes':sum(z-a for a,z in lost),
            'omitted_ranges':max(0,len(lost)-12),
            'authority':'Diagnostic only; bounded summaries may omit anchors. Consult both full artifacts.'}


def generate(write=True):
    tasks={}
    records=[]
    for folder in ['attempts','experiments','diagnostics']:
        paths=(ROOT/'recovery'/folder).glob('*/*/report.json')
        for path in sorted(paths,key=lambda p:(p.parent.parent.name,not p.parent.name.startswith('prior_'),p.parent.name)):
            r=read_json(path)
            stamp=r.get('started_utc',r.get('utc',''))
            if not stamp and folder=='diagnostics':
                from datetime import datetime,timezone
                stamp=datetime.strptime(path.parent.name,'%Y%m%dT%H%M%S%fZ').replace(tzinfo=timezone.utc).isoformat()
            records.append((stamp,folder,path,r))
    # Undated legacy evidence comes first. Timestamp ties preserve stable folder/path order.
    for _,folder,path,r in sorted(records,key=lambda item:item[0]):
        task=r.get('task',path.parent.parent.name)
        diagnostic=r.get('diagnostics',{});comparison=diagnostic.get('comparison',r.get('comparison',{}))
        row={'path':path.relative_to(ROOT).as_posix(),'identity':identity(path.read_bytes()),
             'kind':folder,'epoch':r.get('epoch'),'status':r.get('status',r.get('scope')),
             'hypothesis':r.get('hypothesis',r.get('prediction')),
             'category':diagnostic.get('category'),'error':r.get('error'),
             'diagnostic_error':comparison.get('diagnostic_error'),
             'evidence_freshness':r.get('evidence_freshness'),
             'sizes':diagnostic.get('emitted_sizes',r.get('emitted_sizes')),
             'first_difference':comparison.get('first_differing_instruction'),
             'same_effective_as':r.get('same_effective_as',r.get('same_object_as',[]))}
        if comparison.get('match_summary'):
            from diagnostics import compact
            summary=compact(comparison['match_summary'])
            for key in ('omitted_anchors','omitted_islands'):
                summary[key]+=comparison['match_summary'].get(key,0)
            row.update(match_summary=summary,source=r.get('source'),recipe_identity=identity(json_bytes(r['recipe'])) if r.get('recipe') else None,
                       full_diagnostic=comparison.get('full_diagnostic'),
                       full_diagnostic_identity=comparison.get('full_diagnostic_identity'))
            previous=next((p for p in reversed(tasks.get(task,[])) if p.get('match_summary') and
                           lost_anchors(p['match_summary'],summary) is not None),None)
            if previous:
                row['anchor_regression']={**lost_anchors(previous['match_summary'],summary),'previous_report':previous['path']}
        tasks.setdefault(task,[]).append(row)
    result={'schema':1,'authority':'Derived search view; immutable archives/control_inputs remain authoritative','tasks':tasks}
    if write:write_json(ROOT/'recovery/attempt-index.json',result)
    return result
