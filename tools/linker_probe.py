"""Research-only historical LINK differential fixtures for external data fixups.

Fixture provider objects are synthetic test inputs, never production owners.
The compiler object is passed to LINK unchanged and without a C runtime.
"""
import argparse
import os
import struct
import subprocess
from pathlib import Path
from common import ROOT, identity, require, write_json
from compiler import compile_source, verify_toolchain
from mz import MZ


def record(kind, body):
    prefix = bytes([kind]) + struct.pack('<H', len(body) + 1) + body
    return prefix + bytes([-sum(prefix) & 255])


def provider(offset, paragraph=False):
    def name(value):
        data = value.encode('ascii')
        return bytes([len(data)]) + data
    return b''.join([
        record(0x80, name('provider')),
        record(0x96, b''.join(name(n) for n in ['_DATA', 'DATA', 'DGROUP'])),
        record(0x98, bytes([0x68 if paragraph else 0x48]) + struct.pack('<H', offset + 16) + b'\x01\x02\x00'),
        record(0x9a, b'\x03\xff\x01'),
        record(0x90, b'\x01\x01' + name('_flags') + struct.pack('<H', offset) + b'\x00'
               + name('__acrtused') + b'\x00\x00\x00'),
        record(0xa0, b'\x01\x00\x00' + bytes(offset + 16)),
        record(0x8a, b'\x00')])


def experiment(offset, addend, profile='msc510-medium', paragraph=False):
    source = ('extern unsigned char flags[]; unsigned char lookup(int index) '
              '{ return flags[index + %d]; }\n' % addend).encode('ascii')
    obj, receipt = compile_source(source, profile)
    work = Path(receipt['work_directory'])
    (work / 'DATA.OBJ').write_bytes(provider(offset, paragraph))
    config, runner = verify_toolchain(profile)
    tc = (ROOT / config['directory']).resolve()
    cmd = [runner['path'], '-e', '-v5.00', str(tc / 'LINK.EXE'),
           '/NOD /MAP UNIT.OBJ+DATA.OBJ,RESULT.EXE,RESULT.MAP;']
    result = subprocess.run(cmd, cwd=work,
        env={'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'},
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60,
        creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    (work / 'link.log').write_bytes(result.stdout)
    require(result.returncode == 0 and (work / 'RESULT.EXE').is_file(), 'LINK failed: ' + result.stdout.decode('ascii', 'replace'))
    data = (work / 'RESULT.EXE').read_bytes()
    mz = MZ.parse(data)
    image = mz.load_image(data)
    # /MAP locates the complete UNIT_TEXT contribution independently.
    import re
    mapping = (work / 'RESULT.MAP').read_text()
    match = re.search(r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+([0-9A-F]+)H\s+UNIT_TEXT\s+CODE', mapping, re.M)
    require(match is not None, 'Missing UNIT_TEXT in historical LINK map')
    start, size = int(match[1], 16), int(match[2], 16)
    require(size == obj.segment_length('UNIT_TEXT'), 'LINK contribution size changed')
    require(not mz.relocations, 'Unexpected relocation in offset-only fixture')
    public = re.search(r'^\s*([0-9A-F]+):([0-9A-F]+)\s+_flags\s*$', mapping, re.M)
    group = re.search(r'^\s*([0-9A-F]+):0\s+DGROUP\s*$', mapping, re.M)
    require(public is not None and group is not None, 'Missing LINK public/group addresses')
    address = int(public[1], 16)*16 + int(public[2], 16)
    frame = int(group[1], 16)*16
    from binder import bind_data_offsets
    symbols = {'_flags': {'group': 'DGROUP', 'frame_load_address': frame, 'load_address': address}}
    bound, binding = bind_data_offsets(obj, 'UNIT_TEXT', '_lookup', size, obj.linker_fixups,
        {'segments': obj.segment_defs, 'groups': obj.groups, 'publics': obj.publics, 'externals': obj.externals}, symbols)
    require(bound == image[start:start+size], 'Binder differs from historical LINK')
    verify_toolchain(profile)
    return obj, image[start:start+size], {
        'profile': profile, 'offset': offset, 'addend': addend, 'paragraph_data': paragraph, 'compiler': receipt,
        'link_command': cmd, 'link_returncode': result.returncode,
        'link_log': result.stdout.decode('ascii', 'replace'), 'map': mapping,
        'executable': identity(data), 'provider_object': identity(provider(offset, paragraph)),
        'fixups': obj.linker_fixups, 'linked_text': image[start:start+size].hex(),
        'relocations': mz.relocations, 'binding': binding, 'historical_link_equal': True}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--offset', type=lambda s:int(s,0), default=0x123)
    p.add_argument('--addend', type=int, default=3)
    a = p.parse_args()
    _, _, report = experiment(a.offset, a.addend)
    write_json(ROOT/'build/linker-probe.json', report)
    print('Historical LINK fixture: build/linker-probe.json')


if __name__ == '__main__':
    main()
