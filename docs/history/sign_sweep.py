import pathlib,subprocess,sys,json,hashlib
root=pathlib.Path('D:/prog/stunts_recon');sys.path.insert(0,str(root/'tools'));from omf import OmfReader
work=root/'build/toolchain-research';(work/'SIGN.C').write_text('int sign(int x) { if(x==0) return 0; if(x>0) return 1; return -1; }\n')
target=(root/'build/oracle/load-image.bin').read_bytes()[0x9de8:0x9de8+23]; print('TARGET',target.hex());results=[]
for ver in ['msc510','msc500']:
 tc=root/'toolchain'/ver;env={'PATH':str(tc),'MSDOS_PATH':str(tc),'TEMP':'.','TMP':'.','MSDOS_TEMP':'.'}
 for driver in ['CL.EXE','QCL.EXE']:
  for opt in ['/Od','/O','/Ox','/Os']:
   if driver=='QCL.EXE' and opt=='/Os':continue
   for gs in [[],['/Gs']]:
    flags=['/c','/AM',opt]+gs;cmd=['C:/tools/msdos/msdos.exe','-e','-v5.00',str(tc/driver)]+flags+['SIGN.C'];p=work/'SIGN.OBJ';p.unlink(missing_ok=True)
    try:
     r=subprocess.run(cmd,cwd=work,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=30,creationflags=subprocess.CREATE_NO_WINDOW)
     row={'version':ver,'driver':driver,'flags':flags,'rc':r.returncode,'log':r.stdout.decode('cp437')}
     if p.exists():
      m=OmfReader().read_file(p);code=bytes(next(v for k,v in m.segments.items() if k.endswith('TEXT')));row.update(code=code.hex(),exact=code==target,fixups=m.fixups,externals=m.externals);saved=work/(ver+'-'+driver[:2]+'-'+opt[1:]+('-Gs' if gs else '')+'.obj');saved.write_bytes(p.read_bytes())
     results.append(row);print(ver,driver,flags,row.get('code'),row.get('exact'),r.returncode,flush=True)
    except Exception as e:results.append({'version':ver,'driver':driver,'flags':flags,'error':str(e)});print(e,flush=True)
(work/'sign-results.json').write_text(json.dumps({'target':target.hex(),'results':results},indent=2))
