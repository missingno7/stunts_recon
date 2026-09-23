from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools'); import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/rect_adjust_from_point').resolve(); trial=case/'trial-02'; trial.mkdir(parents=True,exist_ok=True); src=case/'candidate-02.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':2,'analysis_level':'local lifetime and register allocation','transition_reason':'Trial 01 corrected far function distance but reproduced the known 84-byte output. Diagnostic mapping showed target caches both point coordinates in SI/DI and stores temp at BP-6, while the candidate rereads fields, uses AX instead of DI for y, and uses BP-2. Promote px/py into explicit locals to test whether lifetime lets MSC retain them in SI/DI and reduce re-reads.','prediction':'The compiler should load pt->px/py once, reuse the values through four bounds checks, and may preserve SI/DI as in target; output should shrink/rearrange toward 76 bytes.','falsifier':'Compiler continues repeated field loads, fails to use a second preserved register, or output remains materially different/greater than 76 bytes. A remaining mismatch then needs source/TU/register-allocation evidence rather than another spelling variant.','discriminating_question':'Does explicitly extending coordinate lifetimes explain the target?s SI/DI cache and stack slot layout?','prior_output':'trial-01 code size 84 / hash 1d41e7d6603f6356bbcbdb917ae8d294a55f56307a658af085b43b91350b39a2'})
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
print(json.dumps({'receipt':r,'strict':c,'object':identity((w/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code_hex':code.hex(),'summary':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-02'))
