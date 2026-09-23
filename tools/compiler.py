"""Pinned historical compiler invocation with fresh disposable inputs."""
import os
import re
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, identity, read_json, require, sha, write_json
from object_probe import read_object


class CompileFailure(ValueError):
    def __init__(self, message, receipt, category):
        super().__init__(message)
        self.receipt, self.category = receipt, category

def verify_toolchain(profile):
    lock = read_json(ROOT / 'layout/toolchain.json')
    require(profile in lock['profiles'], 'Unknown compiler profile')
    config = lock['profiles'][profile]
    for item in config['files'] + [lock['runner']]:
        path = Path(item['path'])
        if not path.is_absolute():
            path = ROOT / path
        require(path.is_file() and identity(path.read_bytes()) == {'size': item['size'], 'sha256': item['sha256']},
                f'Toolchain hash mismatch: {path}')
    return config, lock['runner']

def compile_source(source, profile, flags=None):
    config, runner = verify_toolchain(profile)
    root = ROOT / 'build/probes'
    root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='p', dir=root))
    # Fixed DOS input basename and deterministic DOS line endings.
    source_receipt={'profile':profile,'source':identity(source),'work_directory':str(work)}
    try:
        text = source.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    except UnicodeDecodeError as error:
        raise CompileFailure('Historical source must be ASCII',source_receipt,'UNSUPPORTED_SOURCE') from error
    for pattern,message in [(r'^\s*#','Preprocessor closure not implemented: directives blocked'),
                            (r'\b(?:_asm|__asm|asm|__emit)\b','Inline assembly/raw emission forbidden for matching C')]:
        if re.search(pattern,text,re.M):
            write_json(work/'receipt.json',source_receipt)
            raise CompileFailure(message,source_receipt,'UNSUPPORTED_SOURCE')
    staged = text.replace('\n', '\r\n').encode('ascii')
    (work / 'UNIT.C').write_bytes(staged)
    tc = (ROOT / config['directory']).resolve()
    argv = [runner['path'], '-e', '-v5.00', str(tc / config['executable']), '/c']
    argv += flags if flags is not None else config['flags']
    argv += ['UNIT.C']
    env = {'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'}
    result = subprocess.run(argv, cwd=work, env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=60,
                            creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    (work / 'compiler.log').write_bytes(result.stdout)
    obj_path = work / 'UNIT.OBJ'
    data = obj_path.read_bytes() if obj_path.exists() else None
    receipt = {'profile': profile, 'command': argv, 'source': identity(source),
               'staged_source': identity(staged), 'object': identity(data) if data is not None else None,
               'work_directory': str(work), 'compiler_stdout_sha256': sha(result.stdout)}
    write_json(work / 'receipt.json', receipt)
    verify_toolchain(profile)
    if result.returncode != 0 or data is None:
        raise CompileFailure(f'Compiler failed; see {work / "compiler.log"}', receipt, 'COMPILER_ERROR')
    try:
        obj = read_object(data)
    except ValueError as error:
        raise CompileFailure(str(error), receipt, 'UNSUPPORTED_OBJECT') from error
    from common import json_bytes
    receipt['effective_code']=sha(json_bytes({'segments':{k:identity(v) for k,v in obj.segments.items()},
        'declarations':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,
        'externals':obj.externals,'fixups':obj.linker_fixups,'profile':profile,'flags':argv[5:-1]}))
    write_json(work/'receipt.json',receipt)
    return obj, receipt
