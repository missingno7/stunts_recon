"""Research-only pinned LINK fixture for the MZ relocation order model.

The acceptance binders model the executable relocation order of one object as
its own relocation-bearing FIXUPPs in emitted order (binder.link_order_sites).
This fixture links synthetic objects with the pinned LINK, unchanged and
without a runtime, and compares the linked MZ table with that model:

* a MASM 5.10 module whose FIXUPPs ascend, spread over several LEDATA records
  (with LIDATA records between them), mixing far CALLs and segment words;
* an MSC 5.10 C object whose FIXUPPs descend inside the record.

Fixture objects are test inputs only; they never own image bytes and the
oracle is not consulted.
"""
import re
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, require, write_json
from compiler import compile_source, verify_toolchain
from object_probe import read_object
from mz import MZ
from binder import fixup_relocation_sites, link_order_sites

MASM_MODULE = '''EXTRN f1:FAR, f2:FAR, f3:FAR
A_TEXT SEGMENT BYTE PUBLIC 'CODE'
 ASSUME CS:A_TEXT
start PROC FAR
 call f1
 call f2
 db 1100 dup(90h)
 mov dx, SEG f3
 call f3
 db 1100 dup(90h)
 mov ax, SEG f1
 call f2
 ret
start ENDP
A_TEXT ENDS
STACK SEGMENT PARA STACK 'STACK'
 db 64 dup(0)
STACK ENDS
END start
'''

MASM_PROVIDER = '''PUBLIC f1, f2, f3
B_TEXT SEGMENT BYTE PUBLIC 'CODE'
 ASSUME CS:B_TEXT
f1 PROC FAR
 ret
f1 ENDP
f2 PROC FAR
 ret
f2 ENDP
f3 PROC FAR
 ret
f3 ENDP
B_TEXT ENDS
END
'''

C_UNIT = (b'extern void far f1(void); extern void far f2(void); extern void far f3(void);\n'
          b'void far start(void) { f1(); f2(); f3(); f1(); }\n')

C_PROVIDER = b'void far f1(void) {} void far f2(void) {} void far f3(void) {}\n'


def _run(tool, arguments, work, profile):
    config, runner = verify_toolchain(profile)
    tc = (ROOT / config['directory']).resolve()
    result = subprocess.run([runner['path'], '-e', '-v5.00', str(tc / tool), *arguments],
                            cwd=work, env={'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.',
                                           'TMP': '.', 'MSDOS_TEMP': '.'},
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=90,
                            creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    verify_toolchain(profile)
    return result.returncode, result.stdout.decode('ascii', 'replace')


def _assemble(work, name, text):
    (work / (name + '.ASM')).write_bytes(text.replace('\n', '\r\n').encode('ascii'))
    code, log = _run('MASM.EXE', ['/Mx', '/I.', f'{name},{name}.OBJ,{name}.LST;'],
                     work, 'masm510-game')
    require(code == 0 and (work / (name + '.OBJ')).is_file(), 'Fixture MASM failed: ' + log)
    return (work / (name + '.OBJ')).read_bytes()


def _link(work, objects, profile):
    code, log = _run('LINK.EXE', ['/NOD /MAP ' + '+'.join(objects) + ',RESULT.EXE,RESULT.MAP;'],
                     work, profile)
    require((work / 'RESULT.EXE').is_file(), 'Fixture LINK failed: ' + log)
    data = (work / 'RESULT.EXE').read_bytes()
    return MZ.parse(data), (work / 'RESULT.MAP').read_text(), log


def _segment_start(mapping, name):
    match = re.search(r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+[0-9A-F]+H\s+' + re.escape(name) + r'\s',
                      mapping, re.M)
    require(match is not None, 'Missing fixture segment in LINK map: ' + name)
    return int(match[1], 16)


def masm_case(link_profile='msc510-medium'):
    work = Path(tempfile.mkdtemp(prefix='lo', dir=ROOT / 'build/probes'))
    obj = read_object(_assemble(work, 'A', MASM_MODULE))
    _assemble(work, 'B', MASM_PROVIDER)
    mz, mapping, log = _link(work, ['A.OBJ', 'B.OBJ'], link_profile)
    start = _segment_start(mapping, 'A_TEXT')
    fixup_order = fixup_relocation_sites(obj.linker_fixups, start, 'A_TEXT')
    linked = [r['segment'] * 16 + r['offset'] for r in mz.relocations]
    return {'object': 'masm510', 'link_profile': link_profile, 'fixup_sites': fixup_order, 'linked_sites': linked,
            'model': link_order_sites(fixup_order), 'link_log': log}


def c_case(profile='msc510-medium'):
    obj, receipt = compile_source(C_UNIT, profile)
    work = Path(receipt['work_directory'])
    provider, provider_receipt = compile_source(C_PROVIDER, profile)
    (work / 'PROV.OBJ').write_bytes((Path(provider_receipt['work_directory']) / 'UNIT.OBJ').read_bytes())
    mz, mapping, log = _link(work, ['UNIT.OBJ', 'PROV.OBJ'], profile)
    segment = next(d['name'] for d in obj.segment_defs if d['class'] == 'CODE')
    starts = [int(m, 16) for m in re.findall(
        r'^\s*([0-9A-F]+)H\s+[0-9A-F]+H\s+[0-9A-F]+H\s+' + re.escape(segment) + r'\s', mapping, re.M)]
    require(len(starts) == 1, 'Ambiguous fixture C segment in LINK map')
    fixup_order = fixup_relocation_sites(obj.linker_fixups, starts[0], segment)
    linked = [r['segment'] * 16 + r['offset'] for r in mz.relocations]
    return {'object': profile, 'fixup_sites': fixup_order, 'linked_sites': linked,
            'model': link_order_sites(fixup_order), 'link_log': log}


def run():
    (ROOT / 'build/probes').mkdir(parents=True, exist_ok=True)
    cases = [masm_case(), masm_case('msc500-medium'), c_case(), c_case('msc500-medium')]
    for case in cases:
        require(case['linked_sites'] == case['model'],
                'Pinned LINK relocation order differs from the FIXUPP-order model')
    report = {'schema': 1, 'authority': 'RESEARCH_ONLY_HISTORICAL_LINK_DIFFERENTIAL', 'cases': cases}
    write_json(ROOT / 'build/link-order-probe.json', report)
    print('Pinned LINK keeps FIXUPP order: build/link-order-probe.json')
    return report


if __name__ == '__main__':
    run()
