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


# integ38: the load image is rounded past _edata, so the first bytes of the
# first _BSS contribution are physically in the image (seg000's 12-byte _BSS
# at [199994,200006); the image ends at 200000).  Those in-image bytes are
# owned by that BSS owner, not raw: one BSS_IN_IMAGE row, the last owner row,
# naming the accepted bss_owners row that starts at bss_start and extends past
# the image end.  Its bytes are that owner's verified zero _BSS prefix
# (_finish rechecks the emitted zero extent against the oracle).
BSS_IN_IMAGE = 'BSS_IN_IMAGE'
_BSS_ACCEPTED = ('MATCHING_C_DATA', 'MATCHING_ASM_DATA', 'KNOWN_TOOLCHAIN_LIBRARY_DATA')


def checked_bss_in_image(owner, manifest, size):
    layout = read_json(ROOT/'layout/data-symbols.json')
    rows = [o for o in manifest.get('bss_owners', []) if o['id'] == owner.get('bss_owner')]
    require(set(owner) == {'id', 'kind', 'start', 'end', 'bss_owner'} and
            owner['id'] == owner['bss_owner'] + '@image' and
            owner['start'] == layout['bss_start'] and owner['end'] == size and
            len(rows) == 1 and rows[0]['kind'] in _BSS_ACCEPTED and rows[0]['segment'] == '_BSS' and
            rows[0]['start'] == owner['start'] and rows[0]['end'] >= owner['end'] and
            manifest['bss_owners'][0] is rows[0],
            'In-image BSS row is not the image prefix of the first accepted _BSS owner')
    return {'bss_in_image': [owner['start'], owner['end']], 'bss_owner': rows[0]['id']}


