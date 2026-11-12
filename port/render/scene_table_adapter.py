"""Resolve the recovered DOS table indices across separate native globals."""

import re

TRACK_TABLE_LOOKUP = r'''
/* DOS: trklst 186376..189386, scene2 189386..189652,
   scene3 189652..189834; all records have a 14-byte DOS stride.
   Native globals retain those records but have no adjacency contract. */
static struct TRACKOBJECT *port_track_object_at(unsigned index)
{
    if (index < 215u) return &trklst[index];
    if (index < 234u) return &scene2[index - 215u];
    if (index < 247u) return &scene3[index - 234u];
    abortmessage((const I8 *)"Invalid DOS track-object index: %u", index);
    return 0;
}
'''


def adapt_scene_table_indices(source: str) -> str:
    signature = "void drawworld(I8 page, struct RECTANGLE* clip) {"
    if source.count(signature) != 1:
        raise ValueError("Missing unique scene-table consumer boundary")
    start = source.index(signature)
    body = source[start:]
    edits = []
    for match in re.finditer(r"\btrklst\[", body):
        opening = match.end() - 1
        depth = 1
        closing = opening + 1
        while closing < len(body) and depth:
            if body[closing] == "[":
                depth += 1
            elif body[closing] == "]":
                depth -= 1
            closing += 1
        if depth:
            raise ValueError("Unterminated DOS track-object index")
        index = body[opening + 1:closing - 1]
        begin = match.start()
        if begin and body[begin - 1] == "&":
            begin -= 1
            replacement = f"port_track_object_at({index})"
        else:
            replacement = f"(*port_track_object_at({index}))"
        edits.append((begin, closing, replacement))
    if len(edits) != 11:
        raise ValueError(f"Expected 11 recovered track-object lookups, found {len(edits)}")
    for begin, end, replacement in reversed(edits):
        body = body[:begin] + replacement + body[end:]
    return source[:start] + TRACK_TABLE_LOOKUP + body
