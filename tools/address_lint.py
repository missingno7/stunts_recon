"""Numeric program-address lint for accepted sources (diagnostic; integ27).

Sources must name program addresses symbolically, even where the original word
carries no MZ relocation (DGROUP offsets, CS-resident data).  The acceptance
path already rejects a literal standing in for an MZ-relocated word
(binder.require_relocations_from_fixups).  This lint covers the unrelocated
16-bit cases: it compiles or assembles each accepted source with its pinned
profile, decodes the emitted CODE, and reports every displacement or
immediate operand that the candidate emits WITHOUT a FIXUPP while its value
equals the address of an independently grounded symbol:

* HIGH   a direct memory operand [disp16] >= 100h (no base/index) equal to a
         grounded DGROUP symbol offset (DS/ES/SS) or, with a CS override, to a
         reviewed CS island symbol; any CS-island address used as a MOV/LEA
         offset;
* REVIEW an indexed displacement or MOV/PUSH/CMP immediate >= 100h, or a
         direct operand below 100h, inside a grounded DGROUP object extent
         (DS may not be DGROUP; small constants coincide with low offsets).

A finding is a lint result, never an acceptance decision: a HIGH finding in
a separately owned source is fixed by a symbolic reference (EXTDEF + FIXUPP)
and re-verified; REVIEW findings are reported for human review (constants can
coincide with addresses).  The report never confers or removes ownership."""
import argparse
import json
import sys

from common import ROOT, read_json, write_json

MIN_REVIEW = 0x100


def _decoder():
    sys.path.insert(0, str(ROOT/'build/python'))
    from capstone import Cs, CS_ARCH_X86, CS_MODE_16
    from capstone import x86 as X
    md = Cs(CS_ARCH_X86, CS_MODE_16)
    md.detail = True
    return md, X


def grounded_addresses():
    """(dgroup ranges, cs ranges) of independently grounded symbols.

    DGROUP: offset -> symbol for every data alias (address, width or 1).
    CS: (frame, offset) ranges of reviewed code-island symbols."""
    layout = read_json(ROOT/'layout/data-symbols.json')
    frame = layout['frame_load_address']
    dgroup, cs = [], []
    for name, symbol in layout['symbols'].items():
        if symbol.get('storage') == 'code_island':
            island = layout['code_islands'][symbol['island']] if symbol.get('island') in \
                layout['code_islands'] else None
            base = island['frame_load_address'] if island else None
            if base is None:
                continue
            cs.append((base, symbol['load_address'] - base,
                       symbol['load_address'] - base + max(1, symbol.get('width', 1)), name))
        else:
            offset = symbol['load_address'] - frame
            if 0 <= offset < 65536:
                dgroup.append((offset, offset + max(1, symbol.get('width') or 1), name))
    for key, island in layout['code_islands'].items():
        base = island['frame_load_address']
        cs.append((base, island['start'] - base, island['end'] - base, 'island:' + key))
    return dgroup, cs


def _match(ranges, value, exact=False):
    return sorted({name for lo, hi, name in ranges if (value == lo if exact else lo <= value < hi)})


def asm_data_offsets(listing, segment):
    """CODE offsets emitted by db/dw/dd directives in a MASM listing (not decoded)."""
    import re
    body = listing.split('Segments and Groups:')[0].replace(chr(12), '').replace(chr(8), '')
    rows, stack = [], []
    for line in body.splitlines():
        text = line.split(chr(9))[-1].strip().lower()
        opened = re.match(r'^(\S+)\s+(segment|struc)\b', text)
        if opened:
            stack.append(opened.group(1))
        elif re.match(r'^\S+\s+ends\b', text) and stack:
            stack.pop()
        emitted = re.match(r'^ ([0-9A-F]{4})  [0-9A-F]{2}', line)
        if stack and stack[-1] == segment.lower() and emitted:
            rows.append((int(emitted.group(1), 16),
                         re.search(r'(^|\s)d[bwd]\s', ' ' + text + ' ') is not None))
    data = set()
    for (offset, is_data), following in zip(rows, rows[1:] + [(None, False)]):
        if is_data:
            data.update(range(offset, following[0] if following[0] is not None else offset + 4))
    return data


