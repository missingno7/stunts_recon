"""Strict complete data-only OMF contribution binding and oracle placement."""
from common import ROOT, identity, read_json, require
from data_symbols import checked_dgroup_layout, resolve_symbols
from binder import _checked_data_addend


def _checked_far_anchor(anchor, start, image, relocations):
    """One original relocated segment word that selects the far-data paragraph."""
    import re, sys
    from common import sha
    reloc = anchor.get('relocation')
    require(reloc in relocations and type(reloc.get('load_offset')) is int and
            int.from_bytes(image[reloc['load_offset']:reloc['load_offset']+2], 'little')*16 == start,
            'Far-data segment anchor lacks an exact relocated paragraph word')
    at = reloc['load_offset']
    if anchor.get('kind') == 'code-immediate':
        site = anchor['site']; raw = bytes.fromhex(anchor['hex'])
        decoder_path = str(ROOT/'build/python')
        if decoder_path not in sys.path: sys.path.insert(0, decoder_path)
        import capstone
        decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16); decoder.detail = True
        rows = list(decoder.disasm(raw, site))
        inventory = read_json(ROOT/'evidence/functions.json')['functions']
        callers = [f for f in inventory if f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
                   and f['start'] <= site and site + len(raw) <= f['end']
                   and sha(image[f['start']:f['end']]) == f['sha256']]
        require(image[site:site+len(raw)] == raw and len(rows) == 1 and rows[0].size == len(raw) and
                rows[0].imm_size == 2 and site + rows[0].imm_offset == at and len(callers) == 1,
                'Far-data code anchor is not one original immediate in a verified procedure')
        return {'kind': 'code-immediate', 'site': site, 'caller': callers[0]['name']}
    require(anchor.get('kind') == 'data-segment-word', 'Unknown far-data anchor')
    # A DGROUP word declared `dw seg X` in the pinned reference at the placed alias.
    from data_symbols import _reference_label_offsets
    name = anchor['alias']
    symbol = resolve_symbols({name}, image, relocations)[name]
    path = 'src/restunts/asmorig/dseg.asm'
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    source = ROOT/'build/references/restunts'/path
    require(identity(source.read_bytes()) == pinned, 'Far-data reference source differs')
    lines = source.read_text(encoding='latin1').splitlines()
    placed = _reference_label_offsets(lines).get(name[1:])
    require(symbol['load_address'] == at and placed is not None and
            placed[0] == at - symbol['frame_load_address'] and
            re.fullmatch(re.escape(name[1:]) + r'\s+dw\s+seg\s+\w+', lines[placed[1]-1].strip(), re.I),
            'Far-data segment word is not the pinned `dw seg` declaration')
    return {'kind': 'data-segment-word', 'alias': name}


