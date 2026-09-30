"""Pinned Restunts source facts available without the ignored source checkout.

The full Restunts checkout remains preferred and is checked against
layout/references.json.  Clean clones can instead use the short, tracked source
excerpts in tests/fixtures/references/restunts_excerpts.json.  Each excerpt row
records the identity of its complete upstream file; placement spans and table
values are reduced to the exact facts used by the resolvers.
"""
import re
from functools import lru_cache
from pathlib import Path

from common import ROOT, identity, read_json, require


FIXTURE = ROOT / 'tests/fixtures/references/restunts_excerpts.json'
SOURCE_ROOT = ROOT / 'build/references/restunts'


def _pinned(path):
    files = read_json(ROOT / 'layout/references.json')['restunts']['evidence_files']
    require(path in files, 'Reference source is not pinned: ' + path)
    return files[path]


@lru_cache(maxsize=1)
def _fixture():
    return read_json(FIXTURE)


@lru_cache(maxsize=None)
def _record(path):
    expected = _pinned(path)
    source = SOURCE_ROOT / path
    if source.is_file():
        require(identity(source.read_bytes()) == expected,
                'Pinned reference source differs: ' + path)
        return source, None
    doc = _fixture()
    require(doc.get('schema') == 'restunts-reference-excerpts-v3' and
            doc.get('line_numbering') == 'python-str.splitlines-v1',
            'Pinned reference excerpt fixture schema differs')
    row = doc.get('files', {}).get(path)
    require(row is not None and row.get('source_identity') == expected,
            'Pinned reference excerpt is missing or has a different source identity: ' + path)
    return None, row


def check_reference_identity(path):
    """Verify the complete source file when present, else its pinned excerpt record."""
    _record(path)
    return _pinned(path)


def reference_line(path, number, mode='splitlines'):
    require(type(number) is int and number > 0, 'Invalid pinned reference line')
    require(mode in ('splitlines', 'lf'), 'Invalid pinned reference line mode')
    source, row = _record(path)
    if source is not None:
        text = source.read_text(encoding='latin1')
        lines = text.splitlines() if mode == 'splitlines' else text.split('\n')
        require(number <= len(lines), 'Pinned reference line is outside the source file')
        return lines[number - 1].rstrip('\r')
    require(number <= row['line_count'], 'Pinned reference line is outside the source file')
    rows = row['lines'] if mode == 'splitlines' else row.get('lf_lines', row['lines'])
    for n, text in rows:
        if n == number:
            return text
    raise ValueError(f'Pinned reference excerpt lacks {path}:{number}')


def reference_lines(path, first, last, mode='splitlines'):
    require(type(first) is int and type(last) is int and first > 0 and last >= first,
            'Invalid pinned reference span')
    return [reference_line(path, n, mode=mode) for n in range(first, last + 1)]


def reference_rows(path, first, last, mode='splitlines'):
    """Available exact line excerpts in the inclusive range, with 1-based numbers."""
    require(type(first) is int and type(last) is int and first > 0 and last >= first,
            'Invalid pinned reference span')
    require(mode in ('splitlines', 'lf'), 'Invalid pinned reference line mode')
    source, row = _record(path)
    if source is not None:
        text = source.read_text(encoding='latin1')
        lines = text.splitlines() if mode == 'splitlines' else text.split('\n')
        return [(n, lines[n - 1].rstrip('\r')) for n in range(first, min(last, len(lines)) + 1)]
    rows = row['lines'] if mode == 'splitlines' else row.get('lf_lines', row['lines'])
    return [(n, text) for n, text in rows if first <= n <= last]


def reference_segment_size(path):
    source, row = _record(path)
    if source is None:
        require(path == 'src/restunts/asmorig/dseg.asm' and
                type(row.get('segment_size')) is int,
                'Pinned reference segment size is unavailable: ' + path)
        return row['segment_size']
    total, inside = 0, False
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    for line in source.read_text(encoding='latin1').splitlines():
        text = line.rstrip('\r').split(';')[0].strip()
        if not inside:
            inside = text.lower().startswith('dseg segment')
            continue
        if text.lower().startswith('dseg ends'):
            return total
        if not text or re.match(r'(?:public|assume)\b', text, re.I):
            continue
        item = re.fullmatch(r'(?:[A-Za-z_$?@][\w$?@]*\s+)?(db|dw|dd|dq)\s+(.*)',
                            text, re.I)
        require(item is not None and re.search(r'\bdup\s*\(', item.group(2), re.I) is None,
                'Unsupported pinned dseg declaration')
        total += sizes[item.group(1).lower()]
    raise ValueError('Pinned dseg segment end missing')


