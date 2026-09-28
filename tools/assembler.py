"""Pinned MASM 5.10 object production with tracked project include expansion."""
import re
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, identity, require, sha, write_json
from compiler import CompileFailure, verify_toolchain
from object_probe import read_object


_ASM_INCLUDE = re.compile(
    r'^\s*include\s+(?:"([^"]+)"|\'([^\']+)\'|([^\s;]+))\s*(?:;.*)?$', re.I)


def prepare_asm(source):
    """Expand literal MASM includes and return their tracked, hash-checked closure."""
    try:
        text = source.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    except UnicodeError as error:
        raise ValueError('ASM source must be ASCII') from error
    require(not re.search(r'^\s*(?:org|phase)\b', text, re.I | re.M),
            'ASM origin manipulation is unsupported')
    include_root = (ROOT/'include').resolve()
    closure = {}

    def expand(contents, stack):
        output = []
        for line in contents.splitlines(keepends=True):
            if re.match(r'^\s*(?:includelib|incbin)\b', line, re.I):
                raise ValueError('ASM INCLUDELIB/INCBIN closure is unsupported')
            if not re.match(r'^\s*include\b', line, re.I):
                output.append(line)
                continue
            parsed = _ASM_INCLUDE.fullmatch(line.rstrip('\r\n'))
            require(parsed is not None, 'ASM INCLUDE must name one literal tracked include/ file')
            name = next(value for value in parsed.groups() if value is not None)
            normalized = name.replace('\\', '/')
            require(normalized and not Path(normalized).is_absolute() and
                    not re.search(r'(^|/)\.\.(/|$)|:', normalized),
                    'ASM INCLUDE path escapes include/')
            path = (ROOT/normalized).resolve()
            require(path.is_relative_to(include_root) and path.is_file(),
                    'ASM INCLUDE must resolve to a file under include/')
            relative = path.relative_to(ROOT).as_posix()
            tracked = subprocess.run(['git', 'ls-files', '--error-unmatch', '--', relative],
                                     cwd=ROOT, capture_output=True, text=True)
            require(tracked.returncode == 0, f'ASM include is not tracked: {relative}')
            data = path.read_bytes()
            value = {'path': relative, **identity(data)}
            previous = closure.setdefault(relative, value)
            require(previous == value, f'ASM include changed during closure: {relative}')
            require(relative not in stack, f'Recursive ASM include: {relative}')
            try:
                included = data.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
            except UnicodeError as error:
                raise ValueError(f'ASM include must be ASCII: {relative}') from error
            expanded_include = expand(included, stack+(relative,))
            output.append(expanded_include)
            if expanded_include and not expanded_include.endswith('\n'):
                output.append('\n')
        return ''.join(output)

    expanded = expand(text, ())
    return (expanded.replace('\n', '\r\n').encode('ascii'),
            [closure[path] for path in sorted(closure)])


def asm_source(source):
    """Return normalized assembler input after expanding tracked project includes."""
    return prepare_asm(source)[0]


def check_asm_closure(source, recipe):
    """Require an ASM recipe to freeze the current include path/hash closure."""
    _, closure = prepare_asm(source)
    require(recipe.get('include_closure', []) == closure,
            'ASM include closure differs from tracked include files')
    return closure


def assemble_source(source, profile, communals=None):
    require(profile == 'masm510-game', 'Unknown ASM reproduction profile')
    config, runner = verify_toolchain(profile)
    staged, include_closure = prepare_asm(source)
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
               'include_closure':include_closure, 'flags':config['flags']}
    write_json(work/'receipt.json', receipt)
    verify_toolchain(profile)
    if result.returncode or data is None:
        raise CompileFailure(f'Assembler failed; see {work / "assembler.log"}', receipt, 'ASSEMBLER_ERROR')
    try:
        # integ39: MASM `COMM NEAR` COMDEFs only for reviewed recipe declarations.
        obj = read_object(data, communals=communals)
    except ValueError as error:
        raise CompileFailure(str(error), receipt, 'UNSUPPORTED_OBJECT') from error
    return obj, receipt
