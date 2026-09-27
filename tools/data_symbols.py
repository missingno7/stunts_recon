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
    import memo
    lines = list(lines)
    return memo.cached('reference_label_offsets', memo.digest('\n'.join(lines).encode('latin1')),
                       lambda: _label_offsets(lines))


def _label_offsets(lines):
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


_NAME_BUFFER_WITNESS = {
    # Caller: four consecutive byte stores to DS:AC74..AC77, then the address
    # of the first byte is pushed as one argument of a relocated far CALL.
    'caller': ('load_tracks_menu_shapes', [
        (107633, '268a07'), (107636, 'a274ac'), (107639, '268a4701'), (107643, 'a275ac'),
        (107646, '268a4702'), (107650, 'a276ac'), (107653, '268a4703'), (107657, 'a277ac'),
        (107660, 'b874ac'), (107663, '50'), (107664, 'ff76d8'), (107667, 'ff76d6'),
        (107670, '9a7d25a21e')]),
    # Callee body shared by the locate_* entries: ES = DGROUP, DI = the name
    # argument [bp+0Ah], and both loops read at most CX = 4 bytes of it.
    'callee': ('locate_sound_fatal', [
        (135097, 'b8772b'), (135100, '8ec0'), (135102, '8b7e0a'), (135105, 'b90400'),
        (135108, 'bb0000'), (135111, '26803900'), (135117, '43'), (135118, 'e2f7'),
        (135134, '8b7e0a'), (135137, 'b90400'), (135140, 'a6'), (135143, 'e2fb')]),
}


