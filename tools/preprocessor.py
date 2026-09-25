"""Resolve historical C includes to a small, hash-checked source closure."""
import re
import subprocess
from pathlib import Path

from common import ROOT, identity, read_json, require

_DIRECTIVE = re.compile(r'^\s*#\s*([A-Za-z_]+)\b(.*)$')
_INCLUDE = re.compile(r'^\s*([<"])([^>"]+)[>"]\s*(?:/\*.*\*/\s*)?$')
_ALLOWED = {'define', 'undef', 'if', 'ifdef', 'ifndef', 'else', 'endif', 'include'}
_PACK = re.compile(r'^pack\s*\(\s*(1|2|4)?\s*\)\s*(?:/\*.*\*/\s*)?$', re.I)
_INTRINSIC_NAMES = frozenset({'inp', 'outp', 'inpw', 'outpw', '_enable', '_disable',
    'acos', 'asin', 'atan', 'atan2', 'cos', 'cosh', 'exp', 'fabs', 'fmod',
    'log', 'log10', 'pow', 'sin', 'sinh', 'sqrt', 'tan', 'tanh'})
_FUNCTION_PRAGMA = re.compile(r'^(intrinsic|function)\s*\(\s*([A-Za-z_]\w*(?:\s*,\s*[A-Za-z_]\w*)*)\s*\)\s*(?:/\*.*\*/\s*)?$', re.I)


def prepare(source, profile):
    """Return expanded ASCII source and ordered unique path/hash closure.

    All literal includes are resolved, including those inside inactive branches.
    This conservative closure prevents a conditional from accessing an unreviewed
    path if a macro changes.  The historical compiler still evaluates #if.
    """
    profiles = read_json(ROOT / 'layout/toolchain.json')['profiles']
    config = profiles.get(profile)
    if config is None:
        require(not re.search(rb'^\s*#\s*include\b', source, re.M),
                'Unknown compiler profile cannot resolve includes')
        config = {'directory': '.', 'files': []}
    include_root = (ROOT / config['directory'] / 'INCLUDE').resolve()
    pinned = {Path(row['path']).name.upper(): row for row in config['files']
              if Path(row['path']).parent.name.upper() == 'INCLUDE'}
    project_root = (ROOT / 'include').resolve()
    closure = {}
    pragmas = []

    def expand(raw, stack):
        try:
            lines = raw.decode('ascii').splitlines(keepends=True)
        except UnicodeDecodeError as error:
            raise ValueError('Historical source and headers must be ASCII') from error
        output = []
        for line_number, line in enumerate(lines, 1):
            match = _DIRECTIVE.match(line)
            if not match:
                output.append(line)
                continue
            directive, argument = match.group(1).lower(), match.group(2).strip()
            if directive == 'pragma':
                pack = _PACK.fullmatch(argument)
                function = _FUNCTION_PRAGMA.fullmatch(argument)
                require(pack is not None or function is not None, 'Unsupported historical pragma')
                if pack is not None:
                    pragmas.append({'pragma': 'pack', 'value': (int(pack.group(1)) if pack.group(1) else None),
                                    'path': stack[-1] if stack else '<source>', 'line': line_number})
                else:
                    names=[name.strip() for name in function.group(2).split(',')]
                    require(all(name in _INTRINSIC_NAMES for name in names),
                            'Unsupported historical pragma intrinsic/function name')
                    pragmas.append({'pragma': function.group(1).lower(), 'names': names,
                                    'path': stack[-1] if stack else '<source>', 'line': line_number})
                output.append(line)
                continue
            require(directive in _ALLOWED, f'Unsupported preprocessor directive: #{directive}')
            if directive != 'include':
                output.append(line)
                continue
            include = _INCLUDE.fullmatch(argument)
            require(include is not None, 'Include must use a literal <...> or "..." path')
            opener, name = include.group(1), include.group(2)
            require(name and not Path(name).is_absolute() and not re.search(r'(^|[/\\])\.\.($|[/\\])|:', name),
                    'Include path escapes its allowed root')
            if opener == '<':
                require('/' not in name and '\\' not in name, 'Pinned include must be a header basename')
                entry = pinned.get(name.upper())
                require(entry is not None, f'Header is not hash-pinned by profile: {name}')
                path = (ROOT / entry['path']).resolve()
                require(path.parent == include_root, 'Pinned header escaped INCLUDE directory')
            else:
                path = (project_root / name).resolve()
                require(path.is_relative_to(project_root) and path.suffix.lower() == '.h' and path.is_file(),
                        'Quoted include must resolve to a project header under include/')
                relative = path.relative_to(ROOT).as_posix()
                tracked = subprocess.run(['git', 'ls-files', '--error-unmatch', '--', relative],
                                         cwd=ROOT, capture_output=True, text=True)
                require(tracked.returncode == 0, f'Project header is not tracked: {relative}')
                entry = None
            data = path.read_bytes()
            value = {'path': path.relative_to(ROOT).as_posix(), **identity(data)}
            if entry is not None:
                require(identity(data) == {'size': entry['size'], 'sha256': entry['sha256']},
                        f'Pinned header hash mismatch: {path}')
            require(value['path'] not in stack, f'Recursive include: {value["path"]}')
            previous = closure.setdefault(value['path'], value)
            require(previous == value, f'Header changed during closure: {path}')
            output.append(expand(data, stack + (value['path'],)))
            output.append('\n')
        return ''.join(output)

    expanded = expand(source, ())
    return expanded.encode('ascii'), [closure[path] for path in sorted(closure)] + pragmas


def check_recipe(recipe, closure):
    require(recipe.get('preprocessor_closure', []) == closure,
            'Recipe preprocessor closure differs from current included headers')
    require(not closure or 'preprocessor_closure' in recipe,
            'Recipe must freeze its nonempty preprocessor closure')
