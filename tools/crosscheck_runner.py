"""Optional independent DOSBox-X compiler cross-check for active C recipes."""
import os
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, read_json, write_json, require, identity
from compiler import verify_toolchain
from preprocessor import prepare, check_recipe
from object_probe import read_object
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from multi_contribution import bind_multi, checked_members
from secondary_contribution import bind_single_secondary
from oracle import verify
from mz import MZ
from build_exact import inputs


def bind_recipe_object(obj, recipe, image, relocations):
    """Use the same complete recipe path as the acceptance probe."""
    if 'members' in recipe:
        checked_members(recipe, image)
        return bind_multi(obj, recipe, image, relocations)
    if recipe.get('secondary_dgroup_segments'):
        return bind_single_secondary(obj, recipe, image, relocations)
    symbols=resolve_recipe_symbols(recipe,image,relocations)
    return bind_contribution(obj,recipe,symbols)


def main():
    report_path=ROOT/'build/validation/independent.json'
    report_path.unlink(missing_ok=True)
    before=inputs()
    runner=Path('C:/DOSBox-X/dosbox-x.exe')
    require(runner.is_file(),'Independent DOSBox-X backend unavailable')
    runner_identity=identity(runner.read_bytes())
    oracle=verify(write=False);image=MZ.parse(oracle[1]).load_image(oracle[1]);rows=[]
    (ROOT/'build/crosschecks').mkdir(parents=True,exist_ok=True)
    active=[ROOT/o['recipe'] for o in read_json(ROOT/'layout/manifest.json')['owners'] if o['kind']=='MATCHING_C']
    for path in active:
        r=read_json(path);config,_=verify_toolchain(r['profile'])
        work=Path(tempfile.mkdtemp(prefix='r',dir=ROOT/'build/crosschecks'))
        source=(ROOT/r['source']).read_bytes()
        expanded, closure = prepare(source, r['profile'])
        check_recipe(r, closure)
        (work/'UNIT.C').write_bytes(expanded.replace(b'\r\n',b'\n').replace(b'\n',b'\r\n'))
        tc=(ROOT/config['directory']).resolve()
        batch=['@echo off','D:\\CL.EXE /c '+' '.join(config['flags'])+' UNIT.C > COMP.LOG',
               'if errorlevel 1 goto failed','echo 0 > RESULT.TXT','goto done',':failed','echo 1 > RESULT.TXT',':done','exit']
        (work/'RUN.BAT').write_bytes(('\r\n'.join(batch)+'\r\n').encode('ascii'))
        conf=work/'dosbox.conf'
        lines=['[sdl]','output=surface','[cpu]','cycles=max','[mixer]','nosound=true','[autoexec]',
               f'mount c "{work}"',f'mount d "{tc}"','c:','set PATH=D:\\','set TEMP=C:\\','RUN.BAT']
        conf.write_text('\n'.join(lines)+'\n')
        env=os.environ.copy();env.update(SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy')
        startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
        cmd=[str(runner),'-conf',str(conf),'-fastlaunch','-exit']
        result=subprocess.run(cmd,cwd=work,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,
                              timeout=45,creationflags=subprocess.CREATE_NO_WINDOW,startupinfo=startup)
        require(result.returncode==0 and (work/'RESULT.TXT').is_file() and (work/'RESULT.TXT').read_text().strip()=='0','Independent DOS compilation failed')
        obj=read_object((work/'UNIT.OBJ').read_bytes())
        require(prepare(source, r['profile'])[1] == closure,
                'Independent preprocessor closure changed during compilation')
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
        rows.append({'task':r['id'],'source':identity(source),'object':identity((work/'UNIT.OBJ').read_bytes()),
                     'preprocessor_closure':closure,
                     'payload':identity(payload),'binding':binding,'exact':True,'command':cmd,'dos_command':batch[1]})
    require(inputs()==before,'Inputs changed during independent compilation')
    require(verify(write=False)[2]==oracle[2],'Oracle changed during independent compilation')
    for profile in {read_json(path)['profile'] for path in active}:
        verify_toolchain(profile)
    require(identity(runner.read_bytes())==runner_identity,'Independent runner changed during compilation')
    write_json(report_path,{'runner':{'path':str(runner),**runner_identity},'inputs':before,'results':rows})
    print('PASS: independent DOSBox-X exact code and complete binding obligations for',len(rows),'functions')

if __name__=='__main__':
    from transaction import exclusive, ensure_consistent
    with exclusive():
        ensure_consistent()
        main()
