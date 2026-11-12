"""Expose shared reconstruction declarations to the existing host adapters.

The historical frontend remains authoritative. This port-only expansion keeps
its scalar include intact and presents ordinary guarded project headers to the
same declaration parser/removal pass used for source-local declarations.
"""
from pathlib import Path
import re

_INCLUDE = re.compile(r'(?m)^[ \t]*#[ \t]*include[ \t]+"([^"\r\n]+)"[^\r\n]*(?:\r?\n|$)')


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
            if path.name == 'stunts_types.h':
                if scalar:
                    return '\n'
                scalar = True
                return '#include "stunts_types.h"\n'
            if path in stack:
                raise ValueError('Recursive project include: ' + name)
            contents = path.read_text(encoding='latin-1')
            # Preserve guards and repeated includes for the C compiler. A
            # first include can be conditional; eager de-duplication would
            # incorrectly erase a later active include.
            return expand(contents, stack + (path,)) + '\n'
        tokens = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)
        def mask(match):
            raw = match[0]
            return ''.join('\n' if c == '\n' else ' ' for c in raw) if raw.startswith(('/*', '//')) else raw
        masked = tokens.sub(mask, source)
        result, cursor = [], 0
        for match in _INCLUDE.finditer(source):
            if not masked[match.start():match.end()].lstrip().startswith('#'):
                continue
            result.extend((source[cursor:match.start()], include(match)))
            cursor = match.end()
        result.append(source[cursor:])
        return ''.join(result)

    return expand(text, ())
