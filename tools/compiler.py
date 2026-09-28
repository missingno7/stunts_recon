"""Pinned historical compiler invocation with fresh disposable inputs."""
import os
import re
import subprocess
import tempfile
from pathlib import Path
from common import ROOT, identity, read_json, require, sha, write_json
from object_probe import read_object
from preprocessor import prepare


class CompileFailure(ValueError):
    def __init__(self, message, receipt, category):
        super().__init__(message)
        self.receipt, self.category = receipt, category

def profile_file(config, logical, root=None):
    """Physical location of a pinned profile file named by its logical path.

    Pinned file identities keep their logical `toolchain/...` paths (manifest
    library owners and preprocessor closures name them); a profile whose hash
    identical copy lives elsewhere records that `directory` together with its
    `logical_directory`."""
    root = ROOT if root is None else root
    path = Path(logical)
    if path.is_absolute():
        return path
    base = Path(config.get('logical_directory', config['directory'])).as_posix()
    text = path.as_posix()
    if text.startswith(base + '/'):
        return root / config['directory'] / text[len(base)+1:]
    return root / path


def toolchain_path(logical):
    """Physical path of a pinned toolchain file across all profiles."""
    lock = read_json(ROOT / 'layout/toolchain.json')
    for config in lock['profiles'].values():
        if any(item['path'] == logical for item in config['files']):
            return profile_file(config, logical)
    return ROOT / logical


def verify_toolchain(profile):
    # Within one verification session the pinned identities are checked once
    # and checked again, uncached, when the session ends (tools/memo.py).
    import memo
    return memo.cached('verify_toolchain', profile, lambda: _verify_toolchain(profile), recheck=True)


def _verify_toolchain(profile):
    lock = read_json(ROOT / 'layout/toolchain.json')
    require(profile in lock['profiles'], 'Unknown compiler profile')
    config = lock['profiles'][profile]
    for item in config['files'] + [lock['runner']]:
        path = profile_file(config, item['path']) if item is not lock['runner'] else Path(item['path'])
        if not path.is_absolute():
            path = ROOT / path
        require(path.is_file() and identity(path.read_bytes()) == {'size': item['size'], 'sha256': item['sha256']},
                f'Toolchain hash mismatch: {path}')
    # The DOS-visible pass directory and TEMP enter every pass's own
    # environment (MSC_CMD_FLAGS -ef/-il) and its near-heap budget; they are
    # part of the pinned profile (tools/pass_environment.py).
    from pass_environment import check_player_directory
    require(lock['runner'].get('argv_options') == ['-e', '-v5.00'], 'Pinned runner options differ')
    check_player_directory(config, (ROOT / config['directory']).resolve())
    return config, lock['runner']

# Readable inline assembly is a reviewed extension of the register-gated MSC
# 6.00 profiles only (seg007 is pinned C 6.00 non-A): each statement
# is an 8086/80286 integer mnemonic with symbolic or numeric operands, or a
# label.  Raw byte emission (`_emit`, `__emit`, data directives, bare
# numbers) is refused under every profile.
INLINE_ASM_POLICY = 'readable-mnemonics-v1'
_ASM_MNEMONICS = frozenset('''
aaa aad aam aas adc add and call cbw clc cld cli cmc cmp cmpsb cmpsw cwd daa das dec div
enter hlt idiv imul in inc insb insw int into iret ja jae jb jbe jc jcxz je jg jge jl jle jmp
jna jnae jnb jnbe jnc jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lds lea
leave les lodsb lodsw loop loope loopne loopnz loopz mov movsb movsw mul neg nop not or out
outsb outsw pop popa popf push pusha pushf rcl rcr ret retf rol ror sahf sal sar sbb scasb
scasw shl shr stc std sti stosb stosw sub test wait xchg xlat xor rep repe repne repnz repz
'''.split())


def _asm_statements(text):
    """(line number, statement) for every `_asm {...}` / `_asm stmt` use."""
    out = []
    for match in re.finditer(r'\b__?asm\b', text):
        line = text.count('\n', 0, match.start()) + 1
        rest = text[match.end():]
        stripped = rest.lstrip(' \t')
        if stripped.startswith('{'):
            close = stripped.find('}')
            require(close > 0, 'Unterminated _asm block')
            body = stripped[1:close]
            for offset, raw in enumerate(body.split('\n')):
                out.append((line + offset, raw))
        else:
            out.append((line, stripped.split('\n', 1)[0]))
    return out


