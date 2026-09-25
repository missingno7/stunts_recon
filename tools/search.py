"""Compile and compare frozen standalone C hypotheses without canonical mutation."""
import argparse
import datetime as dt
import json
import os
import shutil
import tempfile
import uuid
from pathlib import Path

from common import ROOT, identity, json_bytes, read_json, require, sha, write_json

SCHEMA = 1


def _relative(path):
    try:
        return Path(path).resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return str(Path(path).resolve())


def _put_immutable(path, data):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists():
        require(path.read_bytes() == data, f'Content-address collision at {path}')
    else:
        fd, temporary = tempfile.mkstemp(prefix='.' + path.name + '.', dir=path.parent)
        try:
            with os.fdopen(fd, 'wb') as stream:
                stream.write(data)
            try:
                os.replace(temporary, path)
            except OSError:
                if not path.exists() or path.read_bytes() != data:
                    raise
        finally:
            Path(temporary).unlink(missing_ok=True)
        require(path.read_bytes() == data, f'Content-address collision at {path}')
    return path


def _candidate_snapshot(source_path):
    path = Path(source_path).expanduser().resolve()
    require(path.is_file(), f'Candidate source does not exist: {source_path}')
    raw = path.read_bytes()
    digest = sha(raw)
    suffix = '.ASM' if path.suffix.lower() == '.asm' else '.c'
    frozen = _put_immutable(ROOT / 'build/search/candidates' / f'{digest}{suffix}', raw)
    return raw, {'sha256': digest, 'size': len(raw), 'name': path.name,
                 'input_path': str(path), 'frozen_path': _relative(frozen)}


def _load_function(name):
    if not name:
        return None
    from function_evidence import current_inventory
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    inventory = current_inventory(MZ.parse(result[1]).load_image(result[1]))
    matches = [row for row in inventory.get('functions', [])
               if name in (row.get('name'), row.get('stable_id'))]
    require(len(matches) == 1, 'Unknown or ambiguous evidence function; use exact name or stable ID from evidence/functions.json')
    return matches[0]


def _load_recipe(recipe_path):
    if not recipe_path:
        return None, None
    path = Path(recipe_path)
    if not path.is_absolute():
        path = ROOT / path
    path = path.resolve()
    require(path.suffix.lower() == '.json' and path.is_file(),
            f'Recipe must be an existing JSON file: {recipe_path}')
    recipe = read_json(path)
    return recipe, path


def _tool_identity(paths):
    result = {}
    for path in paths:
        path = Path(path)
        if not path.is_absolute():
            path = ROOT / path
        resolved = path.resolve()
        require(resolved.is_file(), f'Required research tool/input is missing: {resolved}')
        result[_relative(resolved)] = identity(resolved.read_bytes())
    return result


