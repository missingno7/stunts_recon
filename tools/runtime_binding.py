"""Bounded relocation of complete pinned runtime members and their storage.

Addresses come from accepted runtime publics or reviewed original data aliases.
Secondary storage stays explicit raw debt and is re-proven on every build.
"""
import struct
from common import ROOT, read_json, require, sha, identity
from data_symbols import resolve_symbols


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


def storage_placements(owner, obj, image, relocations, manifest):
    main = owner['segment']
    placements = {main:{'start':owner['start'], 'end':owner['end'], 'ownership':'owned'}}
    extra = owner['binding'].get('storage', {})
    require(set(extra) == {n for n,s in obj.segment_lengths.items() if n != main and s},
            'Every nonzero runtime DATA/BSS/secondary segment must be accounted for')
    layout = read_json(ROOT/'layout/data-symbols.json')
    for name, row in extra.items():
        require(set(row) == {'start','end','ownership','anchor'} and row['ownership'] == 'proven-raw',
                'Unsupported runtime secondary storage policy')
        start,end = row['start'],row['end']
        require(type(start) is int and type(end) is int and end-start == obj.segment_length(name),
                'Runtime secondary storage extent differs from complete SEGDEF')
        anchor = row['anchor']
        if anchor['kind'] == 'data-alias':
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
        else:
            raise ValueError('Unsupported runtime storage anchor')
        seg = next(s for s in obj.segment_defs if s['name'] == name)
        require(seg['combine'] in ('public','private'), 'Runtime COMMON/STACK storage needs placement proof')
        if name in obj.segments:
            require(0 <= start < end <= len(image) and
                    any(o['kind'] == 'UNRESOLVED_RAW' and o['start'] <= start and end <= o['end']
                        for o in manifest['owners']), 'Runtime secondary data no longer wholly raw-owned')
        else:
            require(seg['class'] == 'BSS' and layout['bss_start'] <= start < end <= layout['bss_end'],
                    'Uninitialized runtime storage outside independently verified BSS')
            # The independently checked startup zero range supplies the storage contract.
            resolve_symbols(set(), image, relocations)
        require(not (start < owner['end'] and owner['start'] < end), 'Runtime storage overlaps code')
        placements[name] = row
    ranges = sorted((p['start'],p['end']) for p in placements.values())
    require(all(a[1] <= b[0] for a,b in zip(ranges,ranges[1:])), 'Runtime storage contributions overlap')
    for other in manifest['owners']:
        if other['kind'] != 'KNOWN_TOOLCHAIN_LIBRARY' or other['id'] == owner['id']: continue
        for a in extra.values():
            for b in other.get('binding',{}).get('storage',{}).values():
                require(not (a['start'] < b['end'] and b['start'] < a['end']),
                        'Distinct runtime members overlap secondary storage')
    return placements


def bind_member(owner, obj, image, relocations, manifest, trail):
    require(owner['binding'].get('mode') == 'runtime-member-v1', 'Unknown runtime binding mode')
    require(owner['binding']['declarations'] == declarations(obj), 'Runtime declarations changed')
    require(owner['expected_fixups'] == obj.linker_fixups, 'Complete ordered runtime FIXUPP differs')
    require(owner['segment'] == '_TEXT', 'Runtime primary segment must be complete _TEXT')
    placements = storage_placements(owner, obj, image, relocations, manifest)
    require(obj.segment_length(owner['segment']) == owner['end']-owner['start'],
            'Runtime primary complete SEGDEF length differs')
    payloads = {n:bytearray(obj.segment_bytes(n)) for n in placements if n in obj.segments}
    require(all(len(p) == obj.segment_length(n) for n,p in payloads.items()),
            'Incomplete initialized runtime segment')
    text_frame = runtime_frame(image, relocations, manifest)
    data_map = owner['binding'].get('data_bindings', {})
    data_symbols = resolve_symbols(set(data_map.values()), image, relocations) if data_map else {}
    used_data = set(); code_cache = {}; fix_receipts = []; sites = []
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
            else:
                require(addend == 0, 'Runtime external code addend must name an exact public')
                if target not in code_cache:
                    code_cache[target] = runtime_public(target, image, relocations, manifest, trail)
                address = code_cache[target]['address']; target_segment = code_cache[target]['segment']
        elif kind == 'segment':
            require(fix['target_method'] == 0 and target in placements and
                    segs[target]['index'] == fix['target_index'], 'Runtime target SEGDEF missing/invalid')
            require(0 <= addend < obj.segment_length(target), 'Runtime internal addend outside complete SEGDEF')
            address = placements[target]['start'] + addend; target_segment = target
        else:
            raise ValueError('Unsupported runtime target kind: '+kind)
        if target_segment == '_TEXT': target_frame = text_frame
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
            require(frame == dgroup_frame, 'Runtime absolute code offset needs explicit pointer proof')
            require(0 <= address-frame <= 65535, 'Runtime DGROUP offset overflow')
            struct.pack_into('<H',payload,at,address-frame)
        elif fix['loc'] == 'pointer32' and width == 4:
            require(segment == '_TEXT' and target_segment == '_TEXT' and at >= 1 and
                    payload[at-1] in (0x9a,0xea) and encoded == bytes(4) and addend == 0,
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
            require(bytes(payloads[name]) == image[p['start']:p['end']],
                    'Bound complete runtime segment differs from oracle: '+name)
        storage.append({'segment':name, 'start':p['start'], 'end':p['end'],
                        'ownership':p['ownership'], 'initialized':name in payloads})
    return bytes(payloads[owner['segment']]), {'mode':'runtime-member-v1', 'fixups':fix_receipts,
            'generated_relocations':expected, 'storage':storage}
