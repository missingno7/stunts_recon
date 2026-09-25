"""Fresh complete-contribution C probe. No object trimming or byte patching."""
import argparse
from common import ROOT, read_json, require, project_path, identity
from oracle import verify
from mz import MZ
from compiler import compile_source, CompileFailure
from preprocessor import prepare, check_recipe
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from multi_contribution import checked_members, bind_multi
from secondary_contribution import bind_single_secondary


class ProbeFailure(ValueError):
    def __init__(self, message, details):
        super().__init__(message)
        self.details = details

def _diagnostic(*args, **kwargs):
    # Even a broken diagnostic implementation cannot bypass the strict probe.
    try:
        from diagnostics import diagnose
        return diagnose(*args, **kwargs)
    except Exception as error:
        return {'diagnostic_error':str(error),'authority':'DIAGNOSTIC_UNAVAILABLE'}

def probe(recipe, oracle_result=None, source_override=None):
    result = oracle_result or verify(write=False)
    image = MZ.parse(result[1]).load_image(result[1])
    start, end = recipe['start'], recipe['end']
    if 'members' in recipe:
        checked_members(recipe, image)
    require(0 <= start < end <= len(image), 'Invalid candidate extent')
    require(identity(image[start:end]) == recipe['target'], 'Candidate target lock mismatch')
    relocs = [r for r in result[2]['unpacked_mz']['relocations'] if start - 1 <= r['load_offset'] < end]
    require(relocs==recipe['expected_relocations'], 'Complete ordered candidate relocation obligations differ')
    source = project_path(recipe['source']).read_bytes() if source_override is None else source_override
    _, closure = prepare(source, recipe['profile'])
    check_recipe(recipe, closure)
    try:
        obj, receipt = compile_source(source, recipe['profile'])
    except CompileFailure as error:
        raise ProbeFailure(str(error), {'category':error.category, 'receipt':error.receipt}) from error
    segment = recipe['object_segment']
    details = {'receipt':receipt, 'expected_size':end-start, 'emitted_sizes':obj.segment_lengths,
               'publics':obj.publics, 'externals':obj.externals, 'fixups':obj.linker_fixups,
               'object_segments':{n:identity(b) for n,b in obj.segments.items()}}
    details['comparison'] = _diagnostic(image[start:end], obj.segments.get(segment,b''), receipt, obj.linker_fixups, segment=segment)
    if obj.segment_length(segment) != end-start or len(obj.segments.get(segment,b'')) != end-start:
        raise ProbeFailure('Complete emitted contribution length differs from target',
                           {**details, 'category':'EXTENT_MISMATCH'})
    try:
        if 'members' in recipe:
            payload, binding = bind_multi(obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        elif recipe.get('secondary_dgroup_segments'):
            payload, binding = bind_single_secondary(
                obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        else:
            symbols = resolve_recipe_symbols(recipe, image, result[2]['unpacked_mz']['relocations'])
            payload, binding = bind_contribution(obj, recipe, symbols)
        require(binding['generated_relocations']==relocs, 'Generated source relocation obligations differ')
    except ValueError as error:
        raise ProbeFailure(str(error), {**details, 'category':'BINDING_REVIEW_REQUIRED'}) from error
    receipt['binding'] = binding
    if payload != image[start:end]:
        details['object_comparison'] = details['comparison']
        details['comparison'] = _diagnostic(image[start:end], payload, receipt, obj.linker_fixups, bound=True, segment=segment)
        differences=[{'offset':i,'load_offset':start+i,'expected':want,'actual':got}
                     for i,(want,got) in enumerate(zip(image[start:end],payload)) if want != got]
        raise ProbeFailure('Candidate bytes differ from full pristine extent',
                           {**details,'category':'BYTE_MISMATCH','difference_count':len(differences),
                            'first_differences':differences[:32], 'bound_payload':identity(payload)})
    if source_override is None:
        require(project_path(recipe['source']).read_bytes() == source, 'Source changed during compilation')
    require(prepare(source, recipe['profile'])[1] == closure,
            'Preprocessor closure changed during compilation')
    return payload, receipt
