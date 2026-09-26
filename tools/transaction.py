"""One OS-locked publisher; durable rollback of interrupted multi-file writes."""
import base64
import os
import time
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


class SnapshotChanged(ValueError):
    """A lock-free verification observed a publication; retrying is safe."""


def generation_path():
    return ROOT/'build/publication.generation'


def read_generation():
    """Publication sequence number: odd while a publisher may be writing."""
    for attempt in range(100):
        try:
            text = generation_path().read_text(encoding='ascii').strip()
            break
        except FileNotFoundError:
            return 0
        except PermissionError:
            # Windows reports a sharing violation while the file is replaced.
            if attempt == 99:
                raise
            time.sleep(0.02)
    require(text.isdigit(), 'Corrupt publication generation file')
    return int(text)


def _write_generation(value):
    atomic_bytes(generation_path(), str(value).encode('ascii'))


@contextmanager
def publishing():
    """Seqlock writer section; the caller holds exclusive() and no journal exists.

    The generation becomes odd before the journal or any canonical file is
    written and even again only once no journal remains.  An odd value on entry
    can only be stale (left by a dead writer), because the caller holds the lock."""
    ensure_consistent()
    value = read_generation()
    value += value % 2
    _write_generation(value + 1)
    try:
        yield
    finally:
        # A journal left by a failed rollback keeps readers out until --recover.
        if not journal_path().exists():
            _write_generation(value + 2)


def _heal_generation():
    # Caller holds exclusive() and no journal exists.
    value = read_generation()
    if value % 2:
        _write_generation(value + 1)


RETRY = ('Canonical acceptance inputs changed or a publication ran during lock-free '
         '--verify-only; nothing was accepted. Retry --verify-only.')


def lock_free_snapshot(action, capture):
    """Run a read-only acceptance check without the exclusive writer lock.

    `capture()` returns the identities of all canonical inputs.  The result is
    reported only when no publication was in progress at the start, the
    publication generation is unchanged at the end, no journal exists, and the
    captured canonical identities are unchanged; otherwise SnapshotChanged is
    raised and retrying is safe.  Ordinary rejections are reported only for a
    stable snapshot, so they are never artefacts of a concurrent publication."""
    start = read_generation()
    if start % 2 or journal_path().exists():
        raise SnapshotChanged('A publication is in progress (or was interrupted: if no publisher is '
                              'running, use python tools/promote.py --recover). Retry --verify-only.')
    try:
        before = capture()
    except OSError as error:
        raise SnapshotChanged(RETRY) from error

    def stable():
        try:
            return (not journal_path().exists() and read_generation() == start
                    and capture() == before)
        except OSError:
            return False
    try:
        result = action(before)
    except SnapshotChanged:
        raise
    except Exception as error:
        if not stable():
            raise SnapshotChanged(RETRY) from error
        raise
    if not stable():
        raise SnapshotChanged(RETRY)
    return result


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
        _heal_generation()


def finish():
    journal_path().unlink()
