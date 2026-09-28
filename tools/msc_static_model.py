"""DIAGNOSTIC model of MSC 5.10 file-scope static `_BSS` placement (integ35).

Imported from L9-mschash.  MSC 5.10 allocates an object's file-scope statics
in its own `_BSS` by an identifier-spelling hash: bucket = sum(byte & 0xDF)
mod 256, buckets ascending, equal buckets newest declaration first; chars pack
on bytes, other objects are word aligned (L8-msstatic, L9-mschash).  It is a
predict-then-compile design aid for choosing readable names: the pinned
compiler's own OMF output is the only authority, and the model never places,
sizes or accepts storage.  tools/bss_link.py is the only importer.
Regression data: tests/fixtures/msc510_static_order_fixtures.json (36 corpus
fixtures, 8 held-out fixtures predicted before compilation, one typed layout).

integ37 adds the translation-unit flush rule (s005bss, `layout`): file-scope
statics stay pending until the next function DEFINITION (prototypes, extern
declarations and initialized data definitions do not flush them); there the
pending set is emitted by the file-static rule above.  Then each block of that
function emits its own statics, ordered by the local-slot hash sum(byte) & 15
(first 31 bytes) ascending, equal buckets newest declaration first; a nested
block follows its enclosing block and sibling blocks follow source order.
Statics still pending at the end of the file are emitted last.  Unreferenced
statics are allocated like referenced ones.  Regression data:
tests/fixtures/msc510_static_flush_fixtures.json (s005bss e1-e8, integ37 e9-e10
and seg005's complete 27-static layout).
"""

def hash_bucket(name: str) -> int:
    """Return the 8-bit bucket for an ASCII C identifier."""
    raw=name.encode("ascii")
    value=0
    for byte in raw:
        value=(value+(byte&0xDF))&0xFF
    return value

def block_bucket(name: str) -> int:
    """Return the 4-bit block-static bucket (the local-slot hash)."""
    return sum(name.encode("ascii")[:31])&15

def _alignment(type_name: str) -> int:
    """Smallest observed medium-model BSS alignment for a C object type."""
    t=type_name.strip().lower().replace(" ","")
    if t in ("char","signedchar","unsignedchar","byte") or t.startswith(("char[","signedchar[","unsignedchar[")):
        return 1
    return 2

def order(names, sizes, types):
    """Return ``{name: BSS offset}`` for names in declaration order.

    ``sizes`` and ``types`` are mappings keyed by identifier.  The BSS walk
    scans hash buckets 0..255 and visits equal-bucket declarations newest first.
    Objects use the supplied extent and their natural byte/word alignment.
    """
    names=list(names)
    if len(set(names))!=len(names): raise ValueError("names must be unique")
    if set(names)!=set(sizes) or set(names)!=set(types): raise ValueError("sizes/types must cover names exactly")
    buckets={}
    for name in names: buckets.setdefault(hash_bucket(name),[]).append(name)
    ordered=[]
    for bucket in range(256): ordered.extend(reversed(buckets.get(bucket,[])))
    offsets={};cursor=0
    for name in ordered:
        size=sizes[name]
        if type(size) is not int or size<=0: raise ValueError(f"invalid size for {name}: {size!r}")
        align=_alignment(types[name])
        cursor=(cursor+align-1)&~(align-1)
        offsets[name]=cursor
        cursor+=size
    return offsets

def ordered_names(names):
    return [name for name,offset in sorted(order(names,{n:1 for n in names},{n:"char" for n in names}).items(),key=lambda x:x[1])]

def _emit(statics, bucket_of, nbuckets, cursor, offsets):
    """Append (name, size, type) statics in bucket order, ties newest first."""
    buckets={}
    for item in statics: buckets.setdefault(bucket_of(item[0]),[]).append(item)
    for bucket in range(nbuckets):
        for name,size,type_name in reversed(buckets.get(bucket,[])):
            if name in offsets: raise ValueError("names must be unique: "+name)
            if type(size) is not int or size<=0: raise ValueError(f"invalid size for {name}: {size!r}")
            align=_alignment(type_name)
            cursor=(cursor+align-1)&~(align-1)
            offsets[name]=cursor
            cursor+=size
    return cursor

def _emit_block(block, cursor, offsets):
    cursor=_emit([tuple(x) for x in block.get("statics",())],block_bucket,16,cursor,offsets)
    for child in block.get("blocks",()):
        cursor=_emit_block(child,cursor,offsets)
    return cursor

def layout(items):
    """Return ``({name: _BSS offset}, _BSS length)`` for a whole translation unit.

    ``items`` is the source order of static-relevant declarations:
    ``{"static": [name, size, type]}`` for a file-scope static, and
    ``{"function": name, "block": {"statics": [[name, size, type], ...],
    "blocks": [nested block, ...]}}`` for a function definition (its outermost
    block; statics in declaration order).  Other declarations do not matter.
    """
    offsets={};pending=[];cursor=0
    for item in items:
        if "static" in item:
            pending.append(tuple(item["static"]))
        elif "function" in item:
            cursor=_emit(pending,lambda n:hash_bucket(n),256,cursor,offsets);pending=[]
            cursor=_emit_block(item.get("block") or {},cursor,offsets)
        else:
            raise ValueError("unknown item: %r"%(item,))
    cursor=_emit(pending,lambda n:hash_bucket(n),256,cursor,offsets)
    return offsets,cursor
