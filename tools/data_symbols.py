"""Resolve reviewed DGROUP binding aliases against the locked original image.

Compact aliases carry an address, not a claimed historical declaration.  Each
resolution finds an unrelocated memory displacement in an instruction-verified
original procedure.  Widths remain separate reviewed hypotheses.
"""
from common import ROOT, read_json, require, sha


_DSEG = 'src/restunts/asmorig/dseg.asm'
_DEFAULT_USE = 'src/restunts/asmorig/seg003.asm'


def _reference_label_offsets(lines):
    """Cumulative DGROUP offset of each label in the pinned reference dseg.asm.

    The reference declares one db/dw/dd/dq item per line inside `dseg segment`;
    a DUP or other data-bearing directive makes the placement fail closed."""
    import re
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    offsets, offset, inside = {}, 0, False
    for number, line in enumerate(lines, 1):
        text = line.split(';')[0].strip()
        if not inside:
            inside = text.lower().startswith('dseg segment')
            continue
        if text.lower().startswith('dseg ends'):
            return offsets
        if not text or re.match(r'(?:public|assume)\b', text, re.I):
            continue
        item = re.fullmatch(r'(?:([A-Za-z_$?@][\w$?@]*)\s+)?(db|dw|dd|dq)\s+(.*)', text, re.I)
        require(item is not None and re.search(r'\bdup\s*\(', item.group(3), re.I) is None,
                'Unsupported reference dseg line affects label placement')
        if item.group(1):
            offsets[item.group(1)] = (offset, number)
        offset += sizes[item.group(2).lower()]
    require(False, 'Reference dseg segment end missing')


def _check_reference_alias(name, symbol, image=None, frame=None, cache=None):
    """Tie reviewed data names to pinned declarations and code uses.

    A reviewed declaration must place its label, by cumulative declaration size
    in the pinned dseg.asm, at the alias's DGROUP offset.  Entries naming a
    `reference_use_path`/`reference_use_proc` additionally tie the pinned use
    line to that procedure's span in the pinned segment file, and every explicit
    anchor to one decoded original instruction, with a 16-bit displacement or
    immediate at its operand offset, inside the same instruction-verified,
    hash-checked inventory procedure.  Without that pair the legacy seg003 use
    line applies."""
    if 'reference_declaration_line' not in symbol:
        return
    from common import identity
    import re
    cache = {} if cache is None else cache
    references = read_json(ROOT/'layout/references.json')['restunts']['evidence_files']
    root = ROOT/'build/references/restunts'
    use_path = symbol.get('reference_use_path', _DEFAULT_USE)
    require(re.fullmatch(r'src/restunts/asmorig/seg\d{3}\.asm', use_path) is not None and
            use_path in references, 'Reviewed data alias use file is not a pinned reference segment')
    for path in (_DSEG, use_path):
        require(identity((root/path).read_bytes()) == references[path],
                'Reviewed data alias reference source differs')
    if _DSEG not in cache:
        cache[_DSEG] = (root/_DSEG).read_text(encoding='latin1').splitlines()
    declaration = cache[_DSEG]
    use = (root/use_path).read_text(encoding='latin1').splitlines()
    label = symbol['reference_label']
    first = symbol['reference_declaration_line']
    source_line = symbol['reference_use_line']
    # MASM 5.10 keeps 31 significant characters; a reviewed truncated alias
    # names the same pinned label.
    truncated = (symbol.get('masm_truncated_public') is True and
                 len('_'+label) > 31 and name == ('_'+label)[:31])
    require((name == '_'+label or truncated) and 1 <= first <= len(declaration) and
            1 <= source_line <= len(use) and
            re.match(r'^'+re.escape(label)+r'\s+(?:db|dw|dd|dq)\b',
                     declaration[first-1].strip(), re.I) and
            re.search(r'\b'+re.escape(label)+r'\b', use[source_line-1].split(';')[0], re.I),
            'Reviewed data alias label/use differs')
    if frame is not None:
        if 'offsets' not in cache:
            cache['offsets'] = _reference_label_offsets(declaration)
        placed = cache['offsets'].get(label)
        require(placed is not None and placed[1] == first and
                placed[0] == symbol['load_address'] - frame,
                'Reviewed data alias declaration placement differs from its DGROUP offset')
    if 'reference_use_path' not in symbol and 'reference_use_proc' not in symbol:
        return
    proc = symbol.get('reference_use_proc')
    require(isinstance(proc, str) and 'reference_use_path' in symbol and image is not None,
            'Reviewed data alias use needs a pinned segment and procedure')
    starts = [i+1 for i, line in enumerate(use)
              if re.match(r'^'+re.escape(proc)+r'\s+proc\b', line.strip(), re.I)]
    ends = [i+1 for i, line in enumerate(use)
            if re.match(r'^'+re.escape(proc)+r'\s+endp\b', line.strip(), re.I)]
    require(len(starts) == 1 and len(ends) == 1 and starts[0] < source_line < ends[0],
            'Reviewed data alias use lies outside its pinned procedure')
    if 'inventory' not in cache:
        from function_evidence import current_inventory
        cache['inventory'] = current_inventory(image)['functions']
    # Emission-verified procedures qualify only with both boundary evidences;
    # every anchor is still decoded as one original instruction inside it.
    functions = [f for f in cache['inventory'] if f.get('name') == proc and
                 (f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                  f.get('status') == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
                  f.get('start_evidence') and f.get('end_evidence'))]
    require(len(functions) == 1, 'Reviewed data alias procedure lacks a unique verified extent')
    function = functions[0]
    require(sha(image[function['start']:function['end']]) == function['sha256'],
            'Reviewed data alias procedure bytes changed')
    anchors = symbol.get('references') or []
    require(anchors, 'Reviewed data alias needs an explicit original anchor')
    import sys
    location = str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0, location)
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    for anchor in anchors:
        raw = bytes.fromhex(anchor['hex']); at = anchor['start']
        require(function['start'] <= at and at + len(raw) <= function['end'] and
                image[at:at+len(raw)] == raw,
                'Reviewed data alias anchor lies outside its verified procedure')
        decoded = list(decoder.disasm(image[at:function['end']], at, 1))
        require(len(decoded) == 1 and decoded[0].size == len(raw),
                'Reviewed data alias anchor is not one original instruction')
        ins = decoded[0]
        require((ins.disp_size == 2 and ins.disp_offset == anchor['operand_offset']) or
                (ins.imm_size == 2 and ins.imm_offset == anchor['operand_offset']),
                'Reviewed data alias anchor operand is not a 16-bit displacement or immediate')


