"""Whole-module ASM contributions and cross-kind subsumption proofs.

A complete MASM module covering one grounded module extent is accepted as one
unit when (a) both ends are grounded from the oracle, (b) every public entry
lands on an inventory-verified function start inside the module, and (c) the
normal strict gates hold (declarations, ordered FIXUPPs, exact bytes, ordered
MZ relocations, fresh whole-image equality).  Procedure ends and unmapped labels
inside the module are internal to the one contribution.  An inventory row whose
end runs one linker-fill byte past a grounded module end is clipped to it.

The same grounded module may subsume an accepted C owner inside it
(reclassified MATCHING_ASM) only with a same-module link proof for that owner:
an odd start without pad, an original short branch crossing its boundary, or
membership of the shared ASM code frame of the module.
"""
from common import require, sha

_RETURNS1 = {0xcb, 0xc3, 0xcf}
_RETURNS3 = {0xca, 0xc2}
VERIFIED = ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED', 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')


def masm_public(name):
    """MASM 5.10 keeps 31 significant characters of a public name."""
    return ('_' + name)[:31]


def c_public(name):
    """MSC 5.10 PUBDEF/EXTDEF spelling: 31 significant identifier characters
    plus the underscore (32 in total), verified with the pinned compiler; the
    pinned MSC 5.00 keeps the full name."""
    return ('_' + name)[:32]


def expected_publics(name, kind):
    """Accepted spellings: the C underscore convention, or its MASM 5.10
    (31 characters) or MSC 5.10 (32 characters) truncation."""
    full = '_' + name
    if kind == 'asm':
        return {full, masm_public(name)} if len(full) > 31 else {full}
    return {full, c_public(name)} if len(full) > 32 else {full}


def _ends_with_return(image, at):
    """True when a complete unconditional transfer (RET/RETF/IRET, or JMP
    short/near/far) ends exactly at `at`: execution cannot fall into a
    following fill byte."""
    return ((at >= 1 and image[at-1] in _RETURNS1) or
            (at >= 2 and image[at-2] == 0xeb) or
            (at >= 3 and image[at-3] in _RETURNS3 | {0xe9}) or
            (at >= 5 and image[at-5] == 0xea))


def _function_rows(inventory, image):
    rows = []
    for f in inventory['functions']:
        if f.get('status') in VERIFIED and type(f.get('start')) is int and type(f.get('end')) is int:
            rows.append(f)
    return rows


def _checked_boundary(proof, at, side, frame, rows, image):
    kind = proof.get('kind') if isinstance(proof, dict) else None
    if kind == 'zero-fill-after-return':
        # LINK/assembler fill: one zero byte after a return, outside the module.
        if side == 'start':
            require(image[at-1] == 0 and _ends_with_return(image, at-1),
                    'Module start lacks a preceding zero fill after a return')
        else:
            require(image[at] == 0 and _ends_with_return(image, at) and
                    proof.get('fill_bytes', 1) == 1 and
                    (image[at+1] != 0 or any(f['start'] == at+1 for f in rows)),
                    'Module end lacks a following zero fill after a return')
    elif kind == 'zero-fill-included':
        require(side == 'end' and image[at-1] == 0 and _ends_with_return(image, at-1),
                'Module end lacks its own terminal zero fill after a return')
    elif kind == 'segment-frame-change':
        # The neighbouring verified function belongs to another code frame.
        if side == 'start':
            neighbours = [f for f in rows if f['end'] == at]
        else:
            neighbours = [f for f in rows if f['start'] == at]
        require(len(neighbours) == 1 and
                neighbours[0].get('segment_paragraph') is not None and
                neighbours[0]['segment_paragraph']*16 != frame and
                sha(image[neighbours[0]['start']:neighbours[0]['end']]) == neighbours[0]['sha256'] and
                proof.get('neighbour') == neighbours[0]['name'],
                'Module boundary lacks a verified neighbour in another code frame')
    else:
        require(False, 'Unknown module boundary proof')


def _instruction_starts(image, lo, hi):
    import sys
    from common import ROOT
    location = str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0, location)
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    rows, at = {}, lo
    for ins in decoder.disasm(image[lo:hi], lo):
        if ins.address != at: break
        rows[at] = bytes(ins.bytes); at += ins.size
    return rows, at


