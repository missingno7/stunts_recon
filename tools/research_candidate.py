"""Supervisor compiler experiment. Diagnostic only; cannot promote or grant FAST."""
import argparse
from pathlib import Path
from common import ROOT, read_json, write_json, identity, sha, require, project_path
from compiler import compile_source
from diagnostics import diagnose
from oracle import verify
from mz import MZ
from workflow import fingerprint, task_inputs


def experiment(source, prediction, profile='msc510-medium', flags=None):
    require(prediction.strip(),'State a predicted observable before compiling')
    document=read_json(ROOT/'layout/function-evidence.json')
    f=next(f for f in document['functions'] if f['name']=='is_facing_camera')
    data=verify(write=False)[1];image=MZ.parse(data).load_image(data)
    require(sha(image[f['start']:f['end']])==f['sha256'],'Research target differs')
    directory=ROOT/'recovery/experiments/is_facing_camera'
    existing=list(directory.glob('*/report.json')) if directory.exists() else []
    destination=directory/f'{len(existing)+1:04d}';destination.mkdir(parents=True,exist_ok=False)
    raw=project_path(source).read_bytes();(destination/'source.c').write_bytes(raw)
    # Persist prediction before compilation, including complete conservative task context.
    context=task_inputs({'source':source,'profile':profile,'flags':flags,'target':f['sha256']})
    report={'scope':'SUPERVISOR_RESEARCH_ONLY; no acceptance receipt', 'prediction':prediction,
            'source':identity(raw),'context':context,'profile':profile,'flags':flags}
    write_json(destination/'prediction.json',report)
    obj,receipt=compile_source(raw,profile,flags)
    require(task_inputs({'source':source,'profile':profile,'flags':flags,'target':f['sha256']})==context,
            'Research dependencies changed during compile')
    candidate=obj.segment_bytes('UNIT_TEXT')
    comparison=diagnose(image[f['start']:f['end']],candidate,receipt,obj.linker_fixups)
    effective=fingerprint({'segments':{k:identity(v) for k,v in obj.segments.items()},'fixups':obj.linker_fixups,
                           'publics':obj.publics,'profile':profile,'flags':flags,
                           'context':{k:v for k,v in context.items() if k not in [source,'@recipe']}})
    same=[str(p.relative_to(ROOT)) for p in existing if read_json(p).get('effective_key')==effective]
    report.update(receipt=receipt,comparison=comparison,emitted_sizes=obj.segment_lengths,
                  publics=obj.publics,fixups=obj.linker_fixups,effective_key=effective,same_effective_as=same)
    (destination/'compiler.log').write_bytes((Path(receipt['work_directory'])/'compiler.log').read_bytes())
    (destination/'diagnostic.json').write_bytes(Path(comparison['full_diagnostic']).read_bytes())
    write_json(destination/'report.json',report)
    print(str(destination.relative_to(ROOT)), 'size',len(candidate),'vs',f['size'],
          'first instruction',comparison.get('first_differing_instruction'), 'same effective',same)
    return obj,report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('source');p.add_argument('--prediction',required=True)
    p.add_argument('--profile',default='msc510-medium');p.add_argument('--flags',nargs='+')
    a=p.parse_args();experiment(a.source,a.prediction,a.profile,a.flags)
