"""Fresh complete-contribution C probe. No object trimming or byte patching."""
import argparse
from common import ROOT, read_json, require, project_path, identity
from oracle import verify
from mz import MZ
from compiler import compile_source, CompileFailure
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols


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

def probe(recipe, oracle_result=None):
    result = oracle_result or verify(write=False)
    image = MZ.parse(result[1]).load_image(result[1])
    start, end = recipe['start'], recipe['end']
    require(0 <= start < end <= len(image), 'Invalid candidate extent')
    require(identity(image[start:end]) == recipe['target'], 'Candidate target lock mismatch')
    relocs = [r for r in result[2]['unpacked_mz']['relocations'] if start - 1 <= r['load_offset'] < end]
    require(relocs==recipe['expected_relocations'], 'Complete ordered candidate relocation obligations differ')
    source = project_path(recipe['source']).read_bytes()
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
    require(project_path(recipe['source']).read_bytes() == source, 'Source changed during compilation')
    return payload, receipt

def archive_diagnostics(details, destination):
    """Persist full evidence separately; archival errors never alter acceptance/budget."""
    for name in ('comparison','object_comparison'):
        comparison=details.get(name,{})
        if not comparison.get('full_diagnostic'):continue
        try:
            source=project_path(comparison['full_diagnostic']);raw=source.read_bytes()
            require(identity(raw)==comparison['full_diagnostic_identity'],'Diagnostic identity changed before archive')
            path=destination/(name+'-full.json');path.write_bytes(raw)
            comparison['original_full_diagnostic']=comparison['full_diagnostic']
            comparison['full_diagnostic']=path.relative_to(ROOT).as_posix()
        except Exception as error:comparison['archive_error']=str(error)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('task')
    parser.add_argument('--supervisor-diagnostic',action='store_true',help='Archive a diagnostic-only fresh probe; no attempt, promotion, or reopen')
    args = parser.parse_args()
    recipe = read_json(ROOT / 'recipes' / (args.task + '.json'))
    source=project_path(recipe['source']).read_bytes()
    try:
        payload, receipt = probe(recipe)
    except ProbeFailure as error:
        from diagnostics import format_summary
        print(format_summary(error.details.get('comparison',{}).get('match_summary'),args.task,'FAIL: '+error.details['category']))
        if args.supervisor_diagnostic:
            from datetime import datetime, timezone
            from common import write_json
            require(project_path(recipe['source']).read_bytes()==source,'Source changed during supervisor diagnostic')
            observed=datetime.now(timezone.utc)
            destination=ROOT/'recovery/diagnostics'/args.task/observed.strftime('%Y%m%dT%H%M%S%fZ')
            destination.mkdir(parents=True,exist_ok=False)
            archive_diagnostics(error.details,destination)
            write_json(destination/'report.json',{'task':args.task,'status':'DIAGNOSTIC_ONLY_FAIL',
                'source':identity(source),'recipe':recipe,'started_utc':observed.isoformat(),
                'budget_charge':False,'error':str(error),'diagnostics':error.details})
            from reconstruction_factory import refresh
            refresh()
        comparison=error.details.get('comparison',{})
        print('Full diagnostic:',comparison.get('full_diagnostic',comparison.get('diagnostic_error')))
        raise SystemExit(1)
    print(f'PASS: {args.task}: {len(payload)} complete compiler contribution; {receipt["binding"]["mode"]}')

if __name__ == '__main__':
    main()
