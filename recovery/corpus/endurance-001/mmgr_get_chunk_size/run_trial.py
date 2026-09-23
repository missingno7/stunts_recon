from pathlib import Path
import sys, shutil, json
sys.path.insert(0,'tools')
import compiler
from common import identity,write_json
from diagnostics import diagnose,format_summary
case=Path('recovery/corpus/endurance-001/mmgr_get_chunk_size').resolve(); trial=case/'trial-01'; trial.mkdir(parents=True,exist_ok=True)
src=case/'candidate-01.c'; source=src.read_bytes()
write_json(trial/'prediction.json',{'iteration':1,'analysis_level':'symbol/type mapping and isolated OMF binding facts','prediction':'Declare the verified MEMCHUNK layout and near pointer globals as unresolved externals; extract the segment word from the far char* argument. The compiler should emit exact object declarations/fixups naming those symbols. Struct field offsets 0x0c/0x0e follow the 18-byte packed record evidence. A call or helper may be emitted for the adjacent fatal path.','falsifier':'Compiler rejects far-pointer/union source, declares wrong global types/fields, or emits no relocations for the external pointer globals.','discriminating_question':'Can isolated C express the symbol/field accesses and expose the exact object-level binding gate independently from missing absolute DGROUP addresses and cross-function tail jump?'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None):
 p=trial/'compiler-work'; p.mkdir(parents=True,exist_ok=False); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,receipt=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e:
 write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
work=Path(receipt['work_directory']); shutil.copy2(work/'compiler.log',trial/'compiler.log'); shutil.copy2(work/'UNIT.OBJ',trial/'UNIT.OBJ'); shutil.copy2(work/'UNIT.C',trial/'staged-source.C'); write_json(trial/'receipt.json',receipt)
seg=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(seg)
write_json(trial/'object-semantics.json',{'object':identity((work/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(seg)})
target=bytes.fromhex('558bec56578b46088b36a24b3b36a04b740a3b440e740883ee12ebf0e9fdfd8b440c5f5e5dcb'); (trial/'target.bin').write_bytes(target)
strict={'extent_equal':len(seg)==len(target),'bytes_equal':seg==target,'fixup_free':not obj.linker_fixups,'target':identity(target),'candidate':identity(seg)}; write_json(trial/'strict-comparison.json',strict)
d=diagnose(target,seg,receipt,obj.linker_fixups); full=Path(d.get('full_diagnostic',''))
if full.is_file(): shutil.copy2(full,trial/'full-diagnostic.json'); d['full_diagnostic']=str(trial/'full-diagnostic.json')
write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':receipt,'strict':strict,'object':json.loads((trial/'object-semantics.json').read_text()),'compact':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'trial-01'))
