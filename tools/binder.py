"""Bounded OMF external DGROUP offset16 binding; all other modes fail closed.

This performs linker relocation on a complete compiler contribution. It never
reads desired operand bytes to choose a value, trims output, or patches an MZ.
"""
import struct
from common import require
from object_probe import extract_no_fixups, declared_externals


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
                    'load_address': 0x36468, 'allowed_addends': [0, 1]},
    '_byte_442E4': {'group': 'DGROUP', 'frame_load_address': 0x2b770,
                    'load_address': 0x342e4, 'allowed_addends': [0]},
}


def link_order_sites(sites):
    """Executable MZ order of relocation sites listed in candidate FIXUPP order.

    The pinned LINK (3.65 and 3.61, both pinned distributions) copies relocation-bearing
    FIXUPPs into the MZ table in the order the object emits them: ascending
    MASM 5.10 FIXUPPs stay ascending and descending MSC FIXUPPs stay
    descending (tools/link_order_probe.py; tests/test_link_order.py run the
    pinned LINK on both).  The oracle MZ table was then EXEPACKed, which
    regroups entries into 64 KiB banks in bank order while keeping the order
    inside each bank.  The input must be the candidate's own emitted order;
    this never consults the oracle order.
    """
    return sorted(sites, key=lambda site: site // 65536)


def fixup_relocation_sites(fixes, start, segment=None):
    """Load offsets of the MZ entries these FIXUPPs generate, in FIXUPP order."""
    sites = []
    for fix in fixes:
        if segment is not None and fix['segment'] != segment:
            continue
        if fix['loc'] == 'pointer32':
            sites.append(start + fix['offset'] + 2)
        elif fix['loc'] == 'base16':
            sites.append(start + fix['offset'])
    return sites


def require_relocations_from_fixups(obj, recipe, relocations):
    """Every original MZ-relocated word inside the contribution needs its own
    candidate FIXUPP generating that relocation (integ25 audit 12a).

    A candidate that emits the original absolute segment:offset literal (for
    example a far pointer constant such as 0x26af0002L) instead of a FIXUPP
    would reproduce the bytes while standing in for a relocation; it is a
    hard-coded program address and fails here, before any binding, for the
    CODE segment and every owned secondary segment alike."""
    spans = [(recipe['object_segment'], recipe['start'], recipe['end'])]
    for name, spec in recipe.get('secondary_dgroup_segments', {}).items():
        if name != '_BSS':
            spans.append((name, spec['start'], spec['end']))
    for segment, start, end in spans:
        generated = set(fixup_relocation_sites(obj.linker_fixups, start, segment))
        for row in relocations:
            site = row['load_offset']
            if start <= site and site + 2 <= end:
                require(site in generated,
                        'Original MZ-relocated word at load offset %d has no candidate '
                        'FIXUPP: an absolute literal stands in for a relocation '
                        '(hard-coded program address)' % site)


def _checked_data_addend(target, encoded):
    addend = int.from_bytes(encoded, 'little')
    signed = int.from_bytes(encoded, 'little', signed=True)
    folded = target.get('folded_addends', {})
    if signed < 0 and signed in folded:
        proof = folded[signed]
        stride, field = proof['stride'], proof['field_offset']
        lower, upper = proof['index_bound']
        width = target['width']
        require(stride > 0 and 0 <= field < stride and
                signed == -lower*stride + field and
                0 <= field and (upper-lower)*stride+field+2 <= width,
                'Folded DGROUP addend escapes reviewed indexed object')
        return signed
    if signed < 0 and signed in target.get('negative_folded_addends', {}):
        # Reviewed per FIXUPP by data_symbols.check_negative_folds (integ28).
        return signed
    require(addend in target.get('allowed_addends', [0]),
            'DGROUP addend leaves independently grounded object/field')
    return addend


def bind_data_offsets(obj, segment, public, length, expected_fixups, declarations, symbols,
                      asm=False):
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
    if asm:
        used |= {fix['frame'] for fix in expected_fixups if fix['frame_method'] == 2}
    require(set(symbols) == used, 'Missing or unused external data binding')
    require(declared_externals(obj) <= used | {'__acrtused', public}, 'Unexpected external declaration')
    dgroup, = [g for g in obj.groups if g['name'] == 'DGROUP']
    occupied = set()
    rows = []
    for fix in expected_fixups:
        require(fix['segment'] == segment and fix['loc'] == 'offset16' and fix['width'] == 2
                and not fix['self_relative'] and fix['target_kind'] == 'external'
                and fix['target_method'] == 2,
                'Unsupported binding mode: only external segment-relative offset16')
        frame = (fix['frame_method'], fix['frame_kind'], fix['frame'], fix['frame_index'])
        if asm and fix['frame_method'] == 1:
            require(frame == (1, 'group', 'DGROUP', dgroup['index']),
                    'ASM group frame is not the reviewed DGROUP')
        elif asm and fix['frame_method'] == 0:
            definitions = [d for d in obj.segment_defs if d['name'] == fix['frame']]
            target_symbol=symbols[fix['target']]
            require(len(definitions) == 1 and fix['frame'] == 'DSEG' and
                    frame == (0, 'segment', 'DSEG', definitions[0]['index']) and
                    definitions[0]['index'] in dgroup['segment_indices'] and
                    ((definitions[0]['length'] == 0 and
                      target_symbol.get('dseg_frame_proof') == 'pinned-dgroup-base' and
                      target_symbol.get('dseg_frame_load_address') ==
                      target_symbol['frame_load_address']) or
                     (definitions[0]['length'] > 0 and
                      target_symbol.get('dseg_frame_proof') == 'secondary-own-data' and
                      type(target_symbol.get('dseg_frame_load_address')) is int)),
                    'ASM DSEG frame lacks grounded DGROUP placement')
        elif asm and fix['frame_method'] == 2:
            require(fix['frame_kind'] == 'external' and
                    1 <= fix['frame_index'] <= len(obj.externals) and
                    obj.externals[fix['frame_index']-1] == fix['frame'] and
                    symbols[fix['frame']].get('group') == 'DGROUP',
                    'ASM external frame does not resolve inside DGROUP')
        else:
            require(frame == (5, 'target', fix['target'], 0),
                    'Unsupported external DGROUP frame')
        require(1 <= fix['target_index'] <= len(obj.externals)
                and obj.externals[fix['target_index']-1] == fix['target'], 'Invalid original FIXUPP datum')
        target = symbols[fix['target']]
        require(target['group'] == 'DGROUP', 'External target is not proven DGROUP data')
        base, address = (target['dseg_frame_load_address'] if asm and fix['frame_method']==0
                         else target['frame_load_address']), target['load_address']
        if asm and fix['frame_method'] == 2:
            require(symbols[fix['frame']]['frame_load_address'] == base,
                    'ASM external frame differs from target DGROUP paragraph')
        group_base=target['frame_load_address']
        require(type(base) is int and type(address) is int and
                type(group_base) is int and group_base>=0 and group_base%16==0 and
                group_base <= base < group_base+65536 and
                (asm and fix['frame_method']==0 or base%16==0) and
                0 <= address - base <= 65535,
                'Invalid DGROUP/DSEG frame or external address')
        at = fix['offset']
        require(type(at) is int and 0 <= at <= length - 2, 'Fixup outside contribution')
        require(not occupied.intersection([at, at+1]), 'Overlapping binding obligations')
        occupied.update([at, at+1])
        encoded = bytes.fromhex(fix['encoded_addend'])
        require(len(encoded) == 2 and payload[at:at+2] == encoded, 'Encoded fixup addend differs')
        displacement = fix['displacement']
        require(type(displacement) is int and
                (displacement == 0 or
                 (asm and type(target.get('width')) is int and
                  0 <= displacement < target['width'])),
                'Target displacement escapes independently grounded object')
        value = address - base + displacement + _checked_data_addend(target, encoded)
        # Wrapping/sign-extension cases are intentionally outside this proven subset.
        require(0 <= value <= 65535, 'Offset fixup overflow is not supported')
        struct.pack_into('<H', payload, at, value)
        rows.append({'offset': at, 'target': fix['target'], 'frame_load_address': base,
                     'target_load_address': address, 'displacement': displacement,
                     'encoded_addend': fix['encoded_addend'], 'linked_value': value})
    return bytes(payload), {'mode': 'asm-external-dgroup-offset16-v1' if asm else
                            'external-dgroup-offset16-v1', 'fixups': rows,
                            'generated_relocations': []}


def bind_contribution(obj, recipe, symbols=None):
    require(not getattr(obj,'local_publics',[]) and not getattr(obj,'local_externals',[]),
            'Local OMF symbols require a complete reviewed group recipe')
    require('relocation_order_basis' not in recipe,
            'Recipe cannot supply a relocation order basis')
    args = (obj, recipe['object_segment'], recipe['public'], recipe['end'] - recipe['start'])
    if (recipe.get('kind','c') == 'c' and
            recipe.get('binding',{}).get('mode') in ('external-far-call-v1',
                                                     'external-dgroup-offset16-v1') and
            any(f['target_kind']=='segment' and f['target']=='_DATA'
                for f in recipe['expected_fixups'])):
        # DGROUP DS reload (`_loadds`/`/Au`) composes per fixup (integ26).
        return bind_composed(obj,recipe,symbols)
    if recipe.get('binding',{}).get('mode') == 'external-frame-callback-v1':
        return bind_frame_callback(*args, recipe['expected_fixups'], recipe['binding']['declarations'],
                                   symbols, recipe['start'], recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'external-far-call-v1':
        return bind_far_calls(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                              recipe['start'], recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'asm-external-far-call-v1':
        require(recipe.get('kind') == 'asm', 'ASM far binding requires ASM recipe')
        if any(f['target_kind']=='segment' for f in recipe['expected_fixups']):
            return bind_composed(obj,recipe,symbols)
        return bind_far_calls(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                              recipe['start'], recipe['expected_relocations'],
                              asm_frame=recipe['original_frame_load_address'])
    if recipe.get('binding',{}).get('mode') == 'asm-external-cs-offset16-v1':
        require(recipe.get('kind') == 'asm', 'ASM CS binding requires ASM recipe')
        return bind_asm_cs_data(*args, recipe['expected_fixups'], recipe['binding']['declarations'],
                                symbols, recipe['original_frame_load_address'],
                                recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'asm-local-code-offset16-v1':
        require(recipe.get('kind') == 'asm' and 'members' not in recipe,
                'Local code offsets require one complete ASM contribution')
        return bind_asm_local_code_offsets(*args, recipe['expected_fixups'],
                                           recipe['binding']['declarations'],
                                           recipe['original_frame_load_address'],
                                           recipe['start'], recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'asm-external-near-transfer-v1':
        require(recipe.get('kind') == 'asm', 'Near transfer binding requires ASM recipe')
        return bind_asm_external_near(*args, recipe['expected_fixups'],
                                      recipe['binding']['declarations'], symbols,
                                      recipe['start'], recipe['original_frame_load_address'],
                                      recipe['expected_relocations'])
    if recipe.get('binding',{}).get('mode') == 'asm-external-far-call-self-base16-v1':
        require(recipe.get('kind') == 'asm', 'Self segment base requires ASM recipe')
        return bind_composed(obj,recipe,symbols)
    if recipe.get('binding',{}).get('mode') == 'asm-external-dgroup-offset16-v1' and (any(
            f['target_kind']=='segment' or f['self_relative']
            for f in recipe['expected_fixups']) or
            any(s.get('kind')=='cs-data' for s in (symbols or {}).values())):
        require(recipe.get('kind')=='asm','Composed ASM recipe required')
        return bind_composed(obj,recipe,symbols)
    if recipe.get('binding',{}).get('mode') == 'external-far-call-dgroup-offset16-v1':
        return bind_composed(obj,recipe,symbols)
    if recipe.get('binding',{}).get('mode') == 'external-far-call-code-pointer-dgroup-offset16-v1':
        return bind_composed(obj,recipe,symbols)
    if recipe.get('binding',{}).get('mode') == 'external-far-call-code-pointer-v1':
        require(recipe['id']=='remove_frame_callback', 'Unreviewed code-pointer-only contribution')
        return bind_mixed_far_data(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                                   recipe['start'], recipe['expected_relocations'],
                                   code_pointers=True, pointer_only=True)
    if recipe.get('binding',{}).get('mode') == 'external-far-call-cs-pointer-v1':
        return bind_cs_pointers(*args, recipe['expected_fixups'], recipe['binding']['declarations'], symbols,
                                recipe['start'], recipe['expected_relocations'])
    require(recipe['expected_relocations'] == [], 'Relocating contributions not supported in this mode')
    if not recipe['expected_fixups']:
        require('binding' not in recipe, 'Unexpected binding for fixup-free recipe')
        return extract_no_fixups(*args), {'mode': 'no-fixups', 'generated_relocations': []}
    binding = recipe['binding']
    if binding['mode'] == 'asm-external-dgroup-offset16-v1':
        require(recipe.get('kind') == 'asm', 'ASM DGROUP binding requires ASM recipe')
        if any(f['target_kind']=='segment' for f in recipe['expected_fixups']):
            return bind_composed(obj,recipe,symbols)
        return bind_data_offsets(*args, recipe['expected_fixups'], binding['declarations'],
                                 symbols, asm=True)
    require(binding['mode'] == 'external-dgroup-offset16-v1', 'Unsupported recipe binding mode')
    return bind_data_offsets(*args, recipe['expected_fixups'], binding['declarations'], symbols)


def bind_far_calls(obj, segment, public, length, expected_fixups, declarations,
                   symbols, start, expected_relocations, asm_frame=None):
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
    require(set(symbols)==used and declared_externals(obj)<=used|{'__acrtused',public}, 'Far external set differs')
    payload=bytearray(obj.segment_bytes(segment)); obligations=[]; occupied=set()
    for fix in expected_fixups:
        require((fix['segment'],fix['loc'],fix['width'],fix['self_relative'],fix['target_kind'],fix['target_method'])
                ==(segment,'pointer32',4,False,'external',2), 'Unsupported far fixup kind/width/target')
        if asm_frame is None:
            require((fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])
                    ==(5,'target',fix['target'],0), 'Unsupported far frame')
        else:
            definition, = [d for d in obj.segment_defs if d['name'] == segment]
            require((fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])
                    in ((0,'segment',segment,definition['index']),
                        (2,'external',fix['target'],fix['target_index'])) and
                    type(asm_frame) is int and asm_frame % 16 == 0 and
                    asm_frame <= start < asm_frame + 65536,
                    'ASM external far call lacks original same-segment frame')
        require(1<=fix['target_index']<=len(obj.externals) and obj.externals[fix['target_index']-1]==fix['target'],
                'Invalid far external index')
        at=fix['offset']; require(type(at)is int and 1<=at<=length-4, 'Far operand outside contribution')
        require(not occupied.intersection(range(at,at+4)), 'Overlapping far fixups')
        occupied.update(range(at,at+4))
        # External far CALL (9A) or far JMP thunk (EA) to a grounded code alias.
        require(payload[at-1] in (0x9a,0xea), 'Far binding supports CALL/JMP operands only')
        require(fix['displacement']==0 and fix['encoded_addend']=='00000000' and payload[at:at+4]==bytes(4),
                'Nonzero far addend/displacement unsupported')
        target=symbols[fix['target']]; frame=target['frame_load_address']; address=target['load_address']
        require(target['kind']=='far-code' and type(frame)is int and type(address)is int and
                0<=frame<=0xffff0 and frame%16==0 and 0<=address-frame<=65535, 'Invalid far symbol/frame')
        struct.pack_into('<HH',payload,at,address-frame,frame//16)
        obligations.append({'load_offset':start+at+2,'target':fix['target'],
                            'offset':address-frame,'paragraph':frame//16})
    expected_sites=[r['load_offset'] for r in expected_relocations]
    actual_sites=[r['load_offset'] for r in obligations]
    require(expected_sites==actual_sites,
            'Source relocation obligations differ')
    for entry in expected_relocations:
        require(set(entry)=={'segment','offset','load_offset'} and 0<=entry['segment']<=65535 and
                0<=entry['offset']<=65535 and entry['segment']*16+entry['offset']==entry['load_offset'],
                'Invalid MZ relocation representation')
    return bytes(payload), {'mode':'asm-external-far-call-v1' if asm_frame is not None else 'external-far-call-v1','fixups':obligations,
                           'generated_relocations':expected_relocations}


def bind_asm_local_code_offsets(obj, segment, public, length, expected_fixups,
                                declarations, frame, start, expected_relocations):
    """Bind local _TEXT table offsets from one complete ASM module."""
    require(expected_fixups and obj.linker_fixups == expected_fixups and
            declarations == {'segments':obj.segment_defs, 'groups':obj.groups,
                             'publics':obj.publics, 'externals':obj.externals},
            'Local code declarations/FIXUPPs differ')
    require(obj.publics == [{'name':public,'segment':segment,'offset':0}] and
            obj.externals == [] and obj.segment_length(segment) == length and
            len(obj.segment_bytes(segment)) == length and
            all(name == segment or size == 0 for name,size in obj.segment_lengths.items()) and
            expected_relocations == [], 'Local code module is incomplete')
    definition, = [d for d in obj.segment_defs if d['name'] == segment]
    require(type(frame) is int and frame % 16 == 0 and frame <= start and
            start + length <= frame + 65536, 'Local code frame/placement differs')
    payload=bytearray(obj.segment_bytes(segment)); occupied=set(); rows=[]
    for fix in expected_fixups:
        at=fix['offset']; displacement=fix['displacement']
        require((fix['segment'],fix['loc'],fix['width'],fix['self_relative'],
                 fix['target_kind'],fix['target'],fix['target_method'],fix['target_index'],
                 fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index']) ==
                (segment,'offset16',2,fix['self_relative'],'segment',segment,0,definition['index'],
                 0,'segment',segment,definition['index']) and
                type(displacement) is int and 0 <= displacement < length and
                type(at) is int and 0 <= at <= length-2 and
                fix['encoded_addend'] == '0000' and payload[at:at+2] == bytes(2) and
                not occupied.intersection((at,at+1)),
                'Unsupported or escaping local code-table offset')
        occupied.update((at,at+1))
        if fix['self_relative']:
            require(at>=1 and payload[at-1] in (0xe8,0xe9),
                    'Local relative fixup lacks near CALL/JMP')
            value=displacement-(at+2)
            require(-32768<=value<=32767,'Local relative displacement overflow')
            struct.pack_into('<h',payload,at,value)
        else:
            value=start-frame+displacement
            require(0 <= value <= 65535, 'Local code offset overflow')
            struct.pack_into('<H',payload,at,value)
        rows.append({'offset':at,'local_offset':displacement,'linked_value':value})
    return bytes(payload), {'mode':'asm-local-code-offset16-v1','fixups':rows,
                            'generated_relocations':[]}


def bind_asm_external_near(obj, segment, public, length, expected_fixups,
                           declarations, symbols, start, frame, expected_relocations):
    """Bind same-physical-segment external near CALL/JMP, with no MZ relocation."""
    require(expected_fixups and obj.linker_fixups == expected_fixups and
            declarations == {'segments':obj.segment_defs,'groups':obj.groups,
                             'publics':obj.publics,'externals':obj.externals},
            'Near transfer declarations/FIXUPPs differ')
    require(obj.publics == [{'name':public,'segment':segment,'offset':0}] and
            obj.segment_length(segment) == length and len(obj.segment_bytes(segment)) == length and
            all(name == segment or size == 0 for name,size in obj.segment_lengths.items()) and
            expected_relocations == [] and
            type(frame) is int and frame%16 == 0 and frame <= start < start+length <= frame+65536,
            'Near transfer complete extent, frame, or relocation differs')
    used = {f['target'] for f in expected_fixups}
    require(set(symbols) == used and declared_externals(obj) == used,
            'Near transfer external declarations differ')
    definition, = [d for d in obj.segment_defs if d['name'] == segment]
    payload = bytearray(obj.segment_bytes(segment)); occupied=set(); rows=[]
    for fix in expected_fixups:
        at=fix['offset']; target=fix['target']
        require((fix['segment'],fix['loc'],fix['width'],fix['self_relative'],
                 fix['target_kind'],fix['target_method']) ==
                (segment,'offset16',2,True,'external',2) and
                1 <= fix['target_index'] <= len(obj.externals) and
                obj.externals[fix['target_index']-1] == target and
                (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index']) in
                ((2,'external',target,fix['target_index']),
                 (5,'target',target,0),
                 (0,'segment',segment,definition['index'])) and
                fix['displacement'] == 0 and fix['encoded_addend'] == '0000' and
                type(at) is int and 1 <= at <= length-2 and
                payload[at-1] in (0xe8,0xe9) and payload[at:at+2] == bytes(2) and
                not occupied.intersection((at,at+1)),
                'Unsupported external near CALL/JMP datum')
        occupied.update((at,at+1))
        symbol=symbols[target]
        require(symbol['kind']=='near-code' and symbol['frame_load_address']==frame and
                frame <= symbol['load_address'] < frame+65536,
                'Near target is outside caller physical segment')
        displacement=symbol['load_address']-(start+at+2)
        require(-32768 <= displacement <= 32767, 'Near transfer displacement overflow')
        struct.pack_into('<h',payload,at,displacement)
        rows.append({'offset':at,'target':target,'displacement':displacement})
    return bytes(payload), {'mode':'asm-external-near-transfer-v1','fixups':rows,
                            'generated_relocations':[]}


def bind_asm_far_self_base(obj, segment, public, length, expected_fixups,
                           declarations, symbols, start, frame, expected_relocations):
    """Bind external far CALLs and the module's own segment base in FIXUPP order."""
    require(expected_fixups and obj.linker_fixups == expected_fixups and
            declarations == {'segments':obj.segment_defs,'groups':obj.groups,
                             'publics':obj.publics,'externals':obj.externals},
            'ASM self-base declarations/FIXUPPs differ')
    require(obj.publics == [{'name':public,'segment':segment,'offset':0}] and
            obj.segment_length(segment) == length and len(obj.segment_bytes(segment)) == length and
            all(name == segment or size == 0 for name,size in obj.segment_lengths.items()),
            'ASM self-base contribution incomplete')
    definition, = [d for d in obj.segment_defs if d['name'] == segment]
    require(type(frame) is int and frame % 16 == 0 and 0 <= frame <= 0xffff0 and
            frame <= start and start + length <= frame + 65536,
            'ASM self-base frame/placement differs')
    used={f['target'] for f in expected_fixups if f['target_kind']=='external'}
    require(set(symbols)==used and declared_externals(obj)==used,
            'ASM self-base external set differs')
    payload=bytearray(obj.segment_bytes(segment)); occupied=set(); rows=[]; sites=[]
    for fix in expected_fixups:
        at=fix['offset']; width=fix['width']
        require(fix['segment']==segment and not fix['self_relative'] and
                (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index']) ==
                (0,'segment',segment,definition['index']) and
                fix['displacement']==0 and type(at) is int and 1<=at<=length-width and
                not occupied.intersection(range(at,at+width)),
                'ASM self-base FIXUPP frame/extent differs')
        occupied.update(range(at,at+width))
        if fix['target_kind']=='segment':
            require((fix['loc'],width,fix['target'],fix['target_method'],fix['target_index']) ==
                    ('base16',2,segment,0,definition['index']) and
                    payload[at-1]==0xba and fix['encoded_addend']=='0000' and
                    payload[at:at+2]==bytes(2),
                    'Unsupported own-segment MOV DX base FIXUPP')
            value=frame//16
            struct.pack_into('<H',payload,at,value)
            site=start+at
        else:
            require(fix['target_kind']=='external' and fix['target_method']==2 and
                    1<=fix['target_index']<=len(obj.externals) and
                    obj.externals[fix['target_index']-1]==fix['target'] and
                    (fix['loc'],width)==('pointer32',4) and payload[at-1]==0x9a and
                    fix['encoded_addend']=='00000000' and payload[at:at+4]==bytes(4),
                    'Unsupported ASM external far CALL FIXUPP')
            target=symbols[fix['target']]
            base,address=target['frame_load_address'],target['load_address']
            require(target['kind']=='far-code' and type(base) is int and base%16==0 and
                    type(address) is int and 0<=address-base<=65535,
                    'Invalid ASM external far target')
            value=[address-base,base//16]
            struct.pack_into('<HH',payload,at,*value)
            site=start+at+2
        sites.append(site)
        rows.append({'offset':at,'target':fix['target'],'linked_value':value,
                     'relocation_site':site})
    require(any(f['target_kind']=='segment' for f in expected_fixups) and
            any(f['target_kind']=='external' for f in expected_fixups),
            'Self-base mode requires base and far call')
    # This mixed MASM form links in reverse FIXUPP order, as independently
    # witnessed by both audio driver timer modules.
    expected_sites=list(reversed(sites))
    require([r['load_offset'] for r in expected_relocations]==expected_sites and
            all(set(r)=={'segment','offset','load_offset'} and
                r['segment']*16+r['offset']==r['load_offset'] and
                0<=r['segment']<=65535 and 0<=r['offset']<=65535
                for r in expected_relocations),
            'ASM self-base ordered MZ relocations differ')
    return bytes(payload), {'mode':'asm-external-far-call-self-base16-v1',
                            'fixups':rows,'generated_relocations':expected_relocations}


def _cs_operand(payload, at):
    """A CS-override memory operand whose complete disp16 starts at `at`, or LEA.

    Reviewed forms: MOV/CMP/ADD/SUB and group-1 immediates, MOV Sreg,mem (8E
    with a segment-register field), ADD r16,mem (03) and IDIV mem (F7 /7)."""
    lea=at>=2 and payload[at-2:at] in (bytes.fromhex('8d36'),bytes.fromhex('8d3e'))
    modrm=payload[at-1] if at>=1 else 0
    memory=(modrm>>6)==2 or (modrm>>6)==0 and (modrm&7)==6
    opcode=payload[at-2] if at>=2 else None
    direct=(at>=3 and payload[at-3]==0x2e and memory and
            (opcode in (0x8a,0x8b,0x88,0x89,0x38,0x39,0x3a,0x3b,
                        0x2a,0x2b,0x80,0x81,0x83,0x03) or
             opcode==0x8e and ((modrm>>3)&7) in (0,1,2,3) or
             opcode==0xf7 and ((modrm>>3)&7)==7))
    direct=direct or (at>=2 and payload[at-2:at] in
                      tuple(bytes.fromhex(x) for x in ('2ea0','2ea1','2ea2','2ea3')))
    return lea or direct


def bind_asm_cs_data(obj, segment, public, length, expected_fixups, declarations,
                     symbols, frame, expected_relocations):
    """Bind reviewed CS data in a complete module, including direct memory operands."""
    require(expected_fixups and obj.linker_fixups == expected_fixups and
            declarations == {'segments':obj.segment_defs,'groups':obj.groups,
                             'publics':obj.publics,'externals':obj.externals},
            'ASM CS object declarations/FIXUPPs differ')
    require(obj.publics == [{'name':public,'segment':segment,'offset':0}] and
            obj.segment_length(segment)==length and len(obj.segment_bytes(segment))==length and
            all(name==segment or size==0 for name,size in obj.segment_lengths.items()),
            'ASM CS complete extent/public differs')
    used={f['target'] for f in expected_fixups}
    require(set(symbols)==used and declared_externals(obj)==used and
            expected_relocations==[], 'ASM CS external/relocation obligations differ')
    definition,=[d for d in obj.segment_defs if d['name']==segment]
    payload=bytearray(obj.segment_bytes(segment)); rows=[]; occupied=set()
    for fix in expected_fixups:
        at=fix['offset']; target=symbols[fix['target']]
        # Shared reviewed CS operand forms (see _cs_operand).
        lea=False
        direct=_cs_operand(payload,at)
        require((fix['segment'],fix['loc'],fix['width'],fix['self_relative'],
                 fix['target_kind'],fix['target_method'])==
                (segment,'offset16',2,False,'external',2) and
                (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                (0,'segment',segment,definition['index']) and
                1<=fix['target_index']<=len(obj.externals) and
                obj.externals[fix['target_index']-1]==fix['target'] and
                type(fix['displacement']) is int and
                0<=fix['displacement']<target['width'] and
                fix['encoded_addend']=='0000' and
                2<=at<=length-2 and (lea or direct) and
                payload[at:at+2]==bytes(2) and
                target['kind']=='cs-data' and target['frame_load_address']==frame and
                target['island_start']<=target['load_address']<
                target['load_address']+target['width']<=target['island_end'] and
                not occupied.intersection((at,at+1)),
                'Unsupported ASM CS data operand/fixup')
        occupied.update((at,at+1))
        value=target['load_address']-frame+fix['displacement']
        require(0<=value<=65535,'ASM CS offset exceeds segment')
        struct.pack_into('<H',payload,at,value)
        rows.append({'offset':at,'target':fix['target'],'linked_value':value})
    return bytes(payload),{'mode':'asm-external-cs-offset16-v1','fixups':rows,
                           'generated_relocations':[]}


def bind_mixed_far_data(obj, segment, public, length, expected_fixups, declarations,
                        symbols, start, expected_relocations, code_pointers=False,
                        pointer_only=False):
    """External far CALLs and DGROUP offsets in one complete OMF contribution.

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
    require(set(symbols) == used and declared_externals(obj) <= used | {'__acrtused', public},
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
        if fix['target'] == '__AHSHIFT':
            require(target.get('kind') == 'absolute-runtime-word' and
                    target.get('public') == '__AHSHIFT' and target.get('value') == 12 and
                    (fix['loc'],fix['width']) == ('loader-offset16',2) and
                    1 <= at <= length-2 and payload[at-1] == 0xb9 and
                    fix['encoded_addend'] == '0000' and payload[at:at+2] == bytes(2) and
                    not occupied.intersection(range(at,at+2)),
                    'Unsupported __AHSHIFT absolute runtime fixup')
            occupied.update(range(at,at+2))
            struct.pack_into('<H',payload,at,12)
            fixup_rows.append({'kind':'absolute-runtime-word','offset':at,
                               'target':'__AHSHIFT','linked_value':12})
        elif fix['target'] == '__AHINCR':
            # Pinned diffhlp.asm absolute PUBDEF 0x1000: C6 adds it only
            # to the far-pointer segment word on an offset carry.
            modrm = payload[at-2] if at >= 3 else 0
            require(target.get('kind') == 'absolute-runtime-word' and
                    target.get('public') == '__AHINCR' and target.get('value') == 4096 and
                    (fix['loc'],fix['width']) == ('loader-offset16',2) and
                    at >= 3 and payload[at-3] == 0x81 and
                    (modrm & 0x38) == 0 and (modrm >> 6) == 1 and (modrm & 7) == 6 and
                    fix['encoded_addend'] == '0000' and payload[at:at+2] == bytes(2) and
                    not occupied.intersection(range(at,at+2)),
                    'Unsupported __AHINCR absolute runtime fixup')
            occupied.update(range(at,at+2))
            struct.pack_into('<H',payload,at,4096)
            fixup_rows.append({'kind':'absolute-runtime-word','offset':at,
                               'target':'__AHINCR','linked_value':4096})
        elif (fix['loc'], fix['width']) == ('pointer32', 4):
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
        elif target.get('kind') == 'cs-data' and (fix['loc'], fix['width']) in (
                ('offset16', 2), ('base16', 2)):
            # Reviewed CS island datum named by an adjacent MOV AX offset /
            # MOV DX segment pair (same rule as the CS pointer mode).
            frame, address = target['frame_load_address'], target['load_address']
            require(1 <= at <= length-2 and fix['encoded_addend'] == '0000' and
                    payload[at:at+2] == bytes(2) and
                    payload[at-1] == (0xb8 if fix['loc'] == 'offset16' else 0xba) and
                    type(frame) is int and frame % 16 == 0 and
                    target['island_start'] <= address and
                    address + target['width'] <= target['island_end'] and
                    0 <= address - frame <= 65535 and
                    not occupied.intersection(range(at, at+2)),
                    'CS data pointer is outside verified island or MOV pair')
            occupied.update(range(at, at+2))
            value = address - frame if fix['loc'] == 'offset16' else frame // 16
            struct.pack_into('<H', payload, at, value)
            fixup_rows.append({'kind': 'cs-data-' + fix['loc'], 'offset': at,
                               'target': fix['target'], 'linked_value': value})
            if fix['loc'] == 'base16':
                relocation_sites.append(start + at)
        elif target.get('kind') == 'far-data' and (fix['loc'], fix['width']) in (
                ('offset16', 2), ('base16', 2)):
            # A public of an accepted FAR_DATA module (integ25): the offset
            # within its paragraph-aligned segment (addend inside the declared
            # object) and the segment word, each an immediate of MOV r16,imm16
            # or MOV [disp16],imm16.  Only the segment word is relocated.
            frame, address = target['frame_load_address'], target['load_address']
            encoded = bytes.fromhex(fix['encoded_addend'])
            addend = int.from_bytes(encoded, 'little')
            immediate = ((at >= 1 and 0xb8 <= payload[at-1] <= 0xbf) or
                         (at >= 4 and payload[at-4] == 0xc7 and payload[at-3] == 0x06))
            require(2 <= at + 2 <= length and immediate and len(encoded) == 2 and
                    payload[at:at+2] == encoded and type(frame) is int and frame % 16 == 0 and
                    type(address) is int and 0 <= address - frame <= 65535 and
                    (addend < target['width'] if fix['loc'] == 'offset16' else addend == 0) and
                    not occupied.intersection(range(at, at+2)),
                    'Unsupported far-data offset/segment field')
            occupied.update(range(at, at+2))
            value = address - frame + addend if fix['loc'] == 'offset16' else frame // 16
            struct.pack_into('<H', payload, at, value)
            fixup_rows.append({'kind': 'far-data-' + fix['loc'], 'offset': at,
                               'target': fix['target'], 'linked_value': value})
            if fix['loc'] == 'base16':
                relocation_sites.append(start + at)
        elif code_pointers and (fix['loc'], fix['width']) in (
                ('base16', 2), ('loader-offset16', 2)):
            require(1 <= at <= length-2 and target.get('kind') == 'far-code' and
                    fix['encoded_addend'] == '0000' and payload[at:at+2] == bytes(2),
                    'Unsupported far code pointer field')
            frame, address = target['frame_load_address'], target['load_address']
            require(type(frame) is int and type(address) is int and
                    0 <= frame <= 0xffff0 and frame % 16 == 0 and
                    0 <= address-frame <= 65535, 'Invalid far code pointer target')
            require(payload[at-1] == (0xba if fix['loc']=='base16' else 0xb8),
                    'Far code pointer lacks MOV immediate anchor')
            require(not occupied.intersection(range(at,at+2)), 'Overlapping code pointer')
            occupied.update(range(at,at+2))
            value = frame//16 if fix['loc']=='base16' else address-frame
            struct.pack_into('<H', payload, at, value)
            fixup_rows.append({'kind':'far-code-'+fix['loc'], 'offset':at,
                               'target':fix['target'], 'linked_value':value})
            if fix['loc']=='base16': relocation_sites.append(start+at)
        elif (fix['loc'], fix['width']) == ('offset16', 2):
            require(0 <= at <= length - 2, 'Mixed data fixup outside contribution')
            require(target.get('group') == 'DGROUP', 'Mixed data target is not proven DGROUP')
            base, address = target['frame_load_address'], target['load_address']
            require(type(base) is int and type(address) is int and base >= 0 and base % 16 == 0 and
                    0 <= address - base <= 65535, 'Invalid mixed DGROUP frame/address')
            encoded = bytes.fromhex(fix['encoded_addend'])
            require(len(encoded) == 2 and payload[at:at + 2] == encoded,
                    'Mixed data encoded addend differs')
            require(not occupied.intersection(range(at, at + 2)), 'Overlapping mixed fixups')
            occupied.update(range(at, at + 2))
            value = address - base + _checked_data_addend(target, encoded)
            require(0 <= value <= 65535, 'Mixed data offset overflow unsupported')
            struct.pack_into('<H', payload, at, value)
            row = {'offset': at, 'target': fix['target'], 'frame_load_address': base,
                   'target_load_address': address, 'displacement': 0,
                   'encoded_addend': fix['encoded_addend'], 'linked_value': value}
            data_rows.append(row)
            fixup_rows.append({'kind': 'dgroup-offset16', **row})
        else:
            require(False, 'Unsupported mixed fixup kind/width')
    cs_rows = [row for row in fixup_rows if row['kind'].startswith('cs-data-')]
    # The pinned absolute __AHSHIFT word (huge-pointer shift, MOV CX,imm) is a
    # reviewed non-far obligation of its own: far CALLs plus __AHSHIFT without
    # any DGROUP data is a complete mixed object as well (seg029, integ25).
    absolute_rows = [row for row in fixup_rows if row['kind'] == 'absolute-runtime-word' or
                     row['kind'].startswith('far-data-')]
    require(far_rows and (not pointer_only and (data_rows or cs_rows or absolute_rows) or
                          pointer_only and code_pointers and not data_rows and not cs_rows),
            'Mixed mode far CALL/data or pointer-only obligations differ')
    cs_pairs = {}
    for row in fixup_rows:
        if row['kind'] in ('cs-data-offset16', 'cs-data-base16'):
            cs_pairs.setdefault(row['target'], {'offset16': [], 'base16': []})[
                row['kind'][8:]].append(row['offset'])
    require(all(sorted(x+3 for x in v['offset16']) == sorted(v['base16'])
                for v in cs_pairs.values()),
            'CS offset/base fixups are not complete adjacent MOV pairs')
    if code_pointers:
        pairs = {}
        for row in fixup_rows:
            if row['kind'] in ('far-code-base16','far-code-loader-offset16'):
                pairs.setdefault(row['target'], {'base16':[], 'loader-offset16':[]})[
                    row['kind'][9:]].append(row['offset'])
        # The code-pointer mode is selected by segment-word/offset shapes; the
        # same shapes may belong to far data or the absolute __AHSHIFT word, so
        # an object without an actual code pointer pair is an ordinary mixed
        # object (pointer-only objects still need one).
        require((pairs or not pointer_only) and
                all(len(v['base16'])==len(v['loader-offset16']) for v in pairs.values()),
                'Unpaired far code pointer fields')
        for fields in pairs.values():
            require(sorted(x-3 for x in fields['base16']) == sorted(fields['loader-offset16']),
                    'Far code pointer words are not adjacent MOV immediates')
    # Ordered relocation sites must follow this object's FIXUPP traversal.
    expected_sites=[r['load_offset'] for r in expected_relocations]
    require(expected_sites==relocation_sites,
            'Mixed source relocation obligations differ')
    for entry in expected_relocations:
        require(set(entry) == {'segment', 'offset', 'load_offset'} and
                0 <= entry['segment'] <= 65535 and 0 <= entry['offset'] <= 65535 and
                entry['segment'] * 16 + entry['offset'] == entry['load_offset'],
                'Invalid mixed MZ relocation representation')
    return bytes(payload), {'mode': ('external-far-call-code-pointer-v1' if pointer_only else
                                     'external-far-call-code-pointer-dgroup-offset16-v1'
                                      if code_pointers else 'external-far-call-dgroup-offset16-v1'),
                            'fixups': fixup_rows,
                            'generated_relocations': expected_relocations}


def bind_composed(obj, recipe, symbols):
    """Bind each reviewed OMF fixup by its own target and frame rule."""
    mode=recipe['binding']['mode']; fixes=recipe['expected_fixups']
    segment=recipe['object_segment']; start=recipe['start']; length=recipe['end']-start
    asm=recipe.get('kind')=='asm'
    if all(f['target_kind']=='external' and not f['self_relative'] and
           (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
           (5,'target',f['target'],0) for f in fixes):
        return bind_mixed_far_data(obj,segment,recipe['public'],length,fixes,
                                   recipe['binding']['declarations'],symbols,start,
                                   recipe['expected_relocations'],
                                   code_pointers='code-pointer' in mode)
    # A complete single MSC C module may carry same-segment offset16 code-table
    # entries (switch tables): MSC stores the in-segment offset in the LEDATA
    # word with a zero FIXUPP displacement.  They bind only to the module's own
    # segment at its original frame and must stay inside the module.
    # It may also reload DS from DGROUP (`_loadds` / `/Au`): a base16 FIXUPP to
    # its zero-length _DATA with the DGROUP group frame (integ26).
    c_local = (not asm and 'members' not in recipe and
               any(f['target_kind']=='segment' for f in fixes) and
               all(f['target_kind']=='external' or
                   (f['target_kind']=='segment' and f['target']==segment and
                    f['loc']=='offset16' and not f['self_relative'] and
                    f['displacement']==0) or
                   (f['target_kind']=='segment' and f['target']=='_DATA' and
                    f['loc']=='base16' and
                    (symbols or {}).get('_DATA',{}).get('kind')=='local-dgroup-base')
                   for f in fixes))
    require((asm or c_local) and fixes and obj.linker_fixups==fixes and
            recipe['binding']['declarations']==
            {'segments':obj.segment_defs,'groups':obj.groups,
             'publics':obj.publics,'externals':obj.externals},
            'Composed OMF declarations/FIXUPPs differ')
    require(obj.publics==[{'name':recipe['public'],'segment':segment,'offset':0}] and
            obj.segment_length(segment)==length and len(obj.segment_bytes(segment))==length and
            all(name==segment or size==0 for name,size in obj.segment_lengths.items()),
            'Composed module extent/public differs')
    definition,=[d for d in obj.segment_defs if d['name']==segment]
    frame=recipe['original_frame_load_address']
    require(type(frame) is int and frame%16==0 and 0<=frame<=0xffff0 and
            frame<=start and start+length<=frame+65536,
            'Composed module placement differs')
    external={f['target'] for f in fixes if f['target_kind']=='external'}
    local={f['target'] for f in fixes if f['target_kind']=='segment'}
    frame_dseg={'DSEG'} if any(f['frame_method']==0 and f['frame']=='DSEG'
                                for f in fixes) else set()
    require(set(symbols)==external|local|frame_dseg and
            declared_externals(obj)<=external|{'__acrtused',recipe['public']},
            'Composed symbol set differs')
    groups=[g for g in obj.groups if g['name']=='DGROUP']
    payload=bytearray(obj.segment_bytes(segment)); occupied=set(); rows=[]; sites=[]
    code_pointer_fields={}; cs_pointer_fields={}
    for fix in fixes:
        at=fix['offset']; width=fix['width']; target_name=fix['target']
        require(fix['segment']==segment and
                (not fix['self_relative'] or
                 fix['loc']=='offset16' and
                 (fix['target_kind']=='external' or
                  fix['target_kind']=='segment' and fix['target']==segment)) and
                type(at) is int and 0<=at<=length-width and
                not occupied.intersection(range(at,at+width)),
                'Composed fixup escapes or overlaps module')
        occupied.update(range(at,at+width))
        encoded=bytes.fromhex(fix['encoded_addend'])
        require(len(encoded)==width and payload[at:at+width]==encoded,
                'Composed encoded addend differs')
        if fix['target_kind']=='segment':
            if fix['loc']=='pointer32':
                require(target_name==segment and width==4 and
                        fix['target_method']==0 and
                        fix['target_index']==definition['index'] and
                        (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                        (0,'segment',segment,definition['index']) and
                        type(fix['displacement']) is int and
                        0<=fix['displacement']<length and encoded==bytes(4) and
                        at>=1 and payload[at-1] in (0x9a,0xea) and
                        symbols[segment]['kind']=='local-text',
                        'Composed in-module far transfer differs')
                value=[start-frame+fix['displacement'],frame//16]
                require(0<=value[0]<=65535,'Composed internal far offset overflows')
                struct.pack_into('<HH',payload,at,*value)
                sites.append(start+at+2)
                rows.append({'offset':at,'target':target_name,'linked_value':value})
                continue
            if fix['loc']=='offset16':
                local_offset=(fix['displacement'] if asm else
                              int.from_bytes(encoded,'little') if len(encoded)==2 else -1)
                require(target_name==segment and width==2 and
                        fix['target_method']==0 and
                        fix['target_index']==definition['index'] and
                        ((fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                         (0,'segment',segment,definition['index']) or
                         not asm and (fix['frame_method'],fix['frame_kind'],fix['frame'],
                                      fix['frame_index'])==(5,'target',segment,0)) and
                        type(fix['displacement']) is int and
                        0<=fix['displacement']<length and 0<=local_offset<length and
                        (encoded==b'\0\0' if asm else
                         fix['displacement']==0 and not fix['self_relative']) and
                        symbols[segment]['kind']=='local-text',
                        'Composed local code target differs')
                if fix['self_relative']:
                    require(at>=1 and payload[at-1] in (0xe8,0xe9),
                            'Composed local relative transfer differs')
                    value=fix['displacement']-(at+2)
                    require(-32768<=value<=32767,'Composed local relative overflow')
                    struct.pack_into('<h',payload,at,value)
                else:
                    value=start-frame+local_offset
                    require(0<=value<=65535,'Composed local offset overflow')
                    struct.pack_into('<H',payload,at,value)
                rows.append({'offset':at,'target':target_name,'linked_value':value})
                continue
            dgroup_base=(not asm and target_name=='_DATA' and len(groups)==1 and
                         (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                         (1,'group','DGROUP',groups[0]['index']))
            require(fix['loc']=='base16' and width==2 and encoded==b'\0\0' and
                    fix['target_method']==0 and fix['target_index']>0 and
                    (dgroup_base or
                     fix['frame_method']==0 and fix['frame_kind']=='segment' and
                     fix['frame']==target_name and fix['frame_index']==fix['target_index']) and
                    fix['displacement']==0 and at>=1 and 0xb8<=payload[at-1]<=0xbf,
                    'Unsupported composed self-segment base')
            definition_target=[d for d in obj.segment_defs if d['index']==fix['target_index']
                               and d['name']==target_name]
            require(len(definition_target)==1 and target_name in (segment,'DSEG','_DATA'),
                    'Composed base lacks own segment declaration')
            if target_name=='_DATA':
                # MOV r16,DGROUP: the zero-length _DATA of this C object is a
                # DGROUP member; the value is the independently grounded DGROUP
                # paragraph and the word needs its own ordered MZ relocation.
                require(dgroup_base and definition_target[0]['length']==0 and
                        definition_target[0]['class']=='DATA' and
                        fix['target_index'] in groups[0]['segment_indices'] and
                        symbols['_DATA']['kind']=='local-dgroup-base',
                        'Composed _DATA base lacks grounded DGROUP placement')
                base=symbols['_DATA']['frame_load_address']
            elif target_name=='DSEG':
                require(definition_target[0]['length']==0 and
                        definition_target[0]['class'] in ('STUNTSD','DATA') and
                        symbols['DSEG']['kind']=='local-dseg-base',
                        'Composed DSEG base lacks independent frame proof')
                base=symbols['DSEG']['frame_load_address']
            else:
                base=frame
            require(base%16==0 and 0<=base<=0xffff0,
                    'Invalid composed segment paragraph')
            value=base//16; struct.pack_into('<H',payload,at,value)
            sites.append(start+at)
        else:
            require(fix['target_kind']=='external' and fix['target_method']==2 and
                    1<=fix['target_index']<=len(obj.externals) and
                    obj.externals[fix['target_index']-1]==target_name,
                    'Composed external datum differs')
            target=symbols[target_name]
            frame_tuple=(fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])
            if target_name=='__AHSHIFT':
                # The pinned absolute runtime word, per fixup exactly as in the
                # mixed binder: a zero-addend MOV CX,imm16 (B9) receiving 12,
                # with no MZ relocation (integ28, whole seg005).
                require(target.get('kind')=='absolute-runtime-word' and
                        target.get('public')=='__AHSHIFT' and target.get('value')==12 and
                        fix['loc']=='loader-offset16' and width==2 and
                        not fix['self_relative'] and fix['displacement']==0 and
                        encoded==bytes(2) and at>=1 and payload[at-1]==0xb9 and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index'])),
                        'Unsupported composed __AHSHIFT absolute runtime fixup')
                value=12
                struct.pack_into('<H',payload,at,value)
            elif target_name=='__AHINCR':
                # Pinned diffhlp.asm absolute PUBDEF 0x1000: C6 adds it only
                # to the far-pointer segment word on an offset carry.
                modrm=payload[at-2] if at>=3 else 0
                require(target.get('kind')=='absolute-runtime-word' and
                        target.get('public')=='__AHINCR' and target.get('value')==4096 and
                        fix['loc']=='loader-offset16' and width==2 and
                        not fix['self_relative'] and fix['displacement']==0 and
                        encoded==bytes(2) and at>=3 and payload[at-3]==0x81 and
                        (modrm & 0x38)==0 and (modrm >> 6)==1 and (modrm & 7)==6 and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index'])),
                        'Unsupported composed __AHINCR absolute runtime fixup')
                value=4096
                struct.pack_into('<H',payload,at,value)
            elif fix['self_relative']:
                require(fix['loc']=='offset16' and width==2 and
                        fix['displacement']==0 and encoded==bytes(2) and
                        at>=1 and payload[at-1] in (0xe8,0xe9) and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index']),
                                        (0,'segment',segment,definition['index'])) and
                        target['kind']=='near-code' and
                        target['frame_load_address']==frame and
                        frame<=target['load_address']<frame+65536,
                        'Composed external near transfer differs')
                value=target['load_address']-(start+at+2)
                require(-32768<=value<=32767,'Composed near displacement overflow')
                struct.pack_into('<h',payload,at,value)
            elif fix['loc']=='pointer32':
                # A far CALL/JMP operand, or (integ30) an entry of a reviewed
                # far-pointer table island naming this alias's mapped entry.
                require(width==4 and at>=1 and
                        (payload[at-1] in (0x9a,0xea) or
                         start+at in target.get('table_sites',())) and
                        encoded==bytes(4) and fix['displacement']==0 and
                        target['kind']=='far-code' and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index']),
                                        (0,'segment',segment,definition['index'])),
                        'Unsupported composed far CALL')
                base=target['frame_load_address']; address=target['load_address']
                require(type(base) is int and base%16==0 and 0<=base<=0xffff0 and
                        type(address) is int and 0<=address-base<=65535,
                        'Composed far target frame/offset differs')
                value=[address-base,base//16]
                struct.pack_into('<HH',payload,at,*value); sites.append(start+at+2)
            elif (not asm and target.get('kind')=='cs-data' and
                  fix['loc'] in ('offset16','base16')):
                # The mixed binder's CS-island pair, per fixup (integ26): a
                # zero-addend MOV AX,offset / MOV DX,seg naming a reviewed
                # island datum; the segment word carries its own MZ relocation.
                base=target['frame_load_address']; address=target['load_address']
                require(width==2 and encoded==bytes(2) and fix['displacement']==0 and at>=1 and
                        payload[at-1]==(0xb8 if fix['loc']=='offset16' else 0xba) and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index'])) and
                        type(base) is int and base%16==0 and 0<=base<=0xffff0 and
                        target['island_start']<=address and
                        address+target['width']<=target['island_end'] and
                        0<=address-base<=65535,
                        'Composed CS data pointer differs')
                value=address-base if fix['loc']=='offset16' else base//16
                struct.pack_into('<H',payload,at,value)
                cs_pointer_fields.setdefault(target_name,{'offset16':[],'base16':[]})[
                    fix['loc']].append(at)
                if fix['loc']=='base16': sites.append(start+at)
            elif fix['loc']=='offset16' and target.get('kind')=='cs-data':
                # Same rule as the complete ASM CS-data mode: an own-segment
                # frame, a CS override memory operand with a complete disp16
                # (or LEA), an in-object displacement inside the reviewed island.
                # A MOV r16,imm16 offset (B8+r) of the island may also name it
                # (e.g. `mov di,offset sprite1` after ES=CS), framed by the
                # own segment or by the island symbol itself (integ26).
                mov_immediate=at>=1 and 0xb8<=payload[at-1]<=0xbf
                require(width==2 and encoded==bytes(2) and asm and
                        (_cs_operand(payload, at) or mov_immediate) and
                        (frame_tuple==(0,'segment',segment,definition['index']) or
                         mov_immediate and
                         frame_tuple==(2,'external',target_name,fix['target_index'])) and
                        type(fix['displacement']) is int and
                        0<=fix['displacement']<target['width'] and
                        target['frame_load_address']==frame and
                        target['island_start']<=target['load_address']<
                        target['load_address']+target['width']<=target['island_end'],
                        'Composed CS data operand differs')
                value=target['load_address']-frame+fix['displacement']
                require(0<=value<=65535,'Composed CS data offset exceeds segment')
                struct.pack_into('<H',payload,at,value)
            elif fix['loc']=='offset16' and target.get('kind')=='near-code':
                require(width==2 and fix['displacement']==0 and encoded==bytes(2) and
                        frame_tuple in ((2,'external',target_name,fix['target_index']),
                                        (0,'segment',segment,definition['index'])) and
                        target['frame_load_address']==frame and
                        frame<=target['load_address']<frame+65536,
                        'Composed external CODE offset differs')
                value=target['load_address']-frame
                struct.pack_into('<H',payload,at,value)
            elif fix['loc']=='offset16':
                require(width==2 and target.get('group')=='DGROUP' and len(groups)==1,
                        'Composed data offset lacks DGROUP target')
                if fix['frame_method']==0:
                    dseg=[d for d in obj.segment_defs if d['name']=='DSEG']
                    require(len(dseg)==1 and dseg[0]['class']=='STUNTSD' and
                            frame_tuple==(0,'segment','DSEG',dseg[0]['index']) and
                            dseg[0]['length']==0 and
                            target.get('dseg_frame_proof')=='pinned-dgroup-base',
                            'Composed F0 DSEG frame differs')
                    base=target['dseg_frame_load_address']
                elif fix['frame_method']==1:
                    require(frame_tuple==(1,'group','DGROUP',groups[0]['index']),
                            'Composed F1 DGROUP frame differs')
                    base=target['frame_load_address']
                elif fix['frame_method']==2:
                    require(fix['frame_kind']=='external' and
                            1<=fix['frame_index']<=len(obj.externals) and
                            obj.externals[fix['frame_index']-1]==fix['frame'] and
                            symbols[fix['frame']].get('group')=='DGROUP',
                            'Composed F2 DGROUP frame differs')
                    base=symbols[fix['frame']]['frame_load_address']
                else:
                    require(frame_tuple==(5,'target',target_name,0),
                            'Composed target frame differs')
                    base=target['frame_load_address']
                address=target['load_address']; displacement=fix['displacement']
                require(type(base) is int and type(address) is int and
                        type(displacement) is int and
                        (displacement==0 or type(target.get('width')) is int and
                         0<=displacement<target['width']) and
                        0<=address-base<=65535,
                        'Composed data target or object addend escapes')
                value=address-base+displacement+_checked_data_addend(target,encoded)
                require(0<=value<=65535,'Composed data offset overflows')
                struct.pack_into('<H',payload,at,value)
            elif ('code-pointer' in mode and fix['loc'] in ('base16','loader-offset16') and
                  target.get('kind')=='far-code'):
                # A far code pointer (MOV AX,offset / MOV DX,seg) to a reviewed
                # far-code alias, as in the mixed binder, per fixup (integ26).
                require(width==2 and encoded==bytes(2) and fix['displacement']==0 and
                        frame_tuple in ((5,'target',target_name,0),
                                        (2,'external',target_name,fix['target_index'])) and at>=1 and
                        payload[at-1]==(0xba if fix['loc']=='base16' else 0xb8),
                        'Composed far code pointer field differs')
                base=target['frame_load_address']; address=target['load_address']
                require(type(base) is int and base%16==0 and 0<=base<=0xffff0 and
                        type(address) is int and 0<=address-base<=65535,
                        'Composed far code pointer target differs')
                value=base//16 if fix['loc']=='base16' else address-base
                struct.pack_into('<H',payload,at,value)
                code_pointer_fields.setdefault(target_name,{'base16':[],'loader-offset16':[]})[
                    fix['loc']].append(at)
                if fix['loc']=='base16': sites.append(start+at)
            else:
                require(False,'Unsupported composed fixup form')
        rows.append({'offset':at,'target':target_name,'linked_value':value})
    require(all(sorted(x-3 for x in v['base16'])==sorted(v['loader-offset16']) and v['base16']
                for v in code_pointer_fields.values()),
            'Composed far code pointer words are not adjacent MOV pairs')
    require(all(v['base16'] and sorted(x+3 for x in v['offset16'])==sorted(v['base16'])
                for v in cs_pointer_fields.values()),
            'Composed CS data pointer words are not adjacent MOV pairs')
    expected_sites=[r['load_offset'] for r in recipe['expected_relocations']]
    require(all(set(r)=={'segment','offset','load_offset'} and
                type(r['segment']) is int and type(r['offset']) is int and
                0<=r['segment']<=65535 and 0<=r['offset']<=65535 and
                16*r['segment']+r['offset']==r['load_offset']
                for r in recipe['expected_relocations']),
            'Composed ordered MZ relocation obligations differ')
    # C and ASM alike: the MZ entries follow this object's own FIXUPP
    # traversal exactly as the pinned LINK emits them (link_order_sites).
    # The candidate stream is compared in order; it is never sorted into, or
    # taken from, the oracle order.
    require(expected_sites==link_order_sites(sites),
            'Composed relocation order differs from its candidate FIXUPP order'
            if asm else 'Composed C relocation order differs from its FIXUPP order')
    return bytes(payload),{'mode':mode,'fixups':rows,
                           'generated_relocations':recipe['expected_relocations']}


def bind_cs_pointers(obj, segment, public, length, expected_fixups, declarations,
                     symbols, start, expected_relocations):
    """Complete far CALLs plus paired MOV offset/base words for reviewed CS data."""
    require(expected_fixups and obj.linker_fixups==expected_fixups,
            'Complete ordered CS FIXUPP differs')
    require(declarations=={'segments':obj.segment_defs,'groups':obj.groups,
                           'publics':obj.publics,'externals':obj.externals} and
            obj.publics==[{'name':public,'segment':segment,'offset':0}],
            'CS pointer declarations/public differ')
    require(obj.segment_length(segment)==length and len(obj.segment_bytes(segment))==length and
            all(n==segment or z==0 for n,z in obj.segment_lengths.items()),
            'Incomplete CS pointer contribution or unowned data')
    used={f['target'] for f in expected_fixups}
    require(set(symbols)==used and declared_externals(obj)<=used|{'__acrtused',public},
            'CS pointer external set differs')
    payload=bytearray(obj.segment_bytes(segment)); occupied=set();pairs={};sites=[];rows=[]
    for fix in expected_fixups:
        require(fix['segment']==segment and not fix['self_relative'] and
                fix['target_kind']=='external' and fix['target_method']==2 and
                (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                (5,'target',fix['target'],0) and fix['displacement']==0 and
                1<=fix['target_index']<=len(obj.externals) and
                obj.externals[fix['target_index']-1]==fix['target'],
                'Unsupported CS pointer FIXUPP target/frame')
        at=fix['offset'];width=fix['width'];loc=fix['loc'];target=symbols[fix['target']]
        require(type(at)is int and 0<=at<=length-width and
                not occupied.intersection(range(at,at+width)) and
                fix['encoded_addend']=='00'*width and payload[at:at+width]==bytes(width),
                'CS pointer fixup addend, extent or overlap differs')
        occupied.update(range(at,at+width))
        frame=target['frame_load_address'];address=target['load_address']
        require(type(frame)is int and type(address)is int and 0<=frame<=0xffff0 and
                frame%16==0 and 0<=address-frame<=65535,
                'CS pointer frame/address invalid')
        if loc=='pointer32' and width==4:
            require(at>=1 and payload[at-1]==0x9a and target['kind']=='far-code',
                    'CS mode only binds external far CALLs')
            struct.pack_into('<HH',payload,at,address-frame,frame//16)
            sites.append(start+at+2)
        elif loc in ('offset16','base16') and width==2:
            require(at>=1 and target['kind']=='cs-data' and
                    target['island_start']<=address and
                    address+target['width']<=target['island_end'] and
                    (payload[at-1]==(0xb8 if loc=='offset16' else 0xba)),
                    'CS data pointer is outside verified island or MOV pair')
            struct.pack_into('<H',payload,at,address-frame if loc=='offset16' else frame//16)
            pairs.setdefault(fix['target'],{}).setdefault(loc,[]).append(at)
            if loc=='base16':sites.append(start+at)
        else:
            require(False,'Unsupported CS pointer fixup kind/width')
        rows.append({'offset':at,'loc':loc,'target':fix['target']})
    require(pairs and any(f['loc']=='pointer32' for f in expected_fixups),
            'CS mode requires data pairs and far CALLs')
    for name,pair in pairs.items():
        require(set(pair)=={'offset16','base16'} and len(pair['offset16'])==len(pair['base16']) and
                sorted(x+3 for x in pair['offset16'])==sorted(pair['base16']),
                'CS offset/base fixups are not complete adjacent MOV pairs')
    require([r['load_offset'] for r in expected_relocations]==sites,
            'Ordered CS pointer relocation sites differ')
    for entry in expected_relocations:
        require(set(entry)=={'segment','offset','load_offset'} and
                0<=entry['segment']<=65535 and 0<=entry['offset']<=65535 and
                entry['segment']*16+entry['offset']==entry['load_offset'],
                'Invalid CS pointer relocation representation')
    return bytes(payload),{'mode':'external-far-call-cs-pointer-v1','fixups':rows,
                           'generated_relocations':expected_relocations}


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