def _environment(profile, recipe, recipe_path, function, closure=None):
    import compiler

    config, runner = compiler.verify_toolchain(profile)
    toolchain_path = ROOT / 'layout/toolchain.json'
    files = list(config.get('files', [])) + [runner]
    tools = ['tools/search.py', 'tools/compiler.py', 'tools/preprocessor.py', 'tools/common.py',
             'tools/object_probe.py', 'tools/omf.py']
    if profile == 'masm510-game':
        tools.append('tools/assembler.py')
    if function:
        tools += ['tools/oracle.py', 'tools/mz.py', 'tools/dsi.py', 'tools/exepack.py', 'tools/diagnostics.py']
    binding_paths = []
    if recipe:
        tools += ['tools/probe_module.py', 'tools/binder.py', 'tools/code_symbols.py',
                  'tools/data_symbols.py', 'tools/library.py']
        binding_paths = ['layout/manifest.json', 'layout/code-symbols.json', 'layout/data-symbols.json']
        binding_paths += [name for name in ('layout/library.json', 'layout/libraries.json') if (ROOT / name).is_file()]

    oracle = {'lock': identity((ROOT / 'layout/oracle.lock.json').read_bytes())}
    # The oracle inputs are part of the experiment identity even when the
    # standalone compile does not ask for a target comparison.
    oracle_names = ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD')
    oracle['assets'] = _tool_identity([ROOT / 'assets' / name for name in oracle_names])

    function_data = None
    if function:
        evidence_bytes = (ROOT / 'evidence/functions.json').read_bytes()
        function_data = {'inventory': identity(evidence_bytes), 'row': function,
                         'row_identity': identity(json_bytes(function))}

    recipe_data = None
    if recipe:
        recipe_bytes = recipe_path.read_bytes()
        owners = read_json(ROOT / 'layout/manifest.json').get('owners', [])
        matching_owner = next((row for row in owners if row.get('recipe') == _relative(recipe_path)), None)
        recipe_data = {'path': _relative(recipe_path), 'identity': identity(recipe_bytes),
                       'value': recipe,
                       'manifest_owner': matching_owner,
                       'active_in_manifest': matching_owner is not None}
    binding_inputs = _tool_identity(binding_paths) if binding_paths else {}

    snapshot = {
        'schema': 1,
        'profile': profile,
        'preprocessor_closure': [] if closure is None else closure,
        'toolchain_lock': identity(toolchain_path.read_bytes()),
        'toolchain_profile': config,
        'runner': runner,
        'tool_files': _tool_identity([item['path'] for item in files]),
        'oracle': oracle,
        'function': function_data,
        'recipe': recipe_data,
        'binding_inputs': binding_inputs,
        'tools': _tool_identity(tools),
        'scope': 'Frozen compiler, oracle, selected evidence/recipe, and research tools. Source, promotion history, and unrelated reports are not inputs.'
    }
    raw = json_bytes(snapshot)
    digest = sha(raw)
    path = _put_immutable(ROOT / 'build/search/environments' / f'{digest}.json', raw)
    return snapshot, {'sha256': digest, 'path': _relative(path)}


def _freeze_compiler_work(receipt, run_dir):
    work_value = receipt.get('work_directory') if receipt else None
    if not work_value:
        return None, None
    source = Path(work_value)
    if not source.is_absolute():
        source = ROOT / source
    if not source.is_dir():
        return None, None
    destination = run_dir / 'compiler-work'
    if destination.exists():
        shutil.rmtree(destination)
    shutil.copytree(source, destination)
    log_path = source / ('assembler.log' if (source/'assembler.log').is_file() else 'compiler.log')
    log = log_path.read_bytes() if log_path.is_file() else b''
    object_path = source / 'UNIT.OBJ'
    object_raw = object_path.read_bytes() if object_path.is_file() else None
    return {'path': _relative(destination), 'log_path': _relative(destination / log_path.name) if log else None,
            'log_sha256': sha(log) if log else None,
            'object_path': _relative(destination / 'UNIT.OBJ') if object_raw is not None else None,
            'object': identity(object_raw) if object_raw is not None else None}, log


def _object_document(obj):
    document = {
        'name': obj.name,
        'segments': {name: {'size': len(data), 'sha256': sha(bytes(data)), 'bytes_hex': bytes(data).hex()}
                     for name, data in sorted(obj.segments.items())},
        'segment_lengths': obj.segment_lengths,
        'segment_declarations': obj.segment_defs,
        'groups': obj.groups,
        'publics': obj.publics,
        'externals': obj.externals,
        'legacy_fixups': obj.fixups,
        'linker_fixups': obj.linker_fixups,
        'comments': obj.comments,
        'research_local_symbol_records': getattr(obj, 'local_symbol_records', [])
    }
    return _jsonable(document)


def _jsonable(value):
    if isinstance(value, bytes):
        return {'bytes_hex': value.hex(), 'size': len(value), 'sha256': sha(value)}
    if isinstance(value, Path):
        return str(value)
    if isinstance(value, dict):
        return {str(key): _jsonable(item) for key, item in value.items()}
    if isinstance(value, (tuple, list)):
        return [_jsonable(item) for item in value]
    if value is None or isinstance(value, (str, int, float, bool)):
        return value
    return repr(value)


