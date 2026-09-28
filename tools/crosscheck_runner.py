"""Optional independent DOSBox-X compiler/assembler cross-check for active recipes."""
import os
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, read_json, write_json, require, identity
from compiler import verify_toolchain, object_policy
from preprocessor import prepare, check_recipe
from object_probe import read_object, recipe_sparse_zero
from binder import bind_contribution, require_relocations_from_fixups
from code_symbols import resolve_recipe_symbols
from multi_contribution import bind_multi, checked_members
from secondary_contribution import bind_single_secondary
from oracle import verify
from mz import MZ
from build_exact import inputs
from assembler import asm_source, prepare_asm
from object_flags import recipe_flags, object_control_flags, same_object

# Hash-identical copy of the former C:/DOSBox-X install (SHA-256 b028a4d3...).
DOSBOX_X='C:/tools/dosbox-x/dosbox-x.exe'


def bind_recipe_object(obj, recipe, image, relocations):
    """Use the same complete recipe path as the acceptance probe."""
    if 'prefix_of_object' in recipe:
        # The record-closed prefix is re-derived from this independent object.
        from prefix_proof import bind_prefix
        return bind_prefix(obj, recipe, image, relocations)
    if recipe.get('data_only'):
        from data_only import bind_data_only
        return bind_data_only(obj,recipe,image,relocations)
    if 'members' in recipe:
        checked_members(recipe, image)
        return bind_multi(obj, recipe, image, relocations)
    if recipe.get('secondary_dgroup_segments'):
        return bind_single_secondary(obj, recipe, image, relocations)
    symbols=resolve_recipe_symbols(recipe,image,relocations)
    return bind_contribution(obj,recipe,symbols)


def _run_dos(runner, work, tc, flags, asm, profile=None):
    """One independent DOS compile/assembly in `work` (UNIT.C or UNIT.ASM).

    C compiles reproduce the pinned pass environment: the profile is mounted
    read-only from a hash-verified mirror at its pinned DOS pass directory,
    TEMP is the pinned `.`, and the pinned DOSBox-X memory configuration gives
    the passes the pinned runner's free memory (tools/pass_environment.py)."""
    import pass_environment as PE
    independent = PE.independent_runner()
    require(Path(runner).resolve() == Path(independent['path']).resolve(),
            'Independent runner differs from the pinned DOSBox-X')
    if asm:
        mounts = [f'mount d "{tc}" -ro', f'mount e "{work}"']
        commands = ['e:', 'set PATH=D:\\', 'set TEMP=.']
        dos_command = 'D:\\MASM.EXE '+' '.join(flags)+' UNIT,UNIT.OBJ,UNIT.LST; > COMP.LOG'
    else:
        from compiler import verify_toolchain
        config, _ = verify_toolchain(profile)
        mounts, commands, exe = PE.dosbox_compile_commands(profile, config, flags)
        mounts.append(f'mount e "{work}"')
        dos_command = exe+' /c '+' '.join(flags)+' UNIT.C > COMP.LOG'
    batch=['@echo off',dos_command,
           'if errorlevel 1 goto failed','echo 0 > RESULT.TXT','goto done',':failed','echo 1 > RESULT.TXT',':done','exit']
    (work/'RUN.BAT').write_bytes(('\r\n'.join(batch)+'\r\n').encode('ascii'))
    conf = PE.dosbox_conf(work, mounts, commands+['RUN.BAT'], independent)
    cmd, result = PE.run_dosbox(work, conf)
    require(result.returncode==0 and (work/'RESULT.TXT').is_file() and (work/'RESULT.TXT').read_text().strip()=='0','Independent DOS compilation failed')
    return cmd,batch


def recipe_declarations_names(r):
    from communal_unit import recipe_declarations
    declared=recipe_declarations(r)
    return None if declared is None else [name for name,_ in declared]


