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


def far_experiment(profile='msc510-medium', library_first=False):
    """Untouched C caller plus pinned untouched lmul member; compare full text.

    Distinct segment order exercises nonzero source/target frames. No oracle is
    used to resolve a fixture operand. LINK's map defines fixture placement.
    """
    import re
    from common import read_json, sha
    from omf import OmfReader
    from binder import bind_far_calls
    source=b'long product(long a, long b, long c, long d) { return a*b-c*d; }\n'
    obj,receipt=compile_source(source,profile);work=Path(receipt['work_directory'])
    row=read_json(ROOT/'layout/library-candidates.json')['library_lmul']
    archive=(ROOT/row['library']).read_bytes()
    require(sha(archive)==row['library_sha256'],'Pinned fixture library changed')
    members=[b for n,b in OmfReader().split_library(archive) if n==row['module'] and sha(b)==row['module_sha256']]
    require(len(members)==1,'Missing fixture runtime member')
    (work/'MUL.OBJ').write_bytes(members[0]);(work/'DATA.OBJ').write_bytes(provider(0))
    config,runner=verify_toolchain(profile);tc=(ROOT/config['directory']).resolve()
    order='MUL.OBJ+UNIT.OBJ+DATA.OBJ' if library_first else 'UNIT.OBJ+MUL.OBJ+DATA.OBJ'
    cmd=[runner['path'],'-e','-v5.00',str(tc/'LINK.EXE'),f'/NOD /MAP {order},RESULT.EXE,RESULT.MAP;']
    result=subprocess.run(cmd,cwd=work,env={'PATH':str(tc),'MSDOS_PATH':str(tc),'TEMP':'.','TMP':'.','MSDOS_TEMP':'.'},
        stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0))
    (work/'link.log').write_bytes(result.stdout)
    require(result.returncode==0 and (work/'RESULT.EXE').exists(),'Far LINK fixture failed: '+str(work))
    data=(work/'RESULT.EXE').read_bytes();mz=MZ.parse(data);image=mz.load_image(data)
    mapping=(work/'RESULT.MAP').read_text()
    segment=re.search(r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+([0-9A-F]+)H\s+UNIT_TEXT\s+CODE',mapping,re.M)
    public=re.search(r'^\s*([0-9A-F]+):([0-9A-F]+)\s+__aFlmul\s*$',mapping,re.M)
    require(segment is not None and public is not None,'Far fixture map incomplete')
    start,size=int(segment[1],16),int(segment[2],16);frame=int(public[1],16)*16;address=frame+int(public[2],16)
    symbols={'__aFlmul':{'kind':'far-code','frame_load_address':frame,'load_address':address}}
    # LINK emits relocation pairs in FIXUPP order relative to the source paragraph.
    expected=[{'segment':start//16,'offset':start%16+f['offset']+2,'load_offset':start+f['offset']+2} for f in obj.linker_fixups]
    require(mz.relocations==expected,'Historical LINK relocation order/coordinates differ')
    declarations={'segments':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals}
    payload,binding=bind_far_calls(obj,'UNIT_TEXT','_product',size,obj.linker_fixups,declarations,symbols,start,expected)
    require(payload==image[start:start+size],'Far binder differs from untouched historical LINK')
    require(identity((work/'UNIT.OBJ').read_bytes())==receipt['object'] and
            sha((work/'MUL.OBJ').read_bytes())==row['module_sha256'],'LINK input object changed')
    verify_toolchain(profile)
    return {'profile':profile,'library_first':library_first,'compiler':receipt,'link_command':cmd,
        'map':mapping,'linked_text':identity(payload),'fixups':obj.linker_fixups,'binding':binding,
        'executable':identity(data),'module_sha256':row['module_sha256'],'historical_link_equal':True}


def near_transform_experiment(profile='msc510-medium'):
    """Research only: determine who emits PUSH CS + near CALL in a shared TU."""
    source=b'int callee(int x) { return x+1; } int caller(int x) { return callee(x); }\n'
    obj,receipt=compile_source(source,profile);work=Path(receipt['work_directory'])
    (work/'DATA.OBJ').write_bytes(provider(0))
    config,runner=verify_toolchain(profile);tc=(ROOT/config['directory']).resolve()
    cmd=[runner['path'],'-e','-v5.00',str(tc/'LINK.EXE'),'/NOD /MAP UNIT.OBJ+DATA.OBJ,RESULT.EXE,RESULT.MAP;']
    result=subprocess.run(cmd,cwd=work,env={'PATH':str(tc),'MSDOS_PATH':str(tc),'TEMP':'.','TMP':'.','MSDOS_TEMP':'.'},
        stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0))
    (work/'link.log').write_bytes(result.stdout)
    require(result.returncode==0 and (work/'RESULT.EXE').exists(),'Shared-TU LINK fixture failed')
    data=(work/'RESULT.EXE').read_bytes();mz=MZ.parse(data);image=mz.load_image(data)
    import re
    mapping=(work/'RESULT.MAP').read_text()
    match=re.search(r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+([0-9A-F]+)H\s+UNIT_TEXT\s+CODE',mapping,re.M)
    require(match is not None,'Shared-TU fixture segment missing')
    start,size=int(match[1],16),int(match[2],16)
    raw=obj.segment_bytes('UNIT_TEXT');linked=image[start:start+size]
    require(size==len(raw),'Unexpected shared-TU LINK extent change')
    require(len(obj.linker_fixups)==1 and not mz.relocations,'Unexpected local-call obligations')
    fix=obj.linker_fixups[0];at=fix['offset']
    require(fix['self_relative'] and fix['loc']=='offset16' and raw[at-2:at]==b'\x0e\xe8',
            'Compiler did not emit PUSH CS + near CALL')
    target=next(p['offset'] for p in obj.publics if p['name']==fix['target'])
    expected=raw[:at]+struct.pack('<h',target-(at+2))+raw[at+2:]
    require(linked==expected,'Historical LINK local displacement differs')
    require(identity((work/'UNIT.OBJ').read_bytes())==receipt['object'],'LINK altered compiler object')
    verify_toolchain(profile)
    return {'scope':'RESEARCH_ONLY: local-call/TU modes remain unsupported in production',
            'profile':profile,'source':source.decode(),'compiler':receipt,'fixups':obj.linker_fixups,
            'publics':obj.publics,'object_text':raw.hex(),'linked_text':linked.hex(),
            'changed_bytes':[{'offset':i,'object':a,'linked':b} for i,(a,b) in enumerate(zip(raw,linked)) if a!=b],
            'link_command':cmd,'map':mapping,'relocations':mz.relocations,'executable':identity(data)}


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
