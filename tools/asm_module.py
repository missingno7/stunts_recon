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


def expected_publics(name, kind):
    """Accepted spellings: the C underscore convention, or its MASM truncation."""
    full = '_' + name
    return {full, masm_public(name)} if kind == 'asm' and len(full) > 31 else {full}


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


def checked_module(recipe, image):
    """Validate a whole-module ASM recipe against the oracle and inventory."""
    from function_evidence import current_inventory
    require(recipe.get('kind') == 'asm', 'Whole-module contributions are ASM modules')
    proof = recipe['module_proof']
    start, end = recipe['start'], recipe['end']
    frame = recipe.get('original_frame_load_address')
    require(set(proof) <= {'kind', 'start_boundary', 'end_boundary', 'cross_kind_links',
                           'embedded_publics'} and
            proof.get('kind') == 'asm-module-extent-v1' and
            type(frame) is int and frame % 16 == 0 and frame <= start < end <= frame + 65536,
            'Whole-module proof shape or frame differs')
    inventory = current_inventory(image)
    require(inventory['load_sha256'] == sha(image), 'Inventory/oracle identity differs')
    rows = _function_rows(inventory, image)
    _checked_boundary(proof['start_boundary'], start, 'start', frame, rows, image)
    _checked_boundary(proof['end_boundary'], end, 'end', frame, rows, image)
    members = recipe['members']
    require(type(members) is list and members and members[0]['start'] == start and
            all(members[i]['start'] < members[i+1]['start'] for i in range(len(members)-1)),
            'Module entries must be ordered and begin at the module start')
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
