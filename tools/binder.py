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
    require(recipe['expected_relocations'] == [], 'Relocating contributions not yet supported')
    if not recipe['expected_fixups']:
        require('binding' not in recipe, 'Unexpected binding for fixup-free recipe')
        return extract_no_fixups(*args), {'mode': 'no-fixups', 'generated_relocations': []}
    binding = recipe['binding']
    require(binding['mode'] == 'external-dgroup-offset16-v1', 'Unsupported recipe binding mode')
    return bind_data_offsets(*args, recipe['expected_fixups'], binding['declarations'], symbols)