def _recipe_binding_context(recipe, oracle_result):
    """Resolve just this recipe's binding facts, excluding unrelated owners."""
    from mz import MZ
    image = MZ.parse(oracle_result[1]).load_image(oracle_result[1])
    relocations = oracle_result[2]['unpacked_mz']['relocations']
    names = sorted({item.get('target') for item in recipe.get('expected_fixups', []) if item.get('target')})
    code_layout_path = ROOT / 'layout/code-symbols.json'
    data_layout_path = ROOT / 'layout/data-symbols.json'
    code_layout = read_json(code_layout_path) if code_layout_path.is_file() else {'symbols': {}}
    data_layout = read_json(data_layout_path) if data_layout_path.is_file() else {'symbols': {}}
    manifest = read_json(ROOT / 'layout/manifest.json')
    selected_codes = {name: code_layout.get('symbols', {}).get(name) for name in names
                      if name in code_layout.get('symbols', {})}
    selected_data = {name: data_layout.get('symbols', {}).get(name) for name in names
                     if name in data_layout.get('symbols', {})}
    owner_ids = set()
    target_ids = set()
    caller_ids = set()
    for symbol in selected_codes.values():
        if symbol.get('owner'):
            owner_ids.add(symbol['owner'])
        mapped = symbol.get('mapped_target', {})
        if mapped.get('stable_id'):
            target_ids.add(mapped['stable_id'])
        caller_ids.update(anchor.get('caller_task') for anchor in symbol.get('anchors', [])
                          if anchor.get('caller_task'))
    if recipe.get('stable_id'):
        target_ids.add(recipe['stable_id'])
    relevant_owners = [owner for owner in manifest.get('owners', [])
                       if owner.get('id') in owner_ids | target_ids | caller_ids]
    try:
        from code_symbols import resolve_recipe_symbols
        resolved = resolve_recipe_symbols(recipe, image, relocations)
        resolution = {'status': 'RESOLVED', 'values': resolved}
    except Exception as error:
        resolution = {'status': 'BLOCKED_OR_UNSUPPORTED', 'error': str(error)}
    return {'recipe': identity(json_bytes(recipe)), 'symbols': names,
            'code_symbol_facts': selected_codes, 'data_symbol_facts': selected_data,
            'relevant_manifest_owners': relevant_owners, 'resolution': _jsonable(resolution)}


def _output_signature(object_document):
    return sha(json_bytes(object_document))


def _prior_equivalents(output_identity):
    if not output_identity:
        return []
    root = ROOT / 'build/search'
    equivalents = []
    if not root.is_dir():
        return equivalents
    for path in root.glob('*/report.json'):
        try:
            report = read_json(path)
        except (OSError, ValueError):
            continue
        if (report.get('observed_output') or {}).get('identity') == output_identity:
            equivalents.append(report.get('run_id'))
    return sorted(value for value in equivalents if value)


def _target_bytes(function, oracle_result):
    from mz import MZ
    image = MZ.parse(oracle_result[1]).load_image(oracle_result[1])
    start, end = function.get('start'), function.get('end')
    require(isinstance(start, int) and isinstance(end, int) and 0 <= start <= end <= len(image),
            'Function evidence range is outside the locked pristine oracle')
    raw = image[start:end]
    require(not function.get('sha256') or sha(raw) == function['sha256'],
            'Evidence function digest differs from the locked pristine oracle')
    return raw