def _check_name_buffer_extent(name, symbol, layout, image, relocations):
    """The 4-byte resource-name buffer at DS:AC74 (integ26).

    The pinned reference splits it into resID_byte1..4 labels; original code
    fills all four bytes and passes the address of the first as one name
    argument to the locate_* body, which reads at most four bytes of it in
    DGROUP. That makes [AC74, AC78) one object (a lower bound: other code
    indexes it as a longer text buffer, whose end is not independently
    known). Every witness instruction is rechecked in its instruction-verified,
    hash-checked procedure, and the far CALL against its MZ relocation and
    verified entry."""
    from function_evidence import current_inventory
    proof = symbol.get('extent_proof', {})
    frame = layout['frame_load_address']
    require(name == '_resID_byte1' and symbol['load_address'] == frame + 0xac74 and
            symbol['storage'] == 'bss' and symbol['width'] == 4 and
            proof == {'kind': 'name-buffer-consumer-v1', 'caller': 'load_tracks_menu_shapes',
                      'callee': 'locate_sound_fatal', 'entry': 'locate_shape_fatal'},
            'Unreviewed name-buffer extent')
    inventory = current_inventory(image)['functions']
    def verified(proc):
        rows = [f for f in inventory if f.get('name') == proc and
                f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                sha(image[f['start']:f['end']]) == f['sha256']]
        require(len(rows) == 1, 'Name-buffer witness procedure is not verified: ' + proc)
        return rows[0]
    for role in ('caller', 'callee'):
        proc, anchors = _NAME_BUFFER_WITNESS[role]
        f = verified(proc)
        at = anchors[0][0]
        for site, raw in anchors:
            data = bytes.fromhex(raw)
            require(role == 'callee' or site == at, 'Name-buffer caller stores are not contiguous')
            require(f['start'] <= site and site + len(data) <= f['end'] and
                    image[site:site+len(data)] == data,
                    'Name-buffer witness instruction differs')
            at = site + len(data)
    require(frame // 16 == 0x2b77, 'Name-buffer consumer ES is not the DGROUP paragraph')
    entry = verified('locate_shape_fatal')
    call = 107670
    require(any(r['load_offset'] == call + 3 for r in relocations) and
            int.from_bytes(image[call+3:call+5], 'little') * 16 +
            int.from_bytes(image[call+1:call+3], 'little') == entry['start'] and
            image[entry['start'] + 9:entry['start'] + 11] == bytes.fromhex('eb0a') and
            entry['start'] + 11 + 0x0a == 135090 and
            not any(r['load_offset'] in (107637, 107644, 107651, 107658, 107661)
                    for r in relocations),
            'Name-buffer far CALL does not reach the verified consumer body')
    base, end = symbol['load_address'], symbol['load_address'] + symbol['width']
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island':
            continue
        width = other.get('width')
        if width is None:
            continue
        other_base = other['load_address']
        require(not (base < other_base + width and other_base < end) or
                (base <= other_base and other_base + width <= end),
                'Name-buffer extent partially overlaps another reviewed object')


_STRIDE_TABLES = {
    # audio_unk walks DI from 81FCh+28h in 76-byte steps while SI counts 0..23
    # and reads [DI] (field 28h of every element) in the loop body.
    '_audiochunks_unk': {
        'label': 'audiochunks_unk', 'offset': 0x81fc, 'stride': 76, 'count': 24,
        'end_label': 'word_4408C', 'interior': ('byte_43B34', 'audiochunks_unk2'),
        'proof': {'kind': 'counted-stride-loop-v1', 'witness': 'audio_unk', 'stride': 76,
                  'count': 24, 'field_offset': 0x28, 'end_label': 'word_4408C'},
        'witness': ('audio_unk', 160304,
                    '2bf6' 'bf2482' '803ec34e01' '7405' '83fe10' '7d12' '8a05' '88844e71'
                    '2bc0' '50' '56' '9a1c066328' '83c404' '83c74c' '46' '83fe18' '7cd9'),
        # sub si,si; mov di,base+28h; head: ... mov al,[di] ...; step: add di,76;
        # inc si; cmp si,24; jl head (rel8 -39 from the end of the JL).
        'shape': lambda c: (c[0:2] == b'\x2b\xf6' and int.from_bytes(c[3:5], 'little') == 0x81fc + 0x28 and
                            c[15:17] == b'\x7d\x12' and 15 + 2 + 0x12 == 35 and
                            c[17:19] == b'\x8a\x05' and c[35:38] == b'\x83\xc7\x4c' and
                            c[38:39] == b'\x46' and c[39:42] == b'\x83\xfe\x18' and
                            c[42:44] == b'\x7c\xd9' and 44 - 0x27 == 5),
        'relocation_free': 27},
    # update_frame copies all 15 eight-byte rectangles: SI counts 0..14, AX/BX =
    # SI*8, LEA AX,[BX+9294h] is the source of four MOVSW (8 bytes) each pass.
    '_rect_unk': {
        'label': 'rect_unk', 'offset': 0x9294, 'stride': 8, 'count': 15,
        'end_label': 'voicefileptr',
        'interior': ('rect_unk2', 'rect_unk6', 'rect_unk12', 'rect_unk15', 'rect_skybox',
                     'rect_unk11', 'rect_unk9'),
        'proof': {'kind': 'counted-stride-loop-v1', 'witness': 'update_frame', 'stride': 8,
                  'count': 15, 'field_offset': 0, 'end_label': 'voicefileptr'},
        'witness': ('update_frame', 49838,
                    '2bf6' '8bc6' 'b103' 'd3e0' '8986acfe' '8bd8' '8d879492' '8b9eacfe'
                    '031e9a00' '56' '57' '8bfb' '8bf0' '1e' '07' 'a5a5a5a5' '5f' '5e' '46'
                    '83fe0f' '7cd4'),
        # sub si,si; head: mov ax,si; shl ax,3; ...; lea ax,[bx+9294h]; ...;
        # mov si,ax; 4 x movsw; ...; inc si; cmp si,15; jl head.
        'shape': lambda c: (c[0:2] == b'\x2b\xf6' and c[2:4] == b'\x8b\xc6' and
                            c[4:8] == b'\xb1\x03\xd3\xe0' and c[12:14] == b'\x8b\xd8' and
                            c[14:16] == b'\x8d\x87' and int.from_bytes(c[16:18], 'little') == 0x9294 and
                            c[30:32] == b'\x8b\xf0' and c[34:38] == b'\xa5' * 4 and
                            c[40:41] == b'\x46' and c[41:44] == b'\x83\xfe\x0f' and
                            c[44:46] == b'\x7c\xd4' and 46 - 0x2c == 2),
        'relocation_free': 46},
}


def _check_counted_stride_extent(name, symbol, layout, image, relocations):
    """A reviewed table of `count` equal `stride`-byte elements (integ26).

    One instruction-verified loop, rechecked byte for byte, counts an index
    over every element and accesses each element (see _STRIDE_TABLES), so one
    object spans all of them. The element base is the placed pinned label and
    the whole table ends exactly at the next placed pinned label outside it;
    the pinned labels in between are interior element names, and reviewed
    objects inside are wholly contained names. A partial overlap is refused."""
    from function_evidence import current_inventory
    spec = _STRIDE_TABLES.get(name)
    frame = layout['frame_load_address']
    require(spec is not None and symbol['load_address'] == frame + spec['offset'] and
            symbol['storage'] == 'bss' and symbol['width'] == spec['stride'] * spec['count'] and
            symbol.get('reference_label', spec['label']) == spec['label'] and
            symbol.get('extent_proof') == spec['proof'],
            'Unreviewed counted-stride extent')
    proc, at, raw = spec['witness']
    found = [f for f in current_inventory(image)['functions'] if f.get('name') == proc and
             f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
             sha(image[f['start']:f['end']]) == f['sha256']]
    require(len(found) == 1, 'Counted-stride witness procedure is not verified')
    f = found[0]
    code = bytes.fromhex(raw)
    require(f['start'] <= at and at + len(code) <= f['end'] and image[at:at+len(code)] == code,
            'Counted-stride witness instructions differ')
    require(spec['shape'](code) and
            not any(at <= r['load_offset'] < at + spec['relocation_free'] for r in relocations),
            'Counted-stride loop shape differs')
    from common import identity
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    require(identity(source.read_bytes()) ==
            read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path],
            'Counted-stride reference source differs')
    offsets = _reference_label_offsets(source.read_text(encoding='latin1').splitlines())
    end_offset = spec['offset'] + spec['stride'] * spec['count']
    require(offsets.get(spec['label'], (None,))[0] == spec['offset'] and
            offsets.get(spec['end_label'], (None,))[0] == end_offset and
            all(not spec['offset'] < off < end_offset or label in spec['interior']
                for label, (off, _) in offsets.items()),
            'Counted-stride pinned endpoints differ')
    base, end = symbol['load_address'], symbol['load_address'] + symbol['width']
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island' or other.get('width') is None:
            continue
        o_base, o_end = other['load_address'], other['load_address'] + other['width']
        require(not (base < o_end and o_base < end) or (base <= o_base and o_end <= end),
                'Counted-stride table partially overlaps another reviewed object')