def bind_far_data(obj, recipe, image, relocations):
    """A complete paragraph-aligned FAR_DATA module placed by relocated segment words."""
    segment = recipe['object_segment']; start, end = recipe['start'], recipe['end']
    definitions = {d['name']: d for d in obj.segment_defs}
    require(segment in definitions and definitions[segment]['class'] == 'FAR_DATA' and
            definitions[segment]['alignment'] == 'paragraph' and start % 16 == 0 and
            all(length == 0 for name, length in obj.segment_lengths.items() if name != segment),
            'Far-data module segment/alignment differs')
    require(set(obj.segment_lengths) == set(definitions) and
            recipe['object_declarations'] == {
                'segments': obj.segment_defs, 'groups': obj.groups,
                'publics': obj.publics, 'externals': obj.externals},
            'Complete far-data OMF declarations differ')
    require(obj.linker_fixups == recipe['expected_fixups'] == [] and
            recipe.get('required_pointer_sites', []) == [],
            'Far-data module FIXUPPs are unsupported')
    raw = obj.segment_bytes(segment)
    require(obj.segment_length(segment) == end-start and len(raw) == end-start,
            'Complete far-data LEDATA extent differs')
    publics = {p['name']: p['offset'] for p in obj.publics if p['segment'] == segment}
    sizes = recipe.get('public_object_sizes', {})
    ordered = sorted((offset, name) for name, offset in publics.items())
    require(len(publics) == len(obj.publics) > 0 and set(sizes) == set(publics) and
            all(type(size) is int and size > 0 for size in sizes.values()) and
            ordered[0][0] == 0 and
            all(offset+sizes[name] == nxt for (offset, name), (nxt, _) in zip(ordered, ordered[1:])) and
            ordered[-1][0]+sizes[ordered[-1][1]] == len(raw),
            'Far-data declared object extents do not cover full segment')
    placement = recipe.get('far_placement', {})
    require(placement.get('basis') == 'relocated-segment-word-v1' and placement.get('anchors'),
            'Far-data module lacks a relocated segment-word placement anchor')
    proofs = [_checked_far_anchor(a, start, image, relocations) for a in placement['anchors']]
    for row in placement.get('offset_anchors', []):
        site = row['site']; operand = row['operand_offset']; code = bytes.fromhex(row['hex'])
        require(row['public'] in publics and image[site:site+len(code)] == code and
                0 <= operand <= len(code)-2 and
                int.from_bytes(code[operand:operand+2], 'little') == publics[row['public']] and
                all(r['load_offset'] not in (site+operand, site+operand+1) for r in relocations),
                'Far-data public offset anchor differs')
    require(bytes(raw) == image[start:end] and identity(raw) == recipe['target'],
            'Complete far-data module differs from oracle')
    require(recipe['expected_relocations'] == [] and
            not any(start-1 <= r['load_offset'] < end for r in relocations),
            'Far-data relocation obligation differs')
    return bytes(raw), {'mode': 'data-only-far-segment-v1', 'anchors': proofs,
                        'generated_relocations': []}


