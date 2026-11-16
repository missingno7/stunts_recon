"""Run the locked save routine with real DOSBox-X file services.

The COM loader reads the complete immutable image as data, applies its ordered
relocations, supplies a normal FAR-call stack and records DOS service requests.
No instruction in the original routine is replaced. All output is scratch.
"""
from __future__ import annotations
import hashlib
import json
import os
import re
from pathlib import Path
import struct
import subprocess

from .oracle import OracleImage, SymbolMap, ROOT, DGROUP_PARAGRAPH


def full_floppy(old=b'OLD'):
    """A FAT12 volume with no free clusters, plus an existing destination."""
    disk = bytearray(1474560)
    disk[:11] = b'\xeb\x3c\x90PORTAUD '
    struct.pack_into('<HBHBHHBHHHII', disk, 11, 512, 1, 1, 2, 224,
                     2880, 0xf0, 9, 18, 2, 0, 0)
    disk[510:512] = b'\x55\xaa'
    fat = bytearray(4608)
    fat[:3] = b'\xf0\xff\xff'
    def cluster(n, value):
        at = n * 3 // 2
        word = int.from_bytes(fat[at:at + 2], 'little')
        word = (word & 0x000f) | (value << 4) if n & 1 else (word & 0xf000) | value
        fat[at:at + 2] = word.to_bytes(2, 'little')
    first = 3 if old else 2
    for n in range(first, 2849):
        cluster(n, n + 1 if n < 2848 else 0xfff)
    if old:
        cluster(2, 0xfff)
    disk[512:5120] = fat
    disk[5120:9728] = fat
    root = 9728
    for at, name, start, size in ((root, b'AUDIT   TRK', 2 if old else 0, len(old)),
                                (root + 32, b'FILL    BIN', first, (2849 - first) * 512)):
        disk[at:at + 11] = name
        disk[at + 11] = 0x20
        struct.pack_into('<HI', disk, at + 26, start, size)
    disk[16896:16896 + len(old)] = old
    return bytes(disk)


def com_source(image, destination, *, readonly=False, exhaust=False, inject=0):
    entry = SymbolMap().resolve(140555)
    frame, offset = entry.far_at(0)
    reads = []
    for at in range(0, len(image.load_image), 0x4000):
        count = min(0x4000, len(image.load_image) - at)
        reads.append(f'''mov ax,[cs:loaded]
 add ax,{at // 16}
 mov ds,ax
 xor dx,dx
 mov bx,[cs:handle]
 mov cx,{count}
 mov ah,3fh
 int 21h
 jc die
 cmp ax,{count}
 jne die''')
    return f'''bits 16
org 100h
jmp start
loaded dw 0
handle dw 0
old21 dd 0
armed db 0
count db 0
requests times 16 db 0
result dw 0
saved_ss dw 0
saved_sp dw 0
callptr dw {offset},0
image_name db 'IMAGE.BIN',0
output_name db 'RESULT.BIN',0
fill_name db 'FILL.BIN',0
target db '{destination}',0
payload db 'abc'
relocs: incbin 'RELOCS.BIN'
start:
 mov bx,1000h
 mov ah,4ah
 int 21h
 jc die
 mov bx,4000h
 mov ah,48h
 int 21h
 jc die
 mov [loaded],ax
 mov dx,image_name
 mov ax,3d00h
 int 21h
 jc die
 mov [handle],ax
 {chr(10).join(reads)}
 mov bx,[cs:handle]
 mov ah,3eh
 int 21h
 push cs
 pop ds
 mov si,relocs
 mov cx,{len(image.relocations)}
fixup:
 lodsw
 mov di,ax
 lodsw
 add ax,[loaded]
 mov es,ax
 mov ax,[loaded]
 add [es:di],ax
 loop fixup
 mov ax,[loaded]
 add ax,{frame}
 mov [callptr+2],ax
 mov ax,[loaded]
 add ax,{DGROUP_PARAGRAPH}
 mov es,ax
 mov di,8000h
 mov si,target
 mov cx,{len(destination) + 1}
 rep movsb
 %if {int(readonly)}
 mov dx,target
 mov cx,1
 mov ax,4301h
 int 21h
 jc die
 %endif
 mov ax,3521h
 int 21h
 mov [old21],bx
 mov [old21+2],es
 mov dx,hook
 mov ax,2521h
 int 21h
 %if {int(exhaust)}
fill_handles:
 mov dx,fill_name
 mov ax,3d00h
 int 21h
 jnc fill_handles
 %endif
 mov [saved_ss],ss
 mov [saved_sp],sp
 mov byte [armed],1
 mov ax,[loaded]
 add ax,{DGROUP_PARAGRAPH}
 cli
 mov ss,ax
 mov sp,0f000h
 mov ds,ax
 sti
 push word 0
 push word 3
 push cs
 push word payload
 push word 8000h
 call far [cs:callptr]
 mov [cs:result],ax
 cli
 mov ax,[cs:saved_ss]
 mov ss,ax
 mov sp,[cs:saved_sp]
 sti
 push cs
 pop ds
 mov byte [armed],0
 mov dx,[old21]
 mov ax,[old21+2]
 mov ds,ax
 mov ax,2521h
 int 21h
 push cs
 pop ds
 mov dx,output_name
 xor cx,cx
 mov ah,3ch
 int 21h
 jc die
 mov bx,ax
 mov dx,result
 mov cx,2
 mov ah,40h
 int 21h
 mov dx,count
 mov cx,17
 mov ah,40h
 int 21h
 mov ah,3eh
 int 21h
 mov ax,4c00h
 int 21h
die:
 mov ax,4c7fh
 int 21h
hook:
 cmp byte [cs:armed],0
 je chain
 pushf
 push bx
 xor bx,bx
 mov bl,[cs:count]
 cmp bl,16
 jae logged
 mov [cs:requests+bx],ah
 inc byte [cs:count]
logged:
 pop bx
 popf
 %if {inject}
 cmp ah,3ch
 jne chain
 mov ax,{inject}
 push bp
 mov bp,sp
 or word [ss:bp+6],1
 pop bp
 iret
 %endif
chain:
 jmp far [cs:old21]
'''


