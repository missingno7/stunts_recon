"""Fixed Stunts diagnostic evaluation cases; no search, compilation, or acceptance."""
import argparse
import importlib.util
import statistics
import subprocess
import time
from pathlib import Path
from common import ROOT, read_json, write_json, identity, json_bytes, require
from oracle import verify
from mz import MZ
from object_probe import read_object

BASELINE='98be0015e3354f54d7b045a136d783965d1eaaa5'
OUT=ROOT/'recovery/evaluation/near-match-families'
CASES=[
    ('canary','is_facing_camera','diagnostics/is_facing_camera/20260923T115355338643Z'),
    ('rect_plain','rect_compare_point','experiments/rect_compare_point/prior_01'),
    ('rect_parameter','rect_compare_point','experiments/rect_compare_point/prior_02'),
    ('rect_accepted','rect_compare_point','experiments/rect_compare_point/prior_03'),
    ('parse_failure','parse_shape2d_helper','attempts/parse_shape2d_helper/0001'),
    ('parse_heldout','parse_shape2d_helper','attempts/parse_shape2d_helper/0003'),
    ('parse_accepted','parse_shape2d_helper3','attempts/parse_shape2d_helper3/0002'),
]


def load_case(name,task,path,image):
    folder=ROOT/'recovery'/path;r=read_json(folder/'report.json');d=r.get('diagnostics',r)
    receipt=d.get('receipt') or r.get('receipt') or r['result']['fast']
    recipe=r.get('recipe') or read_json(ROOT/'recipes'/f'{task}.json')
    target=image[recipe['start']:recipe['end']];require(identity(target)==recipe['target'],'Target identity')
    source=folder/'source.c'
    if not source.exists():source=ROOT/recipe['source']
    source_id=identity(source.read_bytes())
    require(source_id in (receipt['source'],receipt['staged_source']),'Preserved source identity')
    object_path=Path(receipt['work_directory'])/'UNIT.OBJ'
    require(object_path.exists(),'Archived object unavailable: '+str(object_path))
    require(identity(object_path.read_bytes())==receipt['object'],'Archived object identity')
    obj=read_object(object_path.read_bytes());candidate=obj.segments[recipe['object_segment']]
    bound=name=='parse_accepted';artifact=None
    if name.startswith('rect_'):
        artifact=folder/'diagnostic.json'
        require(identity(artifact.read_bytes())==r['comparison']['full_diagnostic_identity'],'Archived bound artifact identity')
        full=read_json(artifact);candidate=bytes.fromhex(''.join(i['bytes'] for i in full['candidate_instructions']))
        require(identity(candidate)==full['candidate'] and len(candidate)==full['decoded_bytes'][1],'Complete bound bytes')
        require(identity(target)==full['target'],'Bound target identity');bound=True
    metadata={'name':name,'task':task,'group':'accepted_control' if 'accepted' in name else 'historical_failure',
        'held_out':name=='parse_heldout','report':(folder/'report.json').relative_to(ROOT).as_posix(),
        'report_identity':identity((folder/'report.json').read_bytes()),'target':identity(target),
        'target_extent':[recipe['start'],recipe['end']],'source_path':source.relative_to(ROOT).as_posix(),
        'source':source_id,'source_representation':'original' if source_id==receipt['source'] else 'compiler-staged CRLF',
        'profile':receipt['profile'],'flags':receipt['command'][5:-1],
        'object_path':str(object_path),'object':receipt['object'],'candidate':identity(candidate),'bound':bound,
        'fixups':obj.linker_fixups,'recipe':recipe,
        'dependency_context':{'recipe_identity':identity(json_bytes(recipe)),
            'toolchain_lock':identity((ROOT/'layout/toolchain.json').read_bytes()),
            'historical_input_fingerprint':r.get('input_fingerprint'),
            'limitation':'Historical complete dependency snapshot not inferred; current lock recorded separately.'},
        'freshness':'Historical bytes, source and object identities rechecked against fresh oracle; no compilation',
        'strict_status':r.get('status','BOUND_BYTES_EXACT_RESEARCH' if r.get('bound_exact') else 'BOUND_BYTES_MISMATCH_RESEARCH')+(' / '+d['category'] if d.get('category') else ''),
        'artifact':artifact.relative_to(ROOT).as_posix() if artifact else None}
    return metadata,target,candidate,obj.linker_fixups,bound


def run(after=False):
    result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1])
    baseline=ROOT/'build/baseline_diagnostics.py'
    baseline.write_bytes(subprocess.check_output(['git','show',BASELINE+':tools/diagnostics.py'],cwd=ROOT))
    spec=importlib.util.spec_from_file_location('baseline_diagnostics',baseline)
    old=importlib.util.module_from_spec(spec);spec.loader.exec_module(old)
    import diagnostics
    rows=[]
    for name,task,path in CASES:
        meta,target,candidate,fixups,bound=load_case(name,task,path,image)
        # Holdout bytes are identity checked but not diagnosed until after implementation.
        if meta['held_out'] and not after:
            rows.append(meta);continue
        folder=OUT/name;folder.mkdir(parents=True,exist_ok=True)
        for label,engine in [('before',old)]+([('after',diagnostics)] if after else []):
            engine.compare_streams(target,candidate,fixups,bound)  # decoder warmup
            timings=[]
            for _ in range(9):
                start=time.perf_counter();report=engine.compare_streams(target,candidate,fixups,bound)
                summary=engine.compact(report);text=engine.format_summary(summary,name,meta['strict_status'])
                timings.append((time.perf_counter()-start)*1000)
            (folder/(label+'.txt')).write_bytes(text.encode('utf-8'))
            write_json(folder/(label+'-summary.json'),summary)
            if label=='after':write_json(folder/'after-full.json',report)
            meta[label]={'lines':len(text.splitlines()),'utf8_bytes':len(text.encode('utf-8')),
                'median_ms':round(statistics.median(timings),3),'min_ms':round(min(timings),3),'max_ms':round(max(timings),3),
                'counts':report['counts'],'engine':identity(Path(engine.__file__).read_bytes())}
        rows.append(meta)
        print(name,meta.get('before'),meta.get('after'))
    write_json(OUT/('results.json' if after else 'cases.json'),{'starting_ref':BASELINE,'cases':rows,'new_compilations':0,
        'measurement':'Nine warm same-process decode+alignment+compact+format timings; I/O excluded. No token counts.',
        'limitation':'Selected small corpus; three rect variants and two parse failures are correlated, not independent functions.'})


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--after',action='store_true')
    run(parser.parse_args().after)
