from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/mmgr_get_chunk_size').resolve(); trial=case/'trial-03'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-03.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':3,'analysis_level':'far-pointer representation and parameter access strategy','transition_reason':'Trial 02 verified the far-function ABI: output changed 84 to 72 bytes, but a long-cast shift introduced stack temporaries and the target expects a direct argument word load. Test the equivalent union view from trial 01 under the corrected far declaration to isolate representation lowering.','prediction':'A union view of the far pointer should access its segment word directly at BP+8 with fewer stack spills than the long cast, while retaining far RETF, resptr1/resptr2 OMF fixups, and fatal_error call.','falsifier':'No simplification or direct BP+8 load; remaining code/fixups show external binding or shared-tail TU context still block strict identity.','discriminating_question':'Can declaration representation remove the cast-generated spill and expose how much remaining mismatch is ABI versus linker/TU structure?','prior_effective_output':'trial-02: 72-byte output d77985402126676899c942bdd668bc4da3bd72085e5176181cb3ffa3664f9217'})
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
print(json.dumps({'receipt':r,'strict':comp,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':code.hex(),'segments':{k:identity(v) for k,v in obj.segments.items()},'compact':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-03'))
