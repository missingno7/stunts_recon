"""Bounded relocation of complete pinned runtime members and their storage.

Addresses come from accepted runtime publics or reviewed original data aliases.
Secondary storage stays explicit raw debt and is re-proven on every build.
"""
import struct
from common import ROOT, read_json, require, sha, identity
from data_symbols import resolve_symbols




def _toolchain_path(logical):
    from compiler import toolchain_path
    return toolchain_path(logical)

def declarations(obj):
    return {'segments':obj.segment_defs, 'groups':obj.groups,
            'publics':obj.publics, 'externals':obj.externals,
            'external_scopes':obj.external_scopes}


def runtime_frame(image, relocations, manifest):
    """Original relocated calls to accepted pinned _TEXT publics ground the frame."""
    from library import bind_library
    layout = read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256'] == sha(image), 'Runtime frame oracle differs')
    frames = set()
    for name, symbol in layout['symbols'].items():
        if 'owner' not in symbol: continue
        owners = [o for o in manifest['owners'] if o['id'] == symbol['owner']
                  and o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' and not o.get('expected_fixups')]
        if len(owners) != 1 or owners[0]['segment'] != '_TEXT': continue
        owner = owners[0]
        bind_library(owner, image, relocations, manifest=manifest)
        pubs = [p for p in owner['publics'] if p['name'] == name and p['segment'] == '_TEXT']
        require(len(pubs) == 1, 'Runtime frame public missing/ambiguous')
        address = owner['start'] + pubs[0]['offset']
        require(symbol.get('anchors'), 'Runtime frame has no original call anchor')
        for anchor in symbol['anchors']:
            at = anchor['site']; raw = bytes.fromhex(anchor['hex'])
            require(len(raw) == 5 and raw[0] == 0x9a and image[at:at+5] == raw,
                    'Runtime frame original CALL changed')
            require(anchor['relocation'] in relocations and anchor['relocation']['load_offset'] == at+3,
                    'Runtime frame original CALL relocation missing')
            frame = int.from_bytes(raw[3:5], 'little') * 16
            require(frame == symbol['frame_load_address'] and
                    frame + int.from_bytes(raw[1:3], 'little') == address,
                    'Runtime frame/public conflict')
            frames.add(frame)
    require(len(frames) == 1, 'Runtime _TEXT frame missing/ambiguous')
    return next(iter(frames))


def runtime_public(name, image, relocations, manifest, trail):
    from library import bind_library
    hits = [(o,p) for o in manifest['owners'] if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY'
            for p in o['publics'] if p['name'] == name and p['segment'] == o['segment']]
    require(len(hits) == 1, 'Runtime public missing/ambiguous: '+name)
    owner, public = hits[0]
    require(owner['id'] not in trail, 'Cyclic runtime dependency needs complete group proof: '+name)
    bind_library(owner, image, relocations, manifest=manifest, trail=trail)
    require(0 <= public['offset'] < owner['end']-owner['start'], 'Runtime public outside owner')
    return {'address':owner['start']+public['offset'], 'segment':owner['segment'],
            'owner':owner['id']}


def group_public(name, manifest):
    """Resolve a staged pinned public without recursive descent through cycles.

    Every member in the closed group is independently rebound by the staged
    whole-image build.  Here we recheck the target's pinned declarations so a
    forged public cannot supply an address during the first member's binding.
    """
    from compiler import verify_toolchain
    from omf import OmfReader
    from object_probe import read_object
    # integ36 (L2-rt): an ACCEPTED zero-fixup code member without a binding
    # (e.g. strlen.asm, ultoa.asm) also provides its pinned publics; it must be
    # the unchanged canonical row, a complete _TEXT member with no FIXUPP.
    canonical = {o['id']:o for o in read_json(ROOT/'layout/manifest.json')['owners']}
    hits = [(o,p) for o in manifest['owners'] if o['kind']=='KNOWN_TOOLCHAIN_LIBRARY'
            and (o.get('binding',{}).get('mode') in ('runtime-member-v1','runtime-owner-group-v2') or
                 (not o.get('binding') and o['id'] in canonical and not o.get('expected_fixups') and
                  o.get('segment')=='_TEXT'))
            for p in o['publics'] if p['name']==name and p['segment']!='?0']
    require(len(hits)==1, 'Runtime group public missing/ambiguous: '+name)
    owner,public=hits[0]
    config,_=verify_toolchain(owner['profile'])
    require(any(f['path']==owner['library'] and f['sha256']==owner['library_sha256']
                for f in config['files']), 'Group public archive is not pinned')
    archive=_toolchain_path(owner['library']).read_bytes()
    require(sha(archive)==owner['library_sha256'], 'Group public archive changed')
    matches=[blob for module,blob in OmfReader().split_library(archive)
             if module==owner['module'] and sha(blob)==owner['module_sha256']]
    require(len(matches)==1, 'Group public module missing/ambiguous')
    obj=read_object(matches[0],ledata_policy=owner.get('ledata_policy'),
                    record_policy=owner.get('record_policy'),
                    sparse_zero=owner.get('binding',{}).get('sparse_zero'))
    require(owner['publics']==obj.publics and owner.get('expected_fixups',[])==obj.linker_fixups,
            'Group public source declarations differ')
    if not owner.get('binding'):
        require(owner==canonical[owner['id']] and not obj.linker_fixups and
                obj.segment_length('_TEXT')==owner['end']-owner['start'] and
                all(n=='_TEXT' or not size for n,size in obj.segment_lengths.items()),
                'Accepted zero-fixup group provider is not a complete code member')
    else:
        require(owner['binding']['declarations']==declarations(obj),
                'Group public source declarations differ')
    segment=public['segment']
    if segment==owner['segment']:
        base=owner['start']
    else:
        place=owner['binding'].get('storage',{}).get(segment)
        require(place is not None, 'Group public secondary segment lacks placement')
        base=place['start']
    end=min([p['offset'] for p in obj.publics
             if p['segment']==segment and p['offset']>public['offset']]
            or [obj.segment_length(segment)])
    require(0<=public['offset']<end<=obj.segment_length(segment),
            'Group public has no bounded object extent')
    return {'address':base+public['offset'],'segment':segment,
            'extent':end-public['offset'],'owner':owner['id']}


def reviewed_main_entry(image, relocations, owner, fix):
    """Bind the CRT0 _main call to the reviewed game entry at load offset zero."""
    from function_evidence import reviewed_functions
    layout=read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256']==sha(image), 'Main code-symbol oracle differs')
    symbol=layout['symbols']['_main']
    entry=reviewed_functions(image)['ported_stuntsmain_']
    require(symbol['mapped_target']=={
                'name':'ported_stuntsmain_', 'stable_id':'load_00000',
                'start':0, 'end':1434, 'sha256':entry['sha256']} and
            (entry['start'],entry['end'])==(0,1434) and
            entry['bytes_hex']==image[:1434].hex(),
            'CRT0 _main lacks exact reviewed game entry')
    anchors=symbol['anchors']
    require(len(anchors)==1 and anchors[0]['caller_module']=='dos\\crt0.asm' and
            owner['module']=='dos\\crt0.asm' and owner['start']==117858 and
            fix['segment']=='_TEXT' and fix['offset']==152 and
            fix['target_kind']=='external' and fix['target']=='_main' and
            fix['loc']=='pointer32' and fix['displacement']==0 and
            not fix['self_relative'], 'CRT0 _main source FIXUPP differs')
    anchor=anchors[0]; site=owner['start']+fix['offset']-1
    raw=bytes.fromhex(anchor['hex'])
    require(anchor['site']==site==118009 and raw==bytes.fromhex('9a00000000') and
            image[site:site+5]==raw and
            anchor['relocation'] in relocations and
            anchor['relocation']['load_offset']==site+3 and
            anchor['relocation']['segment']*16+anchor['relocation']['offset']==site+3,
            'CRT0 relocated _main CALL differs')
    return 0


def verify_runtime_common(manifest, image):
    """Reproduce the complete original MSG COMMON writes in link order."""
    from omf import OmfReader
    from object_probe import read_object
    sources=[
        ('dos\\crt0msg.asm',118410,118446,'32b9df2be00084e1bb42d5636bf86fff7565e148073ab1a0a5e26fe7fa244c0c'),
        ('crt0fp.asm',118446,118452,'cf30ca0199881cb3f859a701c8a2b5191f42a4c77a825101ae9a4dbf13387a52'),
        ('chksum.asm',118488,118526,'022580d6eb4620ff1d3c5677e730ffa05bbe616386cef3f4b468146e7dfd80c4'),
        ('dos\\nmsghdr.asm',119038,119124,'fe764ada95f461809b56ee7558ad23c3430123ac5728722e28d0d9de814a4738'),
        ('abort.asm',123846,123880,'c4b70d7f29159d913b1cc9df3689393b23bc9e9b4dd884d5c2fce752c7ad69f8'),
    ]
    archive=_toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
    require(sha(archive)=='a8c6bf65551559a795f3bdcef3f70178086bf944296413716e48cba95814b9cb',
            'COMMON source archive changed')
    modules=OmfReader().split_library(archive)
    pad=bytearray(19); epad=bytearray(1)
    for name,start,end,digest in sources:
        require(any(o['start']<=start and end<=o['end'] and
                    o['kind']=='UNRESOLVED_RAW' or
                    o['start']==start and o['end']==end and
                    o['kind']=='KNOWN_TOOLCHAIN_LIBRARY' and o['module']==name
                    for o in manifest['owners']), 'COMMON source code ownership changed')
        hits=[blob for module,blob in modules if module==name and sha(blob)==digest]
        require(len(hits)==1, 'COMMON module missing')
        obj=read_object(hits[0])
        require(obj.segment_length('_TEXT')==end-start,
                'COMMON source complete CODE extent differs')
        for name, buffer in (('PAD',pad),('EPAD',epad)):
            if obj.segment_length(name):
                require(name in obj.segments and len(obj.segment_bytes(name))==obj.segment_length(name),
                        'Incomplete COMMON contribution')
                buffer[:obj.segment_length(name)]=obj.segment_bytes(name)
    require(bytes(pad)==image[199973:199992] and bytes(epad)==image[199992:199993],
            'Complete ordered runtime COMMON differs from oracle')


def crt0dat_data_public(name, image, relocations):
    """Bind a raw-owned CRT data public from its complete pinned DATA object."""
    from omf import OmfReader
    from object_probe import read_object
    archive=_toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
    require(sha(archive)=='a8c6bf65551559a795f3bdcef3f70178086bf944296413716e48cba95814b9cb',
            'CRT data archive changed')
    blobs=[b for n,b in OmfReader().split_library(archive)
           if n=='dos\\crt0dat.asm' and
           sha(b)=='b7bb3dfd16da5b78b0eb3dc7d409ebfd49028cc0b3f84f7e6d84e4b30b662ea5']
    require(len(blobs)==1, 'Pinned CRT data source missing')
    obj=read_object(blobs[0],sparse_zero={'CDATA':{'initialized_prefix':2,'declared_length':14}})
    base=191948; frame=read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
    require(obj.segment_length('_DATA')==80 and
            [(f['segment'],f['offset'],f['target'],f['loc']) for f in obj.linker_fixups
             if f['segment']=='_DATA']==[('_DATA',66,'_DATA','offset16'),
                                      ('_DATA',68,'DGROUP','base16')],
            'CRT data complete source fixups differ')
    payload=bytearray(obj.segment_bytes('_DATA'))
    struct.pack_into('<H',payload,66,base+70-frame)
    struct.pack_into('<H',payload,68,frame//16)
    require(bytes(payload)==image[base:base+80] and
            [r['load_offset'] for r in relocations if base<=r['load_offset']<base+80]==[base+68],
            'CRT data complete image/fixups differ')
    matches=[p for p in obj.publics if p['name']==name and p['segment'] in ('_DATA','CDATA')]
    require(len(matches)==1, 'CRT data public missing/ambiguous')
    segment=matches[0]['segment']
    if segment=='CDATA':
        base=199708
        require(obj.segment_length('CDATA')==14 and obj.segment_bytes('CDATA')==bytes(14)
                and image[base:base+14]==bytes(14) and
                image[118056+34:118056+36]==bytes.fromhex('b054') and
                frame+int.from_bytes(image[118056+34:118056+36],'little')==base+4,
                'CRT CDATA complete extent/placement differs')
    offset=matches[0]['offset']
    end=min([p['offset'] for p in obj.publics
             if p['segment']==segment and p['offset']>offset] or [obj.segment_length(segment)])
    require(0<=offset<end<=obj.segment_length(segment), 'CRT data public lacks bounded extent')
    return {'address':base+offset,'frame':frame,'extent':end-offset}


def file_data_public(name, image, relocations):
    """Ground stdio DATA and COMMON publics from the pinned _file.c object."""
    from omf import OmfReader
    from data_symbols import checked_dgroup_layout
    import struct as _struct
    archive=_toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
    require(sha(archive)=='a8c6bf65551559a795f3bdcef3f70178086bf944296413716e48cba95814b9cb',
            'stdio data archive changed')
    blobs=[b for n,b in OmfReader().split_library(archive) if n=='_file.c' and
           sha(b)=='96d7effbd74974f0011699fdf45a72e68886eddbb13a43bc2fef94be4dbc8d78']
    require(len(blobs)==1, 'stdio data module missing')
    blob=blobs[0]; filtered=bytearray(); comdefs=[]; at=0
    while at<len(blob):
        require(at+3<=len(blob), 'Truncated stdio OMF record')
        kind,length=blob[at],_struct.unpack_from('<H',blob,at+1)[0]
        end=at+3+length
        require(length>=1 and end<=len(blob) and
                (blob[end-1]==0 or sum(blob[at:end])&255==0),
                'Stdio OMF record/checksum differs')
        if kind==0xb0: comdefs.append(blob[at+3:end-1].hex())
        else: filtered.extend(blob[at:end])
        at=end
    require(comdefs==['075f5f627566696e0062810002085f5f6275666f75740062810002085f5f6275666572720062810002'],
            'Complete stdio COMDEF definitions differ')
    obj=OmfReader().read(bytes(filtered))
    require(obj.segment_length('_DATA')==282 and
            [(p['name'],p['segment'],p['offset']) for p in obj.publics]==[
                ('__iob2','_DATA',160),('__lastiob','_DATA',280),('__iob','_DATA',0)],
            'Stdio DATA publics/extent differ')
    require([(f['segment'],f['offset'],f['loc'],f['target_kind'],f['target'])
             for f in obj.linker_fixups]==[
                ('_DATA',4,'offset16','external','?2'),
                ('_DATA',0,'offset16','external','?2'),
                ('_DATA',280,'offset16','segment','_DATA')],
            'Stdio DATA complete FIXUPP sequence differs')
    base=192064; layout=checked_dgroup_layout(image,relocations)
    frame=layout['frame_load_address']
    source=bytearray(obj.segment_bytes('_DATA'))
    require(len(source)==282 and image.find(source[16:48],frame,layout['bss_start'])==base+16 and
            image.find(source[16:48],base+17,layout['bss_start'])==-1,
            'Stdio DATA unique original anchor differs')
    _struct.pack_into('<H',source,0,215782-frame)
    _struct.pack_into('<H',source,4,215782-frame)
    _struct.pack_into('<H',source,280,base+152-frame)
    require(bytes(source)==image[base:base+282] and
            not any(base<=r['load_offset']<base+282 for r in relocations),
            'Stdio complete initialized DATA differs')
    commons={'__bufin':215782,'__bufout':213134,'__buferr':221650}
    if name in commons:
        address=commons[name]; extent=512
        require(layout['bss_start']<=address and address+extent<=layout['bss_end'] and
                all(a+512<=b or b+512<=a for n,a in commons.items() for m,b in commons.items()
                    if n<m), 'Stdio COMDEF object outside disjoint zeroed BSS')
    else:
        spans={'__iob':(0,160),'__iob2':(160,280),'__lastiob':(280,282)}
        require(name in spans, 'Unknown stdio DATA public')
        lo,hi=spans[name];address=base+lo;extent=hi-lo
    return {'address':address,'frame':frame,'extent':extent}


CRT_ORDER_KIND = 'crt-dgroup-data-order-v1'


def _accepted_neighbour(owner, reference, anchor_kinds):
    """A storage boundary of another ACCEPTED runtime owner (canonical manifest).

    Staged candidates never ground each other's placement: the neighbour must
    already be published, must not be the member under test, and its storage
    row must carry its own independent anchor.
    """
    require(isinstance(reference, dict) and set(reference) == {'owner', 'segment'},
            'Invalid runtime neighbour reference')
    canonical = read_json(ROOT/'layout/manifest.json')['owners']
    rows = [o for o in canonical if o['id'] == reference['owner']]
    require(len(rows) == 1 and rows[0]['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' and
            rows[0]['id'] != owner['id'] and rows[0].get('module') != owner.get('module'),
            'Runtime placement neighbour must be another accepted runtime owner')
    neighbour = rows[0]
    row = neighbour.get('binding', {}).get('storage', {}).get(reference['segment'])
    require(row is not None and row.get('anchor', {}).get('kind') in anchor_kinds and
            type(row.get('start')) is int and type(row.get('end')) is int,
            'Runtime placement neighbour lacks an independently anchored boundary')
    declared = neighbour['binding']['declarations']['segments']
    return neighbour, row, declared


def _inventory_code_entry(name, image):
    """An instruction-verified inventory entry whose name is exactly ``name``."""
    from function_evidence import current_inventory
    rows = [f for f in current_inventory(image)['functions'] if f.get('name') == name and
            f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    require(len(rows) == 1 and sha(image[rows[0]['start']:rows[0]['end']]) == rows[0]['sha256'],
            'Runtime far-pointer target lacks a unique verified inventory entry: ' + name)
    return rows[0]['start']


def crt_order_placements(owner, obj, image, relocations, manifest):
    """Place CRT DATA-class sections from pinned SEGDEF order and a grounded neighbour.

    Inputs are independent of the member's own bytes: an accepted neighbour's
    boundary (the next DGROUP class), the member's pinned SEGDEF order, and the
    exact pinned contributions that lie between.  Every intervening contribution
    must be a complete far-pointer table from a pinned library member whose
    pointers land on verified inventory entries through relocated original words.
    """
    proof = owner['binding'].get('dgroup_order_proof')
    if proof is None:
        return {}
    require(owner['binding'].get('mode') == 'runtime-owner-group-v2' and
            isinstance(proof, dict) and
            set(proof) == {'kind', 'upper_neighbour', 'segments', 'intervening'} and
            proof['kind'] == CRT_ORDER_KIND, 'Invalid runtime DGROUP order proof')
    groups = {g['name']: g for g in obj.groups}
    require('DGROUP' in groups, 'Runtime DGROUP order proof needs DGROUP')
    dgroup = groups['DGROUP']['segments']
    # MSC LINK orders a class by first appearance.  _DATA already exists when
    # the startup members are linked, so every other DATA-class DGROUP segment
    # of this member follows in its own SEGDEF order.
    declared = [s for s in obj.segment_defs
                if s['name'] in dgroup and s['class'] == 'DATA' and s['name'] != '_DATA']
    require([s['name'] for s in declared] == proof['segments'] and declared,
            'Runtime DATA-class section order differs from pinned SEGDEF order')
    require(all(s['alignment'] == 'word' and s['combine'] in ('public', 'common') and
                obj.segment_length(s['name']) % 2 == 0 for s in declared),
            'Runtime DATA-class section alignment/combine differs')
    _, row, neighbour_declared = _accepted_neighbour(owner, proof['upper_neighbour'],
                                                     ('unique-literal',))
    upper = [s for s in neighbour_declared if s['name'] == proof['upper_neighbour']['segment']]
    require(len(upper) == 1 and upper[0]['class'] not in ('DATA', 'BSS', 'STACK', 'BEGDATA'),
            'Runtime upper neighbour is not the next DGROUP class')
    from compiler import verify_toolchain
    from omf import OmfReader
    from object_probe import read_object
    config, _ = verify_toolchain(owner['profile'])
    text_frame = runtime_frame(image, relocations, manifest)
    items = proof['intervening']
    require(isinstance(items, list), 'Invalid runtime intervening contribution list')
    contributions = {name: [] for name in proof['segments']}
    for item in items:
        require(isinstance(item, dict) and
                set(item) == {'library', 'module', 'module_sha256', 'segment', 'length', 'targets'} and
                item['segment'] in contributions and item['segment'] != proof['segments'][0] and
                any(f['path'] == item['library'] for f in config['files']),
                'Invalid runtime intervening contribution')
        archive = _toolchain_path(item['library']).read_bytes()
        pinned = [f for f in config['files'] if f['path'] == item['library']]
        require(len(pinned) == 1 and sha(archive) == pinned[0]['sha256'],
                'Intervening contribution archive is not pinned')
        blobs = [b for n, b in OmfReader().split_library(archive)
                 if n == item['module'] and sha(b) == item['module_sha256']]
        require(len(blobs) == 1, 'Intervening contribution module missing/ambiguous')
        other = read_object(blobs[0])
        name = item['segment']
        seg = [s for s in other.segment_defs if s['name'] == name]
        other_groups = {g['name']: g for g in other.groups}
        require(len(seg) == 1 and seg[0]['class'] == 'DATA' and seg[0]['alignment'] == 'word' and
                name in other_groups.get('DGROUP', {}).get('segments', []) and
                other.segment_length(name) == item['length'] > 0 and item['length'] % 4 == 0,
                'Intervening contribution declaration differs')
        fixes = [f for f in other.linker_fixups if f['segment'] == name]
        require([f['offset'] for f in sorted(fixes, key=lambda f: f['offset'])] ==
                list(range(0, item['length'], 4)) and
                all(f['loc'] == 'pointer32' and f['target_kind'] == 'external' and
                    f['displacement'] == 0 and f['encoded_addend'] == '00000000' and
                    not f['self_relative'] for f in fixes) and
                [f['target'] for f in sorted(fixes, key=lambda f: f['offset'])] == item['targets'] and
                bytes(other.segment_bytes(name)) == bytes(item['length']),
                'Intervening contribution is not a complete far-pointer table')
        contributions[name].append(item)
    total = sum(obj.segment_length(n) for n in proof['segments']) + sum(i['length'] for i in items)
    upper_start = row['start']
    base = upper_start - total
    places = {}; at = base; pointer_sites = []
    for name in proof['segments']:
        start = at
        at += obj.segment_length(name)
        for item in contributions[name]:
            for index, target in enumerate(item['targets']):
                address = _inventory_code_entry(target, image)
                site = at + 4*index
                require(0 <= address - text_frame <= 65535 and
                        image[site:site+4] == struct.pack('<HH', address-text_frame, text_frame//16),
                        'Intervening far pointer differs from its verified target')
                pointer_sites.append(site + 2)
            at += item['length']
        places[name] = {'start': start, 'end': at}
    require(at == upper_start, 'Runtime DATA-class order does not close at its neighbour')
    inside = sorted(r['load_offset'] for r in relocations if base <= r['load_offset'] < upper_start)
    require(inside == sorted(pointer_sites),
            'Unexplained relocated contribution between runtime sections and neighbour')
    return places


def dosseg_boundary(owner, obj, target, addend, placements, image, relocations):
    """LINK /DOSSEG symbols: _edata = start of class BSS, _end = start of class STACK."""
    proof = owner['binding'].get('dosseg_boundaries', {}).get(target)
    require(owner['binding'].get('mode') == 'runtime-owner-group-v2' and
            target in ('_edata', '_end') and isinstance(proof, dict) and
            target in obj.externals, 'Unreviewed DOSSEG boundary symbol')
    signed = addend - 65536 if addend >= 32768 else addend
    require(signed in proof.get('addends', []), 'DOSSEG boundary addend is not reviewed')
    if target == '_end':
        require(set(proof) == {'kind', 'addends'} and proof['kind'] == 'stack-class-start-v1',
                'DOSSEG _end needs the MZ-grounded STACK class start')
        # The original MZ header SS names the STACK class paragraph (independent
        # of every member's bytes); CRT0's own STACK row must agree with it.
        from oracle import verify
        from mz import MZ
        header = MZ.parse(verify(write=False)[1])
        address = header.ss * 16
        frame = read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
        require(0 < address - frame < 65536, 'DOSSEG _end outside DGROUP')
        if 'STACK' in placements:
            segs = {s['name']: s for s in obj.segment_defs}
            require(segs['STACK']['class'] == 'STACK' and segs['STACK']['combine'] == 'stack' and
                    placements['STACK'].get('anchor', {}).get('kind') == 'mz-stack-v1' and
                    placements['STACK']['start'] == address,
                    'DOSSEG _end STACK declaration differs')
    else:
        require(set(proof) == {'kind', 'after', 'alignment', 'addends'} and
                proof['kind'] == 'bss-class-start-v1' and proof['alignment'] == 2,
                'DOSSEG _edata proof differs')
        _, row, declared = _accepted_neighbour(owner, proof['after'], ('common-v1',))
        dgroup = [s for s in declared if s['name'] == proof['after']['segment']]
        # The neighbour closes the last non-BSS DGROUP class in its pinned SEGDEF order.
        require(len(dgroup) == 1 and dgroup[0]['class'] not in ('BSS', 'STACK') and
                declared[-1]['name'] == proof['after']['segment'],
                'DOSSEG _edata neighbour is not the final pinned non-BSS section')
        end = row['end']
        address = end + (-end % proof['alignment'])
        require(image[end:address] == bytes(address-end) and
                not any(end <= r['load_offset'] < address+2 for r in relocations),
                'DOSSEG _edata alignment gap differs')
    return address + signed


# integ36: storage placed by the real link.  A storage row is either
# 'proven-raw' (its bytes stay explicit raw debt, re-proven on every build) or
# 'linked': the pinned member OBJ itself is linked by the real LINK
# (tools/reallink.py runtime DGROUP model), which must place the contribution
# exactly there (reallink.runtime_placement, gated by promote_runtime and
# validate), and the bytes are owned as pinned runtime data:
# * initialized: one KNOWN_TOOLCHAIN_LIBRARY_DATA owner row (parent, segment,
#   extent) carved out of raw DGROUP debt;
# * the member's own `_BSS`: one KNOWN_TOOLCHAIN_LIBRARY_DATA `bss_owners` row
#   (placement link-runtime-member-v1), grounded by the image operands of every
#   own-`_BSS` FIXUPP of the hash-pinned code (`member-operands-v1`);
# * a zero-length BSS-class section (CRT0DAT XOB/XO/XOE): anchor `real-link-v1`
#   only.  Its independent prediction is the end of the complete `_BSS` class
#   (DOSSEG class order: `_BSS`, then CRT0DAT's BSS sections in pinned SEGDEF
#   order, then c_common): the end of the last `_BSS` row of the checked
#   bss_owners partition.  A data alias (for example the reference label at
#   that address) is never a placement anchor for a zero-length section.
STORAGE_OWNERSHIP = ('proven-raw', 'linked')
RUNTIME_DATA = 'KNOWN_TOOLCHAIN_LIBRARY_DATA'
RUNTIME_BSS_PLACEMENT = 'link-runtime-member-v1'
REAL_LINK_ANCHOR = 'real-link-v1'
MEMBER_OPERANDS_ANCHOR = 'member-operands-v1'


def bss_class_end(manifest):
    """End of the complete `_BSS` class in the checked bss_owners partition:
    the last row that is a `_BSS` contribution (accepted row, raw object
    placeholder or its word fill), before the communal unit and (integ39) the
    c_common paragraph fill that LINK leaves after the BSS-class sections."""
    import bss_link
    rows = bss_link.load_partition(manifest)
    ends = [r['end'] for r in rows if r['form'] not in (bss_link.COMMUNAL_UNIT, bss_link.COMMUNAL_FILL_BASIS)]
    require(ends, 'BSS partition has no _BSS contribution')
    return max(ends)


# integ37: hash-pinned data-only runtime members (no code: _cflush.asm,
# _file.c, cmiscdat.asm, ctype.asm).  Such a member has no code extent in the
# ownership partition, so its descriptor lives in manifest `runtime_data_members`
# (kind KNOWN_TOOLCHAIN_LIBRARY, module_form data-only, binding mode
# runtime-data-member-v1).  Its bytes are owned only through its `linked`
# storage rows (KNOWN_TOOLCHAIN_LIBRARY_DATA owners carved out of raw DGROUP),
# bound from the pinned member on every build and placed by the real link.
DATA_MEMBER_MODE = 'runtime-data-member-v1'
DATA_MEMBER_KEYS = {'id', 'kind', 'module_form', 'classification', 'library', 'library_sha256', 'module',
                    'module_sha256', 'name', 'profile', 'publics', 'externals', 'expected_fixups',
                    'expected_relocations', 'binding', 'omf_policy'}


def runtime_data_members(manifest):
    return list(manifest.get('runtime_data_members', []))


def runtime_owners(manifest):
    """Every accepted pinned runtime member: code owners and data-only members."""
    return [o for o in manifest['owners'] if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY'] + runtime_data_members(manifest)


def is_data_member(owner):
    return owner.get('module_form') == 'data-only'


def check_data_member_form(owner):
    policy = owner.get('omf_policy', {})
    require(set(owner) <= DATA_MEMBER_KEYS and owner.get('kind') == 'KNOWN_TOOLCHAIN_LIBRARY' and
            is_data_member(owner) and owner.get('classification') == 'RUNTIME_LIBRARY' and
            owner.get('binding', {}).get('mode') == DATA_MEMBER_MODE and
            'segment' not in owner and 'start' not in owner and 'end' not in owner and
            isinstance(policy, dict) and set(policy) <= {'iterated_fixups', 'communals'} and
            policy.get('iterated_fixups', True) is True and
            isinstance(policy.get('communals', []), list),
            'Invalid data-only runtime member form')


# integ37: the multi-member MSG COMMON overlay (PAD, EPAD) owned as pinned
# runtime data.  LINK overlays the COMMON contributions of the members in link
# order; the owned row belongs to the member whose contribution covers the
# complete COMMON extent and that is the last contributor with bytes in link
# (code-address) order, so its own bytes are the final overlay.  Every other
# contributor must be an accepted pinned member whose storage row for that
# COMMON stays `proven-raw` (overwritten, never owned), and the complete ordered
# overlay is re-derived from all pinned sources (verify_runtime_common).
COMMON_EXTENTS = {'PAD': (199973, 199992), 'EPAD': (199992, 199993)}


def common_overlay_owner(owner, name, obj, manifest):
    """Whether `owner`'s `name` COMMON row may be owned (linked) as the overlay."""
    full = COMMON_EXTENTS.get(name)
    if full is None or obj.segment_length(name) != full[1] - full[0]:
        return False
    rows = [(o, o['binding']['storage'][name]) for o in runtime_owners(manifest)
            if name in o.get('binding', {}).get('storage', {})]
    later = [o for o, r in rows if o['id'] != owner['id'] and r['end'] > r['start'] and
             o.get('start', -1) > owner.get('start', -1)]
    others_raw = all(r['ownership'] == 'proven-raw' and r['anchor']['kind'] == 'common-v1'
                     for o, r in rows if o['id'] != owner['id'])
    return not later and others_raw and owner.get('start') is not None


def runtime_data_rows(manifest, owner_id):
    return [o for o in manifest['owners'] if o['kind'] == RUNTIME_DATA and o.get('parent') == owner_id]


def runtime_bss_rows(manifest, owner_id):
    return [o for o in manifest.get('bss_owners', []) if o['kind'] == RUNTIME_DATA and o.get('parent') == owner_id]


def storage_placements(owner, obj, image, relocations, manifest):
    main = owner.get('segment')          # integ37: None for a data-only member
    placements = {main:{'start':owner['start'], 'end':owner['end'], 'ownership':'owned'}} if main else {}
    extra = owner['binding'].get('storage', {})
    nonzero = {n for n,s in obj.segment_lengths.items() if n != main and s}
    require(nonzero <= set(extra), 'Every nonzero runtime DATA/BSS/secondary segment must be accounted for')
    require(all(n in nonzero or (n in obj.segment_lengths and obj.segment_length(n) == 0 and
                                 extra[n].get('ownership') == 'linked' and
                                 extra[n].get('anchor', {}).get('kind') == REAL_LINK_ANCHOR)
                for n in extra),
            'Zero-length runtime section needs the real-link anchor')
    layout = read_json(ROOT/'layout/data-symbols.json')
    order = None
    segdefs = {s['name']: s for s in obj.segment_defs}
    dgroup = {g['name']: g for g in obj.groups}.get('DGROUP', {}).get('segments', [])
    for name, row in extra.items():
        require(set(row) == {'start','end','ownership','anchor'} and row['ownership'] in STORAGE_OWNERSHIP,
                'Unsupported runtime secondary storage policy')
        require(name in segdefs, 'Runtime storage row names no SEGDEF')
        start,end = row['start'],row['end']
        require(type(start) is int and type(end) is int and end-start == obj.segment_length(name),
                'Runtime secondary storage extent differs from complete SEGDEF')
        anchor = row['anchor']
        seg = segdefs[name]
        if anchor['kind'] == REAL_LINK_ANCHOR:
            # Zero-length BSS-class section: placed by the staged real link's MAP.
            require(set(anchor) == {'kind'} and row['ownership'] == 'linked' and start == end and
                    seg['class'] == 'BSS' and seg['combine'] == 'public' and name in dgroup and
                    name != '_BSS' and layout['bss_start'] <= start <= layout['bss_end'],
                    'Real-link anchor is only for a zero-length DGROUP BSS-class section')
            require(start == bss_class_end(manifest),
                    'Zero-length BSS section does not follow the complete _BSS class')
            placements[name] = row
            continue
        require(start < end, 'Zero-length runtime section needs the real-link anchor')
        if anchor['kind'] == MEMBER_OPERANDS_ANCHOR:
            from bss_link import grounded_member_bss
            grounded, count = grounded_member_bss(obj, owner['start'], image, layout['frame_load_address'])
            require(set(anchor) == {'kind'} and name == '_BSS' and seg['class'] == 'BSS' and
                    row['ownership'] == 'linked' and count > 0 and grounded == start,
                    'Runtime _BSS is not grounded by its own code operands')
        elif anchor['kind'] == 'data-alias':
            require(set(anchor) == {'kind','symbol','offset'}, 'Invalid runtime storage alias anchor')
            symbol = resolve_symbols({anchor['symbol']}, image, relocations)[anchor['symbol']]
            require(type(anchor['offset']) is int and 0 <= anchor['offset'] < end-start and
                    start + anchor['offset'] == symbol['load_address'],
                    'Runtime storage alias placement differs')
        elif anchor['kind'] == 'unique-literal':
            require(set(anchor) == {'kind'} and name in obj.segments and
                    not any(f['segment'] == name for f in obj.linker_fixups),
                    'Unique literal storage must have no fixups')
            data = obj.segment_bytes(name)
            in_dgroup = any(g['name'] == 'DGROUP' and name in g['segments'] for g in obj.groups)
            lo,hi = (layout['frame_load_address'],layout['bss_start']) if in_dgroup else (0,len(image))
            require(len(data) == end-start and lo <= start < end <= hi and
                    image.find(data,lo,hi) == start and image.find(data,start+1,hi) == -1,
                    'Runtime storage is not a unique complete literal in its grounded group')
        elif anchor['kind'] == 'code-field-v1':
            require(set(anchor)=={'kind','site','target_offset','hex'} and
                    type(anchor['site']) is int and type(anchor['target_offset']) is int and
                    owner['start'] <= anchor['site'] < owner['end']-1 and
                    bytes.fromhex(anchor['hex']) == image[anchor['site']:anchor['site']+2],
                    'Runtime storage code-field anchor changed')
            fixups=[f for f in obj.linker_fixups if f['segment']==owner['segment']
                    and owner['start']+f['offset']==anchor['site'] and
                    f['target_kind']=='segment' and f['target']==name and
                    f['loc']=='offset16' and not f['self_relative']]
            require(len(fixups)==1 and
                    layout['frame_load_address']+int.from_bytes(bytes.fromhex(anchor['hex']),'little')
                    == start+anchor['target_offset'] and
                    anchor['target_offset']==fixups[0]['displacement']+
                    int.from_bytes(bytes.fromhex(fixups[0]['encoded_addend']),'little'),
                    'Runtime storage code-field placement differs')
        elif anchor['kind'] == 'common-v1':
            require(set(anchor)=={'kind'} and name in ('PAD','EPAD') and
                    (start,end)==((199973,199973+obj.segment_length(name)) if name=='PAD'
                                 else (199992,199992+obj.segment_length(name))),
                    'Runtime COMMON placement differs')
        elif anchor['kind'] == 'dgroup-order-v1':
            if order is None:
                order = crt_order_placements(owner, obj, image, relocations, manifest)
            require(set(anchor) == {'kind'} and name in order and
                    (start, end) == (order[name]['start'], order[name]['end']),
                    'Runtime DGROUP-order placement differs')
        elif anchor['kind'] == 'mz-stack-v1':
            from oracle import verify
            from mz import MZ
            mz=MZ.parse(verify(write=False)[1])
            require(set(anchor)=={'kind'} and name=='STACK' and
                    (start,end)==(222352,224400) and
                    mz.ss*16==start and mz.sp==8000 and end<=mz.ss*16+mz.sp,
                    'Runtime STACK does not fit original MZ reservation')
        else:
            raise ValueError('Unsupported runtime storage anchor')
        require(seg['combine'] in ('public','private') or
                (seg['combine']=='common' and anchor['kind'] in ('common-v1','dgroup-order-v1')) or
                (seg['combine']=='stack' and anchor['kind']=='mz-stack-v1'),
                'Runtime COMMON/STACK storage needs placement proof')
        if row['ownership'] == 'linked':
            # Owned pinned runtime data: a DGROUP contribution, never the stack
            # nor the multi-member MSG COMMON overlay.  A COMMON segment only as
            # its sole declared contribution (CRT0DAT CDATA); the real link must
            # then show the whole MAP segment equal to the row.
            require(name in dgroup and (seg['combine'] == 'public' or
                    (seg['combine'] == 'common' and anchor['kind'] != 'common-v1' and
                     not any(name in o.get('binding', {}).get('storage', {}) for o in runtime_owners(manifest)
                             if o['id'] != owner['id'])) or
                    (seg['combine'] == 'common' and anchor['kind'] == 'common-v1' and
                     common_overlay_owner(owner, name, obj, manifest))),
                    'Linked runtime storage must be a public or sole COMMON DGROUP contribution, '
                    'or the complete final MSG COMMON overlay')
        if name in obj.segments:
            require(0 <= start < end <= len(image), 'Runtime secondary data outside the image')
            if row['ownership'] == 'linked':
                rows = [o for o in runtime_data_rows(manifest, owner['id']) if o.get('segment') == name]
                require(len(rows) == 1 and (rows[0]['start'], rows[0]['end']) == (start, end),
                        'Linked runtime data lacks exactly one owned data row')
            elif anchor['kind'] == 'common-v1' and any(
                    o['kind'] == RUNTIME_DATA and o.get('segment') == name and o['start'] <= start and end <= o['end']
                    for o in manifest['owners']):
                # integ37: an overwritten COMMON contribution inside the owned overlay.
                pass
            else:
                require(any(o['kind'] == 'UNRESOLVED_RAW' and o['start'] <= start and end <= o['end']
                            for o in manifest['owners']), 'Runtime secondary data no longer wholly raw-owned')
        else:
            require((seg['class'] == 'BSS' and layout['bss_start'] <= start < end <= layout['bss_end']) or
                    (anchor['kind']=='mz-stack-v1' and seg['class']=='STACK'),
                    'Uninitialized runtime storage outside independently verified BSS')
            if row['ownership'] == 'linked':
                rows = runtime_bss_rows(manifest, owner['id'])
                require(anchor['kind'] == MEMBER_OPERANDS_ANCHOR,
                        'Linked runtime _BSS must be grounded by its own code operands')
                require(name == '_BSS' and len(rows) == 1 and rows[0].get('segment') == '_BSS' and
                        (rows[0]['start'], rows[0]['end']) == (start, end) and
                        rows[0].get('placement') == RUNTIME_BSS_PLACEMENT,
                        'Linked runtime _BSS lacks exactly one owned BSS row')
            # The independently checked startup zero range supplies the storage contract.
            resolve_symbols(set(), image, relocations)
        require(main is None or not (start < owner['end'] and owner['start'] < end), 'Runtime storage overlaps code')
        placements[name] = row
    linked = {n for n, r in extra.items() if r['ownership'] == 'linked' and r['start'] < r['end']}
    require({o.get('segment') for o in runtime_data_rows(manifest, owner['id'])} == linked - {'_BSS'} and
            len(runtime_data_rows(manifest, owner['id'])) == len(linked - {'_BSS'}) and
            len(runtime_bss_rows(manifest, owner['id'])) == len(linked & {'_BSS'}),
            'Owned runtime data rows differ from the linked storage rows')
    ranges = sorted((p['start'],p['end']) for n,p in placements.items()
                    if (main and p is placements[main]) or (p['anchor']['kind']!='common-v1' and p['start'] < p['end']))
    require(all(a[1] <= b[0] for a,b in zip(ranges,ranges[1:])), 'Runtime storage contributions overlap')
    for other in runtime_owners(manifest):
        if other['id'] == owner['id']: continue
        for a in extra.values():
            for b in other.get('binding',{}).get('storage',{}).values():
                require(a['anchor']['kind']=='common-v1' and b['anchor']['kind']=='common-v1' or
                        a['start'] == a['end'] or b['start'] == b['end'] or
                        not (a['start'] < b['end'] and b['start'] < a['end']),
                        'Distinct runtime members overlap secondary storage')
    return placements



# integ36 (L2-rt): the bounded `__cfltcvt_tab` far-pointer table of cmiscdat.asm.
# The pinned member's iterated LIDATA carries its FIXUPPs, so the table is not
# bound as a member here: the reviewed reference label span (five `dd __fptrap`
# declarations, next label `word_3EF98`), the independently anchored alias at
# its placed start and the five original relocated pointers onto the accepted
# `__fptrap` entry bound the object to 20 bytes; only element addends are allowed.
FPTRAP_TABLE = {'kind':'pinned-fptrap-table-v1','start_label':'off_3EF84',
                'end_label':'word_3EF98','entry_count':5,'entry_size':4,
                'target':'__fptrap','allowed_addends':[0,4,8,12,16]}


def fptrap_table_proof(owner, target, proof, data_map, data_symbols, image, relocations, manifest, text_frame):
    import re
    from pinned_reference import check_reference_identity, reference_label_offsets, reference_lines
    require(owner.get('module') == 'output.c' and target == '__cfltcvt_tab' and
            proof == FPTRAP_TABLE and data_map.get(target) == '_off_3EF84',
            'Unreviewed runtime data-object proof')
    source_path = 'src/restunts/asmorig/dseg.asm'
    refs = read_json(ROOT/'layout/references.json')['restunts']['evidence_files']
    require(check_reference_identity(source_path) == refs[source_path], 'Pinned dseg source differs')
    offsets = reference_label_offsets(source_path)
    expected_labels = {'off_3EF84':(14356,15367),'off_3EF88':(14360,15368),
                       'off_3EF90':(14368,15370),'off_3EF94':(14372,15371),
                       'word_3EF98':(14376,15372)}
    require({k:offsets.get(k) for k in expected_labels} == expected_labels,
            'Pinned __cfltcvt_tab label span differs')
    start_offset, start_line = offsets['off_3EF84']
    end_offset, end_line = offsets['word_3EF98']
    require(all(not start_offset < pos < end_offset or name in ('off_3EF88','off_3EF90','off_3EF94')
                for name, (pos, _) in offsets.items()), 'Unreviewed interior label in __cfltcvt_tab')
    rows = [r'off_3EF84\s+dd\s+__fptrap', r'off_3EF88\s+dd\s+__fptrap', r'dd\s+__fptrap',
            r'off_3EF90\s+dd\s+__fptrap', r'off_3EF94\s+dd\s+__fptrap']
    lines = reference_lines(source_path, start_line, end_line)
    require(all(re.fullmatch(pattern, lines[i].split(';')[0].strip(), re.I)
                for i, pattern in enumerate(rows)) and
            re.fullmatch(r'word_3EF98\s+dw\s+0', lines[-1].split(';')[0].strip(), re.I),
            'Pinned __cfltcvt_tab declarations differ')
    frame = read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
    start, end = frame+start_offset, frame+end_offset
    require(data_symbols['_off_3EF84']['load_address'] == start and (start, end) == (192388, 192408) and
            end < read_json(ROOT/'layout/data-symbols.json')['bss_start'],
            'Pinned __cfltcvt_tab placement differs')
    fptrap = group_public('__fptrap', manifest)
    require(fptrap['segment'] == '_TEXT' and fptrap['address'] == 118446, 'Pinned __fptrap target differs')
    require([r['load_offset'] for r in relocations if start <= r['load_offset'] < end] ==
            [start+4*i+2 for i in range(5)], 'Original __cfltcvt_tab ordered relocations differ')
    for i in range(5):
        at = start+4*i
        require(int.from_bytes(image[at:at+2], 'little') + text_frame == fptrap['address'] and
                int.from_bytes(image[at+2:at+4], 'little') == text_frame//16,
                'Original __cfltcvt_tab entry does not name __fptrap')
    return list(proof['allowed_addends'])


# integ36: reviewed in-member near-dispatch tables.  A review names the
# dispatch FIXUPP (the disp16 of `JMP CS:[BX+table]`), the table start and its
# entry count, and a bound: `unsigned-guard-v1` (`CMP AX,n-1; JBE` straight
# into `ADD AX,AX; XCHG BX,AX` and the dispatch) or `member-prefix-v1` (the
# table fills the member's bytes before its first public entry).  Every table
# word is an own-segment offset16 FIXUPP (frame _TEXT) naming a byte of the
# member's own code outside the table.
CODE_TABLE_KEYS = {'dispatch', 'table', 'entries', 'bound'}


def code_table_proofs(owner, obj, code):
    reviews = owner['binding'].get('code_tables', [])
    require(isinstance(reviews, list) and (not reviews or owner['binding'].get('mode') == 'runtime-owner-group-v2'),
            'Invalid runtime code-table review')
    fixes = {f['offset']: f for f in obj.linker_fixups if f['segment'] == '_TEXT'}
    size = obj.segment_length('_TEXT')
    sites = set()
    for review in reviews:
        require(isinstance(review, dict) and set(review) == CODE_TABLE_KEYS and
                all(type(review[k]) is int for k in ('dispatch', 'table', 'entries')) and
                review['entries'] > 0, 'Invalid runtime code-table review')
        d, t, n = review['dispatch'], review['table'], review['entries']
        f = fixes.get(d)
        require(f is not None and f['loc'] == 'offset16' and f['target_kind'] == 'segment' and
                f['target'] == '_TEXT' and not f['self_relative'] and
                f['frame_method'] in (0, 5) and d >= 3 and code[d-3:d] == bytes.fromhex('2effa7') and
                int.from_bytes(bytes.fromhex(f['encoded_addend']), 'little') + f['displacement'] == t,
                'Runtime code-table dispatch differs')
        require(0 <= t and t + 2*n <= size and not (t <= d < t + 2*n), 'Runtime code table outside member')
        for i in range(n):
            e = fixes.get(t + 2*i)
            target = None if e is None else int.from_bytes(bytes.fromhex(e['encoded_addend']), 'little') + e['displacement']
            require(e is not None and e['loc'] == 'offset16' and e['target_kind'] == 'segment' and
                    e['target'] == '_TEXT' and e['frame_method'] == 0 and e['frame'] == '_TEXT' and
                    not e['self_relative'] and 0 <= target < size and not (t <= target < t + 2*n),
                    'Runtime code-table entry is not an own code offset')
            sites.add(t + 2*i)
        bound = review['bound']
        require(isinstance(bound, dict), 'Invalid runtime code-table bound')
        if bound.get('kind') == 'unsigned-guard-v1':
            g = bound.get('site')
            require(set(bound) == {'kind', 'site'} and type(g) is int and 0 <= g and g + 5 <= d - 6 and
                    code[g] == 0x3d and int.from_bytes(code[g+1:g+3], 'little') == n - 1 and
                    code[g+3] == 0x76 and g + 5 + code[g+4] == d - 6 and
                    code[d-6:d-3] == bytes.fromhex('03c093'),
                    'Runtime code-table guard differs')
        elif bound.get('kind') == 'member-prefix-v1':
            entries = [p['offset'] for p in obj.publics if p['segment'] == '_TEXT']
            require(set(bound) == {'kind'} and t == 0 and entries and min(entries) == 2*n,
                    'Runtime code table is not the member prefix before its first entry')
        else:
            raise ValueError('Unsupported runtime code-table bound')
        sites.add(d)
    require(len(sites) == sum(r['entries'] + 1 for r in reviews), 'Overlapping runtime code-table reviews')
    return sites


def bind_member(owner, obj, image, relocations, manifest, trail):
    mode=owner['binding'].get('mode')
    require(mode in ('runtime-member-v1','runtime-owner-group-v2',DATA_MEMBER_MODE), 'Unknown runtime binding mode')
    require((mode == DATA_MEMBER_MODE) == is_data_member(owner), 'Data-only runtime member mode differs')
    # integ37: a data-only member binds its linked DGROUP storage exactly as a
    # code member binds its secondary segments; group-bound targets as v2.
    group_mode = mode in ('runtime-owner-group-v2', DATA_MEMBER_MODE)
    require(owner['binding']['declarations'] == declarations(obj), 'Runtime declarations changed')
    require(owner['expected_fixups'] == obj.linker_fixups, 'Complete ordered runtime FIXUPP differs')
    if mode == DATA_MEMBER_MODE:
        check_data_member_form(owner)
        require(all(s['length'] == 0 for s in obj.segment_defs if s['class'] == 'CODE'),
                'Data-only runtime member declares code')
        storage = owner['binding'].get('storage', {})
        require(storage and all(r.get('ownership') == 'linked' for r in storage.values()),
                'Data-only runtime member storage must be linked (owned) rows')
        require(set(owner['binding']) <= {'mode', 'declarations', 'storage', 'data_bindings', 'raw_file_publics'},
                'Unsupported data-only runtime member review')
    else:
        require(owner['segment'] == '_TEXT', 'Runtime primary segment must be complete _TEXT')
    placements = storage_placements(owner, obj, image, relocations, manifest)
    if mode != DATA_MEMBER_MODE:
        require(obj.segment_length(owner['segment']) == owner['end']-owner['start'],
                'Runtime primary complete SEGDEF length differs')
    payloads = {n:bytearray(obj.segment_bytes(n)) for n in placements if n in obj.segments}
    require(all(len(p) == obj.segment_length(n) for n,p in payloads.items()),
            'Incomplete initialized runtime segment')
    text_frame = runtime_frame(image, relocations, manifest)
    data_map = owner['binding'].get('data_bindings', {})
    data_symbols = resolve_symbols(set(data_map.values()), image, relocations) if data_map else {}
    proofs = owner['binding'].get('data_object_proofs', {})
    require(isinstance(proofs, dict) and set(proofs) <= set(data_map), 'Invalid runtime data-object proof map')
    for target, proof in proofs.items():
        data_symbols[data_map[target]]['allowed_addends'] = fptrap_table_proof(
            owner, target, proof, data_map, data_symbols, image, relocations, manifest, text_frame)
    tables = code_table_proofs(owner, obj, payloads[owner['segment']]) if mode != DATA_MEMBER_MODE else set()
    used_data = set(); used_raw_publics=set(); used_file_publics=set(); code_cache = {}; fix_receipts = []; sites = []
    used_code_offsets = set(); used_boundaries = set(); used_aliases = set(); used_tables = set()
    code_aliases = owner['binding'].get('code_aliases', [])
    require(isinstance(code_aliases, list) and len(set(code_aliases)) == len(code_aliases) and
            (not code_aliases or mode == 'runtime-owner-group-v2'), 'Invalid runtime code-alias review')
    code_offsets = owner['binding'].get('external_code_offsets', [])
    require(isinstance(code_offsets, list) and
            all(isinstance(p, dict) and set(p) == {'segment','offset','target'} for p in code_offsets) and
            len({(p['segment'],p['offset']) for p in code_offsets}) == len(code_offsets) and
            (not code_offsets or mode == 'runtime-owner-group-v2'),
            'Invalid runtime external code-offset review')
    boundaries = owner['binding'].get('dosseg_boundaries', {})
    require(isinstance(boundaries, dict) and (not boundaries or mode == 'runtime-owner-group-v2'),
            'Invalid runtime DOSSEG boundary review')
    # Zero-length sections have no storage row; only an order proof places them.
    order = crt_order_placements(owner, obj, image, relocations, manifest)
    zero_sections = {n:p for n,p in order.items() if obj.segment_length(n) == 0}
    # integ36: zero-length BSS-class sections placed by the staged real link.
    zero_sections.update({n:p for n,p in placements.items()
                          if obj.segment_length(n) == 0 and p.get('anchor', {}).get('kind') == REAL_LINK_ANCHOR})
    segs = {s['name']:s for s in obj.segment_defs}
    groups = {g['name']:g for g in obj.groups}
    local = {p['name']:p for p in obj.publics if p['segment'] in placements}
    resolve_symbols(set(), image, relocations)
    dgroup_frame = read_json(ROOT/'layout/data-symbols.json')['frame_load_address']
    for fix in obj.linker_fixups:
        segment = fix['segment']; at = fix['offset']; width = fix['width']
        require(segment in payloads and 0 <= at <= len(payloads[segment])-width,
                'Runtime fixup outside complete initialized contribution')
        payload = payloads[segment]; encoded = bytes.fromhex(fix['encoded_addend'])
        require(len(encoded) == width and bytes(payload[at:at+width]) == encoded,
                'Runtime encoded addend changed')
        kind, target = fix['target_kind'], fix['target']
        require(type(fix['displacement']) is int and 0 <= fix['displacement'] <= 65535,
                'Invalid runtime displacement')
        addend = int.from_bytes(encoded[:2], 'little') + fix['displacement']
        target_segment = None; target_frame = None
        if kind == 'external':
            require(fix['target_method'] == 2 and 1 <= fix['target_index'] <= len(obj.externals)
                    and obj.externals[fix['target_index']-1] == target, 'Runtime EXTDEF index differs')
            if target in local:
                p = local[target]; target_segment = p['segment']
                require(target_segment in placements, 'Runtime local public has no complete contribution')
                require(0 <= p['offset']+addend < obj.segment_length(target_segment),
                        'Runtime local public addend outside contribution')
                address = placements[target_segment]['start'] + p['offset'] + addend
            elif target in data_map:
                used_data.add(target); symbol = data_symbols[data_map[target]]
                require(addend in symbol['allowed_addends'], 'Runtime external data addend exceeds grounded object')
                address = symbol['load_address'] + addend; target_frame = symbol['frame_load_address']
            elif mode=='runtime-owner-group-v2' and target in owner['binding'].get('raw_crt0dat_publics',[]):
                used_raw_publics.add(target)
                raw_public=crt0dat_data_public(target,image,relocations)
                require(0<=addend<raw_public['extent'],
                        'Raw CRT data public addend outside object')
                address=raw_public['address']+addend; target_frame=raw_public['frame']
            elif group_mode and target in owner['binding'].get('raw_file_publics',[]):
                used_file_publics.add(target)
                raw_public=file_data_public(target,image,relocations)
                require(0<=addend<raw_public['extent'],
                        'Raw stdio data public addend outside object')
                address=raw_public['address']+addend; target_frame=raw_public['frame']
            elif mode == 'runtime-owner-group-v2' and target in boundaries:
                require(fix['frame_method']==1 and fix['frame_kind']=='group' and
                        fix['frame']=='DGROUP' and fix['loc']=='offset16' and
                        not fix['self_relative'], 'DOSSEG boundary fixup must be a DGROUP offset')
                used_boundaries.add(target)
                address=dosseg_boundary(owner,obj,target,addend,placements,image,relocations)
                target_frame=dgroup_frame
            elif mode == 'runtime-owner-group-v2' and target in code_aliases:
                # A far CALL into raw runtime code: the reviewed alias carries its
                # own original relocated CALL anchors and verified mapped entry.
                from code_symbols import resolve_code_symbols
                resolved = resolve_code_symbols({target}, image, relocations)[target]
                require(fix['loc'] == 'pointer32' and not fix['self_relative'] and addend == 0 and
                        resolved['frame_load_address'] == text_frame,
                        'Runtime code alias must be an exact far CALL in the runtime frame')
                if any(p['name'] == target for o in manifest['owners']
                       if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' for p in o['publics']):
                    # Once the provider is owned, its pinned public must agree.
                    require(group_public(target, manifest)['address'] == resolved['load_address'],
                            'Runtime code alias disagrees with the owned runtime public: '+target)
                used_aliases.add(target)
                address = resolved['load_address']; target_segment = '_TEXT'
            elif mode == 'runtime-owner-group-v2' and target=='_main':
                require(addend==0, 'CRT0 _main addend must name exact entry')
                address=reviewed_main_entry(image,relocations,owner,fix)
                target_segment='_TEXT'; target_frame=0
            elif group_mode:
                resolved=group_public(target,manifest)
                target_segment=resolved['segment']
                require(0 <= addend < resolved['extent'],
                        'Runtime group public addend outside referenced object')
                address=resolved['address']+addend
            else:
                require(addend == 0, 'Runtime external code addend must name an exact public')
                if target not in code_cache:
                    code_cache[target] = runtime_public(target, image, relocations, manifest, trail)
                address = code_cache[target]['address']; target_segment = code_cache[target]['segment']
        elif kind == 'segment' and target in zero_sections:
            require(fix['target_method'] == 0 and segs[target]['index'] == fix['target_index'] and
                    addend == 0, 'Zero-length runtime section reference differs')
            address = zero_sections[target]['start']; target_segment = target
        elif kind == 'segment':
            require(target in placements or obj.segment_length(target) != 0,
                    'Zero-length runtime section lacks independently grounded placement: '+target)
            require(fix['target_method'] == 0 and target in placements and
                    segs[target]['index'] == fix['target_index'], 'Runtime target SEGDEF missing/invalid')
            boundary=(mode=='runtime-owner-group-v2' and
                      owner['module']=='dos\\nmsghdr.asm' and segment=='_TEXT' and
                      at==11 and target=='HDR' and addend==8 and
                      placements['HDR']['end']==199734 and
                      any(o.get('module')=='dos\\crt0msg.asm' and
                          o.get('binding',{}).get('storage',{}).get('MSG',{}).get('start')==199734
                          for o in manifest['owners']) and
                      image[owner['start']+10]==0xbe)
            require(0 <= addend < obj.segment_length(target) or boundary,
                    'Runtime internal addend outside complete SEGDEF')
            address = placements[target]['start'] + addend; target_segment = target
        elif kind == 'group' and group_mode:
            require(target=='DGROUP' and fix['target_method']==1 and
                    fix['target_index']==groups['DGROUP']['index'] and addend==0,
                    'Unsupported runtime group target')
            address=dgroup_frame; target_frame=dgroup_frame
        else:
            raise ValueError('Unsupported runtime target kind: '+kind)
        if kind == 'external' and target == '_main' and mode == 'runtime-owner-group-v2':
            # The reviewed game entry keeps its own relocated frame (paragraph 0).
            require(target_segment == '_TEXT' and target_frame == 0 and address == 0,
                    'Reviewed CRT0 _main frame differs')
        elif target_segment == '_TEXT': target_frame = text_frame
        elif target_segment is not None:
            require('DGROUP' in groups and target_segment in groups['DGROUP']['segments'],
                    'Runtime data target is outside DGROUP')
            target_frame = dgroup_frame
        method = fix['frame_method']
        if method == 0:
            require(fix['frame_kind'] == 'segment' and fix['frame'] == '_TEXT' and
                    fix['frame_index'] == segs['_TEXT']['index'] and target_frame == text_frame,
                    'Runtime frame-method-0 requires grounded merged _TEXT')
            frame = text_frame
        elif method == 1:
            require(fix['frame_kind'] == 'group' and fix['frame'] == 'DGROUP' and
                    'DGROUP' in groups and fix['frame_index'] == groups['DGROUP']['index'] and
                    target_frame == dgroup_frame, 'Runtime group frame differs')
            frame = dgroup_frame
        elif method == 5:
            require(fix['frame_kind'] == 'target' and fix['frame'] == target and fix['frame_index'] == 0,
                    'Runtime target frame differs')
            frame = target_frame
        elif method == 2:
            require(kind == 'external' and fix['frame_kind'] == 'external' and
                    fix['frame'] == target and fix['frame_index'] == fix['target_index'],
                    'Runtime external frame must name the same exact target')
            frame = target_frame
        else:
            raise ValueError('Unsupported runtime frame method: '+str(method))
        source_at = placements[segment]['start'] + at
        if fix['self_relative']:
            require(fix['loc'] == 'offset16' and width == 2 and segment == '_TEXT' and
                    target_segment == '_TEXT' and at >= 1 and payload[at-1] in (0xe8,0xe9),
                    'Runtime self-relative fixup must be a near CODE CALL/JMP')
            value = address-(source_at+2)
            require(-32768 <= value <= 32767, 'Runtime near target is out of range')
            struct.pack_into('<h',payload,at,value)
        elif fix['loc'] == 'offset16' and width == 2:
            review = [p for p in code_offsets if (p['segment'],p['offset']) == (segment,at)]
            if review:
                # An external 16-bit code offset: the target is a grounded
                # _TEXT public; the site is an immediate or a code-pointer word.
                if segment == '_TEXT':
                    site_ok = ((at >= 1 and 0xb8 <= payload[at-1] <= 0xbf) or
                               (at >= 4 and payload[at-4] == 0xc7 and payload[at-3] == 0x06))
                else:
                    site_ok = (segment in groups.get('DGROUP',{}).get('segments',[]) and
                               any(p['segment'] == segment and p['offset'] == at for p in obj.publics))
                require(kind == 'external' and target == review[0]['target'] and
                        target_segment == '_TEXT' and frame == text_frame and
                        fix['frame_method'] == 0 and addend == 0 and site_ok,
                        'Runtime external code offset differs from its review')
                used_code_offsets.add((segment,at))
            elif frame==text_frame and mode=='runtime-owner-group-v2':
                require(segment=='_TEXT' and target_segment=='_TEXT' and
                        kind=='segment' and target=='_TEXT' and
                        (at in tables or
                         (at in owner['binding'].get('code_offset_fixups',[]) and
                          (payload[at-1] in (0xb8,0xba) or
                           payload[at-2:at]==bytes.fromhex('8d1e')))),
                        'Runtime absolute code offset needs explicit table proof')
                if at in tables:
                    used_tables.add(at)
            else:
                require(frame == dgroup_frame, 'Runtime absolute code offset needs explicit pointer proof')
            require(0 <= address-frame <= 65535, 'Runtime absolute offset overflow')
            struct.pack_into('<H',payload,at,address-frame)
        elif fix['loc'] == 'base16' and width == 2 and group_mode:
            require(target_frame==frame and frame is not None and frame%16==0,
                    'Runtime base fixup frame differs')
            struct.pack_into('<H',payload,at,frame//16)
            sites.append(source_at)
        elif (fix['loc'] == 'pointer32' and width == 4 and mode == DATA_MEMBER_MODE and
              segment in groups.get('DGROUP', {}).get('segments', []) and segment in payloads):
            # integ37: a far code pointer stored in a data-only member's DGROUP
            # data (cmiscdat `dd __fptrap` table, _cflush XP `dd _flushall`):
            # an exact accepted _TEXT public, one MZ relocation for its segment.
            require(kind == 'external' and target_segment == '_TEXT' and encoded == bytes(4) and
                    addend == 0 and frame == text_frame and 0 <= address-frame <= 65535,
                    'Data far pointer must name an exact accepted _TEXT public')
            struct.pack_into('<HH',payload,at,address-frame,frame//16)
            sites.append(source_at+2)
        elif fix['loc'] == 'pointer32' and width == 4:
            require(segment == '_TEXT' and target_segment == '_TEXT' and at >= 1 and
                    payload[at-1] in (0x9a,0xea) and encoded == bytes(4) and
                    (addend == 0 or (mode=='runtime-owner-group-v2' and
                                     kind=='segment' and target=='_TEXT')),
                    'Runtime far fixup must name an exact CALL/JMP public')
            require(frame is not None and frame % 16 == 0 and 0 <= address-frame <= 65535,
                    'Runtime far frame/offset overflow')
            struct.pack_into('<HH',payload,at,address-frame,frame//16)
            sites.append(source_at+2)
        else:
            raise ValueError('Unsupported runtime fixup location: '+fix['loc'])
        fix_receipts.append({'segment':segment,'offset':at,'target':target,'address':address,
                             'frame':frame,'linked':bytes(payload[at:at+width]).hex()})
    require(used_data == set(data_map), 'Unused or misclassified runtime data binding')
    require(used_raw_publics==set(owner['binding'].get('raw_crt0dat_publics',[])),
            'Unused raw CRT data public binding')
    require(used_file_publics==set(owner['binding'].get('raw_file_publics',[])),
            'Unused raw stdio data public binding')
    require(used_code_offsets == {(p['segment'],p['offset']) for p in code_offsets},
            'Unused runtime external code-offset review')
    require(used_boundaries == set(boundaries), 'Unused runtime DOSSEG boundary review')
    require(used_aliases == set(code_aliases), 'Unused runtime code-alias review')
    require(used_tables == set(tables), 'Unused runtime code-table review')
    # EXEPACK emits relocation banks in address order while preserving within-bank order.
    sites = sorted(sites, key=lambda site:site//65536)
    expected = owner['expected_relocations']
    actual = [r for r in relocations if any(p['start']-1 <= r['load_offset'] < p['end']
                                           for p in placements.values())]
    require(expected == actual and [r['load_offset'] for r in expected] == sites,
            'Complete ordered runtime MZ relocations differ')
    for r in expected:
        require(set(r) == {'segment','offset','load_offset'} and
                0 <= r['segment'] <= 65535 and 0 <= r['offset'] <= 65535 and
                r['segment']*16+r['offset'] == r['load_offset'], 'Invalid runtime relocation coordinate')
    storage = []
    for name,p in placements.items():
        if name in payloads:
            if p.get('anchor',{}).get('kind')!='common-v1':
                require(bytes(payloads[name]) == image[p['start']:p['end']],
                        'Bound complete runtime segment differs from oracle: '+name)
        storage.append({'segment':name, 'start':p['start'], 'end':p['end'],
                        'ownership':p['ownership'], 'initialized':name in payloads})
    if any(p.get('anchor',{}).get('kind')=='common-v1' for p in placements.values()):
        verify_runtime_common(manifest,image)
    owned = {name: (bytes(payloads[name]) if name in payloads else bytes(p['end'] - p['start'])).hex()
             for name, p in placements.items()
             if name != owner.get('segment') and p.get('ownership') == 'linked' and p['start'] < p['end']}
    primary = bytes(payloads[owner['segment']]) if mode != DATA_MEMBER_MODE else b''
    return primary, {'mode':mode, 'fixups':fix_receipts,
            'generated_relocations':expected, 'storage':storage, 'secondary_payloads':owned}