def check_inline_asm(text, config):
    """Profile-gated inline assembly check; returns the reviewed statement count."""
    require(not re.search(r'\b(?:__emit|_emit)\b', text), 'Raw byte emission forbidden for matching C')
    uses = re.search(r'\b(?:_asm|__asm|asm)\b', text)
    if uses is None:
        return None
    policy = config.get('source_extensions', {}).get('inline_asm')
    require(policy == INLINE_ASM_POLICY and not re.search(r'(?<![_\w])asm\b', text),
            'Inline assembly/raw emission forbidden for matching C')
    count = 0
    for line, raw in _asm_statements(text):
        statement = re.sub(r';.*$', '', raw).strip()
        if not statement:
            continue
        if re.fullmatch(r'[A-Za-z_]\w*:', statement):
            continue
        word = statement.split()[0].lower()
        require(word in _ASM_MNEMONICS,
                f'Inline assembly statement is not a reviewed mnemonic (line {line}): {statement}')
        require(not re.search(r'\b(?:db|dw|dd|dq|dt|_emit|__emit)\b', statement, re.I),
                f'Inline assembly data emission forbidden (line {line})')
        count += 1
    return {'policy': INLINE_ASM_POLICY, 'statements': count}


def object_policy(config):
    """Reviewed object-reader options of a profile (MSC 6 /Zi CodeView segments)."""
    policy = config.get('object_debug_segments')
    return {'debug_segments': policy} if policy else {}


def compile_source(source, profile, flags=None, *, research_local_symbols=False, sparse_zero=None, communals=None):
    config, runner = verify_toolchain(profile)
    root = ROOT / 'build/probes'
    root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='p', dir=root))
    # Fixed DOS input basename and deterministic DOS line endings.
    source_receipt={'profile':profile,'source':identity(source),'work_directory':str(work),
                    'research_local_symbols':research_local_symbols}
    try:
        expanded, closure = prepare(source, profile)
        text = expanded.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
        source_receipt['preprocessor_closure'] = closure
    except ValueError as error:
        write_json(work/'receipt.json',source_receipt)
        raise CompileFailure(str(error),source_receipt,'UNSUPPORTED_SOURCE') from error
    try:
        source_receipt['inline_asm'] = check_inline_asm(text, config)
    except ValueError as error:
        write_json(work/'receipt.json',source_receipt)
        raise CompileFailure(str(error),source_receipt,'UNSUPPORTED_SOURCE') from error
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
               'preprocessor_closure': closure,
               'work_directory': str(work), 'compiler_stdout_sha256': sha(result.stdout),
               'research_local_symbols': research_local_symbols}
    if source_receipt.get('inline_asm'):
        receipt['inline_asm'] = source_receipt['inline_asm']
    if sparse_zero is not None:
        receipt['sparse_zero'] = sparse_zero
    write_json(work / 'receipt.json', receipt)
    verify_toolchain(profile)
    if result.returncode != 0 or data is None:
        raise CompileFailure(f'Compiler failed; see {work / "compiler.log"}', receipt, 'COMPILER_ERROR')
    try:
        # integ39: COMDEF records only for a recipe's reviewed communal
        # declarations (tools/communal_unit.py); otherwise refused.
        obj = read_object(data, research_local_symbols=research_local_symbols,
                          sparse_zero=sparse_zero, communals=communals, **object_policy(config))
    except ValueError as error:
        raise CompileFailure(str(error), receipt, 'UNSUPPORTED_OBJECT') from error
    from common import json_bytes
    receipt['effective_code']=sha(json_bytes({'segments':{k:identity(v) for k,v in obj.segments.items()},
        'declarations':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,
        'externals':obj.externals,'fixups':obj.linker_fixups,
        'local_symbol_records':obj.local_symbol_records,'profile':profile,'flags':argv[5:-1],
        **({'communals':obj.communals} if getattr(obj,'communals',None) else {})}))
    if obj.debug_segments:
        receipt['debug_segments'] = obj.debug_segments
    write_json(work/'receipt.json',receipt)
    return obj, receipt