def validate_layout(manifest, size):
    require(len({o['id'] for o in manifest['owners']})==len(manifest['owners']),'Duplicate owner IDs')
    at = 0
    for owner in manifest['owners']:
        require(owner['start'] == at and at < owner['end'] <= size, 'Ownership gap/overlap/invalid extent')
        require(owner['kind'] in ['UNRESOLVED_RAW', 'MATCHING_C', 'MATCHING_ASM',
                                  'MATCHING_C_DATA', 'MATCHING_ASM_DATA', 'KNOWN_TOOLCHAIN_LIBRARY',
                                  'KNOWN_TOOLCHAIN_LIBRARY_DATA', 'LINK_FILL', BSS_IN_IMAGE],
                'Unsupported production ownership')
        if owner['kind'] == BSS_IN_IMAGE:
            checked_bss_in_image(owner, manifest, size)
        require(owner.get('contribution_form') in (None, 'prefix_of_object') and
                (owner.get('contribution_form') is None or owner['kind'] == 'MATCHING_C'),
                'Unsupported contribution form')
        at = owner['end']
    require(at == size, 'Ownership does not cover full initialized image')
    parents={o['id']:o for o in manifest['owners'] if o['kind'] in ('MATCHING_C','MATCHING_ASM')}
    # integ36: owned pinned runtime data (initialized rows and one _BSS row per
    # member) mirrors exactly the member's `linked` storage rows.
    libraries={o['id']:o for o in manifest['owners'] if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY'}
    # integ37: data-only pinned runtime members own bytes only through their
    # linked storage rows; they have no code extent in the partition.
    import runtime_binding
    members=runtime_binding.runtime_data_members(manifest)
    require(len({o['id'] for o in manifest['owners']}|{m['id'] for m in members})==
            len(manifest['owners'])+len(members), 'Duplicate owner IDs')
    for member in members:
        runtime_binding.check_data_member_form(member)
        require(any(r.get('ownership')=='linked' and r['end']>r['start']
                    for r in member['binding']['storage'].values()),
                'Data-only runtime member owns no bytes')
        libraries[member['id']]=member
    runtime_rows=[o for o in manifest['owners'] if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY_DATA']
    runtime_bss=[o for o in manifest.get('bss_owners',[]) if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY_DATA']
    for row in runtime_rows+runtime_bss:
        parent=libraries.get(row.get('parent'))
        linked=(parent or {}).get('binding',{}).get('storage',{}).get(row.get('segment'))
        require(parent is not None and linked is not None and linked.get('ownership')=='linked' and
                (linked['start'],linked['end'])==(row['start'],row['end']) and
                row.get('id')==f"{parent['id']}:{row['segment']}" and
                (row['segment']=='_BSS')==(row in runtime_bss) and
                row.get('target',{}).get('size')==row['end']-row['start'],
                'Orphaned or unlisted pinned runtime data owner')
    for parent in libraries.values():
        linked={n for n,r in parent.get('binding',{}).get('storage',{}).items()
                if r.get('ownership')=='linked' and r['end']>r['start']}
        require(sorted(r['segment'] for r in runtime_rows+runtime_bss if r['parent']==parent['id'])==sorted(linked),
                'Linked runtime storage lacks exactly one owned row')
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
        # integ35: raw BSS debt is one reviewed placeholder per object (bss_link).
        import bss_link
        bss_link.load_partition(manifest)
        for owner in manifest['bss_owners']:
            require(owner['start']==position and position<owner['end']<=layout['bss_end'],
                    'BSS ownership gap/overlap')
            position=owner['end']
            if owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY_DATA':
                import runtime_binding
                require(owner.get('placement')==runtime_binding.RUNTIME_BSS_PLACEMENT and owner['segment']=='_BSS',
                        'Pinned runtime BSS owner lacks its real-link placement kind')
            elif owner['kind'] in ('MATCHING_C_DATA','MATCHING_ASM_DATA'):
                parent=parents.get(owner.get('parent'))
                import bss_link
                require(owner.get('placement') in bss_link.PLACEMENTS,
                        'Accepted BSS owner lacks a real-link placement kind')
                require(owner['segment']=='_BSS' and parent is not None and
                        owner['kind']==('MATCHING_ASM_DATA' if parent['kind']=='MATCHING_ASM' else 'MATCHING_C_DATA') and
                        {'segment':'_BSS','start':owner['start'],'end':owner['end'],
                         'target':owner['target']} in parent.get('data_intervals',[]),
                        'Orphaned BSS owner')
            elif owner['kind']=='LINK_FILL':
                if owner.get('basis') == bss_link.BSS_WORD_FILL:
                    # bss_link.load_partition above derives this one-byte gap
                    # from the adjacent complete _BSS objects and evidence row.
                    pass
                else:
                    # integ39: LINK's c_common paragraph fill, re-derived from its neighbours.
                    import communal_unit
                    communal_unit.checked_communal_fill(owner, manifest)
            elif owner['kind']=='LINK_COMMUNAL':
                # integ39: the whole c_common unit (declarers checked after the fresh compile).
                import communal_unit
                communal_unit.check_row_form(owner, layout)
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
    # integ39: COMDEF declarations belong to the whole accepted communal unit
    # (conditions a, c, d without a link; the real link decides b).
    import communal_unit
    communal=communal_unit.check_manifest(
        manifest, lambda owner: (recipe_overrides or {}).get(owner['recipe']) or read_json(ROOT/owner['recipe']),
        lambda path: (source_overrides or {}).get(path) or (ROOT/path).read_bytes())
    fill=0
    bss_image=0
    for owner_id, result in compiled.items():
        emitted[owner_id]={name:bytes.fromhex(raw) for name,raw in
            result[1]['binding'].get('secondary_payloads',{}).items()}
    # integ37: data-only pinned runtime members, bound from the hash-pinned member.
    import runtime_binding
    for member in runtime_binding.runtime_data_members(manifest):
        _,receipt=bind_library(member,original,mz.relocations,manifest=manifest)
        receipts.append(receipt)
        emitted[member['id']]={name:bytes.fromhex(raw) for name,raw in
            receipt['binding'].get('secondary_payloads',{}).items()}
    for owner in manifest['owners']:
        start, end = owner['start'], owner['end']
        if owner['kind'] == 'UNRESOLVED_RAW':
            chunks.append(original[start:end])
        elif owner['kind']==BSS_IN_IMAGE:
            receipts.append(checked_bss_in_image(owner, manifest, len(original)))
            chunks.append(bytes(end-start))
            bss_image+=end-start
        elif owner['kind']=='LINK_FILL':
            # LINK paragraph alignment fill, re-derived every build (link_fill).
            from link_fill import checked_fill
            receipts.append({'link_fill': checked_fill(owner, manifest, original, recipe_overrides)})
            chunks.append(original[start:end])
            fill+=end-start
        elif owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY':
            payload,receipt=bind_library(owner,original,mz.relocations,manifest=manifest)
            chunks.append(payload)
            receipts.append(receipt)
            libraries+=len(payload)
            emitted[owner['id']]={name:bytes.fromhex(raw) for name,raw in
                receipt.get('binding',{}).get('secondary_payloads',{}).items()}
        elif owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY_DATA':
            # integ36: owned pinned runtime data, bound with its member above.
            require(owner['parent'] in emitted and owner['segment'] in emitted[owner['parent']],
                    'Pinned runtime data payload is missing from its bound member')
            payload=emitted[owner['parent']][owner['segment']]
            require(len(payload)==end-start and identity(payload)==owner['target'] and
                    payload==original[start:end], 'Pinned runtime data payload differs from oracle')
            chunks.append(payload)
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
    if communal is not None:
        receipts.append(communal)
    return _finish(manifest, recipe_overrides, source_overrides, before, production_before, oracle, mz,
                   original, chunks, receipts, matching, matching_asm, libraries, emitted,
                   publish, artifact, output, fill, bss_image)


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
            publish, artifact, output, fill=0, bss_image=0):
    image = b''.join(chunks)
    require(image == original, 'Full image mismatch')
    matching_bss=0; matching_asm_bss=0
    library_bss=0
    for owner in manifest.get('bss_owners',[]):
        if owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY_DATA':
            require(owner['parent'] in emitted and owner['segment']=='_BSS' and
                    emitted[owner['parent']].get('_BSS')==bytes(owner['end']-owner['start']) and
                    identity(emitted[owner['parent']]['_BSS'])==owner['target'],
                    'Pinned runtime BSS contribution is not the verified zero extent')
            library_bss+=owner['end']-owner['start']
            continue
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
    report = {'status': 'HYBRID_EXACT', 'fully_recovered': matching + matching_asm + libraries + fill + bss_image == len(image),
              'executable': identity(executable), 'load_image': identity(image),
              'matching_c_bytes': matching, 'matching_asm_bytes': matching_asm,
              'matching_c_bss_bytes':matching_bss,
              'matching_asm_bss_bytes':matching_asm_bss,
              'library_bss_bytes':library_bss,
              'raw_initialized_bytes': len(image) - matching - matching_asm - libraries - fill - bss_image,
              'library_production_bytes':libraries,
              'link_fill_bytes': fill, 'bss_in_image_bytes': bss_image,
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
