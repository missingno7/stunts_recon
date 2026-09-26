"""Small shared primitives; no implicit lock updates."""
import hashlib
import json
import os
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def sha(data):
    return hashlib.sha256(data).hexdigest()

def identity(data):
    return {'size': len(data), 'sha256': sha(data)}

def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8'))

def json_bytes(value):
    return (json.dumps(value, indent=2, sort_keys=True) + '\n').encode('utf-8')


def write_json(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    data=json_bytes(value)
    if path.exists() and path.read_bytes()==data:return
    # Readers see an old or a complete new file, never partially written JSON.
    fd,name=tempfile.mkstemp(prefix='.'+path.name+'.',dir=path.parent)
    try:
        with os.fdopen(fd,'wb') as stream:stream.write(data)
        replace_file(name,path)
    finally:
        Path(name).unlink(missing_ok=True)

def replace_file(source, target, attempts=150):
    """os.replace that tolerates transient Windows sharing violations.

    Lock-free readers (verify-only snapshots) may briefly hold a canonical file
    open; Windows then refuses the rename for a moment instead of replacing."""
    for attempt in range(attempts):
        try:
            os.replace(source, target)
            return
        except PermissionError:
            if os.name != 'nt' or attempt == attempts - 1:
                raise
            time.sleep(0.02)


def require(condition, message):
    if not condition:
        raise ValueError(message)

def project_path(value):
    path = (ROOT / value).resolve()
    require(path.is_relative_to(ROOT), 'Path escapes project')
    return path


def atomic_bytes(path, data):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, name = tempfile.mkstemp(prefix='.' + path.name + '.', dir=path.parent)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        replace_file(name, path)
    finally:
        Path(name).unlink(missing_ok=True)
