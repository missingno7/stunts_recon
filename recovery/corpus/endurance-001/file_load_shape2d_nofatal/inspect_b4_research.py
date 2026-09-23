"""One-case, read-only exploration of the strict OMF gate; no production code changes."""
import json, struct, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[4]
sys.path.insert(0,str(ROOT/'tools'))
from common import identity, read_json, write_json
from omf import OmfReader
from diagnostics import diagnose
from mz import MZ
from oracle import verify
case=ROOT/'recovery/corpus/endurance-001/file_load_shape2d_nofatal'
trial_number=(sys.argv[1] if len(sys.argv)>1 else '001')
trial=case/'trials'/trial_number
raw=(trial/'object.obj').read_bytes()
record_rows=[]
at=0
while at<len(raw):
    kind=raw[at]; length=struct.unpack_from('<H',raw,at+1)[0]
    end=at+3+length
    body=raw[at+3:end-1]
    record_rows.append({'offset':at,'type':f'0x{kind:02x}','name':{0x80:'THEADR',0x88:'COMENT',0x8c:'EXTDEF',0x96:'LNAMES',0x98:'SEGDEF',0x9a:'GRPDEF',0x9c:'FIXUPP',0xa0:'LEDATA',0xb4:'LEXTDEF',0x90:'PUBDEF',0x8a:'MODEND'}.get(kind,'unknown'),'length':length,'body_sha256':identity(body)['sha256'],'body_hex':body.hex() if kind==0xb4 else None,'checksum_ok':(raw[at:end-1][-1:]==b'\0' or sum(raw[at:end])&255==0)})
    at=end
obj=OmfReader().read(raw,label='nofatal_same_tu_H3')
source_card=read_json(ROOT/'recovery/cards/load_2a9ea.json')
verified=verify(write=False)[1]
image=MZ.parse(verified).load_image(verified)
ev=source_card['evidence']; target=image[ev['start']:ev['end']]
rec=read_json(trial/'trial.json')['receipt']
code=obj.segment_bytes('UNIT_TEXT')
diag=diagnose(target,code,rec,obj.linker_fixups,segment='UNIT_TEXT')
full=diag.get('full_diagnostic')
if full:
    full_path=Path(full)
    if full_path.exists():
        import shutil
        shutil.copyfile(full_path,trial/'research-full-diagnostic.json')
summary={
 'purpose':'Read-only research: establish whether generic existing OmfReader can inspect an object that strict object_probe rejects at LEXTDEF/B4. This does not change production parser/tool behavior or acceptance.',
 'trial':str((trial/'trial.json').relative_to(ROOT)).replace('\\','/'),
 'strict_status':'object_probe rejected LEXTDEF record 0xB4 before generic object interpretation; do not treat this as a compiler failure.',
 'raw_object':identity(raw),
 'record_inventory':record_rows,
 'generic_reader':'tools/omf.py OmfReader existing implementation; parsed without modifying tools.',
 'module_name':getattr(obj,'module_name',None),
 'segment_definitions':obj.segment_defs,
 'segment_lengths':obj.segment_lengths,
 'segments':{name:identity(data) for name,data in obj.segments.items()},
 'externals':obj.externals,
 'publics':obj.publics,
 'groups':obj.groups,
 'fixups':obj.linker_fixups,
 'target':identity(target),
 'complete_contribution_code':identity(code),
 'complete_contribution_byte_equal':code==target,
 'complete_contribution_extent_equal':len(code)==len(target),
 'frozen_diagnostic':{k:v for k,v in diag.items() if k!='full_diagnostic'},
 'full_diagnostic_path':f'recovery/corpus/endurance-001/file_load_shape2d_nofatal/trials/{trial_number}/research-full-diagnostic.json' if (trial/'research-full-diagnostic.json').exists() else None,
 'engine_sha256':(json.loads((trial/'research-full-diagnostic.json').read_text(encoding='utf-8')).get('engine_sha256') if (trial/'research-full-diagnostic.json').exists() else diag.get('engine_sha256'))
}
write_json(trial/'research-omf-inspection.json',summary)
print(json.dumps({k:summary[k] for k in ['raw_object','record_inventory','externals','publics','segment_lengths','fixups','target','complete_contribution_code','complete_contribution_extent_equal','complete_contribution_byte_equal','full_diagnostic_path','engine_sha256']},indent=2,default=str))