def _checked_private_prefix(proof, start, end, frame, rows, image):
    """Private code before the module's first public entry (integ26).

    The module start is grounded by its start boundary; the prefix bytes
    [start, start+length) must decode completely, every path from the module
    start must end in a return or a direct transfer (or fall-through) onto an
    instruction of a verified procedure of this module and frame, no indirect
    jump may occur, and bytes no path reaches are 90 pads.  At least one path
    enters a verified module procedure, tying the prefix to this module."""
    require(isinstance(proof, dict) and set(proof) == {'kind', 'length'} and
            proof['kind'] == 'private-entry-prefix-v1' and type(proof['length']) is int and
            0 < proof['length'] and start + proof['length'] < end,
            'Private module prefix proof shape differs')
    first = start + proof['length']
    by, at = _instruction_starts(image, start, first)
    require(at == first, 'Private module prefix does not decode completely')
    module_rows = [f for f in rows if start <= f['start'] < end and
                   f.get('segment_paragraph') is not None and f['segment_paragraph']*16 == frame and
                   sha(image[f['start']:f['end']]) == f['sha256']]
    decoded = {}
    def enters_module(target):
        for f in module_rows:
            if f['start'] <= target < f['end']:
                if f['start'] not in decoded:
                    decoded[f['start']] = _instruction_starts(image, f['start'], f['end'])[0]
                return target in decoded[f['start']]
        return False
    pending, seen, entered = [start], set(), False
    while pending:
        at = pending.pop()
        if at in seen: continue
        if not start <= at < first:
            require(enters_module(at), 'Private prefix leaves the module outside a verified procedure')
            entered = True; continue
        require(at in by, 'Private prefix branch is not an instruction boundary')
        seen.add(at); raw = by[at]; nxt = at + len(raw)
        op = raw[0]
        if op in _RETURNS1 | _RETURNS3: continue
        require(op not in (0xea, 0x9a) and not (op == 0xff and ((raw[1] >> 3) & 7) in (2, 3, 4, 5)),
                'Private prefix has an indirect or far transfer')
        if op == 0xeb or 0x70 <= op <= 0x7f or 0xe0 <= op <= 0xe3:
            pending.append(nxt + int.from_bytes(raw[1:2], 'little', signed=True))
            if op == 0xeb: continue
        elif op in (0xe9, 0xe8):
            pending.append(nxt + int.from_bytes(raw[1:3], 'little', signed=True))
            if op == 0xe9: continue
        pending.append(nxt)
    require(entered and all(by[at] == b'\x90' for at in set(by) - seen),
            'Private prefix lacks a module transfer or has non-pad unreachable bytes')
    return first


def _checked_data_public(p, start, end, frame, rows, starts, image):
    """A labelled data declaration outside every verified procedure (integ26).

    The public is the underscore spelling of a label declared by a `db`/`dw`/`dd`
    line of a pinned reference segment listing; it lies inside the module and
    outside every verified procedure.  It binds nothing; a spelling equal to a
    reviewed code/data alias must name that alias's own address."""
    import re
    from common import ROOT, identity, read_json
    require(isinstance(p, dict) and set(p) == {'public', 'offset', 'reference_path', 'reference_line'} and
            type(p['offset']) is int and type(p['reference_line']) is int,
            'Module data-public proof shape differs')
    at = start + p['offset']
    require(start < at < end and at not in starts and
            not any(f['start'] <= at < f['end'] for f in rows),
            'Module data public lies inside a procedure or outside the module')
    path = p['reference_path']
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files']
    require(re.fullmatch(r'src/restunts/asmorig/seg\d{3}\.asm', path) is not None and path in pinned,
            'Module data public listing is not pinned')
    source = ROOT/'build/references/restunts'/path
    require(identity(source.read_bytes()) == pinned[path], 'Module data public listing differs')
    # Pinned listings contain latin1 bytes that str.splitlines would treat
    # as separators; line numbers count LF only.
    lines = [line.rstrip(chr(13)) for line in source.read_text(encoding='latin1').split(chr(10))]
    require(1 <= p['reference_line'] <= len(lines), 'Module data public line missing')
    match = re.match(r'^([A-Za-z_$?@][\w$?@]*)\s+(?:db|dw|dd)\b', lines[p['reference_line']-1].strip(), re.I)
    require(match is not None and p['public'] in ('_' + match.group(1), masm_public(match.group(1))),
            'Module data public differs from its pinned label declaration')
    data = read_json(ROOT/'layout/data-symbols.json')['symbols'].get(p['public'])
    code = read_json(ROOT/'layout/code-symbols.json')['symbols'].get(p['public'])
    require((data is None or data.get('load_address') == at) and code is None,
            'Module data public conflicts with a reviewed alias')
    return at