def run(source_path, profile=None, recipe_path=None, function=None):
    """Compile one frozen C or ASM source and archive standalone observations."""
    from compiler import CompileFailure, compile_source

    source, candidate = _candidate_snapshot(source_path)
    recipe, recipe_file = _load_recipe(recipe_path)
    if recipe and not function:
        function = recipe.get('name')
        if not function and recipe.get('stable_id'):
            function = recipe['stable_id']
    function_row = _load_function(function) if function else None
    if recipe and function_row is None:
        function_row = _load_function(recipe.get('stable_id') or recipe.get('name'))
    if recipe and function_row:
        require(recipe.get('start') == function_row.get('start') and recipe.get('end') == function_row.get('end'),
                'Recipe range differs from the selected evidence function')

    asm = Path(source_path).suffix.lower() == '.asm' or (recipe or {}).get('kind') == 'asm'
    selected_profile = profile or (recipe or {}).get('profile') or ('masm510-game' if asm else 'msc510-medium')
    if asm:
        from assembler import asm_source
        asm_source(source)
        closure=[]
    else:
        from preprocessor import prepare
        _, closure = prepare(source, selected_profile)
    oracle_result = None
    binding_before = None
    if recipe:
        try:
            from oracle import verify
            oracle_result = verify(write=False)
            binding_before = _recipe_binding_context(recipe, oracle_result)
        except Exception as error:
            binding_before = {'status': 'SNAPSHOT_ERROR', 'error': str(error)}
    environment, environment_ref = _environment(selected_profile, recipe, recipe_file, function_row, closure)
    run_id = str(uuid.uuid4())
    run_dir = ROOT / 'build/search' / run_id
    run_dir.mkdir(parents=True, exist_ok=False)
    report = {
        'schema': SCHEMA,
        'run_id': run_id,
        'created_utc': dt.datetime.now(dt.timezone.utc).isoformat(),
        'candidate': candidate,
        'environment': environment_ref,
        'function': ({'name': function_row.get('name'), 'stable_id': function_row.get('stable_id'),
                      'start': function_row.get('start'), 'end': function_row.get('end'),
                      'sha256': function_row.get('sha256'), 'evidence_status': function_row.get('status')}
                     if function_row else None),
        'context_hypothesis': ('Candidate is a standalone scratch translation unit. No original translation-unit membership, source authorship, or production ownership is inferred.'
                               if not function_row else
                               'Candidate is a standalone source context hypothesis for the selected evidence function. This does not assert the original module boundary.'),
        'compiler': {'profile': selected_profile, 'status': 'NOT_STARTED'},
        'binding': {'status': 'NOT_REQUESTED' if not recipe else 'PENDING',
                    'authority': 'Production binding is separate from unbound code-generation diagnostics.'},
        'comparison': {'status': 'NOT_REQUESTED', 'authority': 'DIAGNOSTIC_ONLY; an unbound object comparison never establishes strict byte or binding acceptance.'},
        'recipe_check': {'status': 'NOT_REQUESTED'} if not recipe else {'status': 'PENDING'},
        'recipe_binding_context_before': binding_before,
        'observed_output': None,
        'equivalent_runs': []
    }

    obj = None
    receipt = None
    compiler_log = b''
    compiler_error = None
    try:
        if asm:
            from assembler import assemble_source
            obj, receipt = assemble_source(source, selected_profile)
            require(receipt.get('include_closure') == [], 'ASM include closure changed')
        else:
            obj, receipt = compile_source(source, selected_profile)
            require(receipt.get('preprocessor_closure', closure) == closure,
                    'Preprocessor closure changed after search snapshot')
        report['compiler'] = {'profile': selected_profile, 'status': 'ASSEMBLED' if asm else 'COMPILED', 'receipt': receipt}
    except CompileFailure as error:
        compiler_error = error
        receipt = error.receipt
        report['compiler'] = {'profile': selected_profile, 'status': 'FAILED', 'category': error.category,
                              'message': str(error), 'receipt': receipt}
    work_artifact, compiler_log = _freeze_compiler_work(receipt or {}, run_dir)
    report['compiler']['artifacts'] = work_artifact
    if compiler_log:
        report['compiler']['log_excerpt'] = compiler_log.decode('utf-8', errors='replace')[-12000:]
        report['compiler']['logged_error_lines'] = [line for line in report['compiler']['log_excerpt'].splitlines()
                                                    if any(word in line.lower() for word in ('error', 'fatal'))]
    if obj is None and compiler_error and compiler_error.category == 'UNSUPPORTED_OBJECT' and work_artifact and work_artifact.get('object_path'):
        try:
            from object_probe import read_object
            raw_object = (ROOT / work_artifact['object_path']).read_bytes()
            obj = read_object(raw_object, research_local_symbols=True)
            report['compiler']['research_fallback'] = {
                'status': 'LOCAL_SYMBOL_RESEARCH_PARSE',
                'object_identity': identity(raw_object),
                'authority': 'Explicit research parser accepted local symbol records for inspection. Production mode rejected this module; unsupported records remain errors, and binding/strict claims are unavailable.'
            }
        except Exception as error:
            report['compiler']['research_fallback'] = {
                'status': 'UNAVAILABLE', 'error': str(error),
                'authority': 'Production parser rejected this module and generic OMF inspection also failed. Archived raw OBJ and compiler log remain available.'
            }

    if obj is not None:
        document = _object_document(obj)
        object_path = run_dir / 'observed-object.json'
        write_json(object_path, document)
        signature = _output_signature(document)
        report['observed_output'] = {
            'identity': signature,
            'path': _relative(object_path),
            'segment_names': sorted(obj.segments),
            'segment_lengths': obj.segment_lengths,
            'declaration_count': len(obj.segment_defs),
            'public_count': len(obj.publics),
            'external_count': len(obj.externals),
            'fixup_count': len(obj.linker_fixups),
            'authority': ('Generic OMF diagnostic observation; production parser rejected this object, so binding/strict claims remain unavailable.'
                          if compiler_error else
                          'Exact compiler object observation, including the full JSON and archived OMF. This is not a bound image.')
        }
        report['equivalent_runs'] = _prior_equivalents(signature)

        target_raw = None
        if function_row:
            try:
                from oracle import verify
                oracle_result = oracle_result or verify(write=False)
                target_raw = _target_bytes(function_row, oracle_result)
            except Exception as error:
                report['comparison'] = {'status': 'TARGET_UNAVAILABLE', 'error': str(error),
                                        'authority': 'No target comparison made; compiler object remains a standalone observation.'}
        if target_raw is not None:
            segment = (recipe or {}).get('object_segment')
            if not segment:
                segment = 'UNIT_TEXT' if 'UNIT_TEXT' in obj.segments else next(iter(obj.segments), None)
            candidate_raw = bytes(obj.segments.get(segment, b'')) if segment else b''
            try:
                from diagnostics import diagnose
                diag_receipt = dict(receipt or {})
                diag_receipt['work_directory'] = str(run_dir)
                details = diagnose(target_raw, candidate_raw, diag_receipt, obj.linker_fixups,
                                   bound=False, segment=segment or 'UNIT_TEXT')
                report['comparison'] = {'status': 'DIAGNOSTIC_AVAILABLE', 'segment': segment,
                                        'summary': details.get('match_summary'),
                                        'full_diagnostic': details.get('full_diagnostic'),
                                        'full_diagnostic_identity': details.get('full_diagnostic_identity'),
                                        'diagnostic_error': details.get('diagnostic_error'),
                                        'authority': 'Unbound object bytes aligned against the locked target for exploration only; strict extent, relocation, binding, and bound-byte checks are separate.'}
            except Exception as error:
                report['comparison'] = {'status': 'DIAGNOSTIC_ERROR', 'error': str(error),
                                        'authority': 'No similarity or acceptance claim is made.'}

        if recipe:
            try:
                from probe_module import probe, ProbeFailure
                _, strict_receipt = probe(recipe, oracle_result=oracle_result, source_override=source)
                fresh_oracle = verify(write=False)
                binding_after = _recipe_binding_context(recipe, fresh_oracle)
                report['recipe_binding_context_after'] = binding_after
                recipe_after = identity(recipe_file.read_bytes()) if recipe_file and recipe_file.is_file() else None
                stable = binding_before == binding_after and recipe_after == (environment.get('recipe') or {}).get('identity')
                report['recipe_check'] = {'status': 'STRICT_EXACT' if stable else 'INPUTS_CHANGED_DURING_PROBE',
                                          'receipt': strict_receipt, 'inputs_stable': stable,
                                          'authority': ('Fresh recipe probe on frozen scratch source passed complete extent, binding, relocation, and byte checks.'
                                                        if stable else 'Strict probe output is retained as an observation, but relevant binding inputs changed during the run and must be reviewed.')}
                report['binding'] = {'status': 'BOUND' if stable else 'INPUTS_CHANGED_DURING_PROBE',
                                     'fixup_count': len(obj.linker_fixups),
                                     'authority': 'The bound recipe probe validated the source object against its exact recipe.'}
            except Exception as error:
                details = getattr(error, 'details', {})
                category = details.get('category', 'RECIPE_PROBE_ERROR') if isinstance(details, dict) else 'RECIPE_PROBE_ERROR'
                report['recipe_check'] = {'status': 'FAILED', 'category': category, 'message': str(error),
                                          'details': details,
                                          'authority': 'Scratch recipe probe failed; this does not alter canonical source or acceptance state.'}
                report['binding'] = {'status': 'BLOCKED' if category in ('BINDING_REVIEW_REQUIRED', 'COMPILER_ERROR', 'UNSUPPORTED_OBJECT') else 'FAILED',
                                     'category': category,
                                     'details': details,
                                     'authority': 'Binding is reported independently from the unbound code-generation diagnostic.'}
    else:
        if recipe:
            report['recipe_check'] = {'status': 'NOT_RUN', 'reason': 'Standalone compiler did not produce a parsed object.'}
            report['binding'] = {'status': 'NOT_RUN', 'reason': 'No parsed object was available for binding diagnostics.'}

    report_path = run_dir / 'report.json'
    write_json(report_path, report)
    return report


