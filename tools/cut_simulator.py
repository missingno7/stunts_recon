#!/usr/bin/env python3
"""Simulate MSC 5.10 CODE LEDATA/FIXUPP cuts and relocation order.

Input is JSON with an ordered ``members`` array. Each member has ``name``,
``length``, optional ``fixups`` (member-relative ``offset``, ``width``,
``kind``), and optional ``relocations`` (member-relative ``site`` plus the
MZ pair fields ``load_offset``, ``offset``, ``segment``). Member extents are
concatenated exactly as emitted; include function padding in ``length``.

Each member supplies ``instruction_ends``: sorted member-relative byte offsets
immediately after emitted instructions (each switch-table word is its own
2-byte entry). Optional ``unit_ends`` gives MSC emission units directly;
otherwise optional ``code_hex`` lets the simulator derive them with
``emission_unit_ends``. Without either, each instruction is one unit.

MSC 5.10 CODE record rule (validated against 4,487 compiled CODE records in
725 objects, build/workers/objmap): a CODE LEDATA record closes right after
the emission unit that brings it to at least 944 bytes, or right after the
unit carrying its 99th FIXUPP subrecord, whichever comes first. A two-byte
``jcc $+3`` immediately followed by a three-byte near ``JMP`` is one unit,
and each switch-table word is one unit. Observed CODE records hold at most
949 bytes. Boundaries inside a FIXUPP field are never unit ends.

Supply ``observed_record_ends`` from a compiled OMF object to use exact
records; ``rule_check`` then reports every record that disagrees with the
rule. FIXUPPs are consumed in descending site order within each record. LINK
order is records ascending, sites descending. The optional EXEPACK view stably
groups that stream by 64-KiB load-image bank.

Example:
    python tools/cut_simulator.py INPUT.json OUTPUT.json

This is an evidence simulator. It never compiles, binds, edits, or accepts an
object, and it never adopts relocation order from the oracle.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path


THRESHOLD = 944
RECORD_MAX = 949
FIXUP_LIMIT = 99
BANK_SIZE = 65536


def emission_unit_ends(code: bytes, instruction_ends: list[int]) -> list[int]:
    """Merge MSC long-branch pairs (jcc $+3; JMP near) into single units."""
    if not isinstance(code, (bytes, bytearray)):
        raise ValueError('code must be bytes')
    ends = list(instruction_ends)
    if ends != sorted(set(ends)) or (code and (not ends or ends[-1] != len(code))):
        raise ValueError('instruction_ends must be sorted and end at the code length')
    starts = [0] + ends[:-1]
    units = []
    k = 0
    while k < len(ends):
        start, end = starts[k], ends[k]
        if (end - start == 2 and 0x70 <= code[start] <= 0x7F and code[start + 1] == 3 and
                k + 1 < len(ends) and ends[k + 1] - end == 3 and code[end] == 0xE9):
            units.append(ends[k + 1])
            k += 2
            continue
        units.append(end)
        k += 1
    return units


def _omf_index(body: bytes, at: int) -> tuple[int, int]:
    if at >= len(body):
        raise ValueError('Truncated OMF index')
    if body[at] & 0x80:
        if at + 1 >= len(body):
            raise ValueError('Truncated OMF index')
        return ((body[at] & 0x7F) << 8) | body[at + 1], at + 2
    return body[at], at + 1


def observed_code_records(data: bytes, segment_index: int) -> list[dict]:
    """Ordered LEDATA records of one segment and their FIXUPP subrecord counts.

    Diagnostic reader for compiled 16-bit OMF: THREAD subrecords are skipped
    and only FIXUP subrecords are counted against the preceding LEDATA.
    """
    records, at, last = [], 0, None
    while at < len(data):
        if at + 3 > len(data):
            raise ValueError('Truncated OMF record header')
        kind = data[at]
        length = data[at + 1] | (data[at + 2] << 8)
        if length < 1 or at + 3 + length > len(data):
            raise ValueError('Truncated OMF record')
        body = data[at + 3:at + 3 + length - 1]
        if kind == 0xA0:
            index, pos = _omf_index(body, 0)
            if pos + 2 > len(body):
                raise ValueError('Truncated LEDATA offset')
            last = {'segment_index': index, 'offset': body[pos] | (body[pos + 1] << 8),
                    'size': len(body) - pos - 2, 'fixups': 0}
            records.append(last)
        elif kind in (0xA2, 0xA1, 0xA3):
            last = None
        elif kind == 0x9C:
            pos = 0
            while pos < len(body):
                head = body[pos]
                if head & 0x80:
                    if last is None:
                        raise ValueError('FIXUPP without preceding LEDATA')
                    pos += 2
                    if pos >= len(body):
                        raise ValueError('Truncated FIXUP subrecord')
                    fixdat = body[pos]; pos += 1
                    if not fixdat & 0x80 and (fixdat >> 4) & 7 < 3:
                        _, pos = _omf_index(body, pos)
                    if not fixdat & 0x08:
                        _, pos = _omf_index(body, pos)
                    if not fixdat & 0x04:
                        pos += 2
                    last['fixups'] += 1
                else:
                    pos += 1
                    # Target threads always carry an index; frame threads F0-F2 do.
                    if not head & 0x40 or (head >> 2) & 7 < 3:
                        _, pos = _omf_index(body, pos)
                if pos > len(body):
                    raise ValueError('Truncated FIXUPP record')
        at += 3 + length
        if kind in (0x8A, 0x8B):
            break
    return [row for row in records if row['segment_index'] == segment_index]


def _checked_boundaries(values, length, name, label):
    if (not isinstance(values, list) or len(values) > 100000 or
            any(type(boundary) is not int for boundary in values) or
            values != sorted(set(values)) or
            any(not 0 < boundary <= length for boundary in values) or
            (length and (not values or values[-1] != length))):
        raise ValueError(f'{name}: {label} must be sorted boundaries ending at length')
    return values


def rule_end(start: int, unit_cuts: list[int], fixup_sites: list[int], cursor: int,
             *, threshold: int = THRESHOLD, fixup_limit: int = FIXUP_LIMIT,
             record_max: int = RECORD_MAX) -> tuple[int, str]:
    """End of the CODE record beginning at ``start`` under the MSC 5.10 rule."""
    count = 0
    sites = iter(sorted(site for site in fixup_sites if site >= start))
    pending = next(sites, None)
    for boundary in unit_cuts:
        if boundary <= start:
            continue
        while pending is not None and pending < boundary:
            count += 1
            pending = next(sites, None)
        if count > fixup_limit:
            raise ValueError(f'one emission unit carries FIXUPP {count} past the limit at {start}')
        if boundary - start > record_max:
            raise ValueError(f'emission unit overflows the {record_max}-byte CODE record maximum at {start}')
        if count == fixup_limit:
            return boundary, 'fixup-count-limit'
        if boundary - start >= threshold:
            return boundary, 'code-threshold-944'
        if boundary == cursor:
            return boundary, 'object-end'
    raise ValueError(f'no emission unit ends after CODE offset {start}')


def simulate(document: dict, *, fixup_limit: int = FIXUP_LIMIT,
             bank_size: int = BANK_SIZE) -> dict:
    if document.get('schema') != 'msc510-code-cut-input-v1':
        raise ValueError('input schema must be msc510-code-cut-input-v1')
    members = document.get('members')
    if not isinstance(members, list) or not members:
        raise ValueError('members must be a non-empty ordered array')
    if len(members) > 4096:
        raise ValueError('member count exceeds simulator limit')
    if fixup_limit <= 0 or bank_size <= 0:
        raise ValueError('limits must be positive')

    normalized = []
    all_fixups = []
    all_relocations = []
    unit_boundaries = set()
    cursor = 0
    for member in members:
        name, length = member.get('name'), member.get('length')
        if not isinstance(name, str) or type(length) is not int or length < 0:
            raise ValueError('each member needs a name and non-negative integer length')
        if cursor + length > 250000:
            raise ValueError('CODE span exceeds simulator limit')
        base = cursor
        fixups = member.get('fixups', [])
        relocations = member.get('relocations', [])
        if not isinstance(fixups, list) or not isinstance(relocations, list):
            raise ValueError(f'{name}: fixups and relocations must be arrays')
        if (len(all_fixups) + len(fixups) > 50000 or
                len(all_relocations) + len(relocations) > 50000):
            raise ValueError('record inputs exceed simulator limit')
        instruction_ends = _checked_boundaries(member.get('instruction_ends'), length,
                                               name, 'instruction_ends')
        units = member.get('unit_ends')
        if units is None and member.get('code_hex') is not None:
            code = bytes.fromhex(member['code_hex'])
            if len(code) != length:
                raise ValueError(f'{name}: code_hex length differs from member length')
            units = emission_unit_ends(code, instruction_ends)
        if units is None:
            units = instruction_ends
        units = _checked_boundaries(units, length, name, 'unit_ends')
        if not set(units) <= set(instruction_ends):
            raise ValueError(f'{name}: every unit end must be an instruction end')
        normalized.append({'name': name, 'start': base, 'end': base + length,
                           'length': length, 'instruction_ends': instruction_ends,
                           'unit_ends': units})
        unit_boundaries.update(base + end for end in units)
        cursor += length
        for raw in fixups:
            offset, width, kind = raw.get('offset'), raw.get('width'), raw.get('kind')
            if type(offset) is not int or type(width) is not int or width <= 0 or not isinstance(kind, str):
                raise ValueError(f'{name}: invalid fixup {raw!r}')
            if not (0 <= offset and offset + width <= length):
                raise ValueError(f'{name}: fixup outside member: {raw!r}')
            all_fixups.append({**raw, 'member': name, 'site': base + offset,
                               'end': base + offset + width})
        for raw in relocations:
            site = raw.get('site')
            if type(site) is not int or not 0 <= site < length:
                raise ValueError(f'{name}: invalid relocation site {raw!r}')
            if not all(type(raw.get(k)) is int for k in ('load_offset', 'offset', 'segment')):
                raise ValueError(f'{name}: relocation needs integer load_offset/offset/segment')
            all_relocations.append({**raw, 'member': name, 'site': base + site})
    all_fixups.sort(key=lambda row: (row['site'], row['end'], row['member']))
    all_relocations.sort(key=lambda row: (row['site'], row['member']))
    fixup_sites = [f['site'] for f in all_fixups]

    field_interiors = {p for f in all_fixups for p in range(f['site'] + 1, f['end'])}

    def inside_field(boundary):
        return boundary in field_interiors
    safe_cuts = sorted(b for b in {0, cursor, *(m['start'] + end for m in normalized
                                                for end in m['instruction_ends'])}
                       if not inside_field(b))
    unit_cuts = sorted(b for b in unit_boundaries | {cursor} if not inside_field(b))
    observed = document.get('observed_record_ends')
    if observed is not None and (
            not isinstance(observed, list) or not observed or len(observed) > 4096 or
            any(type(end) is not int for end in observed) or
            observed != sorted(set(observed)) or observed[0] <= 0 or observed[-1] != cursor or
            any(end not in safe_cuts for end in observed)):
        raise ValueError('observed CODE cuts must be safe ascending ends covering the object')
    records = []
    rule_violations = []
    start = 0
    while start < cursor:
        if len(records) >= 4096:
            raise ValueError('record count exceeds simulator limit')
        if observed is not None:
            end = observed[len(records)]
            reasons = ['observed-omf-ledata']
            if end - start > RECORD_MAX:
                raise ValueError('observed CODE record exceeds the 949-byte record maximum')
            try:
                predicted, why = rule_end(start, unit_cuts, fixup_sites, cursor,
                                          fixup_limit=fixup_limit)
            except ValueError as error:
                predicted, why = None, str(error)
            if predicted != end:
                rule_violations.append({'record': len(records), 'start': start, 'observed_end': end,
                                        'rule_end': predicted, 'rule_reason': why})
        else:
            end, why = rule_end(start, unit_cuts, fixup_sites, cursor, fixup_limit=fixup_limit)
            reasons = [why]
        record_fixups = [f for f in all_fixups if start <= f['site'] < end]
        record_relocations = [r for r in all_relocations if start <= r['site'] < end]
        if len(record_fixups) > fixup_limit:
            raise ValueError('record exceeds FIXUPP limit')
        # The relocations arising from FIXUPPs are in descending location order.
        record_relocations.sort(key=lambda row: row['site'], reverse=True)
        records.append({
            'index': len(records), 'start': start, 'end': end,
            'code_bytes': end - start, 'fixup_count': len(record_fixups),
            'reason': reasons,
            'fixup_sites_descending': [f['site'] for f in sorted(record_fixups,
                                         key=lambda row: row['site'], reverse=True)],
            'relocations': record_relocations,
        })
        start = end

    link_relocations = [r for record in records for r in record['relocations']]
    exepack_relocations = [row for _, row in sorted(enumerate(link_relocations),
                             key=lambda pair: (pair[1]['load_offset'] // bank_size,
                                               pair[0]))]
    result = {
        'schema': 'msc510-code-cut-output-v2',
        'cut_basis': 'observed-omf-ledata' if observed is not None else 'msc510-threshold-rule',
        'limits': {'record_threshold_bytes': THRESHOLD,
                   'code_bytes_per_record_max': RECORD_MAX,
                   'fixupp_subrecords_per_record': fixup_limit,
                   'exepack_bank_bytes': bank_size},
        'members': normalized,
        'code_length': cursor,
        'records': records,
        'link_order_relocations': link_relocations,
        'exepack_order_relocations': exepack_relocations,
    }
    if observed is not None:
        result['rule_check'] = {'consistent': not rule_violations, 'violations': rule_violations}
    return result


def main(argv: list[str]) -> int:
    if len(argv) not in (2, 3):
        print(__doc__.split('Example:')[0].strip(), file=sys.stderr)
        return 2
    source = Path(argv[1])
    document = json.loads(source.read_text(encoding='utf-8'))
    result = simulate(document)
    rendered = json.dumps(result, indent=2) + '\n'
    if len(argv) == 3:
        Path(argv[2]).write_text(rendered, encoding='utf-8')
    else:
        sys.stdout.write(rendered)
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