def checked_module(recipe, image):
    """Validate a whole-module ASM recipe against the oracle and inventory."""
    from function_evidence import current_inventory
    require(recipe.get('kind') == 'asm', 'Whole-module contributions are ASM modules')
    proof = recipe['module_proof']
    start, end = recipe['start'], recipe['end']
    frame = recipe.get('original_frame_load_address')
    require(set(proof) <= {'kind', 'start_boundary', 'end_boundary', 'cross_kind_links',
                           'embedded_publics', 'private_prefix', 'data_publics'} and
            proof.get('kind') == 'asm-module-extent-v1' and
            type(frame) is int and frame % 16 == 0 and frame <= start < end <= frame + 65536,
            'Whole-module proof shape or frame differs')
    inventory = current_inventory(image)
    require(inventory['load_sha256'] == sha(image), 'Inventory/oracle identity differs')
    rows = _function_rows(inventory, image)
    _checked_boundary(proof['start_boundary'], start, 'start', frame, rows, image)
    _checked_boundary(proof['end_boundary'], end, 'end', frame, rows, image)
    members = recipe['members']
    first = start
    if 'private_prefix' in proof:
        first = _checked_private_prefix(proof['private_prefix'], start, end, frame, rows, image)
    require(type(members) is list and members and members[0]['start'] == first and
            all(members[i]['start'] < members[i+1]['start'] for i in range(len(members)-1)),
            'Module entries must be ordered and begin at the module start or its private prefix end')
    clip = end + (1 if proof['end_boundary'].get('kind') == 'zero-fill-after-return' else 0)
    relocations = None
    for m in members:
        require(set(m) <= {'name', 'public', 'start', 'stable_id', 'entry_anchors'} and
                start <= m['start'] < end,
                'Module entry lies outside the module')
        if 'entry_anchors' in m:
            # A mapped entry whose end is unmapped: its start is grounded by
            # original relocated far CALLs from instruction-verified callers.
            if relocations is None:
                from oracle import verify
                relocations = verify(write=False)[2]['unpacked_mz']['relocations']
            partial = [f for f in inventory['functions'] if f.get('name') == m['name'] and
                       f.get('status') == 'PARTIAL_UNMAPPED' and f.get('start') == m['start']]
            require(len(partial) == 1 and m['entry_anchors'] and
                    m.get('stable_id') == partial[0].get('stable_id') and
                    partial[0].get('segment_paragraph') is not None and
                    partial[0]['segment_paragraph']*16 == frame and
                    m['public'] in expected_publics(m['name'], 'asm'),
                    'Partial module entry lacks its inventory start: ' + m['name'])
            for a in m['entry_anchors']:
                raw = bytes.fromhex(a['hex']); site = a['site']
                callers = [f for f in rows if f['status'] == VERIFIED[0] and
                           f['start'] <= site and site + 5 <= f['end'] and
                           not start <= site < end]
                require(len(raw) == 5 and raw[0] == 0x9a and image[site:site+5] == raw and
                        a['relocation'] in relocations and a['relocation']['load_offset'] == site+3 and
                        int.from_bytes(raw[3:5], 'little')*16 == frame and
                        frame + int.from_bytes(raw[1:3], 'little') == m['start'] and len(callers) == 1,
                        'Partial module entry anchor differs: ' + m['name'])
            continue
        found = [f for f in rows if f.get('name') == m['name']]
        require(len(found) == 1, 'Module public lacks an inventory-verified entry: ' + m['name'])
        f = found[0]
        require(f['start'] == m['start'] and m.get('stable_id') == f.get('stable_id') and
                f.get('segment_paragraph') is not None and f['segment_paragraph']*16 == frame and
                sha(image[f['start']:f['end']]) == f['sha256'] and
                (f['end'] <= end or f['end'] == clip) and
                m['public'] in expected_publics(m['name'], 'asm'),
                'Module entry differs from inventory evidence: ' + m['name'])
    # A data label inside a verified procedure (an embedded table) may be a
    # module public; it binds nothing and is not a function entry.
    starts = {m['start'] for m in members}
    for p in proof.get('embedded_publics', []):
        at = start + p.get('offset', -1) if type(p.get('offset')) is int else -1
        require(set(p) == {'public', 'offset'} and start < at < end and at not in starts and
                any(f['start'] < at < f['end'] and start <= f['start'] and
                    f.get('segment_paragraph') is not None and f['segment_paragraph']*16 == frame
                    for f in rows),
                'Embedded module public lies outside a verified module procedure')
    names = {m['public'] for m in members} | {p['public'] for p in proof.get('embedded_publics', [])}
    for p in proof.get('data_publics', []):
        at = _checked_data_public(p, start, end, frame, rows, starts, image)
        require(p['public'] not in names, 'Module data public duplicates another public')
        names.add(p['public'])
    # Every verified function starting inside the module must end inside it
    # (or at the clipped fill byte): a module never cuts a verified procedure.
    for f in rows:
        if start <= f['start'] < end:
            require(f['end'] <= end or f['end'] == clip,
                    'Verified function crosses the module end: ' + f['name'])
        require(not (f['start'] < start < f['end']),
                'Verified function crosses the module start: ' + f['name'])
    return inventory