def _oracle_instruction_anchors(addresses, image, relocations, frame):
    """Find one original instruction per requested compact address, from scratch."""
    import hashlib
    import sys
    location = str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0, location)
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    inventory = read_json(ROOT/'evidence/functions.json')
    pending = {address - frame for address in addresses}
    found = {}
    relocated = {r['load_offset'] for r in relocations}
    for function in inventory['functions']:
        if not pending: break
        if function.get('status') != 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
            continue
        start, end = function.get('start'), function.get('end')
        if not isinstance(start, int) or not isinstance(end, int) or end > len(image): continue
        body = image[start:end]
        if not any(offset.to_bytes(2, 'little') in body for offset in pending): continue
        require(hashlib.sha256(body).hexdigest() == function['sha256'],
                'Original instruction-verified procedure hash changed')
        for ins in decoder.disasm(body, start):
            if ins.disp_size != 2 or ins.disp_offset <= 0: continue
            at = ins.address + ins.disp_offset
            if at-1 in relocated or at in relocated or at+1 in relocated: continue
            offset = int.from_bytes(image[at:at+2], 'little')
            if offset not in pending: continue
            if not any(op.type == capstone.x86.X86_OP_MEM and
                       op.mem.disp & 0xffff == offset and
                       ins.reg_name(op.mem.segment) not in ('cs', 'es', 'ss') and
                       (ins.reg_name(op.mem.base) != 'bp' or
                        ins.reg_name(op.mem.segment) == 'ds')
                       for op in ins.operands):
                continue
            found[offset + frame] = {'start': ins.address, 'hex': ins.bytes.hex(),
                                     'operand_offset': ins.disp_offset,
                                     'function': function['name']}
            pending.remove(offset)
            if not pending: break
    require(not pending, 'Data alias lacks an exact unrelocated original instruction in a verified procedure: ' +
            ', '.join(hex(frame + offset) for offset in sorted(pending)))
    return found


def _check_reference_width(name, symbol, frame=None, cache=None):
    """Check a compact width against an adjacent label in the pinned reference.

    The spanned label must also be placed, by cumulative pinned declaration
    size, at the alias's own DGROUP offset: a span measured at another address
    would grant addends into unrelated objects."""
    import re
    provenance = symbol.get('width_provenance')
    if provenance is None: return  # Legacy reviewed widths retain their references.
    match = re.fullmatch(r'reference-label-span: dseg\.asm:(\d+)-(\d+)', provenance)
    require(match is not None, 'Unknown data width provenance')
    first, second = map(int, match.groups())
    require(second > first, 'Reference label span is not forward')
    lines = (ROOT/'build/references/restunts/src/restunts/asmorig/dseg.asm').read_text(
        encoding='latin1').splitlines()
    require(1 <= first and second <= len(lines), 'Reference label span missing')
    label = re.fullmatch(r'([A-Za-z_]\w*)\s+(db|dw|dd|dq)\s+.*', lines[first-1].strip(), re.I)
    following = re.fullmatch(r'[A-Za-z_]\w*\s+(db|dw|dd|dq)\s+.*',
                             lines[second-1].strip(), re.I)
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    span = 0
    for index, line in enumerate(lines[first-1:second-1]):
        item = re.fullmatch(r'(?:([A-Za-z_]\w*)\s+)?(db|dw|dd|dq)\s+.*', line.strip(), re.I)
        require(item is not None and (index == 0 or item.group(1) is None),
                'Reference span contains an unsupported or intervening declaration')
        span += sizes[item.group(2).lower()]
    # With a DGROUP frame, the span is tied to the alias address by the label's
    # checked placement below, so the alias spelling may differ from the label.
    require(label is not None and following is not None and
            (frame is not None or
             name.lower() in (label.group(1).lower(), '_' + label.group(1).lower())) and
            symbol['width'] == span,
            'Reviewed reference label span differs')
    if frame is not None:
        cache = {} if cache is None else cache
        if 'offsets' not in cache:
            cache['offsets'] = _reference_label_offsets(lines)
        require(cache['offsets'].get(label.group(1)) == (symbol['load_address'] - frame, first),
                'Reviewed reference label span is not placed at the alias address')


