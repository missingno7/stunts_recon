"""Generated candidate queues, bounded cards, blockers and current status."""
import argparse
import collections
from common import ROOT, read_json, write_json, require, sha, identity
from build_exact import inputs, production_inputs
from workflow import workflow_inputs, fingerprint, state, attempt_ledger, save_snapshot
from triage import capabilities, recipe_matches_inventory
from mz import MZ


def refresh():
    inventory=read_json(ROOT/'recovery/restunts-inventory.json')
    manifest=read_json(ROOT/'layout/manifest.json')
    blockers=read_json(ROOT/'recovery/blockers.json')
    promoted={o.get('name') for o in manifest['owners'] if o['kind']!='UNRESOLVED_RAW'}
    recipes={p.stem:read_json(p) for p in (ROOT/'recipes').glob('*.json')}
    from oracle import verify
    from compiler import verify_toolchain
    oracle=verify(write=False)
    image=MZ.parse(oracle[1]).load_image(oracle[1])
    from function_evidence import apply_reviewed
    reviewed=apply_reviewed(inventory,image)
    for profile in {o.get('profile') for o in manifest['owners'] if o.get('profile')} | {r['profile'] for r in recipes.values()}:
        verify_toolchain(profile)
    snapshot=inputs()
    workflow_snapshot=workflow_inputs()
    scope_ref=save_snapshot(snapshot)
    workflow_ref=save_snapshot(workflow_snapshot)
    ledger=attempt_ledger()
    control_path=ROOT/'recovery/task-control.json'
    control_context={'controls':read_json(control_path).get('tasks',{}) if control_path.exists() else {},
                     'blockers':blockers['attempts']}
    data_layout=read_json(ROOT/'layout/data-symbols.json')
    known_data={s['load_address']-data_layout['frame_load_address'] for s in data_layout['symbols'].values()}
    known_data.update(s['load_address']-data_layout['frame_load_address']+f['offset']
                      for s in data_layout['symbols'].values() for f in s.get('fields',[]))
    queue=[]
    small_index={f['name']:f for f in inventory['small_candidates']}
    for f in inventory['functions']:
        if f['name'] in promoted:continue
        if f.get('start') is not None and f.get('end') is not None and any(o['kind']!='UNRESOLVED_RAW' and o['start']<f['end'] and o['end']>f['start'] for o in manifest['owners']):continue
        complete=f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
        if complete:
            require(sha(image[f['start']:f['end']])==f['sha256'],'Inventory target hash mismatch: '+f['name'])
        disassembly=reviewed.get(f['name'],small_index.get(f['name'],{})).get('disassembly',[])
        capability_blockers,risks=capabilities(f,disassembly,known_data)
        if f['name'] in recipes:
            recipe=recipes[f['name']]
            if not recipe_matches_inventory(recipe,f):
                capability_blockers.append('Recipe identity/extent differs from current verified inventory; supervisor remapping required')
            elif f.get('binding_review') in ('external-far-call-v1','external-far-call-dgroup-offset16-v1') and recipe.get('binding',{}).get('mode')==f['binding_review']:
                # Reviewed bounded non-leaf mode; the strict binder still checks every
                # declaration, fixup field, public and ordered source relocation.
                capability_blockers=[b for b in capability_blockers if b not in
                    ['MZ relocation binding is unsupported','Calls need supervisor linking/TU review']]
        if not small_index.get(f['name'],{}).get('ordinary_c_hint'):
            risks.append('No ordinary-C hint; review source/compiler expressibility')
        if disassembly and complete:
            require(b''.join(bytes.fromhex(x['bytes']) for x in disassembly)==image[f['start']:f['end']],
                    'Triage disassembly differs from original extent: '+f['name'])
        tier='SUPERVISOR'
        if complete and f.get('size',999999)<=80 and not capability_blockers:
            tier='MEDIUM'
        if f['name'] in recipes and not capability_blockers:tier='CHEAP'
        task_state=state(f['name'],[f['start'],f['end']] if complete else None,ledger,control_context)
        if task_state['blocked']:tier='SUPERVISOR'
        identifier=f.get('stable_id') if complete else f['unresolved_evidence_id']
        row={'id':identifier,'name':f['name'],'tier':tier,'size':f.get('size'),
             'source':f['provenance'],'boundary_status':f['status'],
             'blocker':task_state['reason'] or ('; '.join(capability_blockers) if capability_blockers else None),
             'capability_blockers':capability_blockers,'risks':risks,'attempts_remaining':task_state['remaining'],
             'card':'recovery/cards/'+(f['name'] if f['name'] in recipes else identifier.replace(':','_'))+'.json'}
        queue.append(row)
        if f['name'] in recipes:
            recipe=recipes[f['name']]
            card={**row,'oracle_extent':[recipe['start'],recipe['end']],
                  'recipe':'recipes/'+f['name']+'.json','source':recipe['source'],'profile':recipe['profile'],
                  'evidence':f,'disassembly':disassembly,'scope_snapshot':scope_ref,'workflow_snapshot':workflow_ref,'attempt_budget':3,
                  'fast':'python tools/grind.py attempt '+f['name']+' --hypothesis "Explain source change"',
                  'promotion':'python tools/grind.py promote '+f['name']+' --hypothesis "Explain source hypothesis"',
                  'rules':['Complete emitted extent; no trimming, patching, byte arrays or inline assembly.',
                           'A local match is FAST only. Promotion recompiles the entire hybrid image.',
                           'Archive repeated compiler/ABI failures and escalate.']}
            write_json(ROOT/'recovery/cards'/(f['name']+'.json'),card)
    # Every queued item has a bounded evidence card, including blocked work.
    small_by_name={f['name']:f for f in inventory['small_candidates']}
    by_name={f['name']:f for f in inventory['functions']}
    for row in queue:
        if row['name'] in recipes:continue
        f=by_name[row['name']]
        card={**row,'evidence':f,'disassembly':reviewed.get(f['name'],small_by_name.get(f['name'],{})).get('disassembly',[]),
              'attempt_budget':3,'profile_hypothesis':'msc510-medium',
              'scope_snapshot':scope_ref,'workflow_snapshot':workflow_ref,
              'dependencies':['Reviewed candidate C source and recipe','Verify full compiler contribution and all OMF fixups'],
              'next_action':'For a fully mapped function: python tools/prepare_candidate.py '+row['id']+' --source recovery/candidates/NAME.c',
              'fast':'Unavailable until a reviewed recipe exists; no raw/unchecked acceptance',
              'promotion':'After FAST: python tools/check_candidate.py TASK --promote'}
        write_json(ROOT/'recovery/cards'/(row['id'].replace(':','_')+'.json'),card)
    queue.sort(key=lambda r:({'CHEAP':0,'MEDIUM':1,'SUPERVISOR':2}[r['tier']],len(r['risks']),r.get('size') or 999999,r['id']))
    require(len({r['id'] for r in queue})==len(queue),'Duplicate queue IDs')
    counts=dict(collections.Counter(r['tier'] for r in queue))
    require(workflow_inputs()==workflow_snapshot,'Workflow changed during queue generation')
    write_json(ROOT/'recovery/queue.json',{'schema':2,'counts':counts,'tasks':queue,
               'input_fingerprint':sha(str(sorted(snapshot.items())).encode()),'workflow_fingerprint':fingerprint(workflow_snapshot)})
    lock=read_json(ROOT/'layout/oracle.lock.json')
    from attempt_index import generate
    generate()
    write_json(ROOT/'recovery/context-index.json',{'schema':1,'oracle':lock['load_image'],
        'attempt_index_sha256':sha((ROOT/'recovery/attempt-index.json').read_bytes()),
        'workflow_fingerprint':fingerprint(workflow_snapshot),
        'tasks':[{'id':r['id'],'name':r['name'],'card':r['card'],'card_sha256':sha((ROOT/r['card']).read_bytes())} for r in queue]})
    acceptance_path=ROOT/'build/exact/acceptance.json'
    acceptance=read_json(acceptance_path) if acceptance_path.exists() else None
    executable_path=ROOT/'build/exact/mcga.exe'
    current=bool(acceptance and acceptance.get('production_inputs')==production_inputs() and executable_path.exists()
                 and identity(executable_path.read_bytes())==acceptance['executable'])
    code=set()
    for f in inventory['functions']:
        if f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':code.update(range(f['start'],f['end']))
    library=read_json(ROOT/'recovery/library-evidence.json')
    lib=set()
    for m in library['proven_literal_runtime_segments']:
        start=m['load_offsets'][0];lib.update(range(start,start+m['size']))
    # DGROUP start is an address-evidence classification, not recovered data.
    data=set(range(0x2b770,lock['load_image']['size']))
    require(not data & code and not data & lib,'Classification overlap')
    code-=lib
    for o in manifest['owners']:
        if o['kind']=='MATCHING_C':code.update(range(o['start'],o['end']))
    classified=code|lib|data
    cbytes=sum(o['end']-o['start'] for o in manifest['owners'] if o['kind']=='MATCHING_C')
    library_bytes=sum(o['end']-o['start'] for o in manifest['owners'] if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY')
    bound=set()
    matching=set()
    for o in manifest['owners']:
        if o['kind']=='MATCHING_C':matching.update(range(o['start'],o['end']))
        if o['kind']!='UNRESOLVED_RAW':bound.update(range(o['start'],o['end']))
    status={'oracle':{k:lock[k] for k in ['packed','unpacked','load_image']},
            'source_code_segments':sum(s['class']=='STUNTSC' for s in inventory['segments']),
            'source_data_segments':sum(s['class']=='STUNTSD' for s in inventory['segments']),
            'procedures':len(inventory['functions']),'mapped_functions':sum(f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or f['name'] in {o.get('name') for o in manifest['owners'] if o['kind']=='MATCHING_C'} for f in inventory['functions']),
            'segment_frames_proven':sum('segment_paragraph'in s for s in inventory['segments']),
            'classification_bytes':{'code_evidence_excluding_library':len(code),'data_evidence':len(data),'library_identified':len(lib),'unknown':lock['load_image']['size']-len(classified)},
            'matching_c_bytes':cbytes,'matching_asm_bytes':0,'library_production_bytes':library_bytes,
            'identified_library_bytes_still_raw':len(lib-bound),
            'raw_initialized_bytes':lock['load_image']['size']-cbytes-library_bytes,
            'raw_executable_bytes_confirmed_minimum':len((code|lib)-bound),
            'raw_executable_bytes_exact':'unknown until all code/data boundaries are established',
            'full_image_status':acceptance['status'] if current else 'NOT_CURRENTLY_VERIFIED',
            'queue_counts':{tier:counts.get(tier,0) for tier in ['CHEAP','MEDIUM','SUPERVISOR']},
            'compiler':'MSC5.0/5.1 medium-model optimized, stack checking off; production pins MSC5.1; unique version/flags not proven',
            'blockers':['Supported: external DGROUP offset16 and reviewed zero-addend external far CALLs; general/self-relative linking and multi-public production remain open',
                        'QuickC BAKPAT rejected; full compiler/version fingerprint remains open',
                        *(['Identified runtime bytes remain raw-owned pending library binding'] if lib-bound else []),
                        'Archived CRT startup checksum anomaly and general runtime linking remain unresolved',
                        'Unmapped functions and internal code/data ambiguity remain'],
            'promoted_game_functions':sorted(o['name'] for o in manifest['owners'] if o['kind']=='MATCHING_C')}
    write_json(ROOT/'docs/current/status.json',status)
    text='# Current generated status\n\n'
    text+=f"Full image: **{status['full_image_status']}**. Matching C: **{cbytes} bytes**; raw initialized: **{status['raw_initialized_bytes']} bytes**.\n\n"
    text+=f"Mapped {status['mapped_functions']} / {status['procedures']} procedures; {status['source_code_segments']} source code segments and {status['segment_frames_proven']} proven segment frames.\n\n"
    text+='Classification counts are conservative evidence counts, not a complete code/data partition. See status.json for unknown bytes.\n\n'
    text+='Queue: '+', '.join(f'{k}={v}' for k,v in status['queue_counts'].items())+'.\n\n'
    text+='Compiler: '+status['compiler']+'.\n\n'
    text+='Blockers:\n\n'+''.join('- '+b+'\n' for b in status['blockers'])
    (ROOT/'docs/current/status.md').write_text(text,encoding='utf-8')
    print('Refreshed',status['full_image_status'],status['queue_counts'])
    return status


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('command',choices=['refresh','reclassify','next'])
    p.add_argument('--tier',choices=['READY','CHEAP','MEDIUM','SUPERVISOR'],default='READY')
    args=p.parse_args()
    if args.command=='refresh':refresh()
    elif args.command=='reclassify':
        # One current-state transaction for supervisor research. Neither
        # report changes eligibility, attempt budgets, source, or ownership.
        refresh()
        from blocker_census import run as census
        from build_topology import run as topology
        from candidate_omf_census import run as candidate_omf
        from partial_mapping_census import run as partial_mapping
        from table_offset_probe import run as table_offsets
        table_offsets()
        census()
        topology()
        candidate_omf()
        partial_mapping()
    else:
        q=read_json(ROOT/'recovery/queue.json')
        require(q.get('workflow_fingerprint')==fingerprint(workflow_inputs()),'Queue is stale; run refresh after reviewing changes')
        selected=next((t for t in q['tasks'] if t['tier']==args.tier or (args.tier=='READY' and t['tier'] in ['CHEAP','MEDIUM'])),None)
        print(selected if selected else 'No '+args.tier+' tasks; supervisor capability or mapping work is required')

if __name__=='__main__':main()
