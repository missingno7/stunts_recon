"""One OS-locked publisher; durable rollback of interrupted multi-file writes."""
import base64
import os
from contextlib import contextmanager
from common import ROOT, atomic_bytes, json_bytes, read_json, require


@contextmanager
def exclusive():
    path = ROOT/'build/promotion.lock'
    path.parent.mkdir(parents=True, exist_ok=True)
    # Keep the inode/path: unlinking a lock lets another process lock a new file.
    with path.open('a+b') as stream:
        stream.seek(0, 2)
        if stream.tell() == 0:
            stream.write(b'0'); stream.flush()
        stream.seek(0)
        try:
            if os.name == 'nt':
                import msvcrt
                msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as error:
            raise ValueError('Another acceptance writer is active') from error
        try:
            yield
        finally:
            stream.seek(0)
            if os.name == 'nt':
                msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(stream, fcntl.LOCK_UN)


def journal_path():
    return ROOT/'build/publication.json'


def ensure_consistent():
    require(not journal_path().exists(),
            'Interrupted publication: run python tools/promote.py --recover')


def file_bytes(path):
    return path.read_bytes() if path.exists() else None


def prepare(changes):
    ensure_consistent()
    rows = []
    for relative, data in changes.items():
        path = (ROOT/relative).resolve()
        require(path.is_relative_to(ROOT.resolve()) and relative.split('/')[0] in ('src', 'asm', 'recipes', 'layout'),
                'Publication path outside canonical state')
        old = file_bytes(path)
        encode = lambda raw: None if raw is None else base64.b64encode(raw).decode('ascii')
        rows.append({'path':relative, 'before':encode(old), 'after':encode(data)})
    atomic_bytes(journal_path(), json_bytes({'files':rows}))
    return rows


def apply(rows):
    for row in rows:
        path = ROOT/row['path']
        before = None if row['before'] is None else base64.b64decode(row['before'])
        require(file_bytes(path) == before, 'Source/state race before publication: '+row['path'])
        atomic_bytes(path, base64.b64decode(row['after']))


def invalidate_receipts():
    for relative in ('build/exact/acceptance.json', 'build/validation/report.json', 'build/validation/independent.json'):
        (ROOT/relative).unlink(missing_ok=True)


def rollback():
    if not journal_path().exists():
        return
    rows = read_json(journal_path())['files']
    decoded = []
    # Validate ALL files before restoring any: never erase a concurrent user edit.
    for row in rows:
        path = (ROOT/row['path']).resolve()
        require(path.is_relative_to(ROOT.resolve()) and row['path'].split('/')[0] in ('src', 'asm', 'recipes', 'layout'),
                'Recovery path outside canonical state')
        before = None if row['before'] is None else base64.b64decode(row['before'])
        after = base64.b64decode(row['after'])
        current = file_bytes(path)
        require(current in (before, after), 'Recovery preserves conflicting edit: '+row['path'])
        decoded.append((path, before, current))
    invalidate_receipts()
    for path, before, current in reversed(decoded):
        if current == before:
            continue
        if before is None:
            path.unlink()
        else:
            atomic_bytes(path, before)
    journal_path().unlink()


def recover():
    with exclusive():
        rollback()


def finish():
    journal_path().unlink()