def history(function=None, limit=20):
    """Return compact prior observations; reports never affect input identity."""
    root = ROOT / 'build/search'
    rows = []
    if root.is_dir():
        for path in root.glob('*/report.json'):
            try:
                report = read_json(path)
            except (OSError, ValueError):
                continue
            record_function = report.get('function') or {}
            if function and record_function.get('name') != function and record_function.get('stable_id') != function:
                continue
            compiler = report.get('compiler') or {}
            rows.append({'run_id': report.get('run_id'), 'created_utc': report.get('created_utc'),
                         'candidate': report.get('candidate'), 'profile': compiler.get('profile'),
                         'compile_status': compiler.get('status'),
                         'output_identity': (report.get('observed_output') or {}).get('identity'),
                         'equivalent_runs': report.get('equivalent_runs', []), 'report': _relative(path)})
    rows.sort(key=lambda row: row.get('created_utc') or '', reverse=True)
    count = len(rows)
    return {'runs': rows[:limit], 'total_runs': count, 'omitted_runs': max(0, count - limit),
            'authority': 'Historical observations are queryable context and do not invalidate or authorize any current experiment.'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('candidates', nargs='*', help='One or more standalone C or ASM source files')
    parser.add_argument('--profile', help='Pinned toolchain profile (defaults to recipe profile or msc510-medium)')
    parser.add_argument('--recipe', help='Optional JSON recipe for a fresh scratch binding probe')
    parser.add_argument('--function', help='Optional exact evidence function name or stable ID for diagnostics')
    parser.add_argument('--history', action='store_true', help='List compact prior runs instead of compiling')
    parser.add_argument('--limit', type=int, default=20, help='Maximum history rows to print')
    args = parser.parse_args()
    if args.history:
        if args.candidates or args.recipe or args.profile:
            parser.error('--history cannot be combined with candidates, --recipe, or --profile')
        print(json.dumps(history(args.function, max(0, args.limit)), indent=2))
        return
    if not args.candidates:
        parser.error('provide one or more C candidate paths, or use --history')
    for source in args.candidates:
        report = run(source, args.profile, args.recipe, args.function)
        compiler = report.get('compiler') or {}
        observed = report.get('observed_output') or {}
        comparison = report.get('comparison') or {}
        summary = comparison.get('summary')
        if summary:
            try:
                from diagnostics import routine_summary
                summary = routine_summary(summary)
            except Exception as error:
                summary = {'summary_error': str(error), 'available_summary': summary}
        print(json.dumps({'run_id': report['run_id'], 'candidate': report['candidate'],
                          'compiler': {'status': compiler.get('status'), 'category': compiler.get('category'),
                                       'message': compiler.get('message'), 'logged_error_lines': compiler.get('logged_error_lines', [])},
                          'emitted_segments': observed.get('segment_lengths'),
                          'fixup_count': observed.get('fixup_count'),
                          'output_identity': observed.get('identity'),
                          'comparison': {'status': comparison.get('status'), 'summary': summary,
                                         'error': comparison.get('error') or comparison.get('diagnostic_error')},
                          'equivalent_runs': report.get('equivalent_runs', []),
                          'recipe_check': {'status': (report.get('recipe_check') or {}).get('status'),
                                           'category': (report.get('recipe_check') or {}).get('category')},
                          'report': f"build/search/{report['run_id']}/report.json"}, indent=2))


if __name__ == '__main__':
    main()
