"""Fresh hybrid construction with explicit unresolved ownership."""
import argparse
from common import ROOT, read_json, write_json, require, sha, identity
from oracle import verify
from mz import MZ
from probe_module import probe
from library import bind_library

def inputs():
    """Acceptance transaction guard; worker candidates and reports are excluded."""
    result = {}
    for folder in ['tools', 'tests', 'src', 'asm', 'include', 'recipes', 'layout', 'evidence']:
        for path in sorted((ROOT / folder).rglob('*')):
            if path.is_file() and '__pycache__' not in path.parts:
                result[path.relative_to(ROOT).as_posix()] = sha(path.read_bytes())
    return result


def production_inputs(manifest=None, recipe_overrides=None, snapshot=None, source_overrides=None):
    result = dict(inputs() if snapshot is None else snapshot)
    manifest = manifest or read_json(ROOT/'layout/manifest.json')
    from common import json_bytes
    result['@active-manifest'] = sha(json_bytes(manifest))
    for owner in manifest['owners']:
        if owner['kind'] not in ('MATCHING_C', 'MATCHING_ASM', 'MATCHING_C_DATA', 'MATCHING_ASM_DATA') or 'recipe' not in owner: continue
        name = owner['recipe']
        recipe = (recipe_overrides or {}).get(name) or read_json(ROOT/name)
        result[name] = sha(json_bytes(recipe))
        raw = (source_overrides or {}).get(recipe['source'])
        if raw is None: raw = (ROOT/recipe['source']).read_bytes()
        result[recipe['source']] = sha(raw)
    return result


def validate_layout(manifest, size):
    require(len({o['id'] for o in manifest['owners']})==len(manifest['owners']),'Duplicate owner IDs')
    at = 0
    for owner in manifest['owners']:
        require(owner['start'] == at and at < owner['end'] <= size, 'Ownership gap/overlap/invalid extent')
        require(owner['kind'] in ['UNRESOLVED_RAW', 'MATCHING_C', 'MATCHING_ASM',
                                  'MATCHING_C_DATA', 'MATCHING_ASM_DATA', 'KNOWN_TOOLCHAIN_LIBRARY',
                                  'LINK_FILL'],
                'Unsupported production ownership')
        require(owner.get('contribution_form') in (None, 'prefix_of_object') and
                (owner.get('contribution_form') is None or owner['kind'] == 'MATCHING_C'),
                'Unsupported contribution form')
        at = owner['end']
    require(at == size, 'Ownership does not cover full initialized image')
    parents={o['id']:o for o in manifest['owners'] if o['kind'] in ('MATCHING_C','MATCHING_ASM')}
    data=[o for o in manifest['owners'] if o['kind'] in ('MATCHING_C_DATA','MATCHING_ASM_DATA')]
    bss_data=[o for o in manifest.get('bss_owners',[]) if o['kind'] in ('MATCHING_C_DATA','MATCHING_ASM_DATA')]
    for row in data:
        if row.get('module_form') == 'data-only':
            require('recipe' in row and 'parent' not in row and
                    (row['segment']=='_DATA' or row.get('far_data') is True) and
                    row['target']['size']==row['end']-row['start'],
                    'Invalid independent data-only owner')
            continue
        parent=parents.get(row.get('parent'))
        require(parent is not None and
                row['kind']==('MATCHING_ASM_DATA' if parent['kind']=='MATCHING_ASM' else 'MATCHING_C_DATA') and
                row['segment'] in ('_DATA','CONST') and
                {'segment':row['segment'],'start':row['start'],'end':row['end'],
                 'target':row['target']} in parent.get('data_intervals',[]),
                'Orphaned or unlisted initialized C data owner')
    for parent in parents.values():
        keys=[(i['segment'],i['start'],i['end'],i['target']['sha256'])
              for i in parent.get('data_intervals',[])]
        require(len(keys)==len(set(keys)), 'Duplicate C data interval claim')
        for interval in parent.get('data_intervals',[]):
            partition=bss_data if interval['segment']=='_BSS' else data
            require(sum(o.get('parent')==parent['id'] and
                        (o['segment'],o['start'],o['end'],o['target'])==
                        (interval['segment'],interval['start'],interval['end'],interval['target'])
                        for o in partition)==1,
                    'C data/BSS interval lacks exactly one owner')
    if 'bss_owners' in manifest:
        layout=read_json(ROOT/'layout/data-symbols.json')
        position=layout['bss_start']
        for owner in manifest['bss_owners']:
            require(owner['start']==position and position<owner['end']<=layout['bss_end'],
                    'BSS ownership gap/overlap')
            position=owner['end']
            if owner['kind'] in ('MATCHING_C_DATA','MATCHING_ASM_DATA'):
                parent=parents.get(owner.get('parent'))
                require(owner['segment']=='_BSS' and parent is not None and
                        owner['kind']==('MATCHING_ASM_DATA' if parent['kind']=='MATCHING_ASM' else 'MATCHING_C_DATA') and
                        {'segment':'_BSS','start':owner['start'],'end':owner['end'],
                         'target':owner['target']} in parent.get('data_intervals',[]),
                        'Orphaned BSS owner')
            else:
                require(owner['kind']=='UNRESOLVED_RAW','Unsupported BSS owner')
        require(position==layout['bss_end'],'BSS ownership does not cover clear range')

