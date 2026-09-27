"""Reuse of pure evidence checks inside one verification session (integ28).

Throughput aid only; it never supplies an acceptance fact.  A session (one
fresh build, probe, crosscheck or batch publication stage) may reuse the result
of a deterministic check only for identical explicit arguments (image hash,
relocation digest, names, profile).  The store lives in memory for the
outermost session only: nothing is written to disk or reused by another
session or process, and every caller already requires the canonical input
snapshot (tools, sources, recipes, layout, evidence) to be unchanged at the end
of its session.  Checks registered with `recheck=True` (toolchain identities)
are computed again, uncached, when the outermost session ends normally and must
return the same result; after any reuse every pinned reference evidence file is
hashed again.  Cached values are returned as fresh copies, so a caller can
never mutate a stored result.
"""
import hashlib
import json
import pickle
import threading
from contextlib import contextmanager

_lock = threading.RLock()
_state = {'depth': 0, 'store': {}, 'rechecks': {}, 'hits': 0, 'misses': 0}
STATS = {'sessions': 0, 'hits': 0, 'misses': 0, 'rechecks': 0}


def active():
    return _state['depth'] > 0


@contextmanager
def session():
    """Enable reuse for the duration of the outermost `with session()`."""
    with _lock:
        if _state['depth'] == 0:
            _state.update(store={}, rechecks={}, hits=0, misses=0)
        _state['depth'] += 1
    finished = False
    try:
        yield
    finally:
        with _lock:
            _state['depth'] -= 1
            finished = _state['depth'] == 0
            if finished:
                hits, rechecks = _state['hits'], _state['rechecks']
                STATS['sessions'] += 1
                STATS['hits'] += hits
                STATS['misses'] += _state['misses']
                _state.update(store={}, rechecks={}, hits=0, misses=0)
    # Only a normally completed outermost session is re-checked; an exception
    # propagates unchanged.
    if finished:
        for (namespace, key), (compute, expected) in rechecks.items():
            STATS['rechecks'] += 1
            if pickle.dumps(compute(), pickle.HIGHEST_PROTOCOL) != expected:
                raise ValueError('Session-checked evidence changed during verification: ' + namespace)
        if hits:
            _check_references()


def _check_references():
    from common import ROOT, identity, read_json, require
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files']
    root = ROOT/'build/references/restunts'
    for path, expected in pinned.items():
        source = root/path
        if source.is_file():
            require(identity(source.read_bytes()) == expected,
                    'Pinned reference evidence changed during a verification session: ' + path)


def digest(value):
    """Stable digest of JSON-like arguments (bytes are hashed)."""
    if isinstance(value, (bytes, bytearray)):
        return 'b:' + hashlib.sha256(value).hexdigest()
    return hashlib.sha256(json.dumps(value, sort_keys=True, default=repr).encode()).hexdigest()


def cached(namespace, key, compute, recheck=False):
    """compute() once per session for an identical (namespace, key)."""
    if not active():
        return compute()
    full = (namespace, key)
    with _lock:
        store = _state['store']
        if full in store:
            _state['hits'] += 1
            return pickle.loads(store[full])
    value = compute()
    with _lock:
        if _state['depth'] > 0 and full not in _state['store']:
            _state['misses'] += 1
            raw = pickle.dumps(value, pickle.HIGHEST_PROTOCOL)
            _state['store'][full] = raw
            if recheck:
                _state['rechecks'][full] = (compute, raw)
    return value


def cached_items(namespace, key, names, compute):
    """Per-name reuse for a function whose result for each name is independent
    of the other requested names: compute(missing_names) -> {name: value}."""
    names = list(names)
    if not active():
        return compute(names)
    result, missing = {}, []
    with _lock:
        store = _state['store']
        for name in names:
            full = (namespace, key, name)
            if full in store:
                _state['hits'] += 1
                result[name] = pickle.loads(store[full])
            elif name not in missing:
                missing.append(name)
    if missing:
        fresh = compute(missing)
        with _lock:
            for name in missing:
                result[name] = fresh[name]
                full = (namespace, key, name)
                if _state['depth'] > 0 and full not in _state['store']:
                    _state['misses'] += 1
                    _state['store'][full] = pickle.dumps(fresh[name], pickle.HIGHEST_PROTOCOL)
    return {name: result[name] for name in names}