def _dseg_labels(source):
    offsets, offset, inside = {}, 0, False
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    for number, line in enumerate(source.read_text(encoding='latin1').splitlines(), 1):
        text = line.rstrip('\r').split(';')[0].strip()
        if not inside:
            inside = text.lower().startswith('dseg segment')
            continue
        if text.lower().startswith('dseg ends'):
            return offsets
        if not text or re.match(r'(?:public|assume)\b', text, re.I):
            continue
        item = re.fullmatch(r'(?:([A-Za-z_$?@][\w$?@]*)\s+)?(db|dw|dd|dq)\s+(.*)',
                            text, re.I)
        require(item is not None and re.search(r'\bdup\s*\(', item.group(3), re.I) is None,
                'Unsupported pinned dseg declaration')
        if item.group(1):
            offsets[item.group(1)] = (offset, number)
        offset += sizes[item.group(2).lower()]
    raise ValueError('Pinned dseg segment end missing')


def reference_label_offsets(path):
    source, row = _record(path)
    if source is not None:
        return _dseg_labels(source)
    require(path == 'src/restunts/asmorig/dseg.asm' and 'labels' in row,
            'Pinned reference label index is unavailable: ' + path)
    return {name: (offset, line) for name, offset, line in row['labels']}


def reference_proc_span(path, name):
    source, row = _record(path)
    if source is not None:
        lines = source.read_text(encoding='latin1').splitlines()
        starts = [i + 1 for i, line in enumerate(lines)
                  if re.match(r'^' + re.escape(name) + r'\s+proc\b', line.strip(), re.I)]
        ends = [i + 1 for i, line in enumerate(lines)
                if re.match(r'^' + re.escape(name) + r'\s+endp\b', line.strip(), re.I)]
    else:
        span = row.get('procedures', {}).get(name)
        require(span is not None, 'Pinned reference procedure span is missing: ' + name)
        starts, ends = [span[0]], [span[1]]
    require(len(starts) == 1 and len(ends) == 1 and starts[0] < ends[0],
            'Pinned reference procedure span differs: ' + name)
    require(re.match(r'^' + re.escape(name) + r'\s+proc\b',
                      reference_line(path, starts[0]).strip(), re.I) and
            re.match(r'^' + re.escape(name) + r'\s+endp\b',
                     reference_line(path, ends[0]).strip(), re.I),
            'Pinned reference procedure labels differ: ' + name)
    return starts[0], ends[0]


def reference_span(path, first, last_exclusive):
    """Return summarized declaration facts for [first, last_exclusive)."""
    require(type(first) is int and type(last_exclusive) is int and
            first > 0 and last_exclusive > first, 'Invalid pinned reference span')
    source, row = _record(path)
    if source is None:
        summary = row.get('spans', {}).get(f'{first}:{last_exclusive}')
        require(summary is not None, 'Pinned reference span summary is missing')
        size, first_label, last_label, interior_labels, all_db0, values_hex = summary
        return {'size': size, 'first_label': first_label, 'last_label': last_label,
                'interior_labels': interior_labels, 'all_db0': all_db0,
                'values_hex': values_hex, 'declarations_only': True}
    lines = source.read_text(encoding='latin1').splitlines()
    require(last_exclusive - 1 <= len(lines), 'Pinned reference span exceeds source file')
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    declarations, labels, values = [], [], []
    for number in range(first, last_exclusive):
        text = lines[number - 1].rstrip('\r').split(';')[0].strip()
        match = re.fullmatch(r'(?:(\w+)\s+)?(db|dw|dd|dq)\s+(.*)', text, re.I)
        require(match is not None, 'Unsupported pinned reference declaration')
        label, directive, value = match.groups()
        declarations.append((label, directive.lower(), value.strip()))
        if label:
            labels.append(label)
        values.append(int(value) if directive.lower() == 'db' and value.strip().isdigit()
                      and 0 <= int(value.strip()) <= 255 else None)
    all_db = all(row[1] == 'db' for row in declarations)
    all_values = all(value is not None for value in values)
    first_label = declarations[0][0] if declarations else None
    last_label = declarations[-1][0] if declarations else None
    size = sum(sizes[directive] for _, directive, _ in declarations)
    return {'size': size, 'first_label': first_label, 'last_label': last_label,
            'interior_labels': labels[1:],
            'all_db0': all_db and all_values and all(value == 0 for value in values),
            'values_hex': bytes(values).hex() if all_db and all_values else None,
            'declarations_only': True}


def reference_contains(path, text):
    source, row = _record(path)
    if source is not None:
        return text in source.read_text(encoding='latin1')
    return any(text in line for _, line in row['lines'])