def checked_cross_kind_link(owner, recipe, image, inventory):
    """Recheck the reviewed same-module link for one subsumed C owner."""
    links = [l for l in recipe['module_proof'].get('cross_kind_links', [])
             if l.get('owner') == owner['id']]
    require(len(links) == 1, 'Cross-kind subsumption lacks a same-module link proof')
    link = links[0]
    start, end = owner['start'], owner['end']
    frame = recipe['original_frame_load_address']
    kind = link.get('kind')
    rows = [f for f in inventory['functions'] if f.get('name') == owner['name']]
    require(len(rows) == 1 and (rows[0]['start'], rows[0]['end']) == (start, end) and
            rows[0].get('segment_paragraph') is not None and
            rows[0]['segment_paragraph']*16 == frame,
            'Cross-kind owner is not an inventory function of the module frame')
    if kind == 'odd-start':
        # MSC word-aligns every function start; an odd start with no pad byte
        # before it continues the preceding code of the same assembled module.
        require(start % 2 == 1 and image[start-1] not in (0x00, 0x90) and
                recipe['start'] < start,
                'Odd-start link differs')
    elif kind == 'short-branch':
        site = link.get('site')
        require(type(site) is int and recipe['start'] <= site < recipe['end'] - 1,
                'Short-branch link site outside module')
        opcode = image[site]
        require(opcode == 0xeb or 0x70 <= opcode <= 0x7f or 0xe0 <= opcode <= 0xe3,
                'Short-branch link is not a rel8 transfer')
        target = site + 2 + int.from_bytes(image[site+1:site+2], 'little', signed=True)
        inside_site = start <= site < end
        inside_target = start <= target < end
        require(inside_site != inside_target and recipe['start'] <= target < recipe['end'] and
                link.get('target') == target,
                'Short-branch link does not cross the owner boundary')
    elif kind == 'shared-asm-frame':
        # The C owner shares the module's code frame (MSC would give a C
        # object its own segment frame), and the neighbours on both sides are
        # members of the same zero-fill-bounded module.
        require(recipe['start'] < start and end < recipe['end'] and
                recipe['module_proof']['start_boundary'].get('kind') in
                ('zero-fill-after-return', 'segment-frame-change') and
                recipe['module_proof']['end_boundary'].get('kind') in
                ('zero-fill-after-return', 'zero-fill-included', 'segment-frame-change') and
                image[start-1] != 0 and image[end] != 0,
                'Shared-frame link differs')
    else:
        require(False, 'Unknown cross-kind link proof')
    return link
