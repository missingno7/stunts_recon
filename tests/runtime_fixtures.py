"""Test-only pinned runtime member and ordered LEDATA fixtures.

The trace builder is retained from the completed runtime LINK investigation.
It constructs mutation/control fixtures; production policies remain manifest inputs.
"""
import struct
from common import ROOT, read_json, require, sha
from compiler import toolchain_path, verify_toolchain
from omf import OmfReader
from object_probe import read_object
from runtime_binding import declarations


def reviewed_policy(data):
    """Create research trace; strict parser and historical LINK must validate it.

    This is not used by production to learn expectations from current objects.
    """
    at = 0; written = {}; records = []
    while at < len(data):
        require(at + 3 <= len(data), 'Truncated research record')
        kind, length = data[at], struct.unpack_from('<H', data, at+1)[0]
        end = at + 3 + length
        require(length >= 1 and end <= len(data), 'Truncated research record body')
        if kind == 0xa0:
            body = data[at+3:end-1]; segment, pos = OmfReader._index(body, 0)
            require(pos+2 <= len(body), 'Truncated research LEDATA')
            offset = struct.unpack_from('<H', body, pos)[0]; payload = body[pos+2:]
            span = set(range(offset, offset+len(payload))); previous = written.setdefault(segment, set())
            records.append({'record_offset':at, 'segment_index':segment, 'offset':offset,
                            'size':len(payload), 'sha256':sha(payload), 'overlap_offsets':sorted(span & previous)})
            previous.update(span)
        at = end
    return {'mode':'pinned-ordered-ledata-v1', 'module_sha256':sha(data), 'records':records}


def original_module(row):
    config, _ = verify_toolchain('msc510-medium')
    relative = 'toolchain/msc510/' + row['library']
    require(any(p['path'] == relative and p['sha256'] == row['library_sha256'] for p in config['files']),
            'Archive not pinned to expected compiler profile')
    from compiler import toolchain_path
    archive = toolchain_path(relative).read_bytes()
    require(sha(archive) == row['library_sha256'], 'Archive identity changed')
    matches = [data for name, data in OmfReader().split_library(archive)
               if name == row['module_name'] and sha(data) == row['module_sha256']]
    require(len(matches) == 1, 'Pinned archive member missing or ambiguous')
    return matches[0]


def selected_rows():
    rows = [r for r in read_json(ROOT/'evidence/library.json')['literal_matches']
            if r['module_name'] in ['ldiv.asm', 'lmul.asm', 'uldiv.asm']]
    require(len(rows) == 3, 'Expected three previously identified complete library members')
    return rows


def runtime_owner(module, public, start, end, archive='MLIBCR.LIB', data=None, storage=None):
    path = 'toolchain/msc510/'+archive
    archive_bytes = toolchain_path(path).read_bytes()
    blobs = [b for n,b in OmfReader().split_library(archive_bytes) if n == module
             and public in [p['name'] for p in OmfReader().read(b).publics]
             and OmfReader().read(b).segment_length('_TEXT') == end-start]
    if len(blobs) != 1: raise AssertionError('Fixture member ambiguous')
    blob = blobs[0]; policy = reviewed_policy(blob)
    policy = policy if any(r['overlap_offsets'] for r in policy['records']) else None
    obj = read_object(blob,ledata_policy=policy)
    result = {'id':'test_runtime_'+public, 'name':module, 'kind':'KNOWN_TOOLCHAIN_LIBRARY',
              'classification':'RUNTIME_LIBRARY','profile':'msc510-medium',
              'library':path,'library_sha256':sha(archive_bytes),'module':module,'module_sha256':sha(blob),
              'segment':'_TEXT','start':start,'end':end,'publics':obj.publics,'externals':obj.externals,
              'expected_fixups':obj.linker_fixups,'expected_relocations':[],
              'binding':{'mode':'runtime-member-v1','declarations':declarations(obj),
                         'data_bindings':data or {},'storage':storage or {}}}
    if policy: result['ledata_policy']=policy
    return result,blob