_PAIR_EXTENTS = {
    # skybox_op indexes a word array at 928Ch by [bp+6]*2 and stores/compares
    # word_463D6 there; sub_19F14 performs the same store/compare at the
    # constant 928Eh: element 1 of the same array (two elements, 4 bytes).
    '_word_449FC': {
        'label': 'word_449FC', 'offset': 0x928c, 'interior': ('word_449FE',), 'end_label': 'opp_res',
        'kind': 'indexed-constant-twin-v1',
        'witnesses': (('skybox_op', 51814, '8b5e06' 'd1e3' 'a166ac' '89878c92'
                                           '8b5e06' 'd1e3' '8b4610' '39878c92'),
                      ('sub_19F14', 40787, 'a166ac' 'a38e92' 'a166ac' '39068e92'))},
    # load_skybox stores a far CALL's DX:AX result at A9F8h/A9FAh; unload_skybox
    # pushes A9FAh then A9F8h as one far pointer argument (4 bytes).
    '_skybox_res_ofs': {
        'label': 'skybox_res_ofs', 'offset': 0xa9f8, 'interior': ('skybox_res_seg',),
        'end_label': 'mouse_xpos', 'kind': 'far-pointer-word-pair-v1',
        'witnesses': (('load_skybox', 55270, '9a3c4ea21e' '83c402' 'a3f8a9' '8916faa9'),
                      ('unload_skybox', 55483, 'ff36faa9' 'ff36f8a9'))},
    # setup_car_shapes stores DX:AX at 9D2Ch/9D2Eh; run_game pushes 9D2Eh then
    # 9D2Ch as one far pointer argument (dastbmp_y2/dastseg labels, 4 bytes).
    '_dastshapeptr': {
        'label': 'dastbmp_y2', 'offset': 0x9d2c, 'interior': ('dastseg',),
        'end_label': 'dasmshapeptr', 'kind': 'far-pointer-word-pair-v1',
        'witnesses': (('setup_car_shapes', 77634, 'a32c9d' '89162e9d'),
                      ('run_game', 73931, 'ff362e9d' 'ff362c9d'))},
}