def build(manifest=None, recipe_overrides=None, publish=True, source_overrides=None, *, allow_pending=False, artifact=None):
    # Pure evidence checks are reused within this one fresh construction only
    # (tools/memo.py); every contribution is still compiled afresh.
    import memo
    with memo.session():
        return _build(manifest, recipe_overrides, publish, source_overrides,
                      allow_pending=allow_pending, artifact=artifact)


def _build(manifest=None, recipe_overrides=None, publish=True, source_overrides=None, *, allow_pending=False, artifact=None):
    output = ROOT / 'build/exact'
    output.mkdir(parents=True, exist_ok=True)
    # A failed invocation must never leave a current PASS receipt.
    if publish:
        (output / 'acceptance.json').unlink(missing_ok=True)
    if not allow_pending:
        from transaction import ensure_consistent
        ensure_consistent()
    before = inputs()
    oracle = verify(write=False)
    mz = MZ.parse(oracle[1])
    original = mz.load_image(oracle[1])
    manifest = manifest or read_json(ROOT / 'layout/manifest.json')
    require(manifest['oracle_sha256']==oracle[2]['load_image']['sha256'],'Manifest belongs to another oracle')
    validate_layout(manifest, len(original))
    production_before = production_inputs(manifest, recipe_overrides, before, source_overrides)
    chunks, receipts, matching, matching_asm, libraries = [], [], 0, 0, 0
    emitted={}
    compiled={}
    compiled = _compile_owners(manifest, recipe_overrides, source_overrides, oracle)
    fill=0
    for owner_id, result in compiled.items():
        emitted[owner_id]={name:bytes.fromhex(raw) for name,raw in
            result[1]['binding'].get('secondary_payloads',{}).items()}
    for owner in manifest['owners']:
        start, end = owner['start'], owner['end']
        if owner['kind'] == 'UNRESOLVED_RAW':
            chunks.append(original[start:end])
        elif owner['kind']=='LINK_FILL':
            # LINK paragraph alignment fill, re-derived every build (link_fill).
            from link_fill import checked_fill
            receipts.append({'link_fill': checked_fill(owner, manifest, original)})
            chunks.append(original[start:end])
            fill+=end-start
        elif owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY':
            payload,receipt=bind_library(owner,original,mz.relocations,manifest=manifest)
            chunks.append(payload)
            receipts.append(receipt)
            libraries+=len(payload)
        elif owner['kind'] in ('MATCHING_C_DATA','MATCHING_ASM_DATA'):
            if owner.get('module_form')=='data-only':
                payload,receipt=compiled[owner['id']]
                receipts.append(receipt)
            else:
                require(owner['parent'] in emitted and owner['segment'] in emitted[owner['parent']],
                        'Secondary C payload is missing from its compiled CODE owner')
                payload=emitted[owner['parent']][owner['segment']]
            require(len(payload)==end-start and identity(payload)==owner['target'] and
                    payload==original[start:end], 'Secondary C payload differs from oracle')
            chunks.append(payload)
            if owner['kind']=='MATCHING_ASM_DATA': matching_asm+=len(payload)
            else: matching+=len(payload)
        else:
            payload, receipt = compiled[owner['id']]
            chunks.append(payload)
            receipts.append(receipt)
            if owner['kind']=='MATCHING_ASM': matching_asm+=len(payload)
            else: matching+=len(payload)
    return _finish(manifest, recipe_overrides, source_overrides, before, production_before, oracle, mz,
                   original, chunks, receipts, matching, matching_asm, libraries, emitted,
                   publish, artifact, output, fill)