def _check_ascii_table_extent(name, symbol, layout, image):
    """The 256 indexed bytes precede one explicit alignment byte in dseg."""
    from common import identity
    import re
    proof=symbol.get('extent_proof',{})
    require(name=='_g_ascii_props' and symbol['width']==256 and
            symbol['load_address']==layout['frame_load_address']+0x382f and
            proof=={'kind':'ascii-table-256-v1','first_line':15377,
                    'last_value_line':15632,'padding_line':15633,
                    'next_label_line':15634},
            'Unreviewed ASCII table extent')
    path='src/restunts/asmorig/dseg.asm'
    source=ROOT/'build/references/restunts'/path
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    require(identity(source.read_bytes())==pinned,'ASCII table reference source differs')
    lines=source.read_text(encoding='latin1').splitlines()
    values=[]
    for number in range(15377,15633):
        line=lines[number-1].strip()
        pattern=(r'g_ascii_props\s+db\s+(\d+)' if number==15377
                 else r'db\s+(\d+)')
        match=re.fullmatch(pattern,line,re.I)
        require(match is not None,'ASCII table declaration differs')
        value=int(match.group(1)); require(0<=value<=255,'ASCII table byte invalid')
        values.append(value)
    require(len(values)==256 and lines[15632].strip().lower()=='db 0' and
            re.fullmatch(r'word_3F0A0\s+dw\s+1',lines[15633].strip(),re.I)
            and image[symbol['load_address']:symbol['load_address']+256]==bytes(values),
            'ASCII table bytes/padding/endpoint differ')


def _check_state_extent(name, symbol, layout):
    """Recount the pinned contiguous state declaration, including its endpoint."""
    from common import identity
    import re

    proof = symbol.get('extent_proof')
    require(name == '_state' and proof == {
        'kind': 'reference-label-span-v1', 'first_line': 34065, 'next_line': 35185
    } and symbol['load_address'] == layout['frame_load_address'] + 0x8d24,
            'Unreviewed state extent coordinates')
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    references = read_json(ROOT/'layout/references.json')['restunts']
    require(path in references['evidence_files'] and
            identity(source.read_bytes()) == references['evidence_files'][path],
            'Data extent reference source differs from pinned checkout')
    lines = source.read_text(encoding='latin1').splitlines()
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    def declaration(number):
        require(1 <= number <= len(lines), 'Extent line missing')
        match = re.fullmatch(r'(?:(\w+)\s+)?(db|dw|dd|dq)\s+.*', lines[number-1].strip(), re.I)
        require(match is not None, 'Unsupported reference declaration')
        return match.group(1), sizes[match.group(2).lower()]
    require(declaration(34065)[0] == 'state' and
            declaration(35185)[0] == 'oppcarshapevecs' and
            declaration(35353)[0] == 'gameconfig' and
            sum(declaration(n)[1] for n in range(34065, 35353)) == 0x510 and
            layout['symbols']['_gameconfig']['load_address'] ==
            layout['frame_load_address'] + 0x9234,
            'Reference DGROUP coordinate calibration differs')
    span = 0
    for number in range(34065, 35185):
        label, size = declaration(number)
        require(number == 34065 or label is None, 'Intervening state reference label')
        span += size
    require(span == symbol['width'] == 1120, 'State reference extent differs')
    base, end = symbol['load_address'], symbol['load_address'] + symbol['width']
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island':
            continue
        other_width = other.get('width')
        other_base = other['load_address']
        require(other_width is None or not (base < other_base + other_width and
                other_base < end), 'Data extent overlaps another reviewed object')


