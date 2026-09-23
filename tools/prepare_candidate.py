"""Create a bounded recipe from an independently mapped function, never guessed offsets."""
import argparse
from common import ROOT, read_json, write_json, require, project_path, identity
from oracle import verify
from mz import MZ
from workflow import state


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('stable_id')
    p.add_argument('--source',required=True)
    p.add_argument('--profile',default='msc510-medium')
    a=p.parse_args()
    inventory=read_json(ROOT/'recovery/restunts-inventory.json')
    found=[f for f in inventory['functions'] if f.get('stable_id')==a.stable_id and f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    require(len(found)==1,'Function lacks unique fully checked boundaries; supervisor mapping required')
    f=found[0]
    status=state(f['name'],[f['start'],f['end']])
    require(not status['blocked'],'Task is blocked: '+str(status['reason']))
    owners=read_json(ROOT/'layout/manifest.json')['owners']
    containing=[o for o in owners if o['start']<=f['start']<f['end']<=o['end']]
    require(len(containing)==1 and containing[0]['kind']=='UNRESOLVED_RAW','Candidate is not wholly raw-owned')
    source=project_path(a.source)
    require(source.is_file() and source.is_relative_to(ROOT/'recovery/candidates'),'Source must exist under recovery/candidates')
    require(not f['relocation_sites'],'Relocation-bearing candidate requires binder work first')
    target=ROOT/'recipes'/(f['name']+'.json')
    require(not target.exists(),'Recipe already exists; explicit supervisor review required to change expectations')
    result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1])
    recipe={'id':f['name'],'stable_id':a.stable_id,'start':f['start'],'end':f['end'],
            'source':source.relative_to(ROOT).as_posix(),'profile':a.profile,'object_segment':'UNIT_TEXT',
            'public':'_'+f['name'],'target':identity(image[f['start']:f['end']]),
            'expected_fixups':[],'expected_relocations':[],'evidence':f['provenance']}
    write_json(target,recipe)
    print('Created',target,'; review, then run reconstruction_factory.py refresh')

if __name__=='__main__':main()
