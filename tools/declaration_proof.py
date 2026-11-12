"""Atomic declaration/header promotion through the shared complete-object proof.

Invoked by promote.py --declarations. Candidates are frozen into an isolated
source tree. Headers are explicit --header include/DOSNAME.h=FILE inputs; neither
verification nor publication writes the git index. Every affected recipe derives
its closure and declaration indexes from a fresh compiler receipt.
"""
import base64
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

from common import ROOT, atomic_bytes, json_bytes, require, identity
from object_equivalence import compare_objects, first_difference
from object_probe import DEBUG_SEGMENT_POLICY
from bounded_process import bounded_run


def declaration_compare(before, after, mapping):
    # Only the pinned compiler's production decoder can attach this reviewed
    # debug-segment metadata. Ordinary objects retain the full record rule.
    policy = DEBUG_SEGMENT_POLICY if (getattr(before[0], 'debug_segments', []) or
                                      getattr(after[0], 'debug_segments', [])) else None
    return compare_objects(before, after, mapping, debug_policy=policy, declarations=True)


def strict_object_compare(before, after, mapping):
    """Stronger option for literal-to-macro edits: every raw OMF byte."""
    diff = first_difference(before[1], after[1], 'raw_object')
    return {'semantic': diff, 'omf': diff} if diff else None


def candidate_changes(target, candidate, headers, files):
    from rename import active_recipes
    active = active_recipes(files)
    sources = {item['recipe']['source'] for item in active.values()
               if target in (item['recipe']['source'], *item['owners'])}
    require(len(sources) == 1, 'Select one active owner or canonical source path')
    source = sources.pop()
    require((source.startswith(('game/', 'lib/')) and source.endswith('.c')) or
            (source.startswith(('game/', 'lib/')) and source.endswith('.ASM')), 'Select a canonical C or MASM source')
    changes = {source: Path(candidate).resolve().read_bytes()}
    for value in headers:
        name, separator, path = value.partition('=')
        require(separator and re.fullmatch(r'include/[A-Za-z_][A-Za-z_0-9]*\.(?:h|inc)', name) and
                (name in files or len(Path(name).stem) <= 8),
                'Header must be include/DOSNAME.h or .inc=CANDIDATE with an 8.3 basename (or an existing header)')
        require(name not in changes, 'Duplicate candidate header: ' + name)
        changes[name] = Path(path).resolve().read_bytes()
    return source, {name: raw for name, raw in changes.items() if files.get(name) != raw}




def worker(request):
    from rename import ProofEngine, canonical_files
    from preprocessor import staged_headers
    files = canonical_files(ROOT)
    changes = {name: base64.b64decode(raw, validate=True) for name, raw in request['changes'].items()}
    from declaration_sources import refresh_source_bases
    from declaration_scope import requires_raw_objects
    raw_reasons = requires_raw_objects(files, changes)
    object_exact = request.get('object_exact') or bool(raw_reasons) or any(p.endswith('.ASM') for p in changes)
    engine = ProofEngine(ROOT, files, changes_factory=lambda files, rows: dict(changes),
                         post_compare=refresh_source_bases,
                         comparator=strict_object_compare if object_exact else declaration_compare)
    # Include inactive source recipes too: none may retain stale closure/index
    # metadata for a source or header changed by this transaction.
    affected_sources = {name for name in changes if name.startswith(('game/', 'lib/'))}
    for name, raw in files.items():
        if name.startswith('recipes/') and name.endswith('.json'):
            recipe = json.loads(raw)
            if recipe.get('source') in affected_sources or any(
                    row.get('path') in changes for row in recipe.get('preprocessor_closure', []) + recipe.get('include_closure', [])):
                engine.recipes.setdefault(name, {'recipe': recipe, 'owners': []})
    with staged_headers({name: raw for name, raw in changes.items() if name.startswith('include/')}):
        result = engine.trial([], whole=True)
    require(not result['failures'], 'Declaration proof refused: ' + json.dumps(result['failures']))
    result['source_basis_updates'] = getattr(engine, 'declaration_basis_updates', [])
    result['external_declaration_changes'], result['complete_objects'] = [], []
    for name in result['affected_recipes']:
        old_compiled = engine.baselines[name]
        new_compiled = next(value for key, value in engine.cache.items() if key[0] == name and not isinstance(value, dict))
        old, new = old_compiled[0], new_compiled[0]
        result['complete_objects'].append({'recipe': name, 'before': identity(old_compiled[1]),
                                          'after': identity(new_compiled[1]),
                                          'byte_identical': old_compiled[1] == new_compiled[1]})
        previous, current = set(old.externals), set(new.externals)
        result['external_declaration_changes'].append({'recipe': name, 'added': sorted(current - previous),
                                                       'removed': sorted(previous - current)})
    used = {row.get('path') for name, raw in engine.current.items() if name.startswith('recipes/')
            for row in json.loads(raw).get('preprocessor_closure', []) + json.loads(raw).get('include_closure', [])}
    require(all(name in used for name in changes if name.startswith('include/')),
            'Candidate includes an unused header')
    proof_log = ROOT / 'build/declaration-validation.log'
    code = bounded_run([sys.executable, 'tools/validate.py', '--proof-only'], cwd=ROOT,
                       env=dict(os.environ), log=proof_log, timeout=240)
    require(code == 0 and 'Proof: PASS HYBRID_EXACT' in proof_log.read_text(errors='replace'),
            'Independent declaration proof failed; see ' + str(proof_log))
    result.update(changes=sorted(engine.current), independent_proof=str(proof_log),
                  comparison_mode='raw-object' if object_exact else 'declarations',
                  raw_object_reasons=raw_reasons,
                  debug_rule='MSC6 reviewed CodeView contents/lengths and LINNUM may differ; image records remain exact')
    atomic_bytes(Path(request['output']), json_bytes(result))