def independent_row(r, source, oracle, image, runner=None):
    """Independent DOSBox-X compile/assembly and complete binding of one recipe.

    `source` is the exact candidate or canonical source bytes; the same
    binders and relocation obligations as the acceptance probe apply."""
    runner=Path(DOSBOX_X) if runner is None else runner
    require(runner.is_file(),'Independent DOSBox-X backend unavailable')
    (ROOT/'build/crosschecks').mkdir(parents=True,exist_ok=True)
    config,_=verify_toolchain(r['profile'])
    reader_policy=object_policy(config)
    work=Path(tempfile.mkdtemp(prefix='r',dir=ROOT/'build/crosschecks'))
    asm = r.get('kind') == 'asm'
    if asm:
        expanded, closure = prepare_asm(source)
        require(r['profile']=='masm510-game' and r.get('include_closure',[])==closure,
                'Independent ASM profile/closure differs')
        require(r.get('assembler_flags')==config['flags'],
                'Independent ASM recipe flags differ')
        (work/'UNIT.ASM').write_bytes(expanded)
    else:
        expanded, closure = prepare(source, r['profile'])
        check_recipe(r, closure)
        (work/'UNIT.C').write_bytes(expanded.replace(b'\r\n',b'\n').replace(b'\n',b'\r\n'))
    tc=(ROOT/config['directory']).resolve()
    flags=config['flags'] if asm else (recipe_flags(r) or config['flags'])
    control=None if asm else object_control_flags(r)
    cmd,batch=_run_dos(runner,work,tc,flags,asm,r['profile'])
    if control is not None:
        # Same per-object flag rule as the acceptance probe: a canonical-flag
        # C contribution in a flagged object must emit the identical object.
        cwork=Path(tempfile.mkdtemp(prefix='r',dir=ROOT/'build/crosschecks'))
        (cwork/'UNIT.C').write_bytes((work/'UNIT.C').read_bytes())
        _run_dos(runner,cwork,tc,control,False,r['profile'])
        cd=recipe_declarations_names(r)
        require(same_object(read_object((cwork/'UNIT.OBJ').read_bytes(),sparse_zero=recipe_sparse_zero(r),communals=cd,
                                        **reader_policy),
                            read_object((work/'UNIT.OBJ').read_bytes(),sparse_zero=recipe_sparse_zero(r),communals=cd,
                                        **reader_policy)),
                'Independent object differs under its registered object flag set')
    from communal_unit import recipe_declarations, check_object_communals
    declared=recipe_declarations(r)
    communals=None if declared is None else [name for name,_ in declared]
    obj=read_object((work/'UNIT.OBJ').read_bytes(),
                    sparse_zero=None if asm else recipe_sparse_zero(r), communals=communals,
                    **reader_policy)
    check_object_communals(obj, r)
    if asm:
        require(asm_source(source)==(work/'UNIT.ASM').read_bytes() and
                prepare_asm(source)[1] == closure,
                'Independent ASM source changed during assembly')
        require(r['object_declarations']==
                {'segments':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals},
                'Independent ASM declarations differ')
        require(obj.linker_fixups==r['expected_fixups'],'Independent ASM FIXUPPs differ')
        if not r.get('data_only'):
            from link_frames import check_asm_object
            check_asm_object(obj, r)
    else:
        require(prepare(source, r['profile'])[1] == closure,
                'Independent preprocessor closure changed during compilation')
    require_relocations_from_fixups(obj,r,oracle[2]['unpacked_mz']['relocations'])
    payload,binding=bind_recipe_object(obj,r,image,oracle[2]['unpacked_mz']['relocations'])
    relocs=[site for site in oracle[2]['unpacked_mz']['relocations'] if r['start']-1<=site['load_offset']<r['end']]
    require(binding['generated_relocations']==r['expected_relocations']==relocs,'Independent source relocation mismatch')
    require(payload==image[r['start']:r['end']],'Independent compiler bytes mismatch')
    for name,spec in r.get('secondary_dgroup_segments',{}).items():
        secondary=bytes.fromhex(binding['secondary_payloads'][name])
        if name=='_BSS':
            require(secondary==bytes(spec['end']-spec['start']),
                    'Independent BSS differs')
        else:
            require(secondary==image[spec['start']:spec['end']] and
                    identity(secondary)==spec['target'],
                    'Independent secondary data differs')
    return {'task':r['id'],'source':identity(source),'object':identity((work/'UNIT.OBJ').read_bytes()),
                 'source_closure':closure, 'kind':r.get('kind','c'),
                 'payload':identity(payload),'binding':binding,'exact':True,'command':cmd,'dos_command':batch[1]}


def main():
    report_path=ROOT/'build/validation/independent.json'
    report_path.unlink(missing_ok=True)
    before=inputs()
    runner=Path(DOSBOX_X)
    require(runner.is_file(),'Independent DOSBox-X backend unavailable')
    runner_identity=identity(runner.read_bytes())
    oracle=verify(write=False);image=MZ.parse(oracle[1]).load_image(oracle[1]);rows=[]
    active=[ROOT/o['recipe'] for o in read_json(ROOT/'layout/manifest.json')['owners']
            if o['kind'] in ('MATCHING_C','MATCHING_ASM','MATCHING_C_DATA','MATCHING_ASM_DATA')
            and 'recipe' in o]
    for path in active:
        r=read_json(path)
        rows.append(independent_row(r,(ROOT/r['source']).read_bytes(),oracle,image,runner))
    require(inputs()==before,'Inputs changed during independent compilation')
    require(verify(write=False)[2]==oracle[2],'Oracle changed during independent compilation')
    for profile in {read_json(path)['profile'] for path in active}:
        verify_toolchain(profile)
    require(identity(runner.read_bytes())==runner_identity,'Independent runner changed during compilation')
    write_json(report_path,{'runner':{'path':str(runner),**runner_identity},'inputs':before,'results':rows})
    print('PASS: independent DOSBox-X exact code and complete binding obligations for',len(rows),'contributions')

if __name__=='__main__':
    from transaction import exclusive, ensure_consistent
    with exclusive():
        ensure_consistent()
        import memo
        with memo.session():
            main()
