"""Expose shared reconstruction declarations to the existing host adapters.

The historical frontend remains authoritative. This port-only expansion keeps
its scalar include mapped to the port width header and presents subsystem
headers to the declaration parser/removal pass. Repeated imports remain in
their original scopes, including intentionally unguarded extern headers.
"""
from pathlib import Path
import re

_INCLUDE = re.compile(r'(?m)^[ \t]*#[ \t]*include[ \t]+"([^"\r\n]+)"[^\r\n]*(?:\r?\n|$)')


_TOKENS = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)


def _masked(source: str) -> str:
    def mask(match):
        raw = match[0]
        return re.sub(r'[^\n]', ' ', raw) if raw.startswith(('/*', '//')) else raw
    return _TOKENS.sub(mask, source)


def expand_project_headers(text: str, root: Path) -> str:
    project = (root / 'include').resolve()
    scalar = False

    def expand(source: str, stack: tuple[Path, ...]) -> str:
        def include(match):
            nonlocal scalar
            name = match[1]
            path = (project / name).resolve()
            if not path.is_relative_to(project):
                raise ValueError('Project include escapes include/: ' + name)
            if not path.is_file():
                return match[0]
            if path.name in {'types.h', 'stunts_types.h'}:
                if scalar:
                    return '\n'
                scalar = True
                return '#include "stunts_types.h"\n'
            if path in stack:
                raise ValueError('Recursive project include: ' + name)
            contents = path.read_text(encoding='latin-1')
            # Preserve guards and repeated includes for the C compiler. A
            # first include can be conditional, or a declaration-only header
            # can be imported in separate scopes. Eager de-duplication would
            # erase declarations that the host adapters still need to see.
            return expand(contents, stack + (path,)) + '\n'
        masked = _masked(source)
        result, cursor = [], 0
        for match in _INCLUDE.finditer(source):
            if not masked[match.start():match.end()].lstrip().startswith('#'):
                continue
            result.extend((source[cursor:match.start()], include(match)))
            cursor = match.end()
        result.append(source[cursor:])
        return ''.join(result)

    return expand(text, ())


def expand_constant_literals(text: str, source: str, root: Path) -> str:
    """Expose the source's numeric macros in an extracted C probe body."""
    expanded = expand_project_headers(source, root)
    constants = {}
    for match in re.finditer(r'(?m)^\s*#define[ \t]+(\w+)[ \t]+([^\r\n]+)', _masked(expanded)):
        value = match[2].strip()
        if re.fullmatch(r'[+-]?(?:0[xX][0-9a-fA-F]+|[0-9]+)[uUlL]*', value):
            constants[match[1]] = value
    # Leave directives, including their defining identifiers, to the C compiler.
    tokens = re.compile(r'^[ \t]*#[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*|\b[A-Za-z_]\w*\b', re.S | re.M)
    return tokens.sub(lambda m: constants.get(m[0], m[0]), text)
