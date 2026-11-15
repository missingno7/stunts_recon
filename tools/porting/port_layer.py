"""Join existing port inventories; do not infer equivalence from lexical signals.

The table is a review/coverage index. Source spans and original-code tests retain
their existing authorities and claim limits; missing mappings remain explicit.
"""
from __future__ import annotations
import argparse
import ast
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(Path(__file__).parent))
from semantic_audit import functions

def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def build(audit,comparisons=()):
    asm=json.loads((ROOT/'docs/porting/asm-migration.json').read_text())
    platform=json.loads((ROOT/'docs/porting/platform-boundary.json').read_text())
    contracts=json.loads((ROOT/'port/semantic-contracts.json').read_text())
    mappings={r['address']:r for r in audit['historical_mapping']}
    rows=[]
    def add(kind,identifier,**fields):
        row=dict(id=f'{kind}:{identifier}',kind=kind,**fields)
        row.update(exercised=False,status='not exercised',evidence=[],capture_contract=None)
        rows.append(row);return row
    for i,r in enumerate(asm['routines']):
        address=(r.get('target_extent') or {}).get('start')
        mapped=mappings.get(address,{})
        add('asm',r['owner_id']+'::'+r['public_symbol'],name=r['routine'],
            original_address=address,original_extent=r.get('target_extent'),
            source_ref=f'docs/porting/asm-migration.json#/routines/{i}',
            category=r['category'],entry_abi=r.get('entry_abi'),
            portable=mapped.get('portable_names',[]),mapping_status=mapped.get('mapping_status','unmapped'),
            original_module=next((o.get('module') for o in asm['owners'] if o['owner_id']==r['owner_id']),None))
    for i,r in enumerate(platform['entries']):
        address=int(r['image_load_offset'],0) if r.get('image_load_offset') else None
        add('host',r['function']+'@'+str(r.get('image_load_offset')),name=r['function'],
            original_address=address,interface=r['interface'],subsystem=r['subsystem'],
            source_ref=f'docs/porting/platform-boundary.json#/entries/{i}',
            portable=mappings.get(address,{}).get('portable_names',[]))
    for r in audit['functions']:
        counterparts=[p for p in r['portable_counterparts'] if p.get('body_changed')]
        if r.get('historical_source') and counterparts:
            add('adapted_c',r['id'],name=r['historical_name'],original_source=r['historical_source'],
                original_line=r['historical_line'],portable=counterparts,
                source_ref='semantic_audit.functions:'+r['id'],prior_evidence=r['evidence_claims'])
    # Explicit compiler/source transformations, including build.py substitutions.
    adapter_paths=list((ROOT/'port').rglob('*.py'))+[ROOT/'tools/porting'/name for name in
        ('host_probe_modes.py','host_probe_declarations.py','project_headers.py')]
    for path in sorted(adapter_paths):
        text=path.read_text(encoding='utf-8');tree=ast.parse(text)
        for node in ast.walk(tree):
            if not isinstance(node,(ast.FunctionDef,ast.AsyncFunctionDef)):continue
            candidates=[]
            for call in ast.walk(node):
                if not isinstance(call,ast.Call):continue
                callee=ast.unparse(call.func)
                if (callee.endswith(('.replace','.sub','.subn')) or callee.startswith('adapt_')
                    or callee in ('_function','route_audio_vectors','probe.transformed_source','host_view_contracts')):
                    candidates.append(dict(line=call.lineno,callee=callee,
                        expression=ast.get_source_segment(text,call)))
            relative=path.relative_to(ROOT).as_posix()
            for operation_index,operation in enumerate(candidates):
                add('build_adapter',relative+'::'+node.name+':'+str(operation_index),name=node.name,
                    portable=[dict(source=relative,line=operation['line'],name=node.name)],operation=operation,
                    function_line=node.lineno,function_sha256=hashlib.sha256(ast.get_source_segment(text,node).encode()).hexdigest())
            if not candidates and (node.name.startswith('adapt_') or node.name in ('phase_adapt','instrument')):
                relative=path.relative_to(ROOT).as_posix()
                add('build_adapter',relative+'::'+node.name,name=node.name,
                    portable=[dict(source=relative,line=node.lineno,name=node.name)],
                    operation=dict(line=node.lineno,callee='generated overlay',expression=ast.get_source_segment(text,node)))
    known={(p['source'],p['name']) for r in rows for p in r.get('portable',[])}
    native={}
    for r in audit['functions']:
        for p in r['portable_counterparts']:
            if p['source'].startswith('port/'):native[p['source'],p['name']]=p
    for path in sorted((ROOT/'port').rglob('*.c')):
        relative=path.relative_to(ROOT).as_posix()
        if path.parent==ROOT/'port':continue # already parsed by semantic_audit
        for name,f in functions(path.read_text(encoding='latin-1')).items():
            native[relative,name]=dict(source=relative,name=name,line=f['line'],signature=f['signature'])
    for path in sorted(list((ROOT/'port').rglob('*.h'))+list((ROOT/'tools/porting/host').rglob('*.h'))+
                       list((ROOT/'tools/porting/port_include').rglob('*.h'))):
        relative=path.relative_to(ROOT).as_posix();text=path.read_text(encoding='latin-1')
        for name,f in functions(text).items():
            native[relative,name]=dict(source=relative,name=name,line=f['line'],signature=f['signature'])
        import re
        abi_names={'I8','I8S','U8','I16','U16','I16S','U16S','I32','U32',
            'far','near','huge','cdecl','pascal','interrupt','loadds','fastcall'}
        for m in re.finditer(r'^\s*#\s*define\s+(\w+)([^\n]*)',text,re.M):
            if not m[2].startswith('(') and m[1].lstrip('_') not in abi_names:continue
            line=text.count('\n',0,m.start())+1
            add('semantic_macro',relative+'::'+m[1]+':'+str(line),name=m[1],portable=[dict(source=relative,
                name=m[1],line=line)],expression=m[0].strip())
    for key,p in sorted(native.items()):
        if key not in known:
            add('native_helper',key[0]+'::'+key[1],name=key[1],portable=[p])
    for i,c in enumerate(contracts['contracts']):
        add('semantic_contract',c['id'],name=c['id'],
            source_ref=f'port/semantic-contracts.json#/contracts/{i}',
            original_authority=c['authority'],routines=c.get('routines',[]),
            portable_routes=c['portable_route'],claim_limit=c['claim_limit'])
    for row in rows:
        names={row['name']}|{p['name'] for p in row.get('portable',[])}
        evidence=[c for c in contracts['contracts'] if names & set(c.get('routines',[]))
                  and c['authority']=='locked_image_live_execution' and c['status']=='differential-oracle']
        row['prior_original_tests']=[dict(contract=c['id'],scope=c['evidence_scope'],tests=c['tests'],
            claim_limit=c['claim_limit']) for c in evidence]
        if evidence:
            row.update(status='exercised and agrees',exercised=True,
                evidence_basis='existing finite original-machine differential tests; not the whole input domain')
    sys.path.insert(0,str(ROOT/'port'))
    from boundary_adapter import CONTRACTS
    captured={c['name']:c for c in CONTRACTS};observed={}
    for comparison in comparisons:
        report=json.loads(comparison.read_text())
        for name,counts in (report['routines']|report.get('driver_entries',{})).items():
            item=observed.setdefault(name,dict(agrees=0,differs=0,unmatched_inputs=0))
            for key in ('agrees','differs','unmatched_inputs'):item[key]+=counts.get(key,0)
            if 'ordered_step_stream_agrees' in counts:
                item['ordered_step_stream_agrees']=item.get('ordered_step_stream_agrees',True) and counts['ordered_step_stream_agrees']
    for row in rows:
        names={row['name']}|{p['name'] for p in row.get('portable',[])}
        for name in sorted(names&captured.keys()):
            c=captured[name]
            row['capture_contract']=dict(name=name,args=c['args'],return_bits=c['ret'],
                regions=[dict(name=n,bytes=size,read=read) for n,expr,size,read in c.get('regions',[])],
                scope='outermost foreground calls; known bounded spans, not interrupt implementation internals')
        for name in sorted(names&observed.keys()):
            counts=observed[name];row['evidence'].append(dict(original_capture='DOSBox-X semantic race',
                name=name,**counts,claim_limit='finite calls with identical bounded inputs; unmatched inputs are not passes'))
            if counts['agrees'] or counts['differs']:
                row.update(exercised=True,status='exercised and differs' if counts['differs'] or
                    counts.get('ordered_step_stream_agrees') is False else 'exercised and agrees')
            if name.startswith('port_audio_driver_'):
                row['capture_contract']=dict(name=name,scope='typed arguments, stable IDs, command payload, defined return; private driver memory unobserved')
        if 'int86' in names and 'int33' in observed:
            row.update(exercised=True,status='exercised and agrees',capture_contract=dict(service='INT15 C201/INT33',
                scope='original finite request/result register cases, not arbitrary interrupts'))
            row['evidence'].append(dict(service='INT15 C201/INT33',original_capture='DOSBox-X race',
                claim_limit='finite defined register projections, not arbitrary interrupts'))
        if row['kind']=='host':
            service='int33' if 'INT 33' in row.get('interface','') else 'int15' if 'INT 15' in row.get('interface','') else None
            if service in observed:
                counts=observed[service];row['capture_contract']=dict(service=service,scope='defined request/result register projection only')
                row['evidence'].append(dict(service=service,**counts,claim_limit='service calls only; not the complete caller routine'))
                row.update(exercised=True,status='exercised and differs' if counts['differs'] else 'exercised and agrees')
    inputs=['docs/porting/asm-migration.json','docs/porting/platform-boundary.json','port/semantic-contracts.json',
            'tools/porting/semantic_audit.py','tools/porting/platform-services.json']
    report=dict(schema='stunts-port-layer-v1',scope='all source replacements/adaptations and native helpers, not unchanged reconstructed C',
        claim_limit='Inventory plus named finite original tests; observed call coverage is merged separately. Never a whole-domain proof.',
        inputs={p:sha(ROOT/p) for p in inputs},rows=rows,
        summary=dict(boundaries=len(rows),kinds=dict(Counter(r['kind'] for r in rows)),
            statuses=dict(Counter(r['status'] for r in rows)),
            changed_historical_c_functions=sum(r['kind']=='adapted_c' for r in rows),
            reconstructed_asm_modules=sorted({r['original_module'] for r in rows if r['kind']=='asm' and
                (r.get('original_module') or '').startswith(('game/','lib/'))}),
            observed_routines=observed))
    old=ROOT/'build/workers/portclock/inventory.json'
    if old.exists():
        report['prior_clock_audit']=dict(path=old.relative_to(ROOT).as_posix(),sha256=sha(old),
            scope='step-0 time/thread sites; historical line numbers, not current call coverage')
    return report

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--audit',type=Path,default=ROOT/'build/workers/portclock/step7/audit/report.json')
    p.add_argument('--out',type=Path,default=ROOT/'tools/porting/port-layer.json')
    p.add_argument('--comparison',type=Path,action='append',default=[])
    a=p.parse_args();report=build(json.loads(a.audit.read_text()),a.comparison)
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(json.dumps(report['summary'],indent=2))

if __name__=='__main__':main()
