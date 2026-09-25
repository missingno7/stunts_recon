"""Bind complete pinned runtime members, retaining every declaration and fixup."""
from common import ROOT, read_json, require, identity, sha
from compiler import verify_toolchain
from omf import OmfReader
from object_probe import read_object


def bind_library(owner, image, relocations, *, manifest=None, trail=()):
    config,_=verify_toolchain(owner['profile'])
    path=ROOT/owner['library']
    require(any(p['path']==owner['library'] and p['sha256']==owner['library_sha256'] for p in config['files']), 'Library is not pinned by compiler profile')
    data=path.read_bytes()
    require(sha(data)==owner['library_sha256'],'Library identity mismatch')
    modules=OmfReader().split_library(data)
    matches=[blob for name,blob in modules if name==owner['module'] and sha(blob)==owner['module_sha256']]
    require(len(matches)==1,'Pinned library module missing/ambiguous')
    obj=read_object(matches[0], ledata_policy=owner.get('ledata_policy'))
    require(obj.publics==owner['publics'],'Library public layout changed')
    require(obj.externals==owner['externals'],'Library external declarations changed')
    if owner.get('binding'):
        from runtime_binding import bind_member
        manifest = manifest if manifest is not None else read_json(ROOT/'layout/manifest.json')
        require(owner['id'] not in trail, 'Cyclic runtime member binding')
        payload, proof = bind_member(owner, obj, image, relocations, manifest, (*trail,owner['id']))
        return payload, {'library':owner['library'], 'module':owner['module'],
                         'module_sha256':sha(matches[0]), 'segment':owner['segment'],
                         'payload':identity(payload), 'binding':proof,
                         'ledata_policy':owner.get('ledata_policy')}
    require(not obj.linker_fixups,'Library contribution needs unsupported binder')
    require(not owner.get('expected_fixups') and not owner.get('expected_relocations'),
            'No-fixup runtime owner has unexpected binding obligations')
    start,end=owner['start'],owner['end']
    require(not any(start-1<=r['load_offset']<end for r in relocations),'Library has unexpected MZ relocation')
    payload=obj.segment_bytes(owner['segment'])
    require(len(payload)==obj.segment_length(owner['segment'])==end-start,'Library contribution length differs')
    require(all(name==owner['segment'] or size==0 for name,size in obj.segment_lengths.items()),'Unowned library data/BSS')
    require(payload==image[start:end],'Library contribution differs from oracle')
    return payload, {'library':owner['library'],'module':owner['module'],'module_sha256':sha(matches[0]),'segment':owner['segment'],'payload':identity(payload),
                     'ledata_policy':owner.get('ledata_policy')}
