"""DIAGNOSTIC model of MS LINK 3.65 communal (COMDEF) allocation order (integ35).

Imported from the L9-lhashX reconciliation of the L7-lhashA black-box model and
the L7-lhashB static reading of the pinned LINK.EXE.  It is a design aid only:
it never places, sizes or accepts storage.  A communal is proven solely by the
MAP address of its public name in a real LINK run that reproduces the image
(tools/bss_link.py `link-communal-v1`); tools/bss_link.py is the only importer.
Regression data: tests/fixtures/link365_communal_fixtures.json (five fresh
real-LINK MAPs: 339 digit/punctuation/mixed-case names, case duplicates,
near/far entries, the pinned `_file.c` COMDEFs with 140 game commons, and 183
commons among 1,130 resolved EXTDEF/PUBDEF nodes).

Behavioral model of MS LINK 3.65 communal placement.

Names are the exact ASCII OMF/linker spellings (MSC C externals therefore
usually include their leading underscore). The static LINK callback at 0xF9AC
in the decompressed pinned LINK 3.65 image computes the 16-bit sum

    n + sum((name[i] | 0x20) << ((i + 1) & 3))

for left-to-right byte index i and counted byte length n. The implementation
walks the counted name right-to-left with shift counts n,n-1,...,1, which is
equivalent. The low byte selects one of 256 buckets. LINK walks buckets in
ascending order; a new name is inserted at its bucket-chain head on first sight.

The function predicts the observed COMDEF relative order and starts. It models
first-seen EXTDEF/PUBDEF nodes, repeated case-insensitive COMDEFs (largest size
wins, first spelling and chain position survive), and independent near/far
cursors. Even total extents begin at even offsets; odd extents may pack at any
byte. Alignment is only verified for the described fixture sizes.
"""
from __future__ import annotations

BUCKET_COUNT = 256
MAX_NAME_BYTES = 255


def _key(name: str) -> bytes:
    if not isinstance(name, str):
        raise TypeError(f"OMF name must be str, got {type(name).__name__}")
    try:
        raw = name.encode("ascii")
    except UnicodeEncodeError as exc:
        raise ValueError(f"non-ASCII OMF name: {name!r}") from exc
    if not 1 <= len(raw) <= MAX_NAME_BYTES:
        raise ValueError(f"LINK counted names must have 1..255 bytes: {name!r}")
    return bytes(ch | 0x20 for ch in raw)


def name_hash(name: str) -> int:
    """Return the 16-bit weighted sum used by LINK 3.65 for an OMF name."""
    raw = _key(name)
    n = len(raw)
    h = n
    shift = n
    for ch in reversed(raw):
        h = (h + (ch << (shift & 3))) & 0xFFFF
        shift -= 1
    return h


def bucket(name: str) -> int:
    """Return the LINK hash bucket (low byte of :func:`name_hash`)."""
    return name_hash(name) & 0xFF


def order(names_with_sizes, other_symbols=(), *, near_base=0, far_base=0):
    """Return COMDEF rows in LINK allocation order.

    ``names_with_sizes`` accepts dictionaries with ``name``, ``size``,
    ``kind`` (``near`` or ``far``), and integer ``encounter`` fields; tuples
    may also be ``(encounter, name, size[, kind])``. ``other_symbols`` accepts
    dictionaries with ``name`` and ``encounter`` or ``(name, encounter)``
    tuples. Encounter numbers represent first sight across input OMF modules and
    records. A prior noncommon symbol can establish a node before its COMDEF.

    Returned ``offset`` values are region-relative. ``address`` additionally
    includes ``near_base`` or ``far_base``. Same-name near/far COMDEF conflicts
    are refused. For duplicate COMDEFs the first spelling is reported and the
    maximum declared byte size is reserved.
    """
    events = []
    for ix, row in enumerate(other_symbols):
        if isinstance(row, dict):
            nm, at = row["name"], row["encounter"]
        else:
            nm, at = row
        events.append((int(at), 0, ix, "other", nm, None, None))

    for ix, row in enumerate(names_with_sizes):
        if isinstance(row, dict):
            nm = row["name"]
            size = row["size"]
            kind = row.get("kind", "near")
            at = row["encounter"]
        else:
            at, nm, size, *tail = row
            kind = tail[0] if tail else "near"
        if kind not in ("near", "far"):
            raise ValueError(f"unsupported communal kind {kind!r} for {nm!r}")
        if not isinstance(size, int) or size < 0:
            raise ValueError(f"invalid communal size for {nm!r}: {size!r}")
        events.append((int(at), 1, ix, "common", nm, size, kind))

    events.sort(key=lambda e: (e[0], e[1], e[2]))
    nodes = {}
    chains = [[] for _ in range(BUCKET_COUNT)]
    for at, _priority, _ix, event_kind, nm, size, common_kind in events:
        key = _key(nm)
        node = nodes.get(key)
        if node is None:
            h = name_hash(nm)
            node = {"name": nm, "first_seen": at, "hash16": h,
                    "bucket": h & 0xFF, "kind": None, "size": 0}
            nodes[key] = node
            chains[node["bucket"]].insert(0, node)
        if event_kind == "common":
            if node["kind"] not in (None, common_kind):
                raise ValueError(f"near/far COMDEF conflict for {nm!r}")
            node["kind"] = common_kind
            node["size"] = max(node["size"], size)

    cursors = {"near": int(near_base), "far": int(far_base)}
    bases = {"near": int(near_base), "far": int(far_base)}
    result = []
    for bucket_ix, chain in enumerate(chains):
        for node in chain:
            kind = node["kind"]
            if kind is None:
                continue
            at = cursors[kind]
            size = node["size"]
            if size and (size & 1) == 0:
                at = (at + 1) & ~1
            result.append({
                "name": node["name"], "kind": kind,
                "offset": at - bases[kind], "address": at, "size": size,
                "bucket": bucket_ix, "hash16": node["hash16"],
                "first_seen": node["first_seen"],
            })
            cursors[kind] = at + size
    return result


def letter_score_to_bucket(score: int) -> int:
    """Convert L7-lhashA's letter/_ score to this model's bucket.

    For that model's verified ASCII letters/underscore domain, LINK's bucket is
    ``(score + 196) mod 256``. The corresponding walk starts at A score 60,
    because B bucket 0 is A score 60.
    """
    return (int(score) + 196) & 0xFF