def assemble_loader(folder, source):
    """Assemble our loader with the port's native GNU tools (no MSYS pipes)."""
    lines = ['.code16', '.intel_syntax noprefix', '.global start']
    for line in source.splitlines():
        line = line.strip()
        if line.startswith(('bits ', 'org ')):
            continue
        line = re.sub(r'\b([0-9][0-9a-fA-F]*)h\b', r'0x\1', line)
        line = re.sub(r'\[cs:([^]]+)\]', r'cs:[\1]', line)
        line = re.sub(r'\[ss:([^]]+)\]', r'ss:[\1]', line)
        line = re.sub(r'\[es:([^]]+)\]', r'es:[\1]', line)
        line = line.replace('call far ', 'call DWORD PTR ').replace('jmp far ', 'jmp DWORD PTR ')
        line = line.replace('%if ', '.if ').replace('%endif', '.endif')
        line = line.replace('byte [', 'BYTE PTR [').replace('word [', 'WORD PTR [')
        line = line.replace('byte cs:', 'BYTE PTR cs:').replace('word ss:', 'WORD PTR ss:')
        line = line.replace('word cs:', 'WORD PTR cs:')
        line = re.sub(r'^push word (\d+|0x[0-9a-f]+)$', r'push \1', line)
        line = line.replace('push word payload', 'push OFFSET payload')
        line = re.sub(r'^(mov (?:dx|si|di),)([a-z_]\w*)$',
                      lambda m: m[0] if m[2] in {'ax', 'bx', 'cx', 'dx', 'si', 'di', 'sp', 'bp'}
                      else m[1] + 'OFFSET ' + m[2], line)
        line = re.sub(r'^(\w+) times (\d+) db 0$', r'\1: .zero \2', line)
        match = re.match(r'^(\w+) (dw|dd|db) (.*)$', line)
        if match:
            name, kind, value = match.groups()
            if value.startswith("'"):
                line = name + ': .asciz ' + json.dumps(value[1:value.rfind("'")])
            else:
                line = name + ': ' + {'dw': '.word ', 'dd': '.long ', 'db': '.byte '}[kind] + value
        line = line.replace("incbin 'RELOCS.BIN'", '.incbin "RELOCS.BIN"')
        lines.append(line)
    (folder / 'PROBE.S').write_text('\n'.join(lines) + '\n')
    bin_dir = Path('C:/msys64/mingw32/bin')
    commands = [[str(bin_dir / 'as.exe'), '--32', 'PROBE.S', '-o', 'PROBE.O'],
        [str(bin_dir / 'ld.exe'), '--image-base', '0', '-Ttext', '0x100',
         '--entry', 'start', 'PROBE.O', '-o', 'LOADER.EXE'],
        [str(bin_dir / 'objcopy.exe'), '-j', '.text', '-O', 'binary', 'LOADER.EXE', 'PROBE.COM']]
    for command in commands:
        ran = subprocess.run(command, cwd=folder, capture_output=True, text=True, timeout=30)
        if ran.returncode:
            raise RuntimeError(ran.stderr)


