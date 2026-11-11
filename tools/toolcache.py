"""Reuse of pinned DOS tool runs across processes (throughput aid only).

A compile or assembly is a function of the staged input bytes, the flags, the
pinned toolchain files, the runner and its configuration.  `run` stores what
one such run left in its work directory (objects, listings, logs), its exit
code and its console output under a digest of all of those, and restores them
into the caller's fresh work directory on the next identical request.  Every
caller still verifies the toolchain hashes, parses the object and applies all
of its checks to the restored files; only the emulator start is skipped.

The complete gate never reuses a run: `STUNTS_FRESH=1` (set by
`tools/validate.py` for a complete run and by its `--fresh` option) disables
both lookup and reuse.  The store is `build/toolcache/` (ignored; `STUNTS_TOOLCACHE`
names another directory, which lets a staged copy of the tree share it);
`python tools/toolcache.py --stats | --clear`.
"""
import hashlib
import json
import os
import shutil
import sys
import tempfile
from pathlib import Path

from common import ROOT

STORE = Path(os.environ.get('STUNTS_TOOLCACHE') or ROOT/'build/toolcache')
SCHEMA = 'toolcache-v1'
STATS = {'hits': 0, 'misses': 0}


def fresh():
    return os.environ.get('STUNTS_FRESH') == '1'


def key(kind, inputs, parameters):
    """Digest of named input files (bytes) and JSON parameters (flags, pinned
    profile and runner entries, which carry the tool hashes)."""
    h = hashlib.sha256(json.dumps([SCHEMA, kind, parameters], sort_keys=True, default=str).encode())
    for name in sorted(inputs):
        data = inputs[name]
        h.update(name.encode() + b'\0' + str(len(data)).encode() + b'\0' + data)
    return h.hexdigest()


def _load(entry, work):
    meta = json.loads((entry/'meta.json').read_text())
    for name, digest in meta['files'].items():
        data = (entry/'files'/name).read_bytes()
        if hashlib.sha256(data).hexdigest() != digest:
            return None
        (work/name).write_bytes(data)
    return meta['returncode'], (entry/'stdout.bin').read_bytes()


def run(kind, work, inputs, parameters, execute):
    """Return (returncode, stdout) of `execute()` run in `work`, or of an
    identical earlier run whose output files are restored into `work`.

    `inputs` are the files already staged in `work`; every other file present
    after the run is an output.  A damaged entry is discarded and re-run."""
    work = Path(work)
    if fresh():
        return execute()
    digest = key(kind, inputs, parameters)
    entry = STORE/digest[:2]/digest
    if (entry/'meta.json').is_file():
        try:
            found = _load(entry, work)
        except (OSError, ValueError, KeyError):
            found = None
        if found is not None:
            STATS['hits'] += 1
            return found
        shutil.rmtree(entry, ignore_errors=True)
    STATS['misses'] += 1
    before = {p.name for p in work.iterdir()}
    returncode, stdout = execute()
    outputs = [p for p in work.iterdir() if p.is_file() and p.name not in before]
    entry.parent.mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix='t', dir=entry.parent))
    try:
        (stage/'files').mkdir()
        files = {}
        for path in outputs:
            data = path.read_bytes()
            (stage/'files'/path.name).write_bytes(data)
            files[path.name] = hashlib.sha256(data).hexdigest()
        (stage/'stdout.bin').write_bytes(stdout or b'')
        (stage/'meta.json').write_text(json.dumps(
            {'schema': SCHEMA, 'kind': kind, 'returncode': returncode, 'files': files}))
        os.replace(stage, entry)
    except OSError:
        shutil.rmtree(stage, ignore_errors=True)      # another process stored it first
    return returncode, stdout


def main():
    if '--clear' in sys.argv:
        shutil.rmtree(STORE, ignore_errors=True)
        print('cleared', STORE)
        return
    entries = list(STORE.glob('??/*/meta.json')) if STORE.is_dir() else []
    size = sum(p.stat().st_size for p in STORE.rglob('*') if p.is_file()) if STORE.is_dir() else 0
    print(f'{len(entries)} runs, {size / 1e6:.1f} MB in {STORE}')


if __name__ == '__main__':
    main()
