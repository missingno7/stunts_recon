"""Locate current communal declarations after complete-object equivalence.

This only refreshes a source citation. Allocation sizes, addresses, declarers,
review notes, indexed-access evidence and all ownership facts stay unchanged.
The compiler's unchanged complete COMDEFs and the subsequent complete link/image
gates remain authoritative; this lexer is a locator, never a size oracle.
"""
import json
import re
from common import json_bytes, require

_PREFIX = '''typedef char I8; typedef signed char I8S; typedef unsigned char U8;
typedef int I16; typedef short I16S; typedef unsigned int U16;
typedef unsigned short U16S; typedef long I32; typedef unsigned long U32;
typedef unsigned char u8; typedef unsigned int u16; typedef unsigned long u32;
'''


def statements(text):
    from rename import C_TOKEN
    def blank(token):
        return ''.join('\n' if c == '\n' else ' ' for c in token)
    masked = ''.join(blank(token) if token.startswith(('/*', '//', '"', "'")) else token
                     for token in C_TOKEN.findall(text))
    masked = re.sub(r'(?m)^[ \t]*#(?:[^\n]*\\\n)*[^\n]*', lambda m: blank(m[0]), masked)
    depth = paren = bracket = 0
    start, function = 0, False
    for i, char in enumerate(masked):
        if char == '(': paren += 1
        elif char == ')': paren -= 1
        elif char == '[': bracket += 1
        elif char == ']': bracket -= 1
        elif char == '{':
            if depth == 0:
                prefix = masked[start:i].strip()
                function = prefix.endswith(')') and '=' not in prefix
            depth += 1
        elif char == '}':
            depth -= 1
            if depth == 0 and function:
                start, function = i + 1, False
        elif char == ';' and depth == paren == bracket == 0:
            match = re.search(r'\S', masked[start:i+1])
            if match:
                yield text[start+match.start():i+1].strip()
            start = i + 1


def refresh_source_bases(engine, changes, affected):
    """Called only after every affected complete object has compared equal."""
    import cdecls
    from rename import C_TOKEN
    manifest = json.loads(engine.files['layout/manifest.json'])
    updates = []
    normalized = lambda text: re.sub(r'\s+', ' ', text).strip()
    candidates = {}
    for key in affected:
        item = engine.recipes[key]
        path = item['recipe']['source']
        if path in changes and path.startswith(('game/', 'lib/')) and path.endswith('.c'):
            current = changes[path].decode('ascii')
            located = []
            for statement in statements(current):
                # The locator does not run the preprocessor. Recognize the
                # project's segmented-pointer spellings without changing the
                # original statement retained as the source citation.
                parsed = ''.join(token.lower() if token in ('FAR', 'NEAR', 'HUGE') else token
                                 for token in C_TOKEN.findall(statement))
                unit = cdecls.Unit(_PREFIX + parsed)
                for name, rows in unit.decls.items():
                    if any(row.get('defined') and row['type']['k'] != 'fn' and
                           row.get('storage') not in ('extern', 'static') for row in rows):
                        located.append((name, statement))
            candidates[path] = located
    for owner in manifest.get('bss_owners', []):
        for item in owner.get('communals', []):
            basis = item['size_basis']
            if basis['kind'] == 'pinned-member': continue
            source_paths = sorted({entry['recipe']['source'] for entry in engine.recipes.values()
                                   if set(entry['owners']) & set(item['declarers'])})
            text = basis.get('declaration', '')
            if any(normalized(text) in normalized(changes.get(path, engine.files[path]).decode('ascii'))
                   for path in source_paths):
                continue
            # Neutral/absorbed-gap reviews carry additional meaning: leave
            # their citations unchanged until separately reviewed if necessary.
            require(basis['kind'] == 'declared-type',
                    'Nontrivial communal source basis needs review: ' + item['name'])
            choices = {(path, statement) for path in source_paths for name, statement in candidates.get(path, [])
                       if '_' + name == item['name']}
            require(len(choices) == 1, 'Cannot uniquely locate current communal declaration: ' + item['name'])
            path, declaration = choices.pop()
            updates.append({'symbol': item['name'], 'source': path, 'before': text, 'after': declaration})
            basis['declaration'] = declaration
    if updates:
        changes['layout/manifest.json'] = json_bytes(manifest)
    engine.declaration_basis_updates = updates
