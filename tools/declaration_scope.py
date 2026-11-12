"""Keep source edits on the raw-object gate, even through --declarations.

This is a conservative scope check, not another C compiler. Function signatures,
bodies and their preprocessing dependencies must be unchanged to select the
declaration comparator. Unrecognized input falls back to raw object equality.
"""
import re


def _tokens(text):
    from rename import C_TOKEN
    return [m for m in C_TOKEN.finditer(text)
            if not m[0].isspace() and not m[0].startswith(('/*', '//'))]


def _expanded(files, source, stack=()):
    if source in stack:
        raise ValueError('Recursive declaration include')
    text = files[source].decode('ascii')
    pattern = re.compile(r'(?m)^[ \t]*#[ \t]*include[ \t]+"([^"\r\n]+)"[^\r\n]*')
    def include(match):
        name = 'include/' + match[1]
        if name not in files:
            raise ValueError('Unresolved project header: ' + name)
        return _expanded(files, name, stack + (source,))
    return pattern.sub(include, text)


def _view(text):
    # Preserve offsets while removing directives from the syntactic scan.
    directives = list(re.finditer(r'(?m)^[ \t]*#(?:[^\n]*\\\n)*[^\n]*', text))
    masked = list(text)
    macros, conditions = {}, set()
    for match in directives:
        logical = match[0].replace('\\\n', ' ')
        definition = re.match(r'\s*#\s*define\s+(\w+)(.*)', logical, re.S)
        if definition:
            # Keep order/redefinitions, collapsing only identical adjacent
            # definitions (ordinary repeated guarded includes).
            sequence = macros.setdefault(definition[1], [])
            value = tuple(m[0] for m in _tokens(definition[2]))
            if not sequence or sequence[-1] != value:
                sequence.append(value)
        undefinition = re.match(r'\s*#\s*undef\s+(\w+)', logical)
        if undefinition:
            macros.setdefault(undefinition[1], []).append(('@undef',))
        conditional = re.match(r'\s*#\s*(?:if|ifdef|ifndef|elif)\b(.*)', logical, re.S)
        if conditional:
            conditions.update(re.findall(r'\b[A-Za-z_]\w*\b', conditional[1]))
        for i in range(match.start(), match.end()):
            if masked[i] != '\n':
                masked[i] = ' '
    tokens = _tokens(''.join(masked))
    bodies, depth, start, function = [], 0, 0, None
    def context(position):
        stack, pragmas = [], []
        for directive in directives:
            if directive.start() >= position:
                break
            row = tuple(t[0] for t in _tokens(directive[0]))
            command = row[1] if len(row) > 1 else ''
            if command in ('if', 'ifdef', 'ifndef'):
                stack.append((row,))
            elif command in ('else', 'elif'):
                if not stack:
                    raise ValueError('Unbalanced preprocessing condition')
                stack[-1] += (row,)
            elif command == 'endif':
                if not stack:
                    raise ValueError('Unbalanced preprocessing condition')
                stack.pop()
            elif command == 'pragma' and (len(row) < 3 or row[2] != 'pack'):
                pragmas.append(row)
        return tuple(stack), tuple(pragmas)
    for i, token in enumerate(tokens):
        value = token[0]
        if depth == 0 and value in ('_asm', '__asm'):
            raise ValueError('File-scope assembly requires raw object equality')
        if value == '{':
            if depth == 0:
                prefix = [t[0] for t in tokens[start:i]]
                if i and tokens[i-1][0] == ')' and '=' not in prefix:
                    function = (start, i)
                elif '=' not in prefix and not any(t in ('struct', 'union', 'enum') for t in prefix):
                    # Includes K&R definitions, whose parameter declaration
                    # semicolons defeat the simple signature locator.
                    raise ValueError('Unrecognized top-level C brace construct')
            depth += 1
        elif value == '}':
            depth -= 1
            if depth < 0:
                raise ValueError('Unbalanced C braces')
            if depth == 0 and function is not None:
                first, brace = function
                signature = tuple(t[0] for t in tokens[first:brace])
                body, j = [], brace
                while j <= i:
                    if tokens[j][0] == 'extern':
                        # Moving an extern into/out of a function is still a
                        # declaration edit. Locals, register and labels stay.
                        while j <= i and tokens[j][0] != ';':
                            j += 1
                        if j > i:
                            raise ValueError('Unterminated block extern')
                    else:
                        body.append(tokens[j][0])
                    j += 1
                local_directives = tuple(tuple(t[0] for t in _tokens(d[0])) for d in directives
                                         if tokens[brace].start() < d.start() < token.end())
                bodies.append((signature, tuple(body), local_directives, context(tokens[brace].start())))
                start, function = i + 1, None
        elif value == ';' and depth == 0:
            start = i + 1
    if depth:
        raise ValueError('Unbalanced C braces')
    return tuple(bodies), macros, conditions


def requires_raw_objects(files, changes):
    """Return reasons for the stronger comparator; empty means declarations."""
    current = {**files, **changes}
    reasons = []
    changed_headers = {p for p in changes if p.startswith('include/')}
    for path in files:
        if not path.startswith('src/') or not path.endswith('.c'):
            continue
        if path not in changes and not changed_headers:
            continue
        try:
            previous, following = _expanded(files, path), _expanded(current, path)
            if previous == following:
                continue
            if re.search(r'(?m)^\s*#\s*include\s*<', previous + '\n' + following):
                reasons.append(path + ': unexpanded pinned-header macros require raw proof')
                continue
            old_body, old_macros, old_conditions = _view(previous)
            new_body, new_macros, _ = _view(following)
            if any(token in ('{', '}') for macros in (old_macros, new_macros)
                   for definitions in macros.values() for definition in definitions for token in definition):
                # A global macro invocation can generate an entire function,
                # including enclosing conditions that this locator cannot see.
                # Record/initializer macros take the stronger gate too.
                reasons.append(path + ': brace-generating macros require raw proof')
                continue
            if old_body != new_body:
                reasons.append(path + ': function signature/body/directive changed')
                continue
            used = set(re.findall(r'\b[A-Za-z_]\w*\b', repr(old_body))) | old_conditions
            pending = list(used)
            while pending:
                name = pending.pop()
                if old_macros.get(name) != new_macros.get(name):
                    reasons.append(path + ': preprocessing dependency changed: ' + name)
                    break
                for definition in old_macros.get(name, ()):
                    for word in definition:
                        if re.fullmatch(r'[A-Za-z_]\w*', word) and word not in used:
                            used.add(word)
                            pending.append(word)
        except (ValueError, KeyError, UnicodeError) as error:
            reasons.append(path + ': scope check requires raw proof: ' + str(error))
    return reasons
