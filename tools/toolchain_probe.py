"""Inspect pinned toolchains or repeat bounded compiler experiments."""
import argparse
from common import ROOT, read_json, write_json, project_path, identity
from compiler import verify_toolchain, compile_source
from oracle import verify
from mz import MZ


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('command',choices=['verify','experiment'])
    p.add_argument('--profile',default='msc510-medium')
    p.add_argument('--task')
    p.add_argument('--flags',help='Space-separated DOS compiler options, e.g. "/AM /Os /Gs"')
    a=p.parse_args()
    verify_toolchain(a.profile)
    if a.command=='verify':
        print('PASS: all compiler/library/header/runner hashes:',a.profile)
        return
    if not a.task:p.error('--task is required for experiment')
    recipe=read_json(ROOT/'recipes'/(a.task+'.json'))
    obj,receipt=compile_source(project_path(recipe['source']).read_bytes(),a.profile,a.flags.split() if a.flags else None)
    original=verify(write=False);image=MZ.parse(original[1]).load_image(original[1]);payload=obj.segment_bytes(recipe['object_segment'])
    report={'status':'RESEARCH_ONLY','task':a.task,'receipt':receipt,'segments':obj.segment_defs,'publics':obj.publics,
            'fixups':obj.linker_fixups,'target':recipe['target'],'emitted':identity(payload),
            'literal_equal':payload==image[recipe['start']:recipe['end']]}
    write_json(ROOT/'build/toolchain-experiment.json',report)
    print('RESEARCH_ONLY literal_equal=',report['literal_equal'],'fixups=',len(obj.linker_fixups))

if __name__=='__main__':main()
