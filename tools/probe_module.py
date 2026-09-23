"""Fresh complete-contribution C probe. No object trimming or byte patching."""
import argparse
from common import ROOT, read_json, require, project_path, identity
from oracle import verify
from mz import MZ
from compiler import compile_source, CompileFailure
from binder import bind_contribution
from data_symbols import resolve_symbols


class ProbeFailure(ValueError):
    def __init__(self, message, details):
        super().__init__(message)
        self.details = details

def probe(recipe, oracle_result=None):
    result = oracle_result or verify(write=False)
    image = MZ.parse(result[1]).load_image(result[1])
    start, end = recipe['start'], recipe['end']
    require(0 <= start < end <= len(image), 'Invalid candidate extent')
    require(identity(image[start:end]) == recipe['target'], 'Candidate target lock mismatch')
    relocs = [r for r in result[2]['unpacked_mz']['relocations'] if start - 1 <= r['load_offset'] < end]
    require(not relocs, 'Relocating candidate requires supervisor binder implementation')
    require(recipe['expected_relocations']==[], 'Recipe requests unsupported relocation obligations')
    source = project_path(recipe['source']).read_bytes()
    try:
        obj, receipt = compile_source(source, recipe['profile'])
    except CompileFailure as error:
        raise ProbeFailure(str(error), {'category':error.category, 'receipt':error.receipt}) from error
    segment = recipe['object_segment']
    details = {'receipt':receipt, 'expected_size':end-start, 'emitted_sizes':obj.segment_lengths,
               'publics':obj.publics, 'externals':obj.externals, 'fixups':obj.linker_fixups,
               'object_segments':{n:identity(b) for n,b in obj.segments.items()}}
    if obj.segment_length(segment) != end-start or len(obj.segments.get(segment,b'')) != end-start:
        raise ProbeFailure('Complete emitted contribution length differs from target',
                           {**details, 'category':'EXTENT_MISMATCH'})
    try:
        symbols = resolve_symbols({f['target'] for f in recipe['expected_fixups']}, image,
                                  result[2]['unpacked_mz']['relocations']) if recipe['expected_fixups'] else None
        payload, binding = bind_contribution(obj, recipe, symbols)
    except ValueError as error:
        raise ProbeFailure(str(error), {**details, 'category':'BINDING_REVIEW_REQUIRED'}) from error
    receipt['binding'] = binding
    if payload != image[start:end]:
        differences=[{'offset':i,'load_offset':start+i,'expected':want,'actual':got}
                     for i,(want,got) in enumerate(zip(image[start:end],payload)) if want != got]
        raise ProbeFailure('Candidate bytes differ from full pristine extent',
                           {**details,'category':'BYTE_MISMATCH','difference_count':len(differences),
                            'first_differences':differences[:32], 'bound_payload':identity(payload)})
    require(project_path(recipe['source']).read_bytes() == source, 'Source changed during compilation')
    return payload, receipt

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('task')
    args = parser.parse_args()
    recipe = read_json(ROOT / 'recipes' / (args.task + '.json'))
    payload, receipt = probe(recipe)
    print(f'PASS: {args.task}: {len(payload)} complete compiler contribution; {receipt["binding"]["mode"]}')

if __name__ == '__main__':
    main()
