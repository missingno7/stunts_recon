from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/mmgr_get_chunk_size').resolve(); trial=case/'trial-02'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-02.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':2,'analysis_level':'ABI/declaration and far-pointer parameter placement','transition_reason':'Trial 01 exposed that the isolated definition was near: its public is near and parameter begins at BP+6, while verified target is a far public, reads the segment at BP+8, and RETF. Correcting declaration distance tests an ABI explanation before changing symbol/binding analysis.','prediction':'Marking mmgr_get_chunk_size far should switch public return to RETF and shift the far pointer parameter from BP+6 to BP+8; it should retain the two named DGROUP external pointer fixups and the fatal_error CALL.','falsifier':'Code remains near/RET, arg access remains BP+6, or far pointer segment extraction does not simplify. Any success on ABI still cannot bind 0x4ba0/0x4ba2 or produce the target shared-code jump.','discriminating_question':'Was the prologue/argument mismatch caused by omitted far function ABI?','prior_output':'trial-01: 84-byte object, effective cc0141218b8351e4f9c7115cd33fcc3f122c5661968c6c8e733ffc0f38a59775; externals _resptr1/_resptr2/_fatal_error and four fixups'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None): p=trial/'compiler-work'; p.mkdir(); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,r=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e: write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
w=Path(r['work_directory']); shutil.copy2(w/'compiler.log',trial/'compiler.log'); shutil.copy2(w/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(w/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',r)
code=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(code); write_json(trial/'object-semantics.json',{'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(code)})
t=bytes.fromhex('558bec56578b46088b36a24b3b36a04b740a3b440e740883ee12ebf0e9fdfd8b440c5f5e5dcb'); (trial/'target.bin').write_bytes(t); comp={'extent_equal':len(code)==len(t),'bytes_equal':code==t,'fixup_free':not obj.linker_fixups,'target':identity(t),'candidate':identity(code)}; write_json(trial/'strict-comparison.json',comp)
d=diagnose(t,code,r,obj.linker_fixups); f=Path(d.get('full_diagnostic',''))
if f.is_file(): shutil.copy2(f,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':r,'strict':comp,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':code.hex(),'segments':{k:identity(v) for k,v in obj.segments.items()},'diagnosis':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-02'))
