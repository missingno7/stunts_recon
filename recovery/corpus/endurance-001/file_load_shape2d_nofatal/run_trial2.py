from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/file_load_shape2d_nofatal').resolve(); trial=case/'trial-02'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-02.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':2,'analysis_level':'same-TU callee definition and near/far call lowering','transition_reason':'Trial 01 showed that an unresolved far callee produces pointer32 CALL; trial 02 explicitly near produced rel16 CALL but no PUSH CS. The pristine source defines the far callee in the same C file immediately after the wrapper. This is a distinct TU-context hypothesis, not another prototype spelling.','prediction':'With a far callee definition after the wrapper in the same translation unit, MSC may lower the call as PUSH CS plus near relative CALL and resolve the displacement from emitted function layout. The wrapper may become exact 18 bytes; whole object will also contain the stub callee and cannot be an acceptance object.','falsifier':'Same-TU placement still emits pointer32 or bare near call, wrapper code differs in bytes/extent, or function-distance behavior is not transformed.','discriminating_question':'Does exact target call lowering depend on same-TU placement of the far callee, and can isolated source reproduce the wrapper contribution without unresolved fixups?'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None): p=trial/'compiler-work'; p.mkdir(); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,r=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e: write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
w=Path(r['work_directory']); shutil.copy2(w/'compiler.log',trial/'compiler.log'); shutil.copy2(w/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(w/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',r)
code=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(code); write_json(trial/'object-semantics.json',{'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(code)})
t=bytes.fromhex('558bec2bc050ff76060ee8050083c4045dcb'); (trial/'target.bin').write_bytes(t); c={'whole_code_extent_equal':len(code)==len(t),'whole_code_bytes_equal':code==t,'fixup_free':not obj.linker_fixups,'target':identity(t),'full_emitted_code':identity(code)}; write_json(trial/'strict-comparison.json',c)
# Public-bounded wrapper diagnosis; full complete object/code remains archived above.
pub=next(x for x in obj.publics if x['name']=='_file_load_shape2d_nofatal'); nxt=min((x['offset'] for x in obj.publics if x['segment']==pub['segment'] and x['offset']>pub['offset']),default=len(code)); wrapper=code[pub['offset']:nxt]; (trial/'wrapper-contribution.bin').write_bytes(wrapper); c['wrapper_range']=[pub['offset'],nxt]; c['wrapper_extent_equal']=len(wrapper)==len(t); c['wrapper_bytes_equal']=wrapper==t; write_json(trial/'strict-comparison.json',c)
d=diagnose(t,code,r,obj.linker_fixups); f=Path(d.get('full_diagnostic',''))
if f.is_file(): shutil.copy2(f,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
wd=diagnose(t,wrapper,r,[]); wf=Path(wd.get('full_diagnostic',''))
if wf.is_file(): shutil.copy2(wf,trial/'wrapper-full-diagnostic.json'); wd['full_diagnostic']=str(trial/'wrapper-full-diagnostic.json')
write_json(trial/'wrapper-diagnosis.json',wd)
print(json.dumps({'receipt':r,'whole_code':c,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'wrapper_hex':wrapper.hex(),'full_code_hex':code.hex(),'wrapper_summary':wd.get('match_summary'),'full_summary':d.get('match_summary')},indent=2)); print(format_summary(wd.get('match_summary'),'wrapper contribution'))
