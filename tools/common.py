"""Small shared primitives; no implicit lock updates."""
import hashlib
import json
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
    path.write_bytes(json_bytes(value))

def require(condition, message):
    if not condition:
        raise ValueError(message)

def project_path(value):
    path = (ROOT / value).resolve()
    require(path.is_relative_to(ROOT), 'Path escapes project')
    return path