_LIST_TABLES = {
    # update_frame selects one list by (count, start) = (1,932h) (2,936h)
    # (2,93Eh) (4,946h); its loop reads two words per element through the one
    # pointer [bp-0DAh], advancing it by 4. The four windows tile
    # [932h,956h) exactly, up to the placed label byte_3C0C6: one 9 x 4 table.
    '_unk_3C0A2': {
        'label': 'unk_3C0A2', 'offset': 0x932, 'element': 4, 'end_label': 'byte_3C0C6',
        'interior': ('unk_3C0A6', 'unk_3C0AE', 'unk_3C0B6'),
        'witness': ('update_frame', 45564,
                    'bf0100c78626ff3209c78622ff0000eb2990bf0200c78626ff3609ebec90'
                    'bf0200c78626ff3e09ebe090bf0400c78626ff4609ebd490ff8622ff39be22ff'
                    '7d728b9e26ff838626ff028b070346bca3ca728b46bea3cc728b9e26ff'
                    '838626ff028b0703'),
        # (offset of MOV DI,count; offset of MOV [bp-0DAh],start) per list.
        'lists': ((0, 3), (18, 21), (30, 33), (42, 45)),
        # loop: cmp [bp-0DEh],di / jge; two reads of [bx] after
        # mov bx,[bp-0DAh] / add word [bp-0DAh],2.
        'loop': ((58, '39be22ff7d72'), (64, '8b9e26ff838626ff028b07'),
                 (87, '8b9e26ff838626ff028b07'))},
}


