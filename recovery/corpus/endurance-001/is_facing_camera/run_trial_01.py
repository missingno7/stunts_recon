from pathlib import Path
import sys,shutil,json
sys.path.insert(0,'tools')
import compiler
from common import read_json,identity,write_json
from oracle import verify
from mz import MZ
from diagnostics import diagnose,format_summary
root=Path('.').resolve(); case=root/'recovery/corpus/endurance-001/is_facing_camera'; out=case/'trials'/'001'; out.mkdir(parents=True,exist_ok=False)
src=case/'candidates'/'001.c'; pred=case/'predictions'/'001.json'; source=src.read_bytes(); prediction=read_json(pred)
recipe=read_json(root/'recipes/is_facing_camera.json'); verified=verify(write=False); image=MZ.parse(verified[1]).load_image(verified[1]); target=image[recipe['start']:recipe['end']]
assert identity(target)==recipe['target'],'Target recipe/oracle identity changed'
(out/'candidate.c').write_bytes(source); (out/'target.bin').write_bytes(target); write_json(out/'prediction.json',prediction)
real=compiler.tempfile.mkdtemp
def mkdtemp(prefix='p',dir=None):
 p=out/'compiler-work'; p.mkdir(parents=True,exist_ok=False); return str(p)
compiler.tempfile.mkdtemp=mkdtemp
try: obj,receipt=compiler.compile_source(source,recipe['profile'])
finally: compiler.tempfile.mkdtemp=real
work=Path(receipt['work_directory']); shutil.copy2(work/'compiler.log',out/'compiler.log'); shutil.copy2(work/'UNIT.OBJ',out/'UNIT.OBJ'); shutil.copy2(work/'UNIT.C',out/'staged-source.C'); write_json(out/'receipt.json',receipt)
seg=obj.segment_bytes(recipe['object_segment']); (out/'code.bin').write_bytes(seg)
sem={'object':identity((work/'UNIT.OBJ').read_bytes()),'segments':{k:identity(v) for k,v in obj.segments.items()},'segment_defs':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals,'fixups':obj.linker_fixups,'code':identity(seg)}; write_json(out/'object-semantics.json',sem)
d=diagnose(target,seg,receipt,obj.linker_fixups,segment=recipe['object_segment']); full=Path(d.get('full_diagnostic',''))
if full.is_file(): shutil.copy2(full,out/'full-diagnostic.json'); d['full_diagnostic']=str(out/'full-diagnostic.json')
strict={'extent_equal':len(seg)==len(target),'bytes_equal':seg==target,'code_identity':identity(seg),'target_identity':identity(target),'object_identity':sem['object'],'fixups_equal_target_contract':obj.linker_fixups==recipe['expected_fixups'],'fixups':obj.linker_fixups}
write_json(out/'diagnosis.json',d); write_json(out/'strict-comparison.json',strict)
print(json.dumps({'strict':strict,'effective_code':receipt.get('effective_code'),'diagnosis':d.get('match_summary')},indent=2)); print(format_summary(d.get('match_summary'),'camera hypothesis 4'))
