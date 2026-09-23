"""Small actionable task packet; expand evidence explicitly, never silently truncate."""
import argparse
import json
from common import ROOT, read_json, require, sha, identity, json_bytes


def packet(identifier, expansions=()):
    index=read_json(ROOT/'recovery/context-index.json')
    matches=[r for r in index['tasks'] if identifier in (r['id'],r['name'])]
    require(len(matches)==1,'Unknown/ambiguous task; refresh queue and use stable ID or exact name')
    row=matches[0];path=ROOT/row['card'];raw=path.read_bytes()
    require(sha(raw)==row['card_sha256'],'Context index/card changed; refresh queue')
    card=json.loads(raw);e=card['evidence']
    rpath=card.get('recipe');recipe=read_json(ROOT/rpath) if rpath else None
    profile=recipe['profile'] if recipe else card.get('profile_hypothesis','msc510-medium')
    config=read_json(ROOT/'layout/toolchain.json')['profiles'][profile]
    # A digest check is deliberately conservative; a stale packet cannot authorize work.
    from workflow import workflow_inputs, fingerprint, state
    current=fingerprint(workflow_inputs())==index['workflow_fingerprint']
    status=state(row['name'],[e['start'],e['end']] if e.get('stable_id') else None)
    out={'id':row['id'],'name':row['name'],'packet_current':current,
         'oracle':index['oracle'],'extent':{k:e.get(k) for k in ['start','end','size','sha256','confidence']},
         'status':card['tier'],'blockers':card['capability_blockers']+[status['reason']] if status['reason'] else card['capability_blockers'],
         'risks':card['risks'],'abi':e.get('abi','Not established; consult assembly before proposing types'),
         'compiler':{'profile':profile,'flags':config['flags'],'identity_lock':'layout/toolchain.json'},
         'evidence':{'card':row['card'],'card_sha256':row['card_sha256'],'reference':e['provenance'],
                     'source_locations':e.get('c_sources',[]),'function_overlay':'layout/function-evidence.json' if e.get('boundary_anchors') else None},
         'remaining_budget':status['remaining'],'next':('Review new evidence and reopen original blocked task' if status['blocked'] else
            card.get('fast',card.get('next_action'))),
         'commands':{'context':'python tools/context.py '+row['id'],
                     'refresh':'python tools/reconstruction_factory.py refresh',
                     'validate':'python tools/validate.py'}}
    if not current:out['next']='Packet is stale; review changes then refresh queue before attempting'
    if recipe:
        source=(ROOT/recipe['source']).read_text();lines=source.splitlines()
        out['candidate']={'path':recipe['source'],'sha256':sha((ROOT/recipe['source']).read_bytes()),
                          'excerpt':'\n'.join(lines[:35]),'omitted_lines':max(0,len(lines)-35),'full_source':recipe['source'],
                          'bindings':recipe.get('binding',{}).get('mode','no-fixups'),
                          'fixup_targets':sorted({f['target'] for f in recipe['expected_fixups']}),
                          'relocations':recipe['expected_relocations']}
    ledger_path=ROOT/'recovery/attempt-index.json'
    require(sha(ledger_path.read_bytes())==index['attempt_index_sha256'],'Attempt index changed; refresh queue')
    ledger=read_json(ledger_path)
    history=ledger['tasks'].get(row['name'],[])
    formal=[r for r in history if r['kind']=='attempts']
    if formal:out['latest_grinder_attempt']=formal[-1]
    out['hypotheses']=history if 'history' in expansions else history[-3:]
    out['omitted_history']=max(0,len(history)-len(out['hypotheses']))
    if history:out['next_discriminator']=history[-1].get('next_discriminator','Change the predicted instruction/ABI outcome, not just the prose hypothesis')
    latest=next((r for r in reversed(history) if r.get('match_summary')),None)
    if latest:
        out['match_diagnosis']={k:latest.get(k) for k in ['path','match_summary','full_diagnostic','full_diagnostic_identity','anchor_regression']}
        out['match_diagnosis']['source_matches_current']=bool(recipe and (latest.get('source') or {}).get('sha256')==out['candidate']['sha256'])
        out['match_diagnosis']['recipe_matches_current']=bool(recipe and latest.get('recipe_identity')==identity(json_bytes(recipe)))
        import diagnostics
        from pathlib import Path
        out['match_diagnosis']['engine_matches_current']=latest['match_summary']['engine_sha256']==sha(Path(diagnostics.__file__).read_bytes())
        out['match_diagnosis']['freshness']='Archived observation, not a fresh FAST result; check source, target, compiler and engine before drawing conclusions.'
        out['next_discriminator']='Inspect the localized '+', '.join(latest['match_summary']['classifications'])+' evidence; preserve exact anchors. Resolve binding/TU blockers separately; no source-level cause is inferred.'
        # Keep a single summary in the default packet; --history explicitly expands older ones.
        if 'history' not in expansions:
            def reference(r):return {k:v for k,v in r.items() if k not in ('match_summary','anchor_regression')}
            out['hypotheses']=[reference(r) for r in out['hypotheses']]
            if formal:out['latest_grinder_attempt']=reference(formal[-1])
    out['expansion']='--asm --callers --globals --history --full; full raw artifacts stay at referenced paths'
    if 'asm' in expansions:out['assembly']=card.get('disassembly',[])
    else:out['omitted_assembly_rows']=len(card.get('disassembly',[]))
    if 'callers' in expansions:out['callers']=e.get('callers','No complete caller index; indirect references remain unbounded')
    if 'globals' in expansions:
        out['global_evidence']={'data':'layout/data-symbols.json','code':'layout/code-symbols.json',
                               'constraint':'Only reviewed bindings are supported; indexed displacements may be fields'}
    if 'full' in expansions:out['full_card']=card
    return out


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('function')
    for name in ['asm','callers','globals','history','full']:p.add_argument('--'+name,action='store_true')
    p.add_argument('--diagnosis',action='store_true',help='Print concise match diagnosis and full artifact path')
    a=p.parse_args();result=packet(a.function,[n for n in ['asm','callers','globals','history','full'] if getattr(a,n)])
    if a.diagnosis:
        from diagnostics import format_summary
        diagnosis=result.get('match_diagnosis',{})
        print(format_summary(diagnosis.get('match_summary'),result['name'],'ARCHIVED OBSERVATION'))
        print('Source matches current:',diagnosis.get('source_matches_current'))
        print('Full diagnostic:',diagnosis.get('full_diagnostic'))
    else:print(json.dumps(result,indent=2))


if __name__=='__main__':main()
