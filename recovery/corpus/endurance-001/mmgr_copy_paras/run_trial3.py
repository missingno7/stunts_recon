from pathlib import Path
import sys, shutil, json, hashlib
sys.path.insert(0,'tools')
import compiler
from common import identity, write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/mmgr_copy_paras').resolve(); trial=case/'trial-03'; trial.mkdir(parents=True,exist_ok=True)
src=case/'candidate-03.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':3,'analysis_level':'integer width/signedness and chunk-boundary CFG','transition_reason':'Trial 02 showed signed short comparison lowered to JNS and a 108-byte scalar loop. The pristine target subtracts 0x1000 then uses JAE, indicating an unsigned carry test around the chunk boundary. Test a uint16-like threshold formulation explicitly.','prediction':'Using unsigned short paras and an explicit >=0x1000 chunk test should produce a carry/unsigned branch closer to target and simplify signed final-chunk logic; it will not necessarily recover REP MOVSW because MSC emitted scalar far-memory stores in trial 02.','falsifier':'Disassembly retains signed JNS semantics or has no unsigned threshold branch; unchanged effective output, larger mismatch, or unresolved far-pointer semantics rules the hypothesis out.','discriminating_question':'Was the observed JNS/target JAE gap caused by source signedness and threshold formulation?','prior_effective_output':'9144a7cebcc5b7cd3619da6a64bfadc433258104119f28308715531ca217687b'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None):
 p=trial/'compiler-work'; p.mkdir(parents=True,exist_ok=False); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,receipt=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e:
 write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
work=Path(receipt['work_directory']); shutil.copy2(work/'compiler.log',trial/'compiler.log'); shutil.copy2(work/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(work/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',receipt)
seg=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(seg); write_json(trial/'object-semantics.json',{'object':identity((work/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(seg)})
target=bytes.fromhex('558bec1e5657fc8e5e068e46088b5e0a0bdb742fb900808bc381eb0010730e81c30010d1e3d1e3d1e38bcb33db33f633fff3a58cd80500108ed88cc00500108ec0ebcd5f5e1f5dcb'); (trial/'target.bin').write_bytes(target)
strict={'extent_equal':len(seg)==len(target),'bytes_equal':seg==target,'fixup_free':not obj.linker_fixups,'target':identity(target),'candidate':identity(seg)}; write_json(trial/'strict-comparison.json',strict)
d=diagnose(target,seg,receipt,obj.linker_fixups); full=Path(d.get('full_diagnostic','')); 
if full.is_file(): shutil.copy2(full,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':receipt,'strict':strict,'segment_defs':obj.segment_defs,'externals':obj.externals,'fixups':obj.linker_fixups,'compact':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-03'))