def lint_object(obj, segment, code_frame=None, grounded=None, data_offsets=()):
    """Findings for one emitted object (candidate CODE, FIXUPP-covered bytes excluded)."""
    md, X = _decoder()
    dgroup, cs = grounded or grounded_addresses()
    # A CS-override operand names a CS island of the code frame (every island
    # frame when the recipe records none); a plain MOV/LEA offset is checked
    # only against islands of the recipe's recorded frame.
    cs_override_set = [(lo, hi, n) for base, lo, hi, n in cs if code_frame is None or base == code_frame]
    cs_here = [(lo, hi, n) for base, lo, hi, n in cs if code_frame is not None and base == code_frame]
    code = bytes(obj.segments.get(segment, b''))
    covered = set()
    for fix in obj.linker_fixups:
        if fix['segment'] == segment:
            covered.update(range(fix['offset'], fix['offset'] + fix['width']))
    findings, at = [], 0
    data_offsets = set(data_offsets)
    while at < len(code):
        if at in data_offsets:
            at += 1
            continue
        insn = next(md.disasm(code[at:at+16], at), None)
        if insn is None:
            at += 1
            continue
        cs_override = X.X86_PREFIX_CS in insn.prefix
        for op in insn.operands:
            if op.type == X.X86_OP_MEM and insn.disp_size == 2:
                field = at + insn.disp_offset
                value = op.mem.disp & 0xFFFF
                direct = op.mem.base == 0 and op.mem.index == 0
                kind = 'disp'
            elif op.type == X.X86_OP_IMM and insn.imm_size == 2:
                field = at + insn.imm_offset
                value = op.imm & 0xFFFF
                direct = False
                kind = 'imm'
            else:
                continue
            if field in covered:
                continue
            level = names = None
            if cs_override:
                names = _match(cs_override_set, value)
                level = 'HIGH' if names else None
            elif (kind == 'imm' and insn.mnemonic == 'mov' or insn.mnemonic == 'lea') and value >= MIN_REVIEW:
                # An offset of a named reviewed CS-island datum (e.g. after
                # ES=CS); an unnamed position inside an island is a REVIEW.
                names = _match(cs_here, value)
                named = [n for n in names if not n.startswith('island:')]
                level = 'HIGH' if named else ('REVIEW' if names else None)
            if level is None and not cs_override:
                if direct:
                    names = _match(dgroup, value)
                    level = ('HIGH' if value >= MIN_REVIEW else 'REVIEW') if names else None
                elif value >= MIN_REVIEW and (kind == 'disp' or insn.mnemonic in ('mov', 'push', 'cmp', 'lea')):
                    names = _match(dgroup, value)
                    level = 'REVIEW' if names else None
            if level:
                findings.append({'offset': at, 'field': field, 'value': value, 'level': level,
                                 'instruction': f'{insn.mnemonic} {insn.op_str}', 'symbols': names[:4]})
        at += insn.size
    return findings


def compile_recipe(recipe):
    from compiler import compile_source
    from assembler import assemble_source
    from object_flags import recipe_flags
    from object_probe import recipe_sparse_zero
    source = (ROOT/recipe['source']).read_bytes()
    if recipe.get('kind') == 'asm':
        obj, receipt = assemble_source(source, recipe['profile'])
        from pathlib import Path
        listing = (Path(receipt['work_directory'])/'UNIT.LST').read_text(encoding='latin1')
        return obj, asm_data_offsets(listing, recipe['object_segment'])
    return compile_source(source, recipe['profile'], recipe_flags(recipe),
                          sparse_zero=recipe_sparse_zero(recipe))[0], set()


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--report', default='build/address_lint.json')
    p.add_argument('--owner', action='append', help='Limit to these manifest owner ids')
    a = p.parse_args(argv)
    grounded = grounded_addresses()
    manifest = read_json(ROOT/'layout/manifest.json')
    rows = []
    for owner in manifest['owners']:
        if owner['kind'] not in ('MATCHING_C', 'MATCHING_ASM') or 'recipe' not in owner:
            continue
        if a.owner and owner['id'] not in a.owner:
            continue
        recipe = read_json(ROOT/owner['recipe'])
        if recipe.get('data_only'):
            continue
        try:
            obj, data_offsets = compile_recipe(recipe)
        except Exception as error:
            rows.append({'owner': owner['id'], 'error': str(error)[:200]})
            continue
        frame = recipe.get('original_frame_load_address')
        findings = lint_object(obj, recipe['object_segment'], frame, grounded, data_offsets)
        if findings:
            rows.append({'owner': owner['id'], 'source': recipe['source'], 'kind': recipe.get('kind', 'c'),
                         'start': recipe['start'], 'findings': findings})
    report = {'schema': 'address-lint-v1', 'authority': 'DIAGNOSTIC lint; never confers or removes ownership',
              'rule': __doc__.split('\n\n')[1], 'owners': rows,
              'high': sum(f['level'] == 'HIGH' for r in rows for f in r.get('findings', [])),
              'review': sum(f['level'] == 'REVIEW' for r in rows for f in r.get('findings', []))}
    write_json(ROOT/a.report, report)
    for row in rows:
        high = [f for f in row.get('findings', []) if f['level'] == 'HIGH']
        if high or 'error' in row:
            print(row['owner'], row.get('error') or [(f['offset'], f['instruction'], f['symbols']) for f in high][:6])
    print('HIGH', report['high'], 'REVIEW', report['review'], '->', a.report)
    return report


if __name__ == '__main__':
    main()
