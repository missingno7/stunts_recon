"""Shared core for blockdiff/permuter: pinned compile, target lookup, member extraction.

Compilation reuses tools/tubench.py::_compile_in_worker (canonical msc510-medium,
/AM /O /Gs, fresh disposable work directory).  The work directory is redirected into
this worker's scratch tree and removed after the object has been parsed.
Target bytes come from the locked oracle image + evidence inventory (tubench's
_function_maps) and are cached per function under cache/targets/.
"""
from __future__ import annotations

import json
import os
import shutil
import sys
import threading
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[0]
TOOLS = ROOT / 'tools'
for p in (str(TOOLS), str(ROOT / 'build/python')):
    if p not in sys.path:
        sys.path.insert(0, p)

import tubench  # noqa: E402  (project tool; only its compile helper and maps are used)

SCRATCH = ROOT / 'build/classifier/scratch'
tubench.WORKSPACE = SCRATCH          # compile work dirs land in our own tree
CACHE = ROOT / 'build/classifier/cache/targets'
_lock = threading.Lock()


class CompileError(RuntimeError):
    def __init__(self, message, log=''):
        super().__init__(message)
        self.log = log


def compile_c(source: bytes, keep=False):
    """Compile C source bytes; return (obj, obj_bytes, run_dir or None).

    Raises CompileError with the compiler log on failure.
    """
    SCRATCH.mkdir(parents=True, exist_ok=True)
    tmp = SCRATCH / 'inputs'
    tmp.mkdir(parents=True, exist_ok=True)
    import uuid
    src_path = tmp / (uuid.uuid4().hex + '.c')
    src_path.write_bytes(source)
    run_dir = None
    try:
        try:
            _, run_dir, obj, obj_bytes = tubench._compile_in_worker(src_path)
        except RuntimeError as error:
            log = ''
            msg = str(error)
            if 'see ' in msg:
                logp = Path(msg.split('see ', 1)[1].strip())
                run_dir = logp.parent
                if logp.is_file():
                    log = logp.read_text(errors='replace')
            raise CompileError(msg, log) from None
        return obj, obj_bytes, (run_dir if keep else None)
    finally:
        src_path.unlink(missing_ok=True)
        if run_dir is not None and not keep:
            shutil.rmtree(run_dir, ignore_errors=True)


def _inventory():
    rows, by_name, _ = tubench._function_maps()
    return rows, by_name


def load_target(name: str):
    """Return dict(name,start,end,bytes,reloc_sites,segment_paragraph...) for an evidence function."""
    CACHE.mkdir(parents=True, exist_ok=True)
    path = CACHE / f'{name}.json'
    if path.is_file():
        doc = json.loads(path.read_text())
        doc['bytes'] = bytes.fromhex(doc['bytes_hex'])
        return doc
    with _lock:
        oracle, image = tubench._read_authority()
        rows, by_name = _inventory()
        matches = by_name.get(name) or by_name.get(name.lstrip('_')) or []
        if len(matches) != 1:
            raise SystemExit(f'Unknown or ambiguous evidence function: {name}')
        row = matches[0]
        start, end = row['start'], row['end']
        relocs = sorted(r['load_offset'] - start for r in oracle[2]['unpacked_mz']['relocations']
                        if start <= r['load_offset'] < end)
        # Near-call edges out of this function (site offsets relative to start).
        doc = {'name': row['name'], 'stable_id': row.get('stable_id'), 'start': start, 'end': end,
               'segment': row.get('segment'), 'segment_offset': row.get('segment_offset'), 'bytes_hex': bytes(image[start:end]).hex(),
               'reloc_sites': relocs,
               'authority': 'Copied from locked oracle image via tubench._read_authority; diagnostic cache.'}
        path.write_text(json.dumps(doc, indent=1))
        doc['bytes'] = bytes.fromhex(doc['bytes_hex'])
        return doc


def code_segment(obj):
    segs = [n for n in obj.segments if any(p.get('segment') == n for p in obj.publics)]
    if 'UNIT_TEXT' in segs:
        return 'UNIT_TEXT'
    if segs:
        return segs[0]
    return 'UNIT_TEXT' if 'UNIT_TEXT' in obj.segments else next(iter(obj.segments), None)


def member_extent(obj, function_name):
    """(segment, start, end, publics) of a compiled member (public to next public)."""
    seg = code_segment(obj)
    data = bytes(obj.segments.get(seg, b''))
    pubs = sorted((p for p in obj.publics if p.get('segment') == seg), key=lambda p: p['offset'])
    if function_name is None:
        return seg, 0, len(data), pubs
    pmap = tubench._public_name_map(obj, seg)
    pub = tubench._choose_public(pmap, function_name)
    if pub is None:
        raise KeyError(f'No emitted public for {function_name}; publics: '
                       + ', '.join(p['name'] for p in pubs))
    start = pub['offset']
    end = next((p['offset'] for p in pubs if p['offset'] > start), len(data))
    return seg, start, end, pubs


def member_bytes(obj, function_name):
    """Return (bytes, fixup_list) for the member; fixup offsets are member-relative."""
    seg, start, end, _ = member_extent(obj, function_name)
    data = bytes(obj.segments.get(seg, b''))[start:end]
    fixups = []
    for fx in obj.linker_fixups:
        if fx.get('segment') != seg:
            continue
        at = fx['offset']
        if start <= at < end:
            fixups.append({'offset': at - start, 'width': fx.get('width', 2), 'loc': fx.get('loc'),
                           'self_relative': bool(fx.get('self_relative')),
                           'target': fx.get('target'), 'target_kind': fx.get('target_kind'),
                           'encoded_addend': fx.get('encoded_addend')})
    return data, fixups