def bind_data_only(obj, recipe, image, relocations):
    if recipe.get('far_data') is True:
        require(recipe.get('data_only') is True, 'Far data must be a data-only module')
        return bind_far_data(obj, recipe, image, relocations)
    segment = recipe['object_segment']
    require(segment == '_DATA' and recipe.get('data_only') is True,
            'Unsupported data-only contribution')
    require(obj.segment_lengths.get('UNIT_TEXT') == 0 and
            all(length == 0 for name, length in obj.segment_lengths.items()
                if name != segment), 'Data-only module emits another nonempty segment')
    require(set(obj.segment_lengths) == {d['name'] for d in obj.segment_defs} and
            recipe['object_declarations'] == {
                'segments': obj.segment_defs, 'groups': obj.groups,
                'publics': obj.publics, 'externals': obj.externals},
            'Complete data-only OMF declarations differ')
    require(obj.linker_fixups == recipe['expected_fixups'],
            'Complete ordered data-only FIXUPPs differ')
    require(sorted(f['offset'] for f in obj.linker_fixups) ==
            sorted(recipe['required_pointer_sites']) and
            len(set(recipe['required_pointer_sites'])) == len(recipe['required_pointer_sites']),
            'Required semantic pointer fields lack OMF FIXUPPs')
    require(obj.segment_length(segment) == recipe['end']-recipe['start'] and
            len(obj.segment_bytes(segment)) == recipe['end']-recipe['start'],
            'Complete data-only LEDATA extent differs')
    definitions = {d['name']: d for d in obj.segment_defs}
    require(all(d['alignment'] == 'word' for d in obj.segment_defs) and
            recipe['start'] % 2 == 0 and recipe['end'] % 2 == 0,
            'Data-only MSC word alignment differs')
    layout = checked_dgroup_layout(image, relocations)
    require(recipe['start'] == layout['frame_load_address'] + recipe['dgroup_offset'] and
            recipe['end'] <= layout['bss_start'], 'Data-only DGROUP placement differs')
    publics = {p['name']: p['offset'] for p in obj.publics if p['segment'] == segment}
    require(len(publics) == len(obj.publics) and len(publics) > 0,
            'Data-only publics missing or outside owned segment')
    sizes=recipe.get('public_object_sizes',{})
    require(set(sizes)==set(publics) and all(type(size) is int and size>0 for size in sizes.values()),
            'Data-only object extents are incomplete')
    ordered=sorted((offset,name) for name,offset in publics.items())
    require(ordered[0][0]==0 and
            all(offset+sizes[name] == next_offset
                for (offset,name),(next_offset,_) in zip(ordered,ordered[1:])) and
            ordered[-1][0]+sizes[ordered[-1][1]]==len(obj.segment_bytes(segment)),
            'Data-only declared object extents do not cover full segment')
    # The observed references are pinned reference-checkout facts. The opcode
    # anchors additionally test real, unrelocated operand words in raw or owned code.
    anchors = recipe.get('placement_anchors', {})
    require(anchors.get('basis') == 'oracle-code-operands-v1' and
            anchors.get('public_offsets') == publics and anchors.get('code_operands'),
            'Data-only module lacks independently checked code operand placement')
    reference = read_json(ROOT/'layout/references.json')['restunts']['evidence_files']
    from common import identity as file_identity
    for path in anchors.get('reference_files', []):
        require(path in reference and
                file_identity((ROOT/'build/references/restunts'/path).read_bytes()) == reference[path],
                'Data-only reference checkout differs')
    for label, row in anchors.get('observed_references', {}).items():
        require(row['oracle_load_address'] == recipe['start']+row['module_offset'] and
                0 <= row['module_offset'] < recipe['end']-recipe['start'],
                'Data-only observed alias placement differs')
        if '_'+label in publics:
            require(publics['_'+label] == row['module_offset'],
                    'Data-only public offset disagrees with observed alias')
        for use in row['references']:
            path='src/restunts/asmorig/'+use['code_file']
            require(path in reference and path in anchors['reference_files'],
                    'Unpinned observed code reference')
            source=(ROOT/'build/references/restunts'/path).read_text(encoding='latin1')
            require(use['operand'].strip() in source and
                    label.lower() in use['operand'].lower(),
                    'Observed code reference differs')
    for row in anchors['code_operands']:
        public = row['public']; at = row['load_offset']; raw = bytes.fromhex(row['instruction_hex'])
        operand = row['operand_offset']
        import sys
        decoder_path=str(ROOT/'build/python')
        if decoder_path not in sys.path: sys.path.insert(0,decoder_path)
        import capstone
        require(capstone.__version__=='5.0.3','Pinned code operand decoder differs')
        decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16)
        decoder.detail=True
        instructions=list(decoder.disasm(raw,at))
        require(len(instructions)==1 and instructions[0].size==len(raw) and
                ((instructions[0].imm_offset==operand and instructions[0].imm_size==2) or
                 (instructions[0].disp_offset==operand and instructions[0].disp_size==2)),
                'Data-only anchor is not a complete original operand instruction')
        require(public in publics and 0 <= at < layout['frame_load_address'] and
                0 <= operand <= len(raw)-2 and image[at:at+len(raw)] == raw and
                int.from_bytes(raw[operand:operand+2], 'little') ==
                recipe['dgroup_offset'] + publics[public] and
                all(r['load_offset'] not in (at+operand, at+operand+1) for r in relocations),
                'Data-only original CODE operand disagrees with placement')
    raw = obj.segment_bytes(segment)
    payload = bytearray(raw)
    external = {f['target'] for f in obj.linker_fixups if f['target_kind'] == 'external'}
    require(external == set(recipe.get('external_data_targets', [])),
            'Unreviewed data-only external pointer target')
    symbols = resolve_symbols(external, image, relocations) if external else {}
    offsets = ordered
    occupied = set(); proof = []
    for fix in obj.linker_fixups:
        at = fix['offset']; encoded = bytes.fromhex(fix['encoded_addend'])
        require(fix['segment'] == segment and fix['loc'] == 'offset16' and
                fix['width'] == 2 and not fix['self_relative'] and
                fix['displacement'] == 0 and len(encoded) == 2 and
                0 <= at <= len(raw)-2 and at not in occupied and at+1 not in occupied and
                raw[at:at+2] == encoded,
                'Unsupported data-only pointer FIXUPP')
        occupied.update((at,at+1))
        addend = int.from_bytes(encoded, 'little')
        if fix['target_kind'] == 'segment':
            require(fix['target'] == segment and fix['target_method'] == 0 and
                    fix['target_index'] == definitions[segment]['index'] and
                    (fix['frame_kind'],fix['frame'],fix['frame_method']) ==
                    ('group','DGROUP',1),
                    'Unsupported in-module pointer target')
            owner = [(off,name) for off,name in offsets if off <= addend]
            require(owner, 'In-module pointer precedes first public')
            base,name = owner[-1]
            require(addend < base+sizes[name],
                    'In-module pointer addend escapes target object')
            value = recipe['dgroup_offset'] + addend
            target = name
        elif fix['target_kind'] == 'external':
            require(fix['target_method'] == 2 and
                    obj.externals[fix['target_index']-1] == fix['target'] and
                    (fix['frame_kind'],fix['frame'],fix['frame_method']) ==
                    ('target',fix['target'],5),
                    'Unsupported external data-only pointer target')
            symbol = symbols[fix['target']]
            require(symbol['group'] == 'DGROUP', 'External pointer is not DGROUP')
            value = (symbol['load_address']-layout['frame_load_address'] +
                     _checked_data_addend(symbol, encoded))
            target = fix['target']
        else:
            require(False, 'Unsupported data-only pointer target kind')
        require(0 <= value <= 65535 and
                image[recipe['start']+at:recipe['start']+at+2] == value.to_bytes(2,'little'),
                'Data-only bound pointer word differs from oracle')
        payload[at:at+2] = value.to_bytes(2,'little')
        proof.append({'offset':at,'target':target,'value':value})
    require(bytes(payload) == image[recipe['start']:recipe['end']] and
            identity(payload) == recipe['target'],
            'Complete data-only module differs from oracle')
    require(recipe['expected_relocations'] == [] and
            not any(recipe['start']-1 <= r['load_offset'] < recipe['end'] for r in relocations),
            'Data-only relocation obligation differs')
    return bytes(payload), {'mode':'data-only-dgroup-v1',
                            'pointer_fixups':proof,'generated_relocations':[]}


