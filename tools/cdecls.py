"""Small MSC 5.x C declaration reader (file scope + block-scope `extern`).

Reads the declarations a translation unit makes about globals and functions:
typedefs, struct/union layouts (MSC /Zp default 2, `#pragma pack(n)`), scalar
kinds, near/far pointers (MSC `far` binds to the following `*` or function
declarator), arrays, prototypes (parameter list, return type, distance).

Not a compiler: expressions in array bounds are evaluated for simple integer
arithmetic and `#define` constants only; anything unknown becomes None.
Function bodies are skipped except for block-scope `extern` declarations.
"""
import re

SCALARS = {'char': 1, 'short': 2, 'int': 2, 'long': 4, 'float': 4, 'double': 8, 'void': 0}
MEMQ = {'far': 'far', '_far': 'far', '__far': 'far', 'near': 'near', '_near': 'near',
        '__near': 'near', 'huge': 'far', '_huge': 'far', '__huge': 'far'}
IGNORED = {'const', 'volatile', 'cdecl', '_cdecl', '__cdecl', 'pascal', '_pascal', 'fortran',
           'interrupt', '_interrupt', '_loadds', '_saveregs', '_export', 'register', 'auto'}
STORAGE = {'extern', 'static', 'typedef'}
TOKEN = re.compile(r'\s+|0[xX][0-9a-fA-F]+[uUlL]*|\d+[uUlL]*|[A-Za-z_]\w*|"(?:\\.|[^"\\])*"|'
                   r"'(?:\\.|[^'\\])*'|->|\.\.\.|<<|>>|[-+*/%&|^~!<>=(){}\[\];,.?:]")


