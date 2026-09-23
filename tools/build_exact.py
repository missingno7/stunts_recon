"""Fresh hybrid construction with explicit unresolved ownership."""
import argparse
from common import ROOT, read_json, write_json, require, sha, identity
from oracle import verify
from mz import MZ
from probe_module import probe
from library import bind_library

def inputs():
    result = {}
    for folder in ['tools', 'src', 'asm', 'include', 'recipes', 'layout', 'recovery/candidates']:
        for path in sorted((ROOT / folder).rglob('*')):
            if path.is_file() and '__pycache__' not in path.parts:
                result[path.relative_to(ROOT).as_posix()] = sha(path.read_bytes())
    return result

def validate_layout(manifest, size):
    require(len({o['id'] for o in manifest['owners']})==len(manifest['owners']),'Duplicate owner IDs')
    at = 0
    for owner in manifest['owners']:
        require(owner['start'] == at and at < owner['end'] <= size, 'Ownership gap/overlap/invalid extent')
        require(owner['kind'] in ['UNRESOLVED_RAW', 'MATCHING_C', 'KNOWN_TOOLCHAIN_LIBRARY'], 'Unsupported production ownership')
        at = owner['end']
    require(at == size, 'Ownership does not cover full initialized image')

def build(manifest=None, recipe_overrides=None, publish=True):
    output = ROOT / 'build/exact'
    output.mkdir(parents=True, exist_ok=True)
    # A failed invocation must never leave a current PASS receipt.
    if publish:
        (output / 'acceptance.json').unlink(missing_ok=True)
    before = inputs()
    oracle = verify(write=False)
    mz = MZ.parse(oracle[1])
    original = mz.load_image(oracle[1])
    staged_manifest = manifest is not None
    manifest = manifest or read_json(ROOT / 'layout/manifest.json')
    if not staged_manifest:
        plan=read_json(ROOT/'layout/production-plan.json')
        expected=[{'owner':o['id'],'recipe':o['recipe'],'source':read_json(ROOT/o['recipe'])['source']} if o['kind']=='MATCHING_C' else {'owner':o['id'],'library':o['library'],'module':o['module']} for o in manifest['owners'] if o['kind']!='UNRESOLVED_RAW']
        require(plan['modules']==expected,'Production plan differs from active ownership')
    require(manifest['oracle_sha256']==oracle[2]['load_image']['sha256'],'Manifest belongs to another oracle')
    validate_layout(manifest, len(original))
    chunks, receipts, matching, libraries = [], [], 0, 0
    for owner in manifest['owners']:
        start, end = owner['start'], owner['end']
        if owner['kind'] == 'UNRESOLVED_RAW':
            chunks.append(original[start:end])
        elif owner['kind']=='KNOWN_TOOLCHAIN_LIBRARY':
            payload,receipt=bind_library(owner,original,mz.relocations)
            chunks.append(payload)
            receipts.append(receipt)
            libraries+=len(payload)
        else:
            recipe = (recipe_overrides or {}).get(owner['recipe']) or read_json(ROOT / owner['recipe'])
            require((recipe['start'], recipe['end']) == (start, end), 'Recipe ownership mismatch')
            require(recipe['source'].startswith('src/'), 'Production must consume recovered src/')
            payload, receipt = probe(recipe, oracle)
            chunks.append(payload)
            receipts.append(receipt)
            matching += len(payload)
    image = b''.join(chunks)
    require(image == original, 'Full image mismatch')
    # Header is an explicit synthetic oracle-metadata owner; no final binary patches.
    executable = oracle[1][:mz.header_size] + image
    require(executable == oracle[1], 'Full MZ mismatch')
    require(MZ.parse(executable).relocations == mz.relocations, 'Relocation order/pairs differ')
    require(verify(write=False)[2]==oracle[2], 'Original assets changed during construction')
    from compiler import verify_toolchain
    for profile in {o['profile'] for o in manifest['owners'] if o.get('profile')}:
        verify_toolchain(profile)
    require(inputs() == before, 'Inputs changed during fresh construction')
    report = {'status': 'HYBRID_EXACT', 'fully_recovered': matching + libraries == len(image),
              'executable': identity(executable), 'load_image': identity(image),
              'matching_c_bytes': matching, 'matching_asm_bytes': 0,
              'raw_initialized_bytes': len(image) - matching - libraries, 'library_production_bytes':libraries,
              'relocation_count': len(mz.relocations), 'inputs': before, 'compiler_receipts': receipts}
    if publish:
        (output / 'mcga.exe').write_bytes(executable)
        write_json(output / 'acceptance.json', report)
    return report

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('command', choices=['verify'])
    p.parse_args()
    result = build()
    print(f"PASS HYBRID_EXACT: {result['matching_c_bytes']} matching C bytes, {result['raw_initialized_bytes']} raw initialized bytes")

if __name__ == '__main__':
    main()
