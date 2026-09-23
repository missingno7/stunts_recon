from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/rect_adjust_from_point').resolve(); trial=case/'trial-01'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-01.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':1,'analysis_level':'function ABI plus local/register allocation baseline','prediction':'The pristine function is a far procedure but its two struct pointers are near DS pointers. Marking only the function far while preserving int fields and shared temp should correct RETF/frame assumptions and reveal whether the 84-versus-76 mismatch is just distance or remains local allocation/codegen. Target saves DI/SI, uses one 2-byte local at BP-6, and reserves six bytes.','falsifier':'Compiler emits non-76 extent or different full bytes; far qualification alone fails to explain the archived 84-byte Restunts mismatch.','discriminating_question':'Does matching the target function distance resolve the known extent gap before testing local lifetime/type choices?','target_source':'Restunts math.c rect_adjust_from_point with only far function distance added.'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None): p=trial/'compiler-work'; p.mkdir(); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,r=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e: write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
w=Path(r['work_directory']); shutil.copy2(w/'compiler.log',trial/'compiler.log'); shutil.copy2(w/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(w/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',r)
code=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(code); write_json(trial/'object-semantics.json',{'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(code)})
t=bytes.fromhex('558bec83ec0657568b5e068b378b7f028b5e0839377e0289378d44018946fa8b5e083947027d038947028b5e08397f047e03897f048d45018946fa8b5e083947067d038947065e5f8be55dcb'); (trial/'target.bin').write_bytes(t); c={'extent_equal':len(code)==len(t),'bytes_equal':code==t,'fixup_free':not obj.linker_fixups,'target':identity(t),'candidate':identity(code)}; write_json(trial/'strict-comparison.json',c)
d=diagnose(t,code,r,obj.linker_fixups); f=Path(d.get('full_diagnostic',''))
if f.is_file(): shutil.copy2(f,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':r,'strict':c,'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code_hex':code.hex(),'summary':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-01'))
