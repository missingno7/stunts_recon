"""Link-faithful ASM segment identity and FIXUPP frames (integ29).

The hybrid binders place each contribution at its original address and
resolve every external through reviewed symbols, so two MASM source defects
stayed invisible to them but break a real LINK of the same objects:

* `extrn X:far` declared INSIDE the module's code segment makes MASM frame the
  far pointer on that segment (F0).  LINK then computes the pointer relative
  to the module's own segment frame: correct only when X lies in the same
  original code frame, otherwise a wrong segment:offset or a fixup overflow
  (L2002).  Declared outside every segment, the FIXUPP frames on the external
  (F2) and LINK uses the target's own segment.
* The code segment must be the output segment of the original code frame,
  class CODE: `_TEXT` would merge the module into the runtime `_TEXT`, and a
  non-CODE class moves it behind every CODE segment.  A contribution starting
  at an odd address needs BYTE alignment (a WORD-aligned SEGDEF would be padded).

Segment names are reconstruction choices, not recovered names: one CODE-class
segment per original code frame, spelled like the `S0nn_TEXT` segments of C
objects compiled as S0nn.C.  The frame table below is keyed by the original
frame load address that every ASM recipe already records.
"""
import re
from common import ROOT, read_json, require

CODE_FRAME_SEGMENTS = {
    40384: 'S002_TEXT',     # seg002 (frame 0x09DC)
    125472: 'S012_TEXT',    # seg012 (frame 0x1EA2)
    158448: 'S018_TEXT',    # seg018 (frame 0x26AF)
}


def frame_segment(frame):
    require(frame in CODE_FRAME_SEGMENTS, 'ASM contribution frame has no reviewed output segment')
    return CODE_FRAME_SEGMENTS[frame]


def module_frame(recipe):
    frame = recipe.get('original_frame_load_address')
    if frame is None:
        # Frameless single routines: the reviewed output segment containing
        # the start (the frames are ordered and non-overlapping).
        candidates = [f for f in CODE_FRAME_SEGMENTS if f <= recipe['start'] < f + 65536]
        require(candidates, 'ASM contribution lies outside every reviewed code frame')
        frame = max(candidates)
    return frame


def require_link_faithful_segment(obj, recipe):
    """The ASM code segment is the frame's output segment, class CODE, and
    byte-aligned when the contribution starts at an odd address."""
    frame = module_frame(recipe)
    segment = recipe['object_segment']
    rows = [d for d in obj.segment_defs if d['name'] == segment]
    require(len(rows) == 1 and segment == frame_segment(frame) and rows[0]['class'] == 'CODE',
            'ASM code segment is not the link-faithful output segment of its frame '
            '(expected %s, class CODE)' % frame_segment(frame))
    require(recipe['start'] % 2 == 0 or rows[0]['alignment'] == 'byte',
            'ASM contribution at an odd address needs a BYTE-aligned code segment')
    for d in obj.segment_defs:
        if d['length'] and d['class'] in ('CODE', 'STUNTSC'):
            require(d['name'] == segment, 'ASM module emits code outside its output segment')


def require_link_faithful_frames(obj, recipe):
    """F0 (own code segment) frames on external far pointers and segment
    words are link-faithful only for targets in the module's own frame."""
    segment = recipe['object_segment']
    own = module_frame(recipe)
    symbols = None
    for fix in obj.linker_fixups:
        if not (fix['frame_method'] == 0 and fix['frame_kind'] == 'segment' and
                fix['frame'] == segment and fix['target_kind'] == 'external' and
                fix['loc'] in ('pointer32', 'base16')):
            continue
        if symbols is None:
            symbols = read_json(ROOT/'layout/code-symbols.json')['symbols']
        target = symbols.get(fix['target'])
        require(isinstance(target, dict) and target.get('frame_load_address') == own,
                'External %s is framed on the module code segment but lies in another code '
                'frame: declare `extrn %s:far` outside every segment' % (fix['target'], fix['target']))


def check_asm_object(obj, recipe):
    require_link_faithful_segment(obj, recipe)
    require_link_faithful_frames(obj, recipe)


_SEGMENT = re.compile(r'^\s*([A-Za-z_$@?][\w$@?]*)\s+segment\b(.*)$', re.I)
_ENDS = re.compile(r'^\s*([A-Za-z_$@?][\w$@?]*)\s+ends\b', re.I)
_EXTRN = re.compile(r'^\s*extrn\s+(.*)$', re.I)


def in_segment_far_externs(text):
    """Source lint: (line, segment, name) for every `extrn NAME:far` declared
    inside a code-class segment (MASM frames such a FIXUPP on that segment)."""
    found, stack = [], []
    for number, line in enumerate(text.replace('\r\n', '\n').split('\n'), 1):
        code = line.split(';', 1)[0]
        m = _SEGMENT.match(code)
        if m:
            klass = re.search(r"'([^']*)'", m.group(2))
            stack.append((m.group(1).upper(), klass.group(1).upper() if klass else ''))
            continue
        m = _ENDS.match(code)
        if m and stack and stack[-1][0] == m.group(1).upper():
            stack.pop()
            continue
        m = _EXTRN.match(code)
        if m and stack and stack[-1][1] in ('CODE', 'STUNTSC'):
            for item in m.group(1).split(','):
                name, _, kind = item.partition(':')
                if kind.strip().lower() == 'far':
                    found.append((number, stack[-1][0], name.strip()))
    return found


def lint_source(text, recipe):
    """In-segment far externs whose target lies outside the module frame."""
    own = module_frame(recipe)
    symbols = read_json(ROOT/'layout/code-symbols.json')['symbols']
    return [(line, segment, name) for line, segment, name in in_segment_far_externs(text)
            if symbols.get(name, {}).get('frame_load_address') != own]


def main():
    import json
    manifest = read_json(ROOT/'layout/manifest.json')
    report = []
    for owner in manifest['owners']:
        if owner['kind'] != 'MATCHING_ASM':
            continue
        recipe = read_json(ROOT/owner['recipe'])
        text = (ROOT/recipe['source']).read_bytes().decode('latin1')
        for line, segment, name in lint_source(text, recipe):
            report.append({'owner': owner['name'], 'source': recipe['source'], 'line': line,
                           'segment': segment, 'extern': name})
    print(json.dumps({'cross_frame_in_segment_far_externs': report}, indent=1))
    return 1 if report else 0


if __name__ == '__main__':
    raise SystemExit(main())