def capture(work, *, dosbox=Path('C:/tools/dosbox-x/dosbox-x.exe')):
    work = Path(work).resolve()
    if not work.is_relative_to((ROOT / 'build').resolve()):
        raise ValueError('DOS service fixtures must stay under ignored build/')
    work.mkdir(parents=True, exist_ok=True)
    image = OracleImage.load()
    results = []
    cases = [('readonly', r'C:\AUDIT.TRK', True, False, 0, None),
             ('directory', r'C:\AUDIT.TRK', False, False, 0, None),
             ('readonly-fat', r'D:\AUDIT.TRK', True, False, 0, b'OLD'),
             ('bad-path', r'C:\BAD\AUDIT.TRK', False, False, 0, None),
             ('handles-full', r'C:\AUDIT.TRK', False, True, 0, None),
             ('create-injected-5', r'C:\AUDIT.TRK', False, False, 5, None),
             ('disk-full-existing', r'D:\AUDIT.TRK', False, False, 0, b'OLD'),
             ('disk-full-empty', r'D:\AUDIT.TRK', False, False, 0, b'')]
    for name, destination, readonly, exhaust, inject, floppy_old in cases:
        destination = destination.replace(chr(92) * 2, chr(92))
        folder = work / name
        folder.mkdir(exist_ok=True)
        target = folder / 'AUDIT.TRK'
        if target.is_file(): target.chmod(0o666)
        if name == 'directory':
            target.mkdir(exist_ok=True)
        else:
            target.write_bytes(b'OLD')
        (folder / 'FILL.BIN').write_bytes(b'x')
        (folder / 'IMAGE.BIN').write_bytes(image.load_image)
        (folder / 'RELOCS.BIN').write_bytes(b''.join(struct.pack('<HH',
            r['load_offset'] & 15, r['load_offset'] >> 4) for r in image.relocations))
        source = com_source(image, destination, readonly=readonly, exhaust=exhaust, inject=inject)
        (folder / 'PROBE.ASM').write_text(source)
        assemble_loader(folder, source)
        mount = ''
        if floppy_old is not None:
            (folder / 'FULL.IMG').write_bytes(full_floppy(floppy_old))
            mount = f'imgmount d "{folder / "FULL.IMG"}" -t floppy\n'
        config = f'''[sdl]
fullscreen=false
output=surface
[dosbox]
memsize=16
[cpu]
core=normal
cycles=fixed 30000
[autoexec]
mount c "{folder}"
c:
{mount}PROBE.COM
exit
'''
        (folder / 'dosbox.conf').write_text(config)
        env = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
        with (folder / 'run.log').open('w') as log:
            ran = subprocess.run([str(dosbox), '-conf', str(folder / 'dosbox.conf'),
                '-fastlaunch', '-exit', '-nomenu'], cwd=folder, env=env, stdout=log,
                stderr=subprocess.STDOUT, timeout=30,
                creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
        raw = (folder / 'RESULT.BIN').read_bytes()
        if ran.returncode or len(raw) != 19:
            raise RuntimeError(f'incomplete DOS service capture: {folder}')
        target = folder / 'AUDIT.TRK'
        if floppy_old is None:
            final = target.read_bytes().hex() if target.is_file() else None
            final_kind = 'directory' if target.is_dir() else 'file' if target.is_file() else 'absent'
        else:
            disk = (folder / 'FULL.IMG').read_bytes()
            first = disk[9728]
            size = struct.unpack_from('<I', disk, 9728 + 28)[0]
            cluster = struct.unpack_from('<H', disk, 9728 + 26)[0]
            final = None if first in (0, 0xe5) else disk[16896 + (cluster - 2) * 512:16896 + (cluster - 2) * 512 + size].hex()
            final_kind = 'absent' if first in (0, 0xe5) else 'file'
        results.append(dict(case=name, destination=destination, result=struct.unpack_from('<H', raw)[0],
                            services=list(raw[3:3 + raw[2]]), final_hex=final, final_kind=final_kind,
                            loader_sha256=hashlib.sha256((folder / 'PROBE.COM').read_bytes()).hexdigest()))
    report = dict(schema='stunts-original-dos-save-cases-v1',
        oracle_lock_sha256=hashlib.sha256((ROOT / 'layout/oracle.lock.json').read_bytes()).hexdigest(),
        image_sha256=hashlib.sha256(image.load_image).hexdigest(),
        dosbox_sha256=hashlib.sha256(dosbox.read_bytes()).hexdigest(), cases=results)
    (work / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def capture_no_joystick(work, *, dosbox=Path('C:/tools/dosbox-x/dosbox-x.exe')):
    """Actual original calibration with DOSBox-X joysticktype=none."""
    folder = Path(work).resolve()
    if not folder.is_relative_to((ROOT / 'build').resolve()):
        raise ValueError('DOS service fixtures must stay under ignored build/')
    folder.mkdir(parents=True, exist_ok=True)
    image = OracleImage.load()
    symbols = SymbolMap()
    save_frame, save_offset = symbols.resolve(140555).far_at(0)
    reset_frame, reset_offset = symbols.resolve(133044).far_at(0)
    sample_frame, sample_offset = symbols.resolve('get_joy_flags').far_at(0)
    if len({save_frame, reset_frame, sample_frame}) != 1:
        raise ValueError('loader requires the independently resolved common code frame')
    source = com_source(image, 'UNUSED.TRK')
    source = source.replace(f'callptr dw {save_offset},0', f'callptr dw {reset_offset},0')
    source = source.replace('requests times 16 db 0', 'requests times 16 db 0\njoystick_results times 160 db 0')
    block = '''push word 0
 push word 3
 push cs
 push word payload
 push word 8000h
 call far [cs:callptr]'''
    if source.count(block) != 1:
        raise ValueError('loader call anchor changed')
    source = source.replace(block, f'''call far [cs:callptr]
 mov word [cs:callptr],{sample_offset}
 mov cx,80
 mov di,joystick_results
sample_absent:
 push cx
 call far [cs:callptr]
 pop cx
 mov [cs:di],ax
 add di,2
 loop sample_absent''')
    source = source.replace('mov dx,count\n mov cx,17\n mov ah,40h\n int 21h',
        'mov dx,count\n mov cx,17\n mov ah,40h\n int 21h\n mov dx,joystick_results\n mov cx,160\n mov ah,40h\n int 21h')
    (folder / 'IMAGE.BIN').write_bytes(image.load_image)
    (folder / 'RELOCS.BIN').write_bytes(b''.join(struct.pack('<HH', r['load_offset'] & 15,
        r['load_offset'] >> 4) for r in image.relocations))
    assemble_loader(folder, source)
    config = f'''[sdl]
fullscreen=false
output=surface
[joystick]
joysticktype=none
[cpu]
core=normal
cycles=fixed 30000
[autoexec]
mount c "{folder}"
c:
PROBE.COM
exit
'''
    (folder / 'dosbox.conf').write_text(config)
    with (folder / 'run.log').open('w') as log:
        subprocess.run([str(dosbox), '-conf', str(folder / 'dosbox.conf'), '-fastlaunch', '-exit', '-nomenu'],
            cwd=folder, env=dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy'),
            stdout=log, stderr=subprocess.STDOUT, timeout=30, check=True,
            creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    raw = (folder / 'RESULT.BIN').read_bytes()
    if len(raw) != 179: raise RuntimeError('incomplete original no-joystick capture')
    report = dict(schema='stunts-original-no-joystick-v1', flags=list(struct.unpack_from('<80H', raw, 19)),
        image_sha256=hashlib.sha256(image.load_image).hexdigest(),
        dosbox_sha256=hashlib.sha256(dosbox.read_bytes()).hexdigest(),
        loader_sha256=hashlib.sha256((folder / 'PROBE.COM').read_bytes()).hexdigest())
    (folder / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    return report
