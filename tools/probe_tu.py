"""Translation-unit inspection; never turns partial function matches into PASS."""
import argparse
from common import ROOT, read_json, project_path, write_json, require, identity
from oracle import verify
from mz import MZ
from compiler import compile_source

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('source')
    p.add_argument('--profile', required=True)
    p.add_argument('--target', help='Reviewed JSON target for full TU contribution comparison')
    args = p.parse_args()
    obj, receipt = compile_source(project_path(args.source).read_bytes(), args.profile)
    report = {'status': 'INSPECTED_NOT_ACCEPTED', 'receipt': receipt,
              'segments': obj.segment_defs, 'groups': obj.groups, 'publics': obj.publics,
              'fixups': obj.linker_fixups,
              'note': 'TU identity and multi-contribution layout must be proven before production binding.'}
    if args.target:
        target=read_json(project_path(args.target))
        result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1]);start,end=target['start'],target['end']
        require(identity(image[start:end])==target['target'],'TU target lock mismatch')
        require(not obj.linker_fixups,'TU fixup binder not implemented')
        require(not any(start-1<=r['load_offset']<end for r in result[2]['unpacked_mz']['relocations']),'TU relocation obligations unsupported')
        require(obj.publics==target['publics'],'TU publics differ')
        require(set(obj.externals)<=({'__acrtused'}|{p['name'] for p in obj.publics}),'Unexpected TU external declarations')
        segment=target['segment'];payload=obj.segment_bytes(segment)
        require(obj.segment_length(segment)==len(payload)==end-start,'TU complete length differs')
        require(all(name==segment or size==0 for name,size in obj.segment_lengths.items()),'Unowned TU data/BSS')
        require(payload==image[start:end],'TU full contribution bytes differ')
        report['status']='TU_EXTENT_MATCH_ONLY'
        report['target']=target
    write_json(ROOT / 'build/tu-report.json', report)
    print(report['status']+': build/tu-report.json (not production promotion)')

if __name__ == '__main__':
    main()