def _check_folded_extent(name, symbol, layout, image, relocations):
    """Recheck the one reviewed 8 x 76 guarded table against the oracle."""
    from common import identity, sha
    import re
    proof = symbol.get('extent_proof', {})
    require(name == '_audiochunks_unk2' and
            symbol['load_address'] == layout['frame_load_address'] + 0x86bc and
            symbol['storage'] == 'bss' and symbol['width'] == 8 * 76 and
            proof.get('kind') == 'guarded-folded-index-v1' and
            proof.get('reference_lines') == [32492, 33100] and
            proof.get('stride') == 76 and proof.get('index_bound') == [16, 23],
            'Unreviewed folded-index extent')
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    require(identity(source.read_bytes()) == pinned, 'Folded-index reference source differs')
    lines = source.read_text(encoding='latin1').splitlines()
    require(re.fullmatch(r'audiochunks_unk2\s+db\s+0', lines[32491].strip(), re.I) and
            re.fullmatch(r'word_4408C\s+dw\s+0', lines[33099].strip(), re.I) and
            all(re.fullmatch(r'(?:audiochunks_unk2\s+)?db\s+0', row.strip(), re.I)
                for row in lines[32491:33099]) and
            layout['symbols']['_word_4408C']['load_address'] ==
            symbol['load_address'] + symbol['width'],
            'Folded-index reference span or endpoint differs')
    expected = [
        ('audio_init_chunk2', 161354, 161430,
         'dae3c6786587ea708d8013c6005590a5750ea737b4ba157ef6d5c6b0b5862aca',
         [(161357,'837e0610'),(161361,'7c41'),(161363,'837e0617'),
          (161367,'7f3b'),(161369,'b84c00'),(161372,'f76e06'),
          (161375,'8bd8'),(161379,'8987fe81'),(161383,'8987fc81')]),
        ('sub_3771E', 161566, 161616,
         'aff5965217bd2966c8550bef114580fdf23261c86514d98e0092b79ae596a6b9',
         [(161582,'837e0610'),(161586,'7cf4'),(161588,'837e0617'),
          (161592,'7fee'),(161594,'b84c00'),(161597,'f76e06'),
          (161600,'8bd8'),(161602,'8b87fc81'),(161606,'0b87fe81')]),
    ]
    inventory = read_json(ROOT/'evidence/functions.json')['functions']
    witnesses = proof.get('witnesses')
    require(type(witnesses) is list and len(witnesses) == 2,
            'Folded-index witnesses missing')
    relocated = {row['load_offset'] for row in relocations}
    for recorded, (function, start, end, digest, anchors) in zip(witnesses, expected):
        require(recorded == {'function': function, 'start': start, 'end': end,
                             'sha256': digest,
                             'anchors': [{'site': at, 'hex': raw} for at, raw in anchors]},
                'Folded-index instruction anchors differ')
        require(len([row for row in inventory if row['name'] == function and
                     row['start'] == start and row['end'] == end and
                     row['sha256'] == digest and
                     row['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']) == 1 and
                sha(image[start:end]) == digest,
                'Folded-index caller identity differs')
        for at, raw in anchors:
            data = bytes.fromhex(raw)
            require(image[at:at+len(data)] == data,
                    'Folded-index oracle instruction differs')
        # Both signed guards jump to a common exit, past both accesses. The
        # same BP+6 value is multiplied by 76, then copied to BX for indexing.
        lower_jump, upper_jump = anchors[1], anchors[3]
        exit_a = lower_jump[0] + 2 + int.from_bytes(bytes.fromhex(lower_jump[1])[1:], 'little', signed=True)
        exit_b = upper_jump[0] + 2 + int.from_bytes(bytes.fromhex(upper_jump[1])[1:], 'little', signed=True)
        require(exit_a == exit_b and
                (exit_a > anchors[-1][0] or exit_a < anchors[0][0]) and
                all(at+2 not in relocated and at+3 not in relocated for at, _ in anchors[-2:]),
                'Folded-index guard or displacement differs')
        for at, raw in anchors[-2:]:
            field = int.from_bytes(bytes.fromhex(raw)[2:4], 'little') - 0x81fc
            require(field in (0, 2) and
                    symbol['load_address'] - 16*76 + field ==
                    layout['frame_load_address'] + int.from_bytes(bytes.fromhex(raw)[2:4], 'little') and
                    0 <= (16-16)*76+field and
                    (23-16)*76+field+2 <= symbol['width'],
                    'Folded-index bound does not remain inside reviewed object')
    base, end = symbol['load_address'], symbol['load_address'] + symbol['width']
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island': continue
        other_width = other.get('width')
        if other_width is not None:
            other_base = other['load_address']
            require(not (base < other_base+other_width and other_base < end),
                    'Folded-index object overlaps another reviewed extent')
    return {-16*76: {'stride': 76, 'field_offset': 0, 'index_bound': [16,23]},
            -16*76+2: {'stride': 76, 'field_offset': 2, 'index_bound': [16,23]}}


def checked_dgroup_layout(image, relocations):
    """Recheck the immutable startup evidence before any DGROUP placement."""
    layout = read_json(ROOT/'layout/data-symbols.json')
    require(layout['schema'] == 1, 'Unknown data-symbol schema')
    from common import sha
    require(layout['oracle_sha256'] == sha(image), 'Data symbols belong to another oracle')
    for anchor in layout['startup_anchors']:
        at = anchor['start']; expected = bytes.fromhex(anchor['hex'])
        require(image[at:at+len(expected)] == expected, 'DGROUP startup evidence changed')
    frame = layout['frame_load_address']
    site = layout['frame_relocation']
    require(site in relocations, 'DGROUP frame lacks required ordered-table relocation entry')
    at = site['load_offset']
    require(int.from_bytes(image[at:at+2], 'little') * 16 == frame, 'DGROUP relocated paragraph differs')
    require(layout['bss_start'] == frame + 0x55ca and layout['bss_end'] == frame + 0xad20,
            'DGROUP BSS does not agree with reviewed startup clear range')
    return layout


def checked_dseg_base(image, relocations):
    """Ground the reference DSEG start at the independently verified DGROUP base."""
    from common import identity, sha
    layout = checked_dgroup_layout(image, relocations)
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    require(identity(source.read_bytes()) == pinned,
            'DSEG base reference source differs')
    lines = source.read_text(encoding='latin1').splitlines()
    require(lines[45].strip().lower() == 'dgroup group dseg' and
            lines[46].strip().lower() == "dseg segment byte public 'stuntsd' use16" and
            lines[1499].strip().lower() == 'word_3b770     dw 0',
            'DSEG group/first declaration differs')
    base = layout['frame_load_address']
    require(layout['symbols']['_word_3B770']['load_address'] == base and
            image[base:base+2] == b'\0\0' and sha(image) == layout['oracle_sha256'],
            'First DSEG item does not occupy DGROUP base')
    return base


def resolve_symbols(names, image, relocations):
    layout = checked_dgroup_layout(image, relocations)
    frame = layout['frame_load_address']
    result = {}
    generated = None
    compact = {layout['symbols'][name]['load_address'] for name in names
               if name in layout['symbols'] and not layout['symbols'][name].get('references')
               and layout['symbols'][name].get('extent_proof', {}).get('kind') != 'guarded-folded-index-v1'
               and layout['symbols'][name].get('generated_extent', {}).get('kind') != 'corroborated-ring-buffer-v1'}
    derived = _oracle_instruction_anchors(compact, image, relocations, frame) if compact else {}
    reference_cache = {}
    for name in names:
        require(name in layout['symbols'], 'Unknown data external: ' + name)
        symbol = layout['symbols'][name]
        _check_reference_alias(name, symbol, image, frame, reference_cache)
        address = symbol['load_address']
        require(0 <= address-frame < 65536, 'Data symbol outside DGROUP')
        if symbol['storage'] == 'bss':
            require(layout['bss_start'] <= address < layout['bss_end'], 'Symbol outside verified BSS')
            if 'width' in symbol:
                require(type(symbol['width']) is int and symbol['width'] > 0 and
                        address + symbol['width'] <= layout['bss_end'],
                        'Reviewed data extent outside verified BSS')
        else:
            require(symbol['storage'] == 'initialized' and frame <= address < layout['bss_start']
                    and address < len(image),
                    'Symbol outside initialized DGROUP')
        is_folded = symbol.get('extent_proof', {}).get('kind') == 'guarded-folded-index-v1'
        is_corroborated = symbol.get('generated_extent', {}).get('kind') == 'corroborated-ring-buffer-v1'
        references = [] if is_folded or is_corroborated else symbol.get('references') or [derived[address]]
        require(references or is_folded or is_corroborated,
                'Data symbol needs original instruction evidence')
        for field in symbol.get('fields',[]):
            require(type(field['offset']) is int and field['offset']>=0 and field['width']==2,
                    'Unsupported reviewed field layout')
            ref=field['reference'];at=ref['start'];raw=bytes.fromhex(ref['hex']);operand=ref['operand_offset']
            require(image[at:at+len(raw)]==raw and 0<=operand<=len(raw)-2 and
                    int.from_bytes(raw[operand:operand+2],'little')==address-frame+field['offset'],
                    'Data field evidence changed')
            storage_end = (layout['bss_end'] if symbol['storage']=='bss' else layout['bss_start'])
            require(address+field['offset']+field['width']<=storage_end,
                    'Field outside reviewed data storage')
            require(not any(at+operand-1<=r['load_offset']<at+operand+2 for r in relocations),
                    'Field operand unexpectedly relocated')
        for ref in references:
            start = ref['start']; code = bytes.fromhex(ref['hex']); operand = ref['operand_offset']
            # A placement-checked label with a reviewed width may be anchored by
            # an original access to an element inside its own extent.
            field = ref.get('field_offset', 0)
            require(field == 0 or (type(field) is int and 'reference_declaration_line' in symbol and
                                   type(symbol.get('width')) is int and 0 < field < symbol['width']),
                    'Data symbol anchor field lies outside its placed extent')
            require(image[start:start+len(code)] == code, 'Data symbol instruction evidence changed')
            require(0 <= operand <= len(code)-2 and
                    int.from_bytes(code[operand:operand+2], 'little') == address-frame+field,
                    'Data symbol disagrees with original address operand')
            require(not any(start+operand-1 <= r['load_offset'] < start+operand+2 for r in relocations),
                    'DGROUP offset unexpectedly has a load relocation')
        # A referenced address is not a license to walk into adjacent globals.
        # Width proves an object extent; reviewed fields prove exact element
        # starts. Without either, only the symbol's own address is grounded.
        width = symbol.get('width')
        if 'generated_extent' in symbol:
            if generated is None:
                from data_extent_generator import derive
                generated, _, _ = derive(image, relocations, layout)
            require(name in generated and
                    symbol['generated_extent'] == generated[name]['extent_proof'] and
                    symbol['load_address'] == generated[name]['load_address'] and
                    symbol['storage'] == generated[name]['storage'] and
                    (width is None or width == generated[name]['width']),
                    'Generated data extent differs from original evidence')
            width = generated[name]['width']
        fields = symbol.get('fields', [])
        if width is not None:
            if symbol.get('extent_proof', {}).get('kind') == 'guarded-folded-index-v1':
                folded = _check_folded_extent(name, symbol, layout, image, relocations)
            elif symbol.get('extent_proof', {}).get('kind') == 'ascii-table-256-v1':
                _check_ascii_table_extent(name,symbol,layout,image)
            elif 'extent_proof' in symbol:
                _check_state_extent(name, symbol, layout)
            elif 'generated_extent' not in symbol:
                _check_reference_width(name, symbol, frame, reference_cache)
            require(type(width) is int and width > 0 and address + width <=
                    (layout['bss_end'] if symbol['storage'] == 'bss' else layout['bss_start']),
                    'Invalid reviewed data object extent')
        allowed_addends = {0}
        allowed_addends.update(field['offset'] for field in fields)
        if width is not None:
            require(all(offset < width for offset in allowed_addends),
                    'Reviewed field outside data object')
            allowed_addends.update(range(width))
        result[name] = {'group': 'DGROUP', 'frame_load_address': frame,
                        'load_address': address, 'allowed_addends': sorted(allowed_addends)}
        if width is not None and symbol.get('extent_proof', {}).get('kind') == 'guarded-folded-index-v1':
            result[name]['width'] = width
            result[name]['folded_addends'] = folded
    return result


def check_folded_recipe(recipe, symbols, image):
    """Tie each folded FIXUPP to the reviewed original indexed instruction."""
    required = []
    for fix in recipe['expected_fixups']:
        target = symbols.get(fix['target']) if symbols else None
        if target is None or 'folded_addends' not in target:
            continue
        encoded = bytes.fromhex(fix['encoded_addend'])
        require(len(encoded) == 2, 'Folded-index fixup must be offset16')
        addend = int.from_bytes(encoded, 'little', signed=True)
        if addend not in target['folded_addends']:
            continue
        require(fix['loc'] == 'offset16' and fix['width'] == 2 and
                fix['displacement'] == 0, 'Unsupported folded-index fixup')
        field = target['folded_addends'][addend]['field_offset']
        absolute = recipe['start'] + fix['offset']
        layout = read_json(ROOT/'layout/data-symbols.json')
        proof = layout['symbols'][fix['target']]['extent_proof']
        matches = [(witness, anchor) for witness in proof['witnesses']
                   for anchor in witness['anchors'][-2:]
                   if anchor['site']+2 == absolute and
                   int.from_bytes(bytes.fromhex(anchor['hex'])[2:4], 'little') ==
                   0x81fc+field and witness['start'] <= absolute < witness['end']]
        require(len(matches) == 1 and image[absolute:absolute+2] ==
                (0x81fc+field).to_bytes(2, 'little'),
                'Folded-index recipe fixup lacks its original guarded access')
        required.append({'offset': fix['offset'], 'target': fix['target'],
                         'field_offset': field, 'witness': matches[0][0]['function']})
    require(recipe.get('folded_index_bindings', []) == required,
            'Folded-index recipe evidence differs')


def _resolve_generic_cs_island(name, symbol, layout, image, relocations):
    """A reviewed labelled `db` table inside a pinned code segment listing.

    The island's pinned listing lines are all `db` values equal to the oracle
    bytes; the symbol's label opens the span it names; an original CS-override
    or immediate operand in an instruction-verified procedure of the same code
    frame names the table offset without a relocation."""
    import re, sys
    from common import identity, sha
    island=layout['code_islands'][symbol['island']]
    start,end,frame=island['start'],island['end'],island['frame_load_address']
    path=island['reference_path']
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    source=ROOT/'build/references/restunts'/path
    require(re.fullmatch(r'src/restunts/asmorig/seg\d{3}\.asm',path) is not None and
            identity(source.read_bytes())==pinned and
            sha(image[start:end])==island['sha256'] and frame%16==0 and
            frame<=start<end<=frame+65536,
            'Generic CS island identity differs')
    first,last=island['reference_lines']
    lines=source.read_text(encoding='latin1').splitlines()[first-1:last]
    values=[];labels={}
    for index,line in enumerate(lines):
        match=re.fullmatch(r'\s*(?:(\w+)\s+)?db\s+(\d+)\s*',line,re.I)
        require(match is not None and 0<=int(match.group(2))<=255,
                'Generic CS island declaration differs')
        if match.group(1): labels[match.group(1)]=index
        values.append(int(match.group(2)))
    require(bytes(values)==image[start:end] and 0 in labels.values(),
            'Generic CS island bytes differ from pinned listing')
    label=symbol['reference_label']
    require(label in labels and name in ('_'+label,label) and
            symbol['load_address']==start+labels[label],
            'Generic CS island label placement differs')
    following=[at for at in labels.values() if at>labels[label]]
    width=(min(following) if following else len(values))-labels[label]
    require(symbol['width']==width,'Generic CS island width differs')
    anchor=symbol['offset_anchor'];at=anchor['site'];raw=bytes.fromhex(anchor['hex'])
    operand=anchor['operand_offset']
    inventory=read_json(ROOT/'evidence/functions.json')['functions']
    callers=[f for f in inventory if f.get('status')=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
             f['start']<=at and at+len(raw)<=f['end'] and
             f.get('segment_paragraph') is not None and f['segment_paragraph']*16==frame and
             sha(image[f['start']:f['end']])==f['sha256']]
    location=str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0,location)
    import capstone
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16); decoder.detail=True
    decoded=list(decoder.disasm(raw,at))
    require(image[at:at+len(raw)]==raw and len(callers)==1 and len(decoded)==1 and
            decoded[0].size==len(raw) and
            ((decoded[0].disp_size==2 and decoded[0].disp_offset==operand and raw[0]==0x2e) or
             (decoded[0].imm_size==2 and decoded[0].imm_offset==operand)) and
            int.from_bytes(raw[operand:operand+2],'little')==symbol['load_address']-frame and
            not any(at+operand-1<=r['load_offset']<at+operand+2 for r in relocations),
            'Generic CS island anchor differs')
    return {'kind':'cs-data','frame_load_address':frame,'load_address':symbol['load_address'],
            'width':width,'island_start':start,'island_end':end}


