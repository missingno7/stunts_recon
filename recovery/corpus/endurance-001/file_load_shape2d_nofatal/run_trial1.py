from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/file_load_shape2d_nofatal').resolve(); trial=case/'trial-01'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-01.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':1,'analysis_level':'cross-contribution near CALL and far wrapper ABI','prediction':'Under pinned medium model, the wrapper should export far and CALL a near external callee. Its object should show CALL rel16 semantics plus PUSH CS, equivalent to target order. A self-relative/external fixup should identify the unresolved same-code-segment/link/TU gate; the source argument is near char* and far pointer return.','falsifier':'Compiler chooses far pointer32 call, mismatches the wrapper stack/return sequence, or rejects/exposes another required calling convention.','discriminating_question':'Can Luna isolate the wrapper and make MSC emit target-style PUSH CS plus near CALL, and what exact OMF relationship remains to link it?'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None): p=trial/'compiler-work'; p.mkdir(); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,r=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e: write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
w=Path(r['work_directory']); shutil.copy2(w/'compiler.log',trial/'compiler.log'); shutil.copy2(w/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(w/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',r)
code=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(code); write_json(trial/'object-semantics.json',{'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(code)})
t=bytes.fromhex('558bec2bc050ff76060ee8050083c4045dcb'); (trial/'target.bin').write_bytes(t); c={'extent_equal':len(code)==len(t),'bytes_equal':code==t,'fixup_free':not obj.linker_fixups,'target':identity(t),'candidate':identity(code)}; write_json(trial/'strict-comparison.json',c)
d=diagnose(t,code,r,obj.linker_fixups); f=Path(d.get('full_diagnostic',''))
if f.is_file(): shutil.copy2(f,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':r,'strict':c,'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code_hex':code.hex(),'summary':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-01'))