def promote_declarations(target, candidate, headers=(), verify_only=False, *, object_exact=False):
    import rename
    from transaction import lock_free_snapshot
    from build_exact import inputs

    def prove(snapshot):
        files = rename.canonical_files(ROOT)
        source, changes = candidate_changes(target, candidate, headers, files)
        require(changes, 'No declaration/header changes to prove')
        work_parent = ROOT / 'build/workers/sharedhdr'
        work_parent.mkdir(parents=True, exist_ok=True)
        work = Path(tempfile.mkdtemp(prefix='proof-', dir=work_parent))
        stage = work / 'tree'
        tracked = rename.tracked_snapshot(ROOT)
        for name, raw in {**tracked, **files}.items():
            if Path(name).parts[0] in (*rename.STATE_DIRS, 'historical'):
                atomic_bytes(stage / name, raw)
        # validate.py deliberately checks local prerequisites as well as their
        # configured identities. Supply copies, never mutate the original inputs.
        for name in ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD'):
            atomic_bytes(stage / 'assets' / name, (ROOT / 'assets' / name).read_bytes())
        shutil.copytree(ROOT / 'build/python', stage / 'build/python',
                        ignore=shutil.ignore_patterns('__pycache__'))
        output = work / 'result.json'
        request = work / 'request.json'
        atomic_bytes(request, json_bytes({'changes': {name: base64.b64encode(raw).decode('ascii')
                                                      for name, raw in changes.items()}, 'output': str(output),
                                         'object_exact': object_exact}))
        env = dict(os.environ, STUNTS_BUILD_WORKERS='1', STUNTS_ASSET_DIR=str(ROOT / 'assets'),
                   GIT_DIR=str(ROOT / '.git'), GIT_WORK_TREE=str(stage),
                   STUNTS_TOOLCACHE=os.environ.get('STUNTS_TOOLCACHE', str(ROOT / 'build/toolcache')),
                   PYTHONPATH=os.pathsep.join([str(ROOT / 'build/python'), os.environ.get('PYTHONPATH', '')]))
        log = work / 'proof.log'
        print('Declaration proof log: ' + str(log), flush=True)
        code = bounded_run([sys.executable, str(stage / 'tools/declaration_proof.py'), str(request)],
                           cwd=stage, env=env, log=log, timeout=600)
        if code:
            print('\n'.join(log.read_text(errors='replace').splitlines()[-15:]), flush=True)
        require(code == 0 and output.is_file(), 'Scratch declaration proof failed: ' + str(log))
        result = json.loads(output.read_bytes())
        published = {name: (stage / name).read_bytes() for name in result['changes']}
        require(all(name in (source, 'layout/manifest.json') or name.startswith(('include/', 'recipes/')) for name in published),
                'Declaration proof attempted an unrelated publication')
        # Check explicit candidate bytes again as well as canonical snapshot.
        require(candidate_changes(target, candidate, headers, files) == (source, changes),
                'Declaration candidates changed during proof')
        require(inputs() == snapshot, 'Canonical inputs changed during declaration proof')
        result.update(source=source, scratch=str(work))
        return files, published, result

    files, changes, result = lock_free_snapshot(prove, inputs)
    if not verify_only:
        def verify():
            request = Path(result['scratch']) / 'canonical-request.json'
            atomic_bytes(request, json_bytes(result['whole_image']['historical_baseline']))
            log = Path(result['scratch']) / 'canonical-proof.log'
            code = bounded_run([sys.executable, str(ROOT / 'tools/declaration_proof.py'),
                                '--canonical-proof', str(request)], cwd=ROOT,
                               env=dict(os.environ, STUNTS_BUILD_WORKERS='1'), log=log, timeout=300)
            require(code == 0, 'Canonical declaration proof failed: ' + str(log))
        rename.publish(changes, files, verify)
    result['status'] = 'DECLARATIONS_VERIFIED_ONLY' if verify_only else 'DECLARATIONS_PUBLISHED'
    atomic_bytes(Path(result['scratch']) / 'report.json', json_bytes(result))
    return result


def canonical_verify(baseline):
    from build_exact import build
    from oracle import verify as verify_oracle
    from rename import exact_bss_gate, historical_proof
    verify_oracle(write=True)
    require(build(publish=False, allow_pending=True)['status'] == 'HYBRID_EXACT',
            'Canonical declaration image differs')
    exact_bss_gate()
    historical = historical_proof()
    require(historical['loaded_library_modules'] == baseline['loaded_library_modules'] and
            historical['explicit_objects'] == baseline['explicit_objects'],
            'Canonical historical module selection changed')


if __name__ == '__main__':
    if sys.argv[1] == '--canonical-proof':
        canonical_verify(json.loads(Path(sys.argv[2]).read_bytes()))
    else:
        worker(json.loads(Path(sys.argv[1]).read_bytes()))
