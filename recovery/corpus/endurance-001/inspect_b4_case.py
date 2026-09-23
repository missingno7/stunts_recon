"""Read-only isolated OMF B4 investigation; production parser and diagnostics stay unchanged."""
import json, struct, sys, shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'tools'))
from common import identity, read_json, write_json
from omf import OmfReader
from diagnostics import diagnose
from mz import MZ
from oracle import verify
case_name, trial_number=sys.argv[1:3]
case=ROOT/'recovery/corpus/endurance-001'/case_name
trial=case/'trials'/trial_number
trial_record=read_json(trial/'trial.json')
raw=(trial/'object.obj').read_bytes()
records=[]; at=0
names={0x80:'THEADR',0x88:'COMENT',0x8c:'EXTDEF',0x96:'LNAMES',0x98:'SEGDEF',0x9a:'GRPDEF',0x9c:'FIXUPP',0xa0:'LEDATA',0xb4:'LEXTDEF',0xb6:'LPUBDEF',0x90:'PUBDEF',0x8a:'MODEND'}
while at<len(raw):
    kind=raw[at]; n=struct.unpack_from('<H',raw,at+1)[0]; end=at+3+n; body=raw[at+3:end-1]
    records.append({'offset':at,'type':f'0x{kind:02x}','name':names.get(kind,'unknown'),'length':n,'body_sha256':identity(body)['sha256'],'body_hex':body.hex() if kind in (0xb4,0xb6) else None,'checksum_ok':(raw[end-1]==0 or sum(raw[at:end])&255==0)})
    at=end
obj=OmfReader().read(raw,label=trial_record.get('function','research'))
card=read_json(ROOT/'recovery/cards'/f"{trial_record['case_id']}.json")
verified=verify(write=False)[1]; image=MZ.parse(verified).load_image(verified)
ev=card['evidence']; target=image[ev['start']:ev['end']]
code=obj.segment_bytes('UNIT_TEXT'); diag=diagnose(target,code,trial_record['receipt'],obj.linker_fixups,segment='UNIT_TEXT')
full=diag.get('full_diagnostic'); full_rel=None
if full and Path(full).is_file():
    out=trial/f'research-full-diagnostic.json'; shutil.copyfile(full,out); full_rel=str(out.relative_to(ROOT)).replace('\\','/')
full_doc=read_json(trial/'research-full-diagnostic.json') if full_rel else {}
result={
 'scope':'Existing generic OmfReader and frozen diagnostics used for research only; strict object_probe/acceptance was not changed or bypassed for promotion.',
 'case':case_name,'trial':trial_number,'strict_reader_result':trial_record.get('error'),
 'raw_object':identity(raw),'record_inventory':records,'segment_definitions':obj.segment_defs,'segment_lengths':obj.segment_lengths,
 'segments':{k:identity(v) for k,v in obj.segments.items()},'externals_with_LEXTDEF_and_EXTDEF_merged':obj.externals,
 'publics_including_LPUBDEF':obj.publics,'groups':obj.groups,'fixups':obj.linker_fixups,
 'target':identity(target),'complete_contribution':identity(code),'complete_extent_equal':len(code)==len(target),'complete_bytes_equal':code==target,
 'diagnostic':{k:v for k,v in diag.items() if k!='full_diagnostic'},'full_diagnostic_path':full_rel,
 'diagnostic_engine_sha256':full_doc.get('engine_sha256')}
write_json(trial/'research-omf-inspection.json',result)
print(json.dumps({k:result[k] for k in ('raw_object','record_inventory','segment_lengths','externals_with_LEXTDEF_and_EXTDEF_merged','publics_including_LPUBDEF','fixups','target','complete_contribution','complete_extent_equal','complete_bytes_equal','full_diagnostic_path','diagnostic_engine_sha256')},indent=2))