def resolve_cs_symbols(names, image, relocations):
    """Resolve reviewed CS-resident data with pinned extents and operands."""
    import re
    from common import identity, sha
    layout=read_json(ROOT/'layout/data-symbols.json')
    require(layout['oracle_sha256']==sha(image), 'CS data oracle identity differs')
    generic={n for n in names if layout['symbols'].get(n if n.startswith('_') else '_'+n,{})
             .get('island') not in (None,'sprite_pair','interpolation')}
    if generic:
        result={n:_resolve_generic_cs_island(n,layout['symbols'][n if n.startswith('_') else '_'+n],
                                             layout,image,relocations) for n in generic}
        rest=names-generic
        if rest: result.update(resolve_cs_symbols(rest,image,relocations))
        return result
    interpolation=names & {'_word_2F448','_off_2F44A'}
    if interpolation:
        # The independent sprite MOV pair establishes this CODE frame; the
        # interpolation source and pristine operand establish each datum.
        sprite_names=names-interpolation
        frame_rows=resolve_cs_symbols(sprite_names|{'sprite1'},image,relocations)
        frame=frame_rows['sprite1']['frame_load_address']
        result={name:row for name,row in frame_rows.items() if name in sprite_names}
        island=layout.get('code_islands',{}).get('interpolation')
        require(island and (island['start'],island['end'],island['frame_load_address'],
                island['reference_lines'])==(128072,130526,125472,[1663,1713]) and
                frame==125472 and sha(image[128072:130526])==island['sha256'],
                'Interpolation CS island identity differs')
        path=island['reference_path']
        pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
        source=ROOT/'build/references/restunts'/path
        require(identity(source.read_bytes())==pinned,
                'Interpolation source differs from pinned listing')
        lines=source.read_text(encoding='latin1').splitlines()
        require(re.fullmatch(r'word_2F448\s+dw\s+50',lines[1662].strip(),re.I) and
                re.fullmatch(r'off_2F44A\s+dw\s+offset\s+\w+',lines[1663].strip(),re.I) and
                len(lines[1663:1713])==50,
                'Interpolation count/table declarations differ')
        for index,line in enumerate(lines[1663:1713]):
            match=re.fullmatch(r'(?:(?:off_2F44A|off_2F4AC)\s+)?dw\s+offset\s+(?:off|word)_([0-9A-Fa-f]+)',
                               line.strip(),re.I)
            require(match is not None and
                    int.from_bytes(image[128074+2*index:128076+2*index],'little')==
                    int(match.group(1),16)-0x10000-frame,
                    'Interpolation table entry differs from pinned source')
        require(image[128072:128074]==(50).to_bytes(2,'little'),
                'Interpolation count differs')
        for name in interpolation:
            symbol=layout['symbols'][name]
            address,width,label=(128072,2,'word_2F448') if name=='_word_2F448' else (
                128074,100,'off_2F44A')
            require(symbol['storage']=='code_island' and symbol['island']=='interpolation' and
                    symbol['load_address']==address and symbol['width']==width and
                    symbol['reference_label']==label and address+width<=128174,
                    'Interpolation object extent differs')
            anchor=symbol['offset_anchor']; at=anchor['site'];raw=bytes.fromhex(anchor['hex'])
            require(anchor['operand_offset']==3 and len(raw)==5 and raw[:1]==b'\x2e' and
                    image[at:at+5]==raw and
                    int.from_bytes(raw[3:5],'little')==address-frame and
                    not any(at+3<=r['load_offset']<at+5 for r in relocations),
                    'Interpolation original CS operand differs')
            result[name]={'kind':'cs-data','frame_load_address':frame,
                          'load_address':address,'width':width,
                          'island_start':128072,'island_end':130526}
        return result
    island=layout.get('code_islands',{}).get('sprite_pair')
    require(island and (island['start'],island['end'],island['frame_load_address'])==
            (149824,149884,125472) and
            sha(image[island['start']:island['end']])==island['sha256'],
            'CS sprite island extent/hash differs')
    path=island['reference_path']
    references=read_json(ROOT/'layout/references.json')['restunts']
    source=ROOT/'build/references/restunts'/path
    require(path in references['evidence_files'] and
            identity(source.read_bytes())==references['evidence_files'][path],
            'CS sprite source is not the pinned reference')
    first,last=island['reference_lines']
    require((first,last)==(13738,13798), 'CS sprite source span differs')
    lines=source.read_text(encoding='latin1').split('\n')[first-1:last-1]
    require(len(lines)==60, 'CS sprite source length differs')
    source_bytes=[]
    for index,line in enumerate(lines):
        match=re.fullmatch(r'\s*(?:(sprite1|sprite2)\s+)?db\s+(\d+)\s*',line,re.I)
        require(match is not None and match.group(1)==
                ('sprite1' if index==0 else 'sprite2' if index==30 else None),
                'CS sprite labels or declarations differ')
        value=int(match.group(2)); require(0<=value<=255,'CS sprite data byte invalid')
        source_bytes.append(value)
    require(bytes(source_bytes)==image[149824:149884],
            'CS sprite source bytes differ from oracle')
    inventory=read_json(ROOT/'evidence/functions.json')['functions']
    def checked_caller(anchor, size):
        at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
        require(len(raw)==size and image[at:at+size]==raw,
                'CS sprite anchor bytes differ')
        matches=[f for f in inventory if f.get('stable_id')==anchor['caller_task'] and
                 f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                 f['start']<=at and at+size<=f['end'] and
                 sha(image[f['start']:f['end']])==f['sha256']]
        require(len(matches)==1, 'CS sprite anchor lacks an independent verified caller')
        return raw
    frame_anchor=island['frame_anchor'];raw=checked_caller(frame_anchor,6)
    require(raw[:1]==b'\xb8' and raw[3:4]==b'\xba' and
            frame_anchor['relocation_load_offset']==frame_anchor['site']+4 and
            any(r['load_offset']==frame_anchor['site']+4 for r in relocations) and
            int.from_bytes(raw[4:6],'little')*16==125472 and
            125472+int.from_bytes(raw[1:3],'little')==149854,
            'CS sprite segment relocation/frame differs')
    result={}
    for name in names:
        symbol_name = name if name.startswith('_') else '_'+name
        require(symbol_name in ('_sprite1','_sprite2') and symbol_name in layout['symbols'],
                'Unknown CS sprite symbol: '+name)
        symbol=layout['symbols'][symbol_name];address=symbol['load_address']
        require(symbol['storage']=='code_island' and symbol['island']=='sprite_pair' and
                symbol['width']==30 and symbol['reference_label']==symbol_name[1:] and
                address==149824+(30 if symbol_name=='_sprite2' else 0) and
                address+30<=149884, 'CS sprite symbol escapes its verified data island')
        anchor=symbol['offset_anchor'];raw=checked_caller(anchor,6 if symbol_name=='_sprite2' else 4)
        if symbol_name=='_sprite2':
            require(raw[:1]==b'\xb8' and raw[3:4]==b'\xba' and
                    anchor['relocation_load_offset']==anchor['site']+4 and
                    any(r['load_offset']==anchor['site']+4 for r in relocations),
                    'CS sprite2 anchor lacks base relocation')
            offset=int.from_bytes(raw[1:3],'little')
        else:
            require(raw[:2]==b'\x8d\x36' and
                    image[anchor['cs_setup_site']:anchor['cs_setup_site']+4]==
                    bytes.fromhex(anchor['cs_setup_hex'])==b'\x8c\xc8\x8e\xd8',
                    'CS sprite1 offset/segment setup differs')
            offset=int.from_bytes(raw[2:4],'little')
        require(125472+offset==address,'CS sprite offset conflicts with original anchor')
        result[name]={'kind':'cs-data','frame_load_address':125472,
                      'load_address':address,'width':30,'island_start':149824,'island_end':149884}
    return result
