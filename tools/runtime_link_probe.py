"""Link untouched pinned LIBH members to verify ordered LEDATA semantics.

Research-only: comparing a complete LINK segment is not a production promotion.
"""
import argparse
import re
import subprocess
import tempfile
import struct
from pathlib import Path
from common import ROOT, identity, read_json, require, sha, write_json
from compiler import verify_toolchain
from mz import MZ
from omf import OmfReader
from object_probe import read_object


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
    archive = (ROOT/relative).read_bytes()
    require(sha(archive) == row['library_sha256'], 'Archive identity changed')
    matches = [data for name, data in OmfReader().split_library(archive)
               if name == row['module_name'] and sha(data) == row['module_sha256']]
    require(len(matches) == 1, 'Pinned archive member missing or ambiguous')
    return matches[0]


def experiment(rows, profile='msc510-medium'):
    config, runner = verify_toolchain(profile)
    root = ROOT/'build/runtime-link'; root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='r', dir=root))
    modules = []
    for i, row in enumerate(rows):
        data = original_module(row)
        (work/f'M{i}.OBJ').write_bytes(data)
        modules.append(read_object(data, ledata_policy=reviewed_policy(data)))
    tc = (ROOT/config['directory']).resolve()
    expression = '+'.join(f'M{i}.OBJ' for i in range(len(rows)))
    cmd = [runner['path'], '-e', '-v5.00', str(tc/'LINK.EXE'),
           f'/NOD /MAP {expression},RESULT.EXE,RESULT.MAP;']
    result = subprocess.run(cmd, cwd=work,
        env={'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'},
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60,
        creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    (work/'link.log').write_bytes(result.stdout)
    require(result.returncode == 0 and (work/'RESULT.EXE').is_file(), 'Historical LINK failed: '+str(work))
    data = (work/'RESULT.EXE').read_bytes(); mz = MZ.parse(data); image = mz.load_image(data)
    mapping = (work/'RESULT.MAP').read_text()
    found = re.findall(r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+([0-9A-F]+)H\s+_TEXT\s+CODE', mapping, re.M)
    require(len(found) == 1, 'LINK did not emit one complete text contribution')
    start, length = (int(v, 16) for v in found[0])
    require(not mz.relocations, 'Unexpected LINK relocation')
    expected = bytearray(); publics = []
    for obj, row in zip(modules, rows):
        require(not obj.linker_fixups and not obj.externals, 'Research member has fixups/externals')
        require(len(obj.segment_bytes('_TEXT')) == obj.segment_length('_TEXT') == row['size'], 'Incomplete library extent')
        require(all(n == '_TEXT' or s == 0 for n, s in obj.segment_lengths.items()), 'Additional library storage')
        # Each contribution has word SEGDEF alignment; LINK combines them in input order.
        if len(expected) % 2:
            expected.append(0)
        for public in obj.publics:
            address = start + len(expected) + public['offset']
            matches = re.findall(r'^\s*([0-9A-F]+):([0-9A-F]+)\s+' + re.escape(public['name']) + r'\s*$', mapping, re.M)
            require(matches and all(int(s,16)*16+int(o,16) == address for s,o in matches), 'LINK public/alias differs')
            publics.append({'name': public['name'], 'load_address': address})
        expected.extend(obj.segment_bytes('_TEXT'))
    require(length == len(expected), 'Historical LINK complete segment length differs')
    require(image[start:start+length] == bytes(expected), 'Historical LINK differs from ordered LEDATA output')
    verify_toolchain(profile); verify_toolchain('msc510-medium')
    return {'status': 'HISTORICAL_LINK_MATCH', 'profile': profile, 'command': cmd,
            'work_directory': str(work), 'modules': [{'name':r['module_name'], 'sha256':r['module_sha256']} for r in rows],
            'link_stdout': result.stdout.decode('ascii', 'replace'), 'map': mapping,
            'text': identity(bytes(expected)), 'executable': identity(data), 'publics': publics,
            'relocations': mz.relocations}


def selected_rows():
    rows = [r for r in read_json(ROOT/'evidence/library.json')['literal_matches']
            if r['module_name'] in ['ldiv.asm', 'lmul.asm', 'uldiv.asm']]
    require(len(rows) == 3, 'Expected three previously identified complete library members')
    return rows


def main():
    argparse.ArgumentParser(description=__doc__).parse_args()
    rows = selected_rows(); reports = []
    for profile in ['msc510-medium', 'msc500-medium']:
        for row in rows:
            reports.append(experiment([row], profile))
        reports.append(experiment(rows, profile))
    write_json(ROOT/'build/runtime-link-proof.json', {'status':'HISTORICAL_LINK_MATCH', 'experiments': reports})
    print('PASS:', len(reports), 'untouched library LINK experiments')


if __name__ == '__main__': main()