def _check_list_table_extent(name, symbol, layout, image, relocations):
    """A reviewed table read as counted element lists through one pointer (integ26)."""
    from function_evidence import current_inventory
    from common import identity
    spec = _LIST_TABLES.get(name)
    frame = layout['frame_load_address']
    proc, at, raw = spec['witness'] if spec else (None, 0, '')
    code = bytes.fromhex(raw)
    require(spec is not None and symbol['load_address'] == frame + spec['offset'] and
            symbol.get('extent_proof') == {'kind': 'pointer-list-table-v1',
                                           'end_label': spec['end_label']},
            'Unreviewed list-table extent')
    rows = [f for f in current_inventory(image)['functions'] if f.get('name') == proc and
            f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
            f['start'] <= at and at + len(code) <= f['end'] and
            sha(image[f['start']:f['end']]) == f['sha256']]
    require(len(rows) == 1 and image[at:at+len(code)] == code and
            not any(at <= r['load_offset'] < at + len(code) for r in relocations) and
            all(code[i:i+len(bytes.fromhex(h))] == bytes.fromhex(h) for i, h in spec['loop']),
            'List-table witness differs')
    windows = []
    for count_at, start_at in spec['lists']:
        require(code[count_at] == 0xbf and code[start_at:start_at+4] == b'\xc7\x86\x26\xff',
                'List-table selector differs')
        count = int.from_bytes(code[count_at+1:count_at+3], 'little')
        first = int.from_bytes(code[start_at+4:start_at+6], 'little')
        windows.append((first, first + count * spec['element']))
    windows.sort()
    require(windows[0][0] == spec['offset'] and
            all(a[1] == b[0] for a, b in zip(windows, windows[1:])) and
            windows[-1][1] == spec['offset'] + symbol['width'],
            'List-table windows do not tile the reviewed extent')
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    require(identity(source.read_bytes()) ==
            read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path],
            'List-table reference source differs')
    offsets = _reference_label_offsets(source.read_text(encoding='latin1').splitlines())
    end_offset = spec['offset'] + symbol['width']
    require(offsets.get(spec['label'], (None,))[0] == spec['offset'] and
            offsets.get(spec['end_label'], (None,))[0] == end_offset and
            all(not spec['offset'] < off < end_offset or label in spec['interior']
                for label, (off, _) in offsets.items()),
            'List-table pinned labels differ')
    base, end = symbol['load_address'], symbol['load_address'] + symbol['width']
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island' or other.get('width') is None:
            continue
        o_base, o_end = other['load_address'], other['load_address'] + other['width']
        require(not (base < o_end and o_base < end) or (base <= o_base and o_end <= end),
                'List table partially overlaps another reviewed object')