def workers():
    """Concurrent fresh compiles per build (each in its own work directory)."""
    import os
    value = os.environ.get('STUNTS_BUILD_WORKERS')
    if value:
        require(value.isdigit() and 1 <= int(value) <= 32, 'Invalid STUNTS_BUILD_WORKERS')
        return int(value)
    return max(1, min(8, (os.cpu_count() or 2) // 2))


def _compile_owners(manifest, recipe_overrides, source_overrides, oracle):
    """Freshly probe every active contribution; failures are reported in
    manifest order independently of completion order."""
    jobs=[]
    for owner in manifest['owners']:
        if owner['kind'] not in ('MATCHING_C','MATCHING_ASM','MATCHING_C_DATA','MATCHING_ASM_DATA') or 'recipe' not in owner: continue
        recipe=(recipe_overrides or {}).get(owner['recipe']) or read_json(ROOT/owner['recipe'])
        require((recipe['start'],recipe['end'])==(owner['start'],owner['end']),
                'Recipe ownership mismatch')
        if owner['kind'] in ('MATCHING_ASM','MATCHING_ASM_DATA'):
            require(recipe.get('kind')=='asm' and recipe['source'].startswith('asm/') and
                    recipe['source'].endswith('.ASM'), 'Production must consume tracked ASM source')
        else:
            require(recipe.get('kind','c')=='c' and recipe['source'].startswith('src/'),
                    'Production must consume recovered C source')
        prefix_owner = owner.get('contribution_form') == 'prefix_of_object'
        require(('prefix_of_object' in recipe) == prefix_owner,
                'Prefix owner and recipe form differ')
        if prefix_owner:
            meta = recipe['prefix_of_object']
            require(owner['kind'] == 'MATCHING_C' and 'members' not in recipe and
                    owner['object_start'] == meta['object_start'] == owner['start'] and
                    owner['prefix_records'] == meta['records'],
                    'Prefix owner metadata differs from its recipe')
        require(bool(recipe.get('data_only')) == (owner.get('module_form')=='data-only') and
                bool(recipe.get('far_data')) == bool(owner.get('far_data')),
                'Data-only owner and recipe form differ')
        jobs.append((owner['id'], owner['recipe'], recipe))

    def run(job):
        owner_id, recipe_path, recipe = job
        try:
            return owner_id, probe(recipe, oracle, (source_overrides or {}).get(recipe['source'])), None
        except Exception as error:  # reported below in manifest order
            # Attribution for batch publication failure isolation.
            error.owner_id, error.owner_recipe = owner_id, recipe_path
            return owner_id, None, error
    count = min(workers(), max(1, len(jobs)))
    if count == 1:
        results = [run(job) for job in jobs]
    else:
        from concurrent.futures import ThreadPoolExecutor
        with ThreadPoolExecutor(max_workers=count) as pool:
            results = list(pool.map(run, jobs))
    compiled = {}
    for owner_id, result, error in results:
        if error is not None:
            raise error
        compiled[owner_id] = result
    return compiled


def _finish(manifest, recipe_overrides, source_overrides, before, production_before, oracle, mz,
            original, chunks, receipts, matching, matching_asm, libraries, emitted,
            publish, artifact, output, fill=0):
    image = b''.join(chunks)
    require(image == original, 'Full image mismatch')
    matching_bss=0; matching_asm_bss=0
    for owner in manifest.get('bss_owners',[]):
        if owner['kind'] not in ('MATCHING_C_DATA','MATCHING_ASM_DATA'): continue
        require(owner['parent'] in emitted and owner['segment']=='_BSS' and
                emitted[owner['parent']].get('_BSS')==bytes(owner['end']-owner['start']) and
                identity(emitted[owner['parent']]['_BSS'])==owner['target'] and
                original[owner['start']:min(owner['end'],len(original))] ==
                    bytes(max(0,min(owner['end'],len(original))-owner['start'])),
                'Emitted BSS contribution is not the verified zero extent')
        if owner['kind']=='MATCHING_C_DATA': matching_bss+=owner['end']-owner['start']
        else: matching_asm_bss+=owner['end']-owner['start']
    # Header is an explicit synthetic oracle-metadata owner; no final binary patches.
    executable = oracle[1][:mz.header_size] + image
    require(executable == oracle[1], 'Full MZ mismatch')
    require(MZ.parse(executable).relocations == mz.relocations, 'Relocation order/pairs differ')
    require(verify(write=False)[2]==oracle[2], 'Original assets changed during construction')
    from compiler import verify_toolchain
    for profile in {o['profile'] for o in manifest['owners'] if o.get('profile')}:
        verify_toolchain(profile)
    require(inputs() == before, 'Inputs changed during fresh construction')
    require(production_inputs(manifest, recipe_overrides, before, source_overrides) == production_before, 'Production dependencies changed')
    report = {'status': 'HYBRID_EXACT', 'fully_recovered': matching + matching_asm + libraries + fill == len(image),
              'executable': identity(executable), 'load_image': identity(image),
              'matching_c_bytes': matching, 'matching_asm_bytes': matching_asm,
              'matching_c_bss_bytes':matching_bss,
              'matching_asm_bss_bytes':matching_asm_bss,
              'raw_initialized_bytes': len(image) - matching - matching_asm - libraries - fill, 'library_production_bytes':libraries,
              'link_fill_bytes': fill,
              'relocation_count': len(mz.relocations), 'inputs': before,
              'production_inputs':production_before, 'compiler_receipts': receipts}
    if artifact is not None:
        artifact['executable'] = executable
    if publish:
        (output / 'mcga.exe').write_bytes(executable)
        write_json(output / 'acceptance.json', report)
    return report

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('command', choices=['verify'])
    p.parse_args()
    from transaction import exclusive
    with exclusive():
        result = build()
    print(f"PASS HYBRID_EXACT: {result['matching_c_bytes']} matching C bytes, {result['raw_initialized_bytes']} raw initialized bytes")

if __name__ == '__main__':
    main()
