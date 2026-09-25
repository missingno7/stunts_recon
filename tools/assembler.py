"""Pinned, self-contained MASM 5.10 object production."""
import re
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, identity, require, sha, write_json
from compiler import CompileFailure, verify_toolchain
from object_probe import read_object


def asm_source(source):
    """Keep the complete assembly input in one tracked source file."""
    try:
        text = source.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    except UnicodeError as error:
        raise ValueError('ASM source must be ASCII') from error
    require(not re.search(r'^\s*(?:include|includelib|incbin)\b', text, re.I | re.M),
            'ASM external source/include closure is unsupported')
    require(not re.search(r'^\s*(?:org|phase)\b', text, re.I | re.M),
            'ASM origin manipulation is unsupported')
    return text.replace('\n', '\r\n').encode('ascii')


def assemble_source(source, profile):
    require(profile == 'masm510-game', 'Unknown ASM reproduction profile')
    config, runner = verify_toolchain(profile)
    staged = asm_source(source)
    root = ROOT/'build/probes'; root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='a', dir=root))
    (work/'UNIT.ASM').write_bytes(staged)
    tc = (ROOT/config['directory']).resolve()
    executable = tc/config['executable']
    args = [str(runner['path']), '-e', '-v5.00', str(executable),
            *config['flags'], 'UNIT,UNIT.OBJ,UNIT.LST;']
    env = {'PATH':str(tc), 'MSDOS_PATH':str(tc), 'TEMP':'.', 'TMP':'.', 'MSDOS_TEMP':'.'}
    result = subprocess.run(args, cwd=work, env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=90,
                            creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    (work/'assembler.log').write_bytes(result.stdout)
    path = work/'UNIT.OBJ'
    data = path.read_bytes() if path.exists() else None
    receipt = {'profile':profile, 'command':args, 'source':identity(source),
               'staged_source':identity(staged), 'object':identity(data) if data else None,
               'assembler_stdout_sha256':sha(result.stdout), 'work_directory':str(work),
               'include_closure':[], 'flags':config['flags']}
    write_json(work/'receipt.json', receipt)
    verify_toolchain(profile)
    if result.returncode or data is None:
        raise CompileFailure(f'Assembler failed; see {work / "assembler.log"}', receipt, 'ASSEMBLER_ERROR')
    try:
        obj = read_object(data)
    except ValueError as error:
        raise CompileFailure(str(error), receipt, 'UNSUPPORTED_OBJECT') from error
    return obj, receipt