def _check_pair_extent(name, symbol, layout, image, relocations):
    """A reviewed 4-byte object made of two pinned word labels (integ26)."""
    from function_evidence import current_inventory
    from common import identity
    spec = _PAIR_EXTENTS.get(name)
    frame = layout['frame_load_address']
    require(spec is not None and symbol['load_address'] == frame + spec['offset'] and
            symbol['width'] == 4 and symbol.get('extent_proof') ==
            {'kind': spec['kind'], 'end_label': spec['end_label']},
            'Unreviewed word-pair extent')
    inventory = current_inventory(image)['functions']
    for proc, at, raw in spec['witnesses']:
        code = bytes.fromhex(raw)
        rows = [f for f in inventory if f.get('name') == proc and
                f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                f['start'] <= at and at + len(code) <= f['end'] and
                sha(image[f['start']:f['end']]) == f['sha256']]
        require(len(rows) == 1 and image[at:at+len(code)] == code,
                'Word-pair witness differs: ' + proc)
    if spec['kind'] == 'far-pointer-word-pair-v1' and spec['witnesses'][0][2].startswith('9a'):
        call = spec['witnesses'][0][1]
        require(any(r['load_offset'] == call + 3 for r in relocations),
                'Word-pair far CALL lacks its MZ relocation')
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    require(identity(source.read_bytes()) ==
            read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path],
            'Word-pair reference source differs')
    offsets = _reference_label_offsets(source.read_text(encoding='latin1').splitlines())
    require(offsets.get(spec['label'], (None,))[0] == spec['offset'] and
            offsets.get(spec['interior'][0], (None,))[0] == spec['offset'] + 2 and
            offsets.get(spec['end_label'], (None,))[0] == spec['offset'] + 4,
            'Word-pair pinned labels differ')
    base, end = symbol['load_address'], symbol['load_address'] + 4
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island' or other.get('width') is None:
            continue
        o_base, o_end = other['load_address'], other['load_address'] + other['width']
        require(not (base < o_end and o_base < end) or (base <= o_base and o_end <= end),
                'Word-pair object partially overlaps another reviewed object')


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
            # A reviewed container wholly holding this table (the counted
            # 24 x 76 audiochunks_unk extent, integ26) is not a conflict.
            container = (other.get('extent_proof', {}).get('kind') == 'counted-stride-loop-v1' and
                         other_base <= base and end <= other_base + other_width)
            require(container or not (base < other_base+other_width and other_base < end),
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


CLONE_KEYS = {'load_address', 'storage', 'width', 'width_provenance', 'clone_of', 'name_provenance'}


def clone_sources(symbols, names):
    """Binding aliases declared as second names of a grounded alias (integ27).

    A `clone_of` entry is an address-bound binding name only (for example a
    pressure-constrained short linker name): it must repeat its source's
    address, storage and width exactly, carry nothing else, and resolves
    through the source alias with all of the source's own evidence checks."""
    mapping = {}
    for name in names:
        symbol = symbols.get(name)
        if symbol is None or 'clone_of' not in symbol:
            continue
        source = symbols.get(symbol['clone_of'])
        require(source is not None and 'clone_of' not in source and set(symbol) <= CLONE_KEYS and
                all(symbol.get(k) == source.get(k)
                    for k in ('load_address', 'storage', 'width', 'width_provenance')),
                'Clone data alias differs from its grounded source alias: ' + name)
        mapping[name] = symbol['clone_of']
    return mapping


def resolve_symbols(names, image, relocations):
    # Each name resolves independently of the other requested names; a
    # verification session reuses identical per-name results (tools/memo.py).
    import memo
    from common import sha
    key = (sha(image), memo.digest(relocations))
    return memo.cached_items('data_symbols.resolve', key, names,
                             lambda wanted: _resolve_symbols(wanted, image, relocations))


def _resolve_symbols(names, image, relocations):
    layout = checked_dgroup_layout(image, relocations)
    clones = clone_sources(layout['symbols'], names)
    if clones:
        wanted = [clones.get(name, name) for name in names]
        resolved = resolve_symbols(list(dict.fromkeys(wanted)), image, relocations)
        return {name: dict(resolved[clones.get(name, name)]) for name in names}
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
            elif symbol.get('extent_proof', {}).get('kind') == 'name-buffer-consumer-v1':
                _check_name_buffer_extent(name,symbol,layout,image,relocations)
            elif symbol.get('extent_proof', {}).get('kind') == 'counted-stride-loop-v1':
                _check_counted_stride_extent(name,symbol,layout,image,relocations)
            elif symbol.get('extent_proof', {}).get('kind') in ('indexed-constant-twin-v1',
                                                               'far-pointer-word-pair-v1'):
                _check_pair_extent(name,symbol,layout,image,relocations)
            elif symbol.get('extent_proof', {}).get('kind') == 'pointer-list-table-v1':
                _check_list_table_extent(name,symbol,layout,image,relocations)
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
    check_negative_folds(recipe, symbols, image)


NEGATIVE_FOLD_FORM = 'negative folded index a[i-k]'
NEGATIVE_FOLD_KEYS = {'offset', 'target', 'k', 'element_size', 'index_operand', 'witness_site', 'form'}
# Extended ruling (Opus, integ29): byte arrays, k <= 4, index computed in the
# same function from the SAME array by strlen (the extension-replacement idiom
# `a[strlen(a) - 4]`); the inline strlen of that array is the witness.
NEGATIVE_FOLD_STRLEN_FORM = 'negative folded index a[strlen(a)-k]'
NEGATIVE_FOLD_STRLEN_KEYS = {'offset', 'target', 'k', 'element_size', 'strlen_site', 'form'}
# The pinned MSC 5.10 /Ox inline strlen of a DGROUP array feeding an index
# register: mov di,offset a / mov ax,ds / mov es,ax / mov cx,-1 / xor ax,ax /
# repne scasb / not cx / dec cx / mov R,cx.
_INLINE_STRLEN = [('mov', 'di, '), ('mov', 'ax, ds'), ('mov', 'es, ax'), ('mov', 'cx, 0xffff'),
                  ('xor', 'ax, ax'), ('repne scasb', 'al, byte ptr es:[di]'), ('not', 'cx'),
                  ('dec', 'cx'), ('mov', None)]


def _checked_strlen_fold(row, fix, recipe, target, signed, frame, image):
    require(set(row) == NEGATIVE_FOLD_STRLEN_KEYS and row['form'] == NEGATIVE_FOLD_STRLEN_FORM and
            row['element_size'] == 1 and row['k'] in (1, 2, 3, 4) and signed == -row['k'] and
            fix['displacement'] == 0 and fix['width'] == 2 and not fix['self_relative'],
            'Negative folded strlen index binding form differs')
    width = target.get('allowed_addends') and max(target['allowed_addends']) + 1
    require(type(width) is int and width >= row['k'],
            'Negative folded strlen index lacks a grounded byte-array extent')
    site = recipe['start'] + fix['offset']
    offset = target['load_address'] - frame
    function, instructions = _decoded_function_instructions(site, image)
    access = [i for i in instructions if i.address < site < i.address + i.size and
              i.address + i.disp_offset == site]
    require(len(access) == 1 and _indexed_operand(access[0]) is not None and
            int.from_bytes(image[site:site+2], 'little') == (offset + signed) & 0xffff,
            'Negative folded strlen index site is not the original runtime-indexed access')
    index = access[0].reg_name(_indexed_operand(access[0]).mem.base)
    at = [n for n, i in enumerate(instructions) if i.address == row['strlen_site']]
    require(len(at) == 1 and at[0] + len(_INLINE_STRLEN) < len(instructions) and
            instructions[at[0] + len(_INLINE_STRLEN)].address == access[0].address,
            'Negative folded strlen witness does not immediately precede the access')
    sequence = instructions[at[0]:at[0] + len(_INLINE_STRLEN)]
    for ins, (mnemonic, operands) in zip(sequence, _INLINE_STRLEN):
        require(ins.mnemonic == mnemonic and
                (operands is None or ins.op_str.startswith(operands) if mnemonic == 'mov' and operands == 'di, '
                 else operands is None or ins.op_str == operands),
                'Negative folded strlen witness is not the inline strlen of the array')
    require(sequence[0].op_str == 'di, 0x%x' % offset and
            sequence[-1].op_str == '%s, cx' % index and index in ('bx', 'si', 'di'),
            'Negative folded strlen witness scans another array or feeds another index')
    # The strlen operand is the same array through its own zero-addend FIXUPP.
    witness_offset = row['strlen_site'] + 1 - recipe['start']
    require(any(f['offset'] == witness_offset and f['target'] == fix['target'] and
                f['encoded_addend'] == '0000' and f['loc'] == 'offset16'
                for f in recipe['expected_fixups']),
            'Negative folded strlen witness lacks its own zero-addend FIXUPP of the array')
    target.setdefault('negative_folded_addends', {})[signed] = {
        'k': row['k'], 'element_size': 1, 'function': function['name'], 'form': row['form']}


def _decoded_function_instructions(site, image):
    """(function row, capstone instructions) of the instruction-verified,
    hash-checked inventory procedure containing `site`."""
    import sys
    from common import sha
    location = str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0, location)
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    from function_evidence import current_inventory
    rows = [f for f in current_inventory(image)['functions']
            if f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
            type(f.get('start')) is int and f['start'] <= site < f['end'] and
            sha(image[f['start']:f['end']]) == f['sha256']]
    require(len(rows) == 1, 'Negative folded index site lies outside one verified procedure')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    return rows[0], list(decoder.disasm(image[rows[0]['start']:rows[0]['end']], rows[0]['start']))


