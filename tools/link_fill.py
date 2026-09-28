"""LINK alignment fill ownership (RULING Opus for integ28).

Zero bytes between an object's last byte and the paragraph-aligned start of
the next segment are written by LINK when it aligns that segment, not by any
object.  A LINK_FILL owner holds exactly such a gap.  It is re-derived on every
build from the oracle and the neighbouring owners, never from a stored claim:

* 1..15 zero bytes, ending at the first paragraph boundary after the object;
* the preceding owner is a complete accepted object contribution ending
  exactly at the fill start (named by the fill row);
* the following segment starts exactly at the fill end and requires paragraph
  alignment: either an accepted contribution whose own emitted SEGDEF for its
  contributed segment has paragraph (or page) alignment, or an instruction-
  verified code entry whose independently grounded segment paragraph starts
  there.

The fill confers no source, code or data ownership and is counted separately.
"""
from common import ROOT, read_json, require

BASIS = 'link-paragraph-alignment-v1'
KEYS = {'id', 'kind', 'start', 'end', 'basis', 'object', 'next_segment'}


def fill_row(object_id, start, end):
    return {'id': f'fill_{start:05x}_{end:05x}', 'kind': 'LINK_FILL', 'start': start, 'end': end,
            'basis': BASIS, 'object': object_id}