def strip_comments(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*', lambda m: ' ' * 0 + ('\n' * m.group(0).count('\n')), text, flags=re.S)


class Type(dict):
    """k: base|ptr|arr|fn|struct.  Hashable enough for reporting via describe()."""


def base(name, unsigned=None):
    return Type(k='base', name=name, unsigned=unsigned)


def describe(t):
    if t is None:
        return '?'
    k = t['k']
    if k == 'base':
        u = t.get('unsigned')
        return ('unsigned ' if u else 'signed ' if u is False and t['name'] == 'char' else '') + t['name']
    if k == 'struct':
        return f"{t.get('kw', 'struct')} {t['tag']}"
    if k == 'ptr':
        return f"{describe(t['to'])} {t['dist']}*"
    if k == 'arr':
        return f"{describe(t['of'])}[{'' if t['n'] is None else t['n']}]"
    if k == 'fn':
        ps = ', '.join(describe(p) for p in t['params']) if t['params'] else ('' if t['params'] is None else 'void')
        if t.get('varargs'):
            ps += ', ...'
        return f"{describe(t['ret'])} {t['dist']} ({ps})"
    return '?'


class Unit:
    """Declarations of one TU: names -> list of (type, storage, scope, line)."""

    def __init__(self, text, path=''):
        self.path = path
        self.defines = {}
        self.typedefs = {}
        self.structs = {}
        self.decls = {}      # name -> list of dict(type, storage, scope, line, defined)
        self.pack = 2
        self._lines = []
        self._parse(text)

    # -- tokens -------------------------------------------------------------------
    def _tokenize(self, text):
        toks = []
        line = 1
        pack_stack = []
        for raw in strip_comments(text).split('\n'):
            s = raw.strip()
            if s.startswith('#'):
                m = re.match(r'#\s*define\s+([A-Za-z_]\w*)\s+(.*)$', s)
                if m and '(' not in m.group(1):
                    self.defines[m.group(1)] = m.group(2).strip()
                m = re.match(r'#\s*pragma\s+pack\s*\(\s*(\d*)\s*\)', s)
                if m:
                    toks.append(('#pack', int(m.group(1)) if m.group(1) else 2, line))
                line += 1
                continue
            for m in TOKEN.finditer(raw):
                tok = m.group(0)
                if not tok.isspace():
                    toks.append((tok, None, line))
            line += 1
        return toks

    def _eval(self, toks):
        expr = ' '.join(toks)
        for _ in range(5):
            expr2 = re.sub(r'[A-Za-z_]\w*', lambda m: self.defines.get(m.group(0), m.group(0)), expr)
            if expr2 == expr:
                break
            expr = expr2
        expr = re.sub(r'(\d)[uUlL]+', r'\1', expr)
        if re.search(r'[A-Za-z_]', expr.replace('0x', '').replace('0X', '')):
            # sizeof(...) or unknown identifiers
            return None
        try:
            return int(eval(expr.replace('/', '//'), {'__builtins__': {}}))
        except Exception:
            return None

    # -- sizes --------------------------------------------------------------------
    def size(self, t):
        if t is None:
            return None
        k = t['k']
        if k == 'base':
            return SCALARS.get(t['name'], 2)
        if k == 'ptr':
            return 4 if t['dist'] == 'far' else 2
        if k == 'arr':
            s = self.size(t['of'])
            return None if s is None or t['n'] is None else s * t['n']
        if k == 'struct':
            st = self.structs.get((t.get('kw', 'struct'), t['tag']))
            return st['size'] if st else None
        return None

    def align(self, t, pack):
        k = t['k']
        if k == 'arr':
            return self.align(t['of'], pack)
        if k == 'struct':
            st = self.structs.get((t.get('kw', 'struct'), t['tag']))
            return st['align'] if st else 1
        s = self.size(t) or 1
        return max(1, min(s, pack, 2 if s >= 2 else 1))

    # -- parse --------------------------------------------------------------------
    def _parse(self, text):
        self.toks = self._tokenize(text)
        self.i = 0
        while self.i < len(self.toks):
            self._external()

    def peek(self, d=0):
        j = self.i + d
        return self.toks[j][0] if j < len(self.toks) else None

    def line(self):
        return self.toks[min(self.i, len(self.toks) - 1)][2] if self.toks else 0

    def take(self):
        tok = self.toks[self.i][0]
        self.i += 1
        return tok

    def _skip_balanced(self, open_, close):
        depth = 0
        start = self.i
        while self.i < len(self.toks):
            t = self.take()
            if t == open_:
                depth += 1
            elif t == close:
                depth -= 1
                if depth == 0:
                    return start, self.i
        return start, self.i

    def _external(self, scope='file'):
        if self.peek() == '#pack':
            self.pack = self.toks[self.i][1]
            self.i += 1
            return
        if self.peek() == ';':
            self.i += 1
            return
        line = self.line()
        spec = self._specifiers()
        if spec is None:
            self.i += 1
            return
        if self.peek() == ';':
            self.i += 1
            return
        while True:
            try:
                name, typ = self._declarator(spec['type'], spec['pending'])
            except (IndexError, ValueError):
                self._recover()
                return
            if name is None:
                self._recover()
                return
            defined = False
            if self.peek() == '=':
                defined = True
                self.i += 1
                self._skip_initializer()
            if spec['storage'] == 'typedef':
                self.typedefs[name] = typ
            else:
                if typ['k'] == 'fn' and self.peek() == '{':
                    self._record(name, typ, spec['storage'], scope, line, True)
                    self._function_body()
                    return
                if typ['k'] == 'fn' and self.peek() not in (',', ';'):
                    # K&R definition: parameter declarations follow
                    while self.peek() not in ('{', None):
                        self.i += 1
                    self._record(name, typ, spec['storage'], scope, line, True)
                    self._function_body()
                    return
                is_def = typ['k'] != 'fn' and spec['storage'] not in ('extern',)
                self._record(name, typ, spec['storage'], scope, line, defined or is_def)
            if self.peek() == ',':
                self.i += 1
                continue
            if self.peek() == ';':
                self.i += 1
            return

    def _recover(self):
        while self.i < len(self.toks) and self.peek() not in (';', '{'):
            self.i += 1
        if self.peek() == '{':
            self._skip_balanced('{', '}')
        elif self.peek() == ';':
            self.i += 1

    def _skip_initializer(self):
        depth = 0
        while self.i < len(self.toks):
            t = self.peek()
            if t in ('{', '('):
                depth += 1
            elif t in ('}', ')'):
                depth -= 1
            elif depth == 0 and t in (',', ';'):
                return
            self.i += 1

    def _function_body(self):
        """Skip a body; pick up block-scope `extern` declarations (any depth)."""
        depth = 0
        while self.i < len(self.toks):
            t = self.peek()
            if t == '{':
                depth += 1
                self.i += 1
            elif t == '}':
                depth -= 1
                self.i += 1
                if depth == 0:
                    return
            elif t == 'extern' and self.toks[self.i - 1][0] in ('{', '}', ';'):
                self._external(scope='block')
            else:
                self.i += 1

    def _record(self, name, typ, storage, scope, line, defined):
        self.decls.setdefault(name, []).append(
            {'type': typ, 'storage': storage, 'scope': scope, 'line': line, 'defined': defined})

    def _specifiers(self):
        storage = None
        unsigned = None
        names = []
        typ = None
        pending = None
        seen = False
        while True:
            t = self.peek()
            if t is None:
                break
            if t in STORAGE:
                storage = t
            elif t in IGNORED:
                pass
            elif t in MEMQ:
                pending = MEMQ[t]
            elif t in ('unsigned', 'signed'):
                unsigned = t == 'unsigned'
                seen = True
            elif t in SCALARS:
                names.append(t)
                seen = True
            elif t in ('struct', 'union', 'enum'):
                typ = self._struct()
                seen = True
                continue
            elif t in self.typedefs and not seen and typ is None:
                typ = self.typedefs[t]
                seen = True
            else:
                break
            self.i += 1
        if typ is None:
            if not seen:
                if storage or pending:
                    names = ['int']    # implicit int
                else:
                    return None
            if 'long' in names:
                bname = 'double' if 'double' in names else 'long'
            elif 'char' in names:
                bname = 'char'
            elif 'short' in names:
                bname = 'short'
            elif names:
                bname = names[0]
            else:
                bname = 'int'
            typ = base(bname, unsigned)
        return {'storage': storage, 'type': typ, 'pending': pending}

    def _struct(self):
        kw = self.take()
        tag = None
        if re.fullmatch(r'[A-Za-z_]\w*', self.peek() or ''):
            tag = self.take()
        if kw == 'enum':
            if self.peek() == '{':
                self._skip_balanced('{', '}')
            return base('int', False) if True else None
        if tag is None:
            tag = f'__anon{self.i}'
        if self.peek() == '{':
            self.i += 1
            fields = []
            pack = self.pack
            while self.peek() not in ('}', None):
                spec = self._specifiers()
                if spec is None:
                    self.i += 1
                    continue
                while True:
                    name, typ = self._declarator(spec['type'], spec['pending'])
                    if self.peek() == ':':    # bit field
                        self.i += 2
                    fields.append((name, typ))
                    if self.peek() == ',':
                        self.i += 1
                        continue
                    break
                if self.peek() == ';':
                    self.i += 1
            self.i += 1
            off, align, layout = 0, 1, []
            for name, typ in fields:
                s = self.size(typ) or 0
                a = self.align(typ, pack)
                if kw == 'union':
                    layout.append((name, 0, typ))
                    off = max(off, s)
                else:
                    off = (off + a - 1) // a * a
                    layout.append((name, off, typ))
                    off += s
                align = max(align, a)
            size = (off + align - 1) // align * align
            self.structs[(kw, tag)] = {'fields': layout, 'size': size, 'align': align}
        return Type(k='struct', kw=kw, tag=tag)

    def _declarator(self, btype, pending=None, abstract=False):
        ptrs = []
        while True:
            t = self.peek()
            if t == '*':
                ptrs.append(pending or 'near')
                pending = None
                self.i += 1
            elif t in MEMQ:
                pending = MEMQ[t]
                self.i += 1
            elif t in IGNORED:
                self.i += 1
            else:
                break
        typ = btype
        for dist in ptrs:
            typ = Type(k='ptr', dist=dist, to=typ)
        name = None
        inner = None
        t = self.peek()
        if t == '(' and self._is_inner_declarator():
            self.i += 1
            inner_start = self.i
            depth = 1
            while depth:
                x = self.take()
                depth += x == '('
                depth -= x == ')'
            inner = (inner_start, self.i - 1)
        elif t is not None and re.fullmatch(r'[A-Za-z_]\w*', t) and t not in SCALARS:
            name = self.take()
        suffixes = []
        while self.peek() in ('[', '('):
            if self.peek() == '[':
                s, e = self._skip_balanced('[', ']')
                n = self._eval([x[0] for x in self.toks[s + 1:e - 1]]) if e - s > 2 else None
                suffixes.append(('arr', n))
            else:
                suffixes.append(('fn',) + self._params())
        for suf in reversed(suffixes):
            if suf[0] == 'arr':
                typ = Type(k='arr', n=suf[1], of=typ)
            else:
                typ = Type(k='fn', params=suf[1], varargs=suf[2], ret=typ,
                           dist=pending or 'far', pnames=suf[3])
                pending = None
        if inner is not None:
            save = self.i
            self.i = inner[0]
            end = inner[1]
            sub_toks = self.toks
            self.toks = sub_toks[:end]
            try:
                name, typ = self._declarator(typ, pending)
            finally:
                self.toks = sub_toks
                self.i = save
        return name, typ

    def _is_inner_declarator(self):
        nxt = self.peek(1)
        return nxt in ('*',) or nxt in MEMQ or nxt in IGNORED or (
            nxt is not None and re.fullmatch(r'[A-Za-z_]\w*', nxt) and nxt not in SCALARS
            and nxt not in self.typedefs and nxt not in ('struct', 'union', 'enum', 'unsigned', 'signed')
            and self.peek(2) == ')' and self.peek(3) in ('(', '['))

    def _params(self):
        self.i += 1  # (
        params, names, varargs = [], [], False
        if self.peek() == ')':
            self.i += 1
            return None, False, []           # unprototyped ()
        while self.peek() not in (')', None):
            if self.peek() == '...':
                varargs = True
                self.i += 1
            else:
                spec = self._specifiers()
                if spec is None:
                    # K&R identifier list
                    names.append(self.take())
                    params.append(None)
                else:
                    name, typ = self._declarator(spec['type'], spec['pending'])
                    if typ['k'] == 'arr':
                        typ = Type(k='ptr', dist='near', to=typ['of'])
                    params.append(typ)
                    names.append(name)
            if self.peek() == ',':
                self.i += 1
        self.i += 1
        if len(params) == 1 and params[0] is not None and params[0]['k'] == 'base' and \
                params[0]['name'] == 'void' and names[0] is None:
            return [], varargs, []
        if params and all(p is None for p in params):
            return None, varargs, names
        return params, varargs, names

    # -- queries ------------------------------------------------------------------
    def leaf_at(self, t, off):
        """Scalar leaf starting at byte offset off of type t -> (leaf type, path) or (None, why)."""
        path = []
        for _ in range(12):
            k = t['k']
            if k in ('base', 'ptr'):
                return (t, path) if off == 0 else (None, 'inside scalar')
            if k == 'arr':
                es = self.size(t['of'])
                if not es:
                    return None, 'unknown element size'
                if t['n'] is not None and off >= es * t['n']:
                    return None, 'beyond array'
                path.append(f'[{off // es}]')
                off %= es
                t = t['of']
                continue
            if k == 'struct':
                st = self.structs.get((t.get('kw', 'struct'), t['tag']))
                if not st:
                    return None, 'unknown struct'
                best = None
                for name, foff, ftyp in st['fields']:
                    fs = self.size(ftyp) or 0
                    if foff <= off < foff + max(fs, 1):
                        best = (name, foff, ftyp)
                        if t.get('kw') != 'union':
                            break
                if not best:
                    return None, 'struct padding'
                path.append('.' + str(best[0]))
                off -= best[1]
                t = best[2]
                continue
            return None, 'function'
        return None, 'too deep'


def scalar_kind(unit, t):
    """(width, signedness, pointer) of a scalar type; signedness True/False/None."""
    if t is None:
        return None
    if t['k'] == 'ptr':
        return {'width': 4 if t['dist'] == 'far' else 2, 'signed': None, 'ptr': t['dist']}
    if t['k'] == 'base':
        n = t['name']
        w = SCALARS.get(n, 2)
        if n in ('float', 'double'):
            return {'width': w, 'signed': None, 'ptr': None, 'float': True}
        signed = not t.get('unsigned')      # MSC default char is signed
        return {'width': w, 'signed': signed, 'ptr': None}
    return None


if __name__ == '__main__':
    import sys
    u = Unit(open(sys.argv[1], encoding='latin-1').read(), sys.argv[1])
    for name, ds in sorted(u.decls.items()):
        for d in ds:
            print(f"{name:32} {d['scope']:5} {str(d['storage']):7} def={int(d['defined'])} L{d['line']:<5} "
                  f"{describe(d['type'])}  size={u.size(d['type'])}")
