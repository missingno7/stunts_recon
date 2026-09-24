"""Bounded OMF external DGROUP offset16 binding; all other modes fail closed.

This performs linker relocation on a complete compiler contribution. It never
reads desired operand bytes to choose a value, trims output, or patches an MZ.
"""
import struct
from common import require
from object_probe import extract_no_fixups


def bind_data_offsets(obj, segment, public, length, expected_fixups, declarations, symbols):
    require(obj.linker_fixups == expected_fixups and expected_fixups,
            'Complete ordered FIXUPP obligations differ or are empty')
    require(declarations == {'segments': obj.segment_defs, 'groups': obj.groups,
                             'publics': obj.publics, 'externals': obj.externals},
            'Object declarations differ from reviewed binding context')
    require(obj.publics == [{'name': public, 'segment': segment, 'offset': 0}],
            'Bound contribution must have exactly its single public at zero')
    require(obj.segment_length(segment) == length, 'Complete bound SEGDEF length differs')
    payload = bytearray(obj.segment_bytes(segment))
    require(len(payload) == length, 'Incomplete bound contribution')
    require(all(name == segment or size == 0 for name, size in obj.segment_lengths.items()),
            'Unowned initialized/BSS contribution')
    used = {fix['target'] for fix in expected_fixups}
    require(set(symbols) == used, 'Missing or unused external data binding')
    require(set(obj.externals) <= used | {'__acrtused', public}, 'Unexpected external declaration')
    require(len([g for g in obj.groups if g['name'] == 'DGROUP']) == 1, 'Missing/ambiguous DGROUP')
    occupied = set()
    rows = []
    for fix in expected_fixups:
        require(fix['segment'] == segment and fix['loc'] == 'offset16' and fix['width'] == 2
                and not fix['self_relative'] and fix['target_kind'] == 'external'
                and fix['target_method'] == 2,
                'Unsupported binding mode: only external segment-relative offset16')
        require((fix['frame_method'], fix['frame_kind'], fix['frame']) == (5, 'target', fix['target']),
                'Unsupported frame: only target-frame external DGROUP data is proven')
        require(fix['frame_index'] == 0 and 1 <= fix['target_index'] <= len(obj.externals)
                and obj.externals[fix['target_index']-1] == fix['target'], 'Invalid original FIXUPP datum')
        target = symbols[fix['target']]
        require(target['group'] == 'DGROUP', 'External target is not proven DGROUP data')
        base, address = target['frame_load_address'], target['load_address']
        require(type(base) is int and type(address) is int and base >= 0 and base % 16 == 0
                and 0 <= address - base <= 65535, 'Invalid DGROUP frame or external address')
        at = fix['offset']
        require(type(at) is int and 0 <= at <= length - 2, 'Fixup outside contribution')
        require(not occupied.intersection([at, at+1]), 'Overlapping binding obligations')
        occupied.update([at, at+1])
        encoded = bytes.fromhex(fix['encoded_addend'])
        require(len(encoded) == 2 and payload[at:at+2] == encoded, 'Encoded fixup addend differs')
        displacement = fix['displacement']
        require(type(displacement) is int and displacement == 0, 'Nonzero target displacement is not yet proven')
        value = address - base + displacement + int.from_bytes(encoded, 'little')
        # Wrapping/sign-extension cases are intentionally outside this proven subset.
        require(0 <= value <= 65535, 'Offset fixup overflow is not supported')
        struct.pack_into('<H', payload, at, value)
        rows.append({'offset': at, 'target': fix['target'], 'frame_load_address': base,
                     'target_load_address': address, 'displacement': displacement,
                     'encoded_addend': fix['encoded_addend'], 'linked_value': value})
    return bytes(payload), {'mode': 'external-dgroup-offset16-v1', 'fixups': rows,
                            'generated_relocations': []}