def _indexed_operand(ins):
    import capstone
    for op in ins.operands:
        if op.type == capstone.x86.X86_OP_MEM and ins.disp_size == 2 and \
                (ins.reg_name(op.mem.base) in ('bx', 'si', 'di') or
                 ins.reg_name(op.mem.index) in ('si', 'di')) and \
                ins.reg_name(op.mem.segment) not in ('cs', 'es', 'ss'):
            return op
    return None


def _index_loaded_from(instructions, at, operand, window=6):
    """The index of the access at `at` is loaded from `operand` shortly before."""
    before = [i for i in instructions if i.address < at][-window:]
    return any(i.mnemonic == 'mov' and i.op_str.split(', ', 1)[-1] == operand for i in before)


def check_negative_folds(recipe, symbols, image):
    """RULING (Opus, integ28): negative folded index addends.

    MSC folds `a[i - k]` into the EXTDEF of `a` with displacement
    -k*sizeof(element).  Such an addend binds only when (1) the original
    instruction at the FIXUPP site is runtime-indexed (base/index register),
    (2) its displacement is the symbol minus k (1..2) elements of the declared
    element size, (3) the same original procedure also accesses the same array
    at a non-negative element offset with the index loaded from the same
    variable, and (4) the recipe records it as 'negative folded index a[i-k]'.
    Everything else stays refused by _checked_data_addend."""
    listed = recipe.get('negative_folded_index_bindings', [])
    require(type(listed) is list, 'Negative folded-index bindings must be a list')
    if not listed:
        return
    layout = read_json(ROOT/'layout/data-symbols.json')
    frame = layout['frame_load_address']
    verified, unlisted = [], []
    for fix in recipe['expected_fixups']:
        target = (symbols or {}).get(fix['target'])
        if not isinstance(target, dict) or target.get('group') != 'DGROUP' or fix['loc'] != 'offset16':
            continue
        encoded = bytes.fromhex(fix['encoded_addend'])
        signed = int.from_bytes(encoded, 'little', signed=True) if len(encoded) == 2 else 0
        if signed >= 0 or signed in target.get('folded_addends', {}):
            continue
        rows = [r for r in listed if r.get('offset') == fix['offset'] and r.get('target') == fix['target']]
        if not rows:
            unlisted.append((fix['target'], signed))
            continue
        row = rows[0]
        if row.get('form') == NEGATIVE_FOLD_STRLEN_FORM:
            _checked_strlen_fold(row, fix, recipe, target, signed, frame, image)
            verified.append(row)
            continue
        require(set(row) == NEGATIVE_FOLD_KEYS and row['form'] == NEGATIVE_FOLD_FORM and
                row['k'] in (1, 2) and row['element_size'] in (1, 2, 4) and
                signed == -row['k'] * row['element_size'] and fix['displacement'] == 0 and
                fix['width'] == 2 and not fix['self_relative'],
                'Negative folded index binding form differs')
        width = target.get('allowed_addends') and max(target['allowed_addends']) + 1
        require(type(width) is int and width >= row['element_size'] and
                width % row['element_size'] == 0,
                'Negative folded index lacks an element-sized grounded array extent')
        site = recipe['start'] + fix['offset']
        offset = target['load_address'] - frame
        function, instructions = _decoded_function_instructions(site, image)
        access = [i for i in instructions if i.address < site < i.address + i.size and
                  i.address + i.disp_offset == site]
        require(len(access) == 1 and _indexed_operand(access[0]) is not None and
                int.from_bytes(image[site:site+2], 'little') == (offset + signed) & 0xffff,
                'Negative folded index site is not the original runtime-indexed access')
        witness = [i for i in instructions if i.address == row['witness_site']]
        operand = _indexed_operand(witness[0]) if len(witness) == 1 else None
        require(operand is not None and
                0 <= ((operand.mem.disp & 0xffff) - offset) < width and
                ((operand.mem.disp & 0xffff) - offset) % row['element_size'] == 0,
                'Negative folded index lacks a non-negative access to the same array')
        require(_index_loaded_from(instructions, access[0].address, row['index_operand']) and
                _index_loaded_from(instructions, witness[0].address, row['index_operand']),
                'Negative folded index and witness do not share the same index variable')
        target.setdefault('negative_folded_addends', {})[signed] = {
            'k': row['k'], 'element_size': row['element_size'], 'function': function['name']}
        verified.append(row)
    require(listed == verified, 'Negative folded-index recipe evidence differs')
    require(not any(signed in (symbols[name].get('negative_folded_addends') or {})
                    for name, signed in unlisted),
            'A negative folded addend is used by an unreviewed FIXUPP')


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
