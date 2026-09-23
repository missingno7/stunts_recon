import pathlib,subprocess,os,json,hashlib,sys
root=pathlib.Path('D:/prog/stunts_recon');work=root/'build/toolchain-research';tc=root/'toolchain/msc510';conf=work/'dosbox.conf';(work/'REF.C').write_bytes((work/'SIGN.C').read_bytes())
conf.write_text('[sdl]\noutput=surface\n[render]\nframeskip=10\n[cpu]\ncycles=max\n[mixer]\nnosound=true\n[autoexec]\nmount c "'+str(work)+'"\nmount d "'+str(tc)+'"\nc:\nset PATH=D:\\nset TEMP=C:\\nD:\\CL.EXE /c /AM /O /Gs REF.C > REF.LOG\nexit\n')
# Correct DOS trailing backslash versus newlines explicitly.
a=['[sdl]','output=surface','[cpu]','cycles=max','[mixer]','nosound=true','[autoexec]',f'mount c "{work}"',f'mount d "{tc}"','c:','set PATH=D:\\','set TEMP=C:\\','D:\\CL.EXE /c /AM /O /Gs REF.C > REF.LOG','exit'];conf.write_text('\n'.join(a)+'\n')
env=os.environ.copy();env['SDL_VIDEODRIVER']='dummy';env['SDL_AUDIODRIVER']='dummy';si=subprocess.STARTUPINFO();si.dwFlags|=subprocess.STARTF_USESHOWWINDOW;si.wShowWindow=0
try:
 r=subprocess.run(['C:/DOSBox-X/dosbox-x.exe','-conf',str(conf),'-fastlaunch','-exit'],cwd=work,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW,startupinfo=si)
 print('RC',r.returncode,'LOG',(work/'REF.LOG').read_text(errors='replace') if (work/'REF.LOG').exists() else r.stdout.decode(errors='replace'))
 if (work/'REF.OBJ').exists():
  sys.path.insert(0,str(root/'tools'));from omf import OmfReader
  a=OmfReader().read_file(work/'REF.OBJ');b=OmfReader().read_file(work/'msc510-CL-O-Gs.obj');aa=next(bytes(v) for k,v in a.segments.items() if k.endswith('TEXT'));bb=next(bytes(v) for k,v in b.segments.items() if k.endswith('TEXT'));print('code parity',aa==bb,aa.hex());(work/'dosbox-parity.json').write_text(json.dumps({'runner':'DOSBox-X 2024.10.01','flags':['/c','/AM','/O','/Gs'],'code_equal':aa==bb,'code':aa.hex(),'fixups_equal':a.fixups==b.fixups},indent=2))
except Exception as e:print(type(e).__name__,str(e))
