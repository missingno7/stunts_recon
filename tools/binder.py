"""Bounded OMF external DGROUP offset16 binding; all other modes fail closed.

This performs linker relocation on a complete compiler contribution. It never
reads desired operand bytes to choose a value, trims output, or patches an MZ.
"""
import struct
from common import require
from object_probe import extract_no_fixups


_FRAME_CALLBACK_OBJECT = bytes.fromhex(
    'c70600000000b80000ba000052509a0000000083c404c606000000cb')
_FRAME_CALLBACK_FIXUPS = [
    (24, 'offset16', 2, '_byte_442E4', 5),
    (15, 'pointer32', 4, '_timer_reg_callback', 3),
    (10, 'base16', 2, '_frame_callback', 2),
    (7, 'loader-offset16', 2, '_frame_callback', 2),
    (2, 'offset16', 2, '_word_46468', 4),
]
_FRAME_CALLBACK_EXTERNALS = [
    '__acrtused', '_frame_callback', '_timer_reg_callback',
    '_word_46468', '_byte_442E4', '_set_frame_callback',
]
_FRAME_CALLBACK_SEGMENTS = [
    {'index': index, 'name': name, 'class': segment_class, 'length': length,
     'alignment_code': 2, 'alignment': 'word', 'combine_code': 2,
     'combine': 'public', 'big': False, 'use_32bit_offset': False,
     'frame': None, 'offset': None, 'overlay_index': 1, 'acbp': 72}
    for index, name, segment_class, length in (
        (1, 'UNIT_TEXT', 'CODE', 28), (2, '_DATA', 'DATA', 0),
        (3, 'CONST', 'CONST', 0), (4, '_BSS', 'BSS', 0))
]
_FRAME_CALLBACK_SYMBOLS = {
    '_frame_callback': {'kind': 'far-code', 'frame_load_address': 0x11b70,
                        'load_address': 0x12596},
    '_timer_reg_callback': {'kind': 'far-code', 'frame_load_address': 0x1ea20,
                            'load_address': 0x202aa},
    '_word_46468': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                    'load_address': 0x36468},
    '_byte_442E4': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                    'load_address': 0x342e4},
}


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
    if recipe.get('binding',{}).get('mode') == 'external-frame-callback-v1':
        return bind_frame_callback(*args, recipe['expected_fixups'], recipe['binding']['declarations'],
                                   symbols, recipe['start'], recipe['expected_relocations'])
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


def bind_frame_callback(obj, segment, public, length, expected_fixups, declarations,
                        symbols, start, expected_relocations):
    """Bind only the reviewed complete 28-byte set_frame_callback contribution.

    The zero-addend object skeleton and ordered OMF obligations are fixed here;
    target addresses must also come from the independently checked resolvers.
    No original PUBDEF or translation-unit ownership is inferred.
    """
    require((segment, public, length, start) ==
            ('UNIT_TEXT', '_set_frame_callback', 28, 0x1255a),
            'Frame callback identity/extent differs')
    require(obj.linker_fixups == expected_fixups and len(expected_fixups) == 5,
            'Complete ordered frame callback FIXUPP differs')
    require(declarations == {'segments': obj.segment_defs, 'groups': obj.groups,
                             'publics': obj.publics, 'externals': obj.externals},
            'Frame callback declarations differ')
    require(obj.publics == [{'name': public, 'segment': segment, 'offset': 0}] and
            obj.externals == _FRAME_CALLBACK_EXTERNALS,
            'Frame callback public/external declarations differ')
    require(obj.segment_defs == _FRAME_CALLBACK_SEGMENTS and
            obj.groups == [{'index': 1, 'name': 'DGROUP', 'segment_indices': [3, 4, 2],
                            'segments': ['CONST', '_BSS', '_DATA']}],
            'Frame callback segment/group declarations differ')
    require(obj.segment_length(segment) == length and
            obj.segment_bytes(segment) == _FRAME_CALLBACK_OBJECT and
            all(name == segment or size == 0 for name, size in obj.segment_lengths.items()),
            'Incomplete or altered frame callback object contribution')
    require(type(symbols) is dict and symbols == _FRAME_CALLBACK_SYMBOLS,
            'Frame callback symbol kinds, frames or addresses differ')

    payload = bytearray(_FRAME_CALLBACK_OBJECT)
    occupied = set()
    rows = []
    relocations = []
    for fix, (at, loc, width, name, index) in zip(expected_fixups, _FRAME_CALLBACK_FIXUPS):
        require((fix['segment'], fix['offset'], fix['loc'], fix['width'], fix['target'],
                 fix['target_index']) == (segment, at, loc, width, name, index) and
                fix['target_kind'] == 'external' and fix['target_method'] == 2 and
                (fix['frame_method'], fix['frame_kind'], fix['frame'], fix['frame_index']) ==
                (5, 'target', name, 0) and not fix['self_relative'] and
                type(fix['displacement']) is int and fix['displacement'] == 0 and
                fix['encoded_addend'] == '00' * width and
                obj.externals[index - 1] == name,
                'Unsupported frame callback FIXUPP mode, datum or addend')
        require(0 <= at <= length - width and
                not occupied.intersection(range(at, at + width)) and
                payload[at:at + width] == bytes(width),
                'Frame callback fixup overlaps or leaves object contribution')
        occupied.update(range(at, at + width))
        target = symbols[name]
        frame, address = target['frame_load_address'], target['load_address']
        require(type(frame) is int and type(address) is int and
                0 <= frame <= 0xffff0 and frame % 16 == 0 and
                0 <= address - frame <= 0xffff,
                'Invalid frame callback symbol frame/address')
        if loc == 'offset16':
            require(target['group'] == 'DGROUP', 'Frame callback data target is not DGROUP')
            value = address - frame
            struct.pack_into('<H', payload, at, value)
        elif loc == 'pointer32':
            require(target['kind'] == 'far-code' and payload[at - 1] == 0x9a,
                    'Frame callback pointer32 is not a far CALL')
            value = (address - frame, frame // 16)
            struct.pack_into('<HH', payload, at, *value)
            relocations.append({'segment': 4096, 'offset': start + at + 2 - 65536,
                                'load_offset': start + at + 2})
        elif loc == 'base16':
            require(target['kind'] == 'far-code' and payload[at - 1] == 0xba,
                    'Frame callback base16 is not the reviewed MOV DX immediate')
            value = frame // 16
            struct.pack_into('<H', payload, at, value)
            relocations.append({'segment': 4096, 'offset': start + at - 65536,
                                'load_offset': start + at})
        else:
            require(loc == 'loader-offset16' and target['kind'] == 'far-code' and
                    payload[at - 1] == 0xb8,
                    'Frame callback loader offset is not the reviewed MOV AX immediate')
            value = address - frame
            struct.pack_into('<H', payload, at, value)
        rows.append({'offset': at, 'loc': loc, 'target': name, 'linked_value': value})
    require(relocations == expected_relocations and
            [r['load_offset'] for r in relocations] == [0x1256b, 0x12564] and
            all(set(r) == {'segment', 'offset', 'load_offset'} and
                0 <= r['segment'] <= 0xffff and 0 <= r['offset'] <= 0xffff and
                r['segment'] * 16 + r['offset'] == r['load_offset'] for r in relocations),
            'Frame callback ordered MZ relocation coordinates differ')
    return bytes(payload), {'mode': 'external-frame-callback-v1', 'fixups': rows,
                            'generated_relocations': relocations}