def _next_segment(following, end, image):
    if following.get('kind') in ('MATCHING_C', 'MATCHING_ASM', 'MATCHING_C_DATA', 'MATCHING_ASM_DATA') \
            and 'recipe' in following and following['start'] == end:
        recipe = read_json(ROOT/following['recipe'])
        segment = recipe.get('object_segment')
        rows = [s for s in recipe.get('object_declarations', {}).get('segments', [])
                if s.get('name') == segment]
        if len(rows) == 1 and rows[0].get('alignment') in ('paragraph', 'page') and \
                recipe.get('start') == end:
            return {'kind': 'accepted-segdef-alignment', 'owner': following['id'],
                    'segment': segment, 'alignment': rows[0]['alignment']}
    from function_evidence import current_inventory
    rows = [f for f in current_inventory(image)['functions']
            if f.get('start') == end and f.get('segment_paragraph') is not None and
            f['segment_paragraph']*16 == end and
            f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    if len(rows) == 1:
        return {'kind': 'verified-code-segment-start', 'entry': rows[0]['name'],
                'segment_paragraph': rows[0]['segment_paragraph']}
    return None


# integ33: DGROUP word-alignment fill.  LINK concatenates the public DGROUP
# segments contribution by contribution; a WORD-aligned contribution that
# follows an odd-length one starts at the next even address and LINK writes
# nothing into the skipped byte.  One such byte is owned as LINK_FILL, again
# re-derived on every build and never from a stored claim:
#
# * exactly one zero byte at an odd DGROUP address below _edata;
# * the preceding owner is a complete accepted DGROUP contribution (its object's
#   whole emitted segment) ending exactly there, i.e. at an odd end;
# * the following owner is an accepted DGROUP contribution starting at the next
#   (even) address whose own emitted SEGDEF for that segment is WORD aligned (or
#   stricter).
WORD_BASIS = 'link-dgroup-word-alignment-v1'
# integ36: owned pinned runtime data (runtime_binding.RUNTIME_DATA) is an
# accepted DGROUP contribution too; its SEGDEF is the hash-pinned member's own.
_DATA_KINDS = ('MATCHING_C_DATA', 'MATCHING_ASM_DATA', 'KNOWN_TOOLCHAIN_LIBRARY_DATA')


def word_fill_row(object_id, start):
    return {'id': f'fill_{start:05x}_{start + 1:05x}', 'kind': 'LINK_FILL', 'start': start,
            'end': start + 1, 'basis': WORD_BASIS, 'object': object_id}


def _owner_recipe(row, manifest):
    """The recipe and emitted segment name of an accepted DGROUP owner row."""
    if row.get('parent'):
        parents = [o for o in manifest['owners'] if o['id'] == row['parent'] and o.get('recipe')]
        require(len(parents) == 1, 'DGROUP fill neighbour lacks its object owner')
        return read_json(ROOT/parents[0]['recipe']), row.get('segment')
    require(row.get('recipe') is not None, 'DGROUP fill neighbour lacks a recipe')
    recipe = read_json(ROOT/row['recipe'])
    require(recipe.get('data_only') is True, 'DGROUP fill neighbour is not a data contribution')
    return recipe, recipe.get('object_segment')


def _pinned_member(row):
    from compiler import toolchain_path
    from common import sha
    from omf import OmfReader
    data = toolchain_path(row['library']).read_bytes()
    require(sha(data) == row['library_sha256'], 'Fill neighbour library identity mismatch')
    blobs = [blob for name, blob in OmfReader().split_library(data)
             if name == row['module'] and sha(blob) == row['module_sha256']]
    require(len(blobs) == 1, 'Fill neighbour pinned member missing')
    return OmfReader(communals=bool(row.get('omf_policy', {}).get('communals'))).read(blobs[0])


def _dgroup_segdef(row, manifest):
    if row.get('kind') == 'KNOWN_TOOLCHAIN_LIBRARY_DATA':
        from runtime_binding import runtime_owners
        parents = [o for o in runtime_owners(manifest) if o['id'] == row.get('parent')]
        require(len(parents) == 1, 'DGROUP fill neighbour lacks its pinned runtime member')
        obj = _pinned_member(parents[0])
        segment = row.get('segment')
        rows = [s for s in obj.segment_defs if s['name'] == segment]
        groups = [g for g in obj.groups if g['name'] == 'DGROUP']
        require(len(rows) == 1 and len(groups) == 1 and segment in groups[0]['segments'],
                'DGROUP fill neighbour segment is not a DGROUP SEGDEF')
        return segment, rows[0]
    recipe, segment = _owner_recipe(row, manifest)
    rows = [s for s in recipe.get('object_declarations', {}).get('segments', []) if s.get('name') == segment]
    groups = [g for g in recipe.get('object_declarations', {}).get('groups', []) if g.get('name') == 'DGROUP']
    require(len(rows) == 1 and len(groups) == 1 and segment in groups[0].get('segments', []),
            'DGROUP fill neighbour segment is not a DGROUP SEGDEF')
    return segment, rows[0]


def checked_word_fill(owner, manifest, image):
    start, end = owner['start'], owner['end']
    require(set(owner) <= KEYS and owner.get('kind') == 'LINK_FILL' and owner.get('basis') == WORD_BASIS and
            owner.get('id') == f'fill_{start:05x}_{end:05x}', 'Unsupported DGROUP fill owner form')
    layout = read_json(ROOT/'layout/data-symbols.json')
    require(type(start) is int and end == start + 1 and start % 2 == 1 and
            layout['frame_load_address'] < start and end <= layout['bss_start'] and end <= len(image) and
            image[start] == 0, 'DGROUP fill must be one zero byte at an odd initialized DGROUP address')
    owners = manifest['owners']
    at = next(i for i, o in enumerate(owners) if o is owner or o == owner)
    require(0 < at < len(owners) - 1, 'DGROUP fill lacks neighbouring owners')
    before, after = owners[at - 1], owners[at + 1]
    if after.get('kind') == 'BSS_IN_IMAGE':
        # integ38: the gap before _edata; the follower is the first _BSS
        # contribution (its in-image prefix row follows the fill).
        rows = [o for o in manifest.get('bss_owners', []) if o['id'] == after.get('bss_owner')]
        require(len(rows) == 1 and end == layout['bss_start'] and rows[0]['start'] == end,
                'DGROUP fill before _BSS lacks its first _BSS contribution')
        after = rows[0]
    require(before['end'] == start and before['id'] == owner.get('object') and before['kind'] in _DATA_KINDS,
            'DGROUP fill does not follow the last byte of a complete accepted DGROUP contribution')
    segment, prior = _dgroup_segdef(before, manifest)
    require(prior.get('length') == before['end'] - before['start'],
            'DGROUP fill predecessor is not its complete emitted segment')
    require(after['start'] == end and after['kind'] in _DATA_KINDS,
            'DGROUP fill is not followed by an accepted DGROUP contribution')
    next_segment, segdef = _dgroup_segdef(after, manifest)
    require(segdef.get('alignment') in ('word', 'paragraph', 'page'),
            'DGROUP fill follower is not WORD aligned')
    following = {'kind': 'accepted-segdef-alignment', 'owner': after['id'],
                 'segment': next_segment, 'alignment': segdef['alignment']}
    if 'next_segment' in owner:
        require(owner['next_segment'] == following, 'DGROUP fill following-segment evidence differs')
    return {'fill': [start, end], 'object': before['id'], 'next_segment': following, 'basis': WORD_BASIS}


# integ34: CODE word-alignment fill.  The same LINK rule inside the code
# segments: a WORD-aligned CODE contribution that follows an odd-length one
# (in the same or a following code segment) starts at the next even address,
# and LINK writes nothing into the skipped byte.  One such byte is owned as
# LINK_FILL, re-derived on every build from both neighbours, never from a claim:
#
# * exactly one zero byte at an odd address below the DGROUP frame;
# * the preceding owner is a complete accepted code contribution (C, ASM or a
#   pinned runtime member; not a prefix) ending exactly there, and its object's
#   emitted CODE SEGDEF is exactly that extent (so the byte is not the object's);
# * the following owner is a complete accepted code contribution starting at the
#   next (even) address whose own emitted CODE SEGDEF is exactly its extent and
#   WORD aligned.
CODE_WORD_BASIS = 'link-code-word-alignment-v1'
_CODE_KINDS = ('MATCHING_C', 'MATCHING_ASM', 'KNOWN_TOOLCHAIN_LIBRARY')


def code_word_fill_row(object_id, start):
    return {'id': f'fill_{start:05x}_{start + 1:05x}', 'kind': 'LINK_FILL', 'start': start,
            'end': start + 1, 'basis': CODE_WORD_BASIS, 'object': object_id}


def _code_segdef(row):
    """The emitted CODE SEGDEF of one accepted code owner row (its recipe's
    verified object declarations, or the pinned runtime member itself)."""
    require(row.get('kind') in _CODE_KINDS and row.get('contribution_form') is None,
            'Code fill neighbour is not a complete accepted code contribution')
    if row['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY':
        rows = [s for s in _pinned_member(row).segment_defs if s['name'] == row['segment']]
    else:
        recipe = read_json(ROOT/row['recipe'])
        require((recipe.get('start'), recipe.get('end')) == (row['start'], row['end']) and
                recipe.get('contribution_form') is None and 'prefix_of_object' not in recipe,
                'Code fill neighbour recipe is not its complete object')
        rows = [s for s in recipe.get('object_declarations', {}).get('segments', [])
                if s.get('name') == recipe.get('object_segment')]
    require(len(rows) == 1 and rows[0].get('class') == 'CODE', 'Code fill neighbour lacks one CODE SEGDEF')
    require(rows[0].get('length') == row['end'] - row['start'],
            'Code fill neighbour is not its object\'s complete emitted CODE segment')
    return rows[0]


def checked_code_word_fill(owner, manifest, image):
    start, end = owner['start'], owner['end']
    require(set(owner) <= KEYS and owner.get('kind') == 'LINK_FILL' and owner.get('basis') == CODE_WORD_BASIS and
            owner.get('id') == f'fill_{start:05x}_{end:05x}', 'Unsupported code fill owner form')
    layout = read_json(ROOT/'layout/data-symbols.json')
    require(type(start) is int and end == start + 1 and start % 2 == 1 and
            end < layout['frame_load_address'] and image[start] == 0,
            'Code fill must be one zero byte at an odd code address')
    owners = manifest['owners']
    at = next(i for i, o in enumerate(owners) if o is owner or o == owner)
    require(0 < at < len(owners) - 1, 'Code fill lacks neighbouring owners')
    before, after = owners[at - 1], owners[at + 1]
    require(before['end'] == start and before['id'] == owner.get('object'),
            'Code fill does not follow the last byte of its named code contribution')
    _code_segdef(before)
    require(after['start'] == end, 'Code fill is not followed by a code contribution')
    segdef = _code_segdef(after)
    require(segdef.get('alignment') == 'word', 'Code fill follower is not WORD aligned')
    following = {'kind': 'accepted-segdef-alignment', 'owner': after['id'],
                 'segment': segdef['name'], 'alignment': segdef['alignment']}
    if 'next_segment' in owner:
        require(owner['next_segment'] == following, 'Code fill following-segment evidence differs')
    return {'fill': [start, end], 'object': before['id'], 'next_segment': following, 'basis': CODE_WORD_BASIS}


# integ37: the /DOSSEG `_TEXT` lead.  With /DOSSEG, LINK reserves 16 zero bytes
# at the start of segment `_TEXT` (null-pointer protection for near code).  The
# 16 bytes are LINK's, not an object's; one LINK_FILL row owns them, re-derived
# on every build and never from a stored claim:
#
# * exactly 16 zero bytes [s, s+16) below the DGROUP frame, `s` even;
# * the preceding owner is a complete accepted code contribution of another
#   CODE segment (its object's whole emitted CODE SEGDEF) ending exactly at `s`,
#   so `_TEXT` (WORD aligned) begins at `s`;
# * the following owner is an accepted pinned runtime member whose own `_TEXT`
#   SEGDEF is WORD aligned and starts at s+16;
# * `s` lies in the first paragraph of the grounded runtime `_TEXT` frame (the
#   frame of the original relocated far CALLs to accepted `_TEXT` publics).
# The staged real link (LINK_OPTIONS carries /DOSSEG) re-creates the bytes.
DOSSEG_LEAD_BASIS = 'link-dosseg-text-lead-v1'
DOSSEG_LEAD = 16


def dosseg_lead_row(object_id, start):
    return {'id': f'fill_{start:05x}_{start + DOSSEG_LEAD:05x}', 'kind': 'LINK_FILL', 'start': start,
            'end': start + DOSSEG_LEAD, 'basis': DOSSEG_LEAD_BASIS, 'object': object_id}


def checked_dosseg_lead(owner, manifest, image):
    start, end = owner['start'], owner['end']
    require(set(owner) <= KEYS and owner.get('kind') == 'LINK_FILL' and owner.get('basis') == DOSSEG_LEAD_BASIS and
            owner.get('id') == f'fill_{start:05x}_{end:05x}', 'Unsupported DOSSEG lead owner form')
    layout = read_json(ROOT/'layout/data-symbols.json')
    require(type(start) is int and end == start + DOSSEG_LEAD and start % 2 == 0 and
            end < layout['frame_load_address'] and image[start:end] == bytes(DOSSEG_LEAD),
            'DOSSEG lead must be 16 zero bytes at an even code address')
    owners = manifest['owners']
    at = next(i for i, o in enumerate(owners) if o is owner or o == owner)
    require(0 < at < len(owners) - 1, 'DOSSEG lead lacks neighbouring owners')
    before, after = owners[at - 1], owners[at + 1]
    require(before['end'] == start and before['id'] == owner.get('object'),
            'DOSSEG lead does not follow its named code contribution')
    prior = _code_segdef(before)
    require(prior['name'] != '_TEXT', 'DOSSEG lead predecessor is itself _TEXT')
    require(after['start'] == end and after['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' and after.get('segment') == '_TEXT',
            'DOSSEG lead is not followed by a pinned runtime _TEXT member')
    segdef = _code_segdef(after)
    require(segdef['name'] == '_TEXT' and segdef.get('alignment') == 'word', 'DOSSEG lead follower is not WORD-aligned _TEXT')
    from runtime_binding import runtime_frame
    from mz import MZ
    from oracle import verify
    oracle = verify(write=False)
    frame = runtime_frame(image, MZ.parse(oracle[1]).relocations, manifest)
    require(frame <= start < frame + 16, 'DOSSEG lead is not in the first paragraph of the runtime _TEXT frame')
    following = {'kind': 'accepted-segdef-alignment', 'owner': after['id'], 'segment': '_TEXT', 'alignment': 'word'}
    if 'next_segment' in owner:
        require(owner['next_segment'] == following, 'DOSSEG lead following-segment evidence differs')
    return {'fill': [start, end], 'object': before['id'], 'next_segment': following, 'basis': DOSSEG_LEAD_BASIS,
            'text_frame': frame}


def checked_fill(owner, manifest, image):
    """Re-derive one LINK_FILL owner; returns its evidence receipt."""
    if owner.get('basis') == DOSSEG_LEAD_BASIS:
        return checked_dosseg_lead(owner, manifest, image)
    if owner.get('basis') == WORD_BASIS:
        return checked_word_fill(owner, manifest, image)
    if owner.get('basis') == CODE_WORD_BASIS:
        return checked_code_word_fill(owner, manifest, image)
    start, end = owner['start'], owner['end']
    require(set(owner) <= KEYS and owner.get('kind') == 'LINK_FILL' and owner.get('basis') == BASIS and
            owner.get('id') == f'fill_{start:05x}_{end:05x}',
            'Unsupported LINK fill owner form')
    require(type(start) is int and type(end) is int and 0 < end - start < 16 and
            end % 16 == 0 and end == (start + 15) // 16 * 16 and end <= len(image),
            'LINK fill must reach exactly the next paragraph boundary')
    require(image[start:end] == bytes(end - start), 'LINK fill bytes are not zero')
    owners = manifest['owners']
    at = next(i for i, o in enumerate(owners) if o is owner or o == owner)
    require(0 < at < len(owners) - 1, 'LINK fill lacks neighbouring owners')
    before, after = owners[at - 1], owners[at + 1]
    require(before['end'] == start and before['id'] == owner.get('object') and
            before['kind'] in ('MATCHING_C', 'MATCHING_ASM') and
            before.get('contribution_form') is None,
            'LINK fill does not follow the last byte of a complete accepted object')
    following = _next_segment(after, end, image)
    require(following is not None,
            'LINK fill lacks an independently grounded paragraph-aligned following segment')
    if 'next_segment' in owner:
        require(owner['next_segment'] == following, 'LINK fill following-segment evidence differs')
    return {'fill': [start, end], 'object': before['id'], 'next_segment': following, 'basis': BASIS}
