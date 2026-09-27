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


def checked_fill(owner, manifest, image):
    """Re-derive one LINK_FILL owner; returns its evidence receipt."""
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