def far_data_publics():
    """name -> (owner row, recipe, public offset) for accepted far-data modules."""
    result = {}
    for owner in read_json(ROOT/'layout/manifest.json')['owners']:
        if not (owner.get('far_data') is True and owner.get('module_form') == 'data-only' and
                owner['kind'] in ('MATCHING_C_DATA', 'MATCHING_ASM_DATA') and 'recipe' in owner):
            continue
        recipe = read_json(ROOT/owner['recipe'])
        for public in recipe['object_declarations']['publics']:
            require(public['name'] not in result, 'Far-data public is declared twice')
            result[public['name']] = (owner, recipe, public['offset'])
    return result


def resolve_far_data_symbols(names, image, relocations):
    """External C references to a public of an accepted FAR_DATA module (integ25).

    The module's paragraph is re-grounded from its own relocated segment-word
    anchors; the public offset and declared object size come from the accepted
    module recipe, whose complete bytes are verified on every build.  Offsets
    bind inside that object only; the segment word is an MZ relocation."""
    publics = far_data_publics()
    result = {}
    for name in names:
        require(name in publics, 'Far-data symbol lacks an accepted far-data module public: ' + name)
        owner, recipe, offset = publics[name]
        start = recipe['start']
        require((start, recipe['end']) == (owner['start'], owner['end']) and start % 16 == 0 and
                recipe.get('far_data') is True, 'Far-data owner and recipe differ')
        placement = recipe.get('far_placement', {})
        require(placement.get('basis') == 'relocated-segment-word-v1' and placement.get('anchors'),
                'Far-data owner lacks its relocated segment-word placement')
        for anchor in placement['anchors']:
            _checked_far_anchor(anchor, start, image, relocations)
        width = recipe['public_object_sizes'][name]
        require(type(width) is int and 0 < width and start + offset + width <= recipe['end'],
                'Far-data public extent differs')
        result[name] = {'kind': 'far-data', 'frame_load_address': start,
                        'load_address': start + offset, 'width': width, 'owner': owner['id']}
    return result
