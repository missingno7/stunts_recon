"""Fresh complete-contribution C probe. No object trimming or byte patching."""
import argparse
from common import ROOT, read_json, require, project_path, identity
from oracle import verify
from mz import MZ
from compiler import compile_source, CompileFailure
from preprocessor import prepare, check_recipe
from binder import bind_contribution, require_relocations_from_fixups
from code_symbols import resolve_recipe_symbols
from multi_contribution import checked_members, bind_multi
from secondary_contribution import bind_single_secondary
from assembler import assemble_source, asm_source
from object_flags import recipe_flags, object_control_flags, same_object
from object_probe import recipe_sparse_zero


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
    import memo
    with memo.session():
        return _probe(recipe, oracle_result, source_override)


def _probe(recipe, oracle_result=None, source_override=None):
    require('relocation_order_basis' not in recipe,
            'Recipe cannot supply a relocation order basis')
    prefix = 'prefix_of_object' in recipe
    if prefix:
        # Record-closed prefix of a whole candidate object: its fixed form is
        # checked here and its proof is re-derived from this fresh compile.
        from prefix_proof import check_recipe_form
        check_recipe_form(recipe)
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
    kind = recipe.get('kind', 'c')
    require(kind in ('c', 'asm'), 'Unknown contribution kind')
    if kind == 'asm':
        from compiler import verify_toolchain
        require(recipe['profile'] == 'masm510-game', 'ASM reproduction profile differs')
        require(recipe.get('assembler_flags') == verify_toolchain(recipe['profile'])[0]['flags'],
                'ASM recipe flags differ from pinned profile')
        asm_source(source)
        require(recipe.get('include_closure') == [], 'ASM include closure differs')
    else:
        _, closure = prepare(source, recipe['profile'])
        check_recipe(recipe, closure)
    control = None
    # integ39: reviewed COMDEF declarations (tentative definitions / MASM COMM
    # NEAR) for the accepted communal unit; any other COMDEF is refused.
    from communal_unit import recipe_declarations, check_object_communals
    declared = recipe_declarations(recipe)
    communals = None if declared is None else [name for name, _ in declared]
    try:
        sparse = recipe_sparse_zero(recipe)
        obj, receipt = (assemble_source(source, recipe['profile'], communals=communals) if kind == 'asm'
                        else compile_source(source, recipe['profile'], recipe_flags(recipe),
                                            sparse_zero=sparse, communals=communals))
        # A canonical-flag C contribution inside an object with a registered
        # per-object flag set must reproduce the same object under that set.
        control = object_control_flags(recipe) if kind == 'c' else None
        if control is not None:
            control_obj, control_receipt = compile_source(source, recipe['profile'], control,
                                                          sparse_zero=sparse, communals=communals)
            receipt['object_flag_control'] = {'flags': control,
                                              'object': control_receipt['object']}
    except CompileFailure as error:
        raise ProbeFailure(str(error), {'category':error.category, 'receipt':error.receipt}) from error
    if control is not None:
        require(same_object(control_obj, obj),
                'C contribution differs under its object registered flag set')
    check_object_communals(obj, recipe)
    segment = recipe['object_segment']
    details = {'receipt':receipt, 'expected_size':end-start, 'emitted_sizes':obj.segment_lengths,
               'publics':obj.publics, 'externals':obj.externals, 'fixups':obj.linker_fixups,
               'object_segments':{n:identity(b) for n,b in obj.segments.items()}}
    if kind == 'asm':
        require(recipe.get('object_declarations') ==
                {'segments':obj.segment_defs, 'groups':obj.groups,
                 'publics':obj.publics, 'externals':obj.externals},
                'ASM complete object declarations differ')
        require(obj.linker_fixups == recipe['expected_fixups'],
                'ASM ordered FIXUPP obligations differ')
    # The diagnostic comparison is computed only for a failure report; it
    # never decides acceptance (integ28 throughput).
    def compared():
        details['comparison'] = _diagnostic(image[start:end], obj.segments.get(segment,b''), receipt,
                                            obj.linker_fixups, segment=segment)
        return details
    if not prefix and (obj.segment_length(segment) != end-start or
                       len(obj.segments.get(segment,b'')) != end-start):
        raise ProbeFailure('Complete emitted contribution length differs from target',
                           {**compared(), 'category':'EXTENT_MISMATCH'})
    try:
        require_relocations_from_fixups(obj, recipe, result[2]['unpacked_mz']['relocations'])
        if kind == 'asm' and not recipe.get('data_only'):
            # A real LINK must place the same object identically (integ29).
            from link_frames import check_asm_object
            check_asm_object(obj, recipe)
        if prefix:
            from prefix_proof import bind_prefix
            payload, binding = bind_prefix(obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        elif recipe.get('data_only'):
            from data_only import bind_data_only
            payload, binding = bind_data_only(obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        elif 'members' in recipe:
            payload, binding = bind_multi(obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        elif recipe.get('secondary_dgroup_segments'):
            payload, binding = bind_single_secondary(
                obj, recipe, image, result[2]['unpacked_mz']['relocations'])
        else:
            symbols = resolve_recipe_symbols(recipe, image, result[2]['unpacked_mz']['relocations'])
            payload, binding = bind_contribution(obj, recipe, symbols)
        require(binding['generated_relocations']==relocs, 'Generated source relocation obligations differ')
    except ValueError as error:
        raise ProbeFailure(str(error), {**compared(), 'category':'BINDING_REVIEW_REQUIRED'}) from error
    receipt['binding'] = binding
    if getattr(obj, 'communals', None):
        receipt['communals'] = [{'name': c['name'], 'size': c['length']} for c in obj.communals]
    if payload != image[start:end]:
        details['object_comparison'] = compared()['comparison']
        details['comparison'] = _diagnostic(image[start:end], payload, receipt, obj.linker_fixups, bound=True, segment=segment)
        differences=[{'offset':i,'load_offset':start+i,'expected':want,'actual':got}
                     for i,(want,got) in enumerate(zip(image[start:end],payload)) if want != got]
        raise ProbeFailure('Candidate bytes differ from full pristine extent',
                           {**details,'category':'BYTE_MISMATCH','difference_count':len(differences),
                            'first_differences':differences[:32], 'bound_payload':identity(payload)})
    if source_override is None:
        require(project_path(recipe['source']).read_bytes() == source, 'Source changed during compilation')
    if kind == 'asm':
        require(asm_source(source) and recipe['include_closure'] == [],
                'ASM source closure changed during assembly')
    else:
        require(prepare(source, recipe['profile'])[1] == closure,
                'Preprocessor closure changed during compilation')
    return payload, receipt
