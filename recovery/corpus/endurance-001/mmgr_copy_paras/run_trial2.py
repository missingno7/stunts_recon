from pathlib import Path
import sys, shutil, json
sys.path.insert(0, 'tools')
import compiler
from common import identity, write_json
from diagnostics import diagnose, format_summary
case=Path('recovery/corpus/endurance-001/mmgr_copy_paras').resolve(); trial=case/'trial-02'; trial.mkdir(parents=True,exist_ok=True)
src=case/'candidate-01.c'; source=src.read_bytes()
write_json(trial/'prediction.json', {'iteration':2,'analysis_level':'same source; harness recovery after an invocation-level output-directory error','prior_compile_calls':1,'prediction':'With the output directory created by the compiler adapter, the unchanged source should reproduce a full object deterministically. Target machine-level prediction remains the trial-01 statement.','falsifier':'Compiler rejects pure-C closure or emits different effective output on unchanged source.','discriminating_question':'Does removing the include gate expose a compileable far-pointer implementation under pinned MSC 5.1?'})
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None):
 p=Path(dir)/'compiler-work'; p.mkdir(parents=True,exist_ok=False); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try:
 obj,receipt=compiler.compile_source(source,'msc510-medium')
except compiler.CompileFailure as e:
 write_json(trial/'failure.json',{'category':e.category,'message':str(e),'receipt':e.receipt}); raise
finally: compiler.tempfile.mkdtemp=real
work=Path(receipt['work_directory']); shutil.copy2(work/'compiler.log',trial/'compiler.log'); shutil.copy2(work/'UNIT.OBJ',trial/'UNIT.OBJ'); write_json(trial/'receipt.json',receipt)
seg=obj.segment_bytes('UNIT_TEXT'); (trial/'code.bin').write_bytes(seg)
write_json(trial/'object-semantics.json',{'object':identity((work/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(seg)})
target=bytes.fromhex('558bec1e5657fc8e5e068e46088b5e0a0bdb742fb900808bc381eb0010730e81c30010d1e3d1e3d1e38bcb33db33f633fff3a58cd80500108ed88cc00500108ec0ebcd5f5e1f5dcb'); (trial/'target.bin').write_bytes(target)
strict={'extent_equal':len(seg)==len(target),'bytes_equal':seg==target,'fixup_free':not obj.linker_fixups,'target':identity(target),'candidate':identity(seg)}; write_json(trial/'strict-comparison.json',strict)
d=diagnose(target,seg,receipt,obj.linker_fixups); write_json(trial/'diagnosis.json',d)
print(json.dumps({'receipt':receipt,'strict':strict,'object_semantics':obj.segment_defs,'externals':obj.externals,'fixups':obj.linker_fixups,'diagnosis':d},indent=2)); print(format_summary(d.get('match_summary'),'trial-02'))