def bind_contribution(obj, recipe, symbols=None):
    args = (obj, recipe['object_segment'], recipe['public'], recipe['end'] - recipe['start'])
    if recipe.get('binding',{}).get('mode') == 'external-far-call-v1':
        return bind_far_calls(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                              recipe['start'], recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'external-far-call-dgroup-offset16-v1':
        return bind_mixed_far_data(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                                   recipe['start'], recipe['expected_relocations'])
    require(recipe['expected_relocations'] == [], 'Relocating contributions not supported in this mode')
    if not recipe['expected_fixups']:
        require('binding' not in recipe, 'Unexpected binding for fixup-free recipe')
        return extract_no_fixups(*args), {'mode': 'no-fixups', 'generated_relocations': []}
    binding = recipe['binding']
    require(binding['mode'] == 'external-dgroup-offset16-v1', 'Unsupported recipe binding mode')
    return bind_data_offsets(*args, recipe['expected_fixups'], binding['declarations'], symbols)


def bind_far_calls(obj, segment, public, length, expected_fixups, declarations,
                   symbols, start, expected_relocations):
    """Bounded unoptimized intersegment CALL; no far-to-near rewriting.

    Only zero-addend external pointer32 at a 9A operand, target frame, is proven.
    The offset and paragraph are derived from reviewed symbol/frame identities.
    MZ entry representation/order remains an explicit hybrid layout obligation.
    """
    require(expected_fixups and obj.linker_fixups==expected_fixups, 'Complete ordered far FIXUPP differs')
    require(declarations=={'segments':obj.segment_defs,'groups':obj.groups,
                          'publics':obj.publics,'externals':obj.externals}, 'Far object declarations differ')
    require(obj.publics==[{'name':public,'segment':segment,'offset':0}], 'Far contribution public mismatch')
    require(obj.segment_length(segment)==length and len(obj.segment_bytes(segment))==length,
            'Complete far contribution length differs')
    require(all(n==segment or size==0 for n,size in obj.segment_lengths.items()), 'Unowned far-call data/BSS')
    used={f['target'] for f in expected_fixups}
    require(set(symbols)==used and set(obj.externals)<=used|{'__acrtused',public}, 'Far external set differs')
    payload=bytearray(obj.segment_bytes(segment)); obligations=[]; occupied=set()
    for fix in expected_fixups:
        require((fix['segment'],fix['loc'],fix['width'],fix['self_relative'],fix['target_kind'],fix['target_method'])
                ==(segment,'pointer32',4,False,'external',2), 'Unsupported far fixup kind/width/target')
        require((fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])
                ==(5,'target',fix['target'],0), 'Unsupported far frame')
        require(1<=fix['target_index']<=len(obj.externals) and obj.externals[fix['target_index']-1]==fix['target'],
                'Invalid far external index')
        at=fix['offset']; require(type(at)is int and 1<=at<=length-4, 'Far operand outside contribution')
        require(not occupied.intersection(range(at,at+4)), 'Overlapping far fixups')
        occupied.update(range(at,at+4))
        require(payload[at-1]==0x9a, 'Far binding supports CALL operands only')
        require(fix['displacement']==0 and fix['encoded_addend']=='00000000' and payload[at:at+4]==bytes(4),
                'Nonzero far addend/displacement unsupported')
        target=symbols[fix['target']]; frame=target['frame_load_address']; address=target['load_address']
        require(target['kind']=='far-code' and type(frame)is int and type(address)is int and
                0<=frame<=0xffff0 and frame%16==0 and 0<=address-frame<=65535, 'Invalid far symbol/frame')
        struct.pack_into('<HH',payload,at,address-frame,frame//16)
        obligations.append({'load_offset':start+at+2,'target':fix['target'],
                            'offset':address-frame,'paragraph':frame//16})
    require([r['load_offset'] for r in expected_relocations]==[r['load_offset'] for r in obligations],
            'Ordered source relocation obligations differ')
    for entry in expected_relocations:
        require(set(entry)=={'segment','offset','load_offset'} and 0<=entry['segment']<=65535 and
                0<=entry['offset']<=65535 and entry['segment']*16+entry['offset']==entry['load_offset'],
                'Invalid MZ relocation representation')
    return bytes(payload), {'mode':'external-far-call-v1','fixups':obligations,
                           'generated_relocations':expected_relocations}


def bind_mixed_far_data(obj, segment, public, length, expected_fixups, declarations,
                        symbols, start, expected_relocations):
    """One external far CALL and external DGROUP offsets in one complete OMF contribution.

    FIXUPP order is retained. Only the far segment word creates an MZ relocation.
    No unsupported frame, addend, multiple public, or private data is inferred.
    """
    require(expected_fixups and obj.linker_fixups == expected_fixups,
            'Complete ordered mixed FIXUPP differs')
    require(declarations == {'segments': obj.segment_defs, 'groups': obj.groups,
                             'publics': obj.publics, 'externals': obj.externals},
            'Mixed object declarations differ')
    require(obj.publics == [{'name': public, 'segment': segment, 'offset': 0}],
            'Mixed contribution public mismatch')
    require(obj.segment_length(segment) == length and len(obj.segment_bytes(segment)) == length,
            'Complete mixed contribution length differs')
    require(all(n == segment or size == 0 for n, size in obj.segment_lengths.items()),
            'Unowned mixed contribution data/BSS')
    require(len([g for g in obj.groups if g['name'] == 'DGROUP']) == 1,
            'Missing/ambiguous DGROUP')
    used = {fix['target'] for fix in expected_fixups}
    require(set(symbols) == used and set(obj.externals) <= used | {'__acrtused', public},
            'Missing or unused mixed external binding')
    payload = bytearray(obj.segment_bytes(segment))
    occupied = set()
    far_rows, data_rows, fixup_rows, relocation_sites = [], [], [], []
    for fix in expected_fixups:
        require(fix['segment'] == segment and not fix['self_relative'] and
                fix['target_kind'] == 'external' and fix['target_method'] == 2 and
                (fix['frame_method'], fix['frame_kind'], fix['frame'], fix['frame_index']) ==
                (5, 'target', fix['target'], 0), 'Unsupported mixed fixup target/frame')
        require(1 <= fix['target_index'] <= len(obj.externals) and
                obj.externals[fix['target_index'] - 1] == fix['target'],
                'Invalid mixed external index')
        require(type(fix['displacement']) is int and fix['displacement'] == 0,
                'Nonzero mixed target displacement unsupported')
        at = fix['offset']
        require(type(at) is int, 'Invalid mixed fixup offset')
        target = symbols[fix['target']]
        if (fix['loc'], fix['width']) == ('pointer32', 4):
            require(1 <= at <= length - 4 and payload[at - 1] == 0x9a,
                    'Mixed far binding supports CALL operands only')
            require(fix['encoded_addend'] == '00000000' and payload[at:at + 4] == bytes(4),
                    'Nonzero mixed far addend unsupported')
            frame, address = target['frame_load_address'], target['load_address']
            require(target['kind'] == 'far-code' and type(frame) is int and type(address) is int and
                    0 <= frame <= 0xffff0 and frame % 16 == 0 and 0 <= address - frame <= 65535,
                    'Invalid mixed far symbol/frame')
            require(not occupied.intersection(range(at, at + 4)), 'Overlapping mixed fixups')
            occupied.update(range(at, at + 4))
            struct.pack_into('<HH', payload, at, address - frame, frame // 16)
            row = {'load_offset': start + at + 2, 'target': fix['target'],
                   'offset': address - frame, 'paragraph': frame // 16}
            far_rows.append(row)
            fixup_rows.append({'kind': 'far-call', **row})
            relocation_sites.append(start + at + 2)
        elif (fix['loc'], fix['width']) == ('offset16', 2):
            require(0 <= at <= length - 2, 'Mixed data fixup outside contribution')
            require(target['group'] == 'DGROUP', 'Mixed data target is not proven DGROUP')
            base, address = target['frame_load_address'], target['load_address']
            require(type(base) is int and type(address) is int and base >= 0 and base % 16 == 0 and
                    0 <= address - base <= 65535, 'Invalid mixed DGROUP frame/address')
            encoded = bytes.fromhex(fix['encoded_addend'])
            require(len(encoded) == 2 and payload[at:at + 2] == encoded,
                    'Mixed data encoded addend differs')
            require(not occupied.intersection(range(at, at + 2)), 'Overlapping mixed fixups')
            occupied.update(range(at, at + 2))
            value = address - base + int.from_bytes(encoded, 'little')
            require(0 <= value <= 65535, 'Mixed data offset overflow unsupported')
            struct.pack_into('<H', payload, at, value)
            row = {'offset': at, 'target': fix['target'], 'frame_load_address': base,
                   'target_load_address': address, 'displacement': 0,
                   'encoded_addend': fix['encoded_addend'], 'linked_value': value}
            data_rows.append(row)
            fixup_rows.append({'kind': 'dgroup-offset16', **row})
        else:
            require(False, 'Unsupported mixed fixup kind/width')
    require(len(far_rows) == 1 and data_rows, 'Mixed mode requires one far CALL and DGROUP offsets')
    require([r['load_offset'] for r in expected_relocations] == relocation_sites,
            'Ordered mixed source relocation obligations differ')
    for entry in expected_relocations:
        require(set(entry) == {'segment', 'offset', 'load_offset'} and
                0 <= entry['segment'] <= 65535 and 0 <= entry['offset'] <= 65535 and
                entry['segment'] * 16 + entry['offset'] == entry['load_offset'],
                'Invalid mixed MZ relocation representation')
    return bytes(payload), {'mode': 'external-far-call-dgroup-offset16-v1',
                            'fixups': fixup_rows,
                            'generated_relocations': expected_relocations}
