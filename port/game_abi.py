"""Port-only adapters for original DOS call and storage conventions."""
import re


def adapt_renderer_word_arithmetic(source: str) -> str:
    """Keep the renderer's signed comparison after SHL CX,1 at 86103/86126."""
    definition = re.search(r"(?:unsigned|U16) trans_op\([^\n]*\)\s*\{", source)
    if definition is None:
        raise ValueError("Could not locate the renderer definition")
    start = definition.start()
    end = source.index("\n}", start) + 2
    body = source[start:end]
    if body.count("ts->unk * 2") != 2:
        raise ValueError("Expected two renderer word-doubling anchors")
    body = body.replace("ts->unk * 2", "port_renderer_double_word(ts->unk)")
    helper = '''/* PORT_BUILD: locked SHL CX,1 before signed culling comparisons. */
static int16_t port_renderer_double_word(int16_t value)
{
    uint16_t bits = (uint16_t)((uint16_t)value * 2u);
    return (int16_t)(bits < 0x8000u ? (int32_t)bits : (int32_t)bits - 0x10000);
}

'''
    return source[:start] + helper + body + source[end:]


def adapt_preview_word_arithmetic(source: str) -> str:
    """Retain the preview's SUB AX / SAR AX coordinate calculations."""
    definition = re.search(r"void draw_track_preview\(void\)\s*\{", source)
    if definition is None:
        raise ValueError("Could not locate the preview definition")
    start = definition.start()
    end = source.index("\n}", start) + 2
    body = source[start:end]
    pattern = (r"\(([^\n;]+?) - (camera_pos_[xyz])\) >> 1"
               r"|\(-camera_pos_y\) >> 1")

    def replace(match):
        coordinate = match.group(1) if match.group(1) else "0"
        camera = match.group(2) if match.group(2) else "camera_pos_y"
        return f"port_preview_coord_half({coordinate}, {camera})"

    body, count = re.subn(pattern, replace, body)
    if count != 9:
        raise ValueError(f"Expected nine preview word-delta anchors, found {count}")
    helper = '''/* PORT_BUILD: locked preview SUB AX followed by SAR AX,1. */
static int16_t port_preview_coord_half(int16_t coordinate, int16_t camera)
{
    uint16_t bits = (uint16_t)((uint16_t)coordinate - (uint16_t)camera);
    int32_t delta = bits < 0x8000u ? (int32_t)bits : (int32_t)bits - 0x10000;
    if (delta >= 0)
        return (int16_t)(delta / 2);
    return (int16_t)(-((-delta + 1) / 2));
}

'''
    return source[:start] + helper + body + source[end:]


def route_audio_vectors(source: str) -> str:
    """Replace entire casted far-call expressions with typed host entries.

    A DOS driver asset is data on the host. Parse balanced parentheses so
    multi-line function-pointer signatures cannot leave a raw executable call.
    """
    stack = []
    pairs = {}
    for pos, ch in enumerate(source):
        if ch == "(":
            stack.append(pos)
        elif ch == ")" and stack:
            pairs[pos] = stack.pop()
    edits = []
    for match in re.finditer(r"\baudiodriverbinary\b(?:\s*\+\s*(0x[0-9a-fA-F]+|[0-9]+))?", source):
        end = match.end()
        while end < len(source) and (source[end].isspace() or source[end] == ")"):
            end += 1
        if end == len(source) or source[end] != "(":
            continue
        close = end - 1
        while source[close].isspace():
            close -= 1
        start = pairs.get(close)
        if start is None or start > match.start():
            continue
        expression = source[start:end].strip()
        if not expression.startswith("(("):
            continue
        offset = int(match.group(1), 0) if match.group(1) else 0
        if offset not in {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 48, 57, 63, 66}:
            raise ValueError(f"Unresolved DOS audio vector {offset:#x}")
        edits.append((start, end, f"port_audio_driver_{offset:02x}"))
    for start, end, replacement in reversed(edits):
        source = source[:start] + replacement + source[end:]
    return source


def adapt_aggregate_views(source: str) -> str:
    """Make packed audio records agree while retaining native host pointers."""
    def adapt(match):
        body = match.group(2)
        name = match.group(1)
        if name == "AUDIOVOICE":
            body = body.replace("I32 unk10;", "I8 FAR *unk10;")
            body = body.replace("I16 unk2A;", "struct AUDIOCHUNK *unk2A;")
        elif name == "AUDIOCHUNK":
            body = body.replace("I32 unk1E;", "I8 FAR *unk1E;")
            body = body.replace("I32 unk48;", "void (FAR *unk48)(I16);")
        return "#pragma pack(push, 1)\nstruct " + name + " {" + body + "};\n#pragma pack(pop)"
    source = re.sub(r"#pragma pack\(push, [12]\)\s*struct (AUDIOCHUNK|AUDIOVOICE|AudioChunk|AudioVoice) \{(.*?)\};\s*#pragma pack\(pop\)", adapt, source, flags=re.S)
    # scene2/scene3 use this producer view but the renderer consumes them as
    # TRACKOBJECT. Its first word is the near info pointer (zero in these
    # tables), so widening the consumer alone changes array stride and reads
    # the middle of a neighbouring shape pointer.
    source = source.replace("unsigned short opaque_first_word;", "void *opaque_first_word;")
    source = re.sub(r"(?m)(?<!#pragma pack\(push, 2\)\n)^struct scene_shape \{(.*?)\};",
                    r"#pragma pack(push, 2)\nstruct scene_shape {\1};\n#pragma pack(pop)",
                    source, flags=re.S)
    # The track-info link overlays two opponent bytes with a DOS near offset.
    # Widening that union member shifts this consumer past the producer's
    # 16-byte native records. Resolve the near offset at its use boundary.
    source = source.replace(
        "union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;",
        "union { U16S dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;")
    return source


def adapt_polygon_storage(source: str) -> str:
    """Preserve the DOS list-head word following its 400 link words."""
    old = "static I16 poly_link_list[POLYINFO_CAPACITY];"
    marker = "static I16 polyinfo_reset_marker;"
    if source.count(old) != 1 or source.count(marker) != 1:
        raise ValueError("Could not locate polygon list sentinel storage")
    return source.replace(old, "static I16 poly_link_list[POLYINFO_CAPACITY + 1];").replace(
        marker, "#define polyinfo_reset_marker poly_link_list[POLYINFO_CAPACITY]")


def host_view_contracts(source: str) -> str:
    """Compile the established i686 shared-layout contracts in their owners.

    These sizes describe translated native pointers, not DOS packed records.
    Consumers and producers must agree before any gameplay code can link.
    """
    unit = source.rsplit("/", 1)[-1]
    views = []
    if unit in {"obj_seg000.c", "obj_seg001_complete.c", "obj_seg003.c",
                "obj_seg004.c", "obj_seg005.c", "obj_seg006.c", "obj_seg009.c"}:
        vertex_field = "verts" if unit == "obj_seg006.c" else "shape3d_verts"
        views.append(("SHAPE3D", 22, {vertex_field: 2}))
    if unit in {"obj_seg000.c", "obj_seg001_complete.c", "obj_seg003.c", "obj_seg006.c"}:
        views.extend((("MATRIX", 18, {}),
                      ("TRANSFORMEDSHAPE3D", 24,
                       {"shapeptr": 6, "rectptr": 10, "rotvec": 14,
                        "ts_flags": 22, "material": 23})))
    if unit == "seg024_matrot.c":
        views.append(("MATRIX", 18, {"m._21": 2, "m._12": 6, "m._13": 12}))
    if unit in {"obj_seg000.c", "obj_seg001_complete.c", "obj_seg003.c",
                "obj_seg004.c", "obj_seg009.c"}:
        shape_field = "shape" if unit == "obj_seg004.c" else "ss_shapePtr"
        views.append(("TRACKOBJECT", 20, {shape_field: 6}))
    if unit == "track_constants_module.c":
        views.extend((("SHAPE3D", 22, {}),
                      ("scene_shape", 20, {"shape": 6}),
                      ("track_object", 20, {"shape": 6}),
                      ("track_object_info", 16, {"camera_data": 8, "opponent1": 12})))
    if unit == "obj_seg001_complete.c":
        views.append(("TRKOBJINFO", 16, {"si_cameraDataOffset": 8, "link": 12}))
    if unit == "obj_seg027.c":
        views.extend((("AUDIOCHUNK", 76, {"unk1E": 30, "unk48": 72}),
                      ("AUDIOVOICE", 48, {"unk10": 16, "unk2A": 42, "unk2C": 46})))
    if unit == "obj_seg028.c":
        views.extend((("AudioChunk", 76, {"data": 30, "callback": 72}),
                      ("AudioVoice", 48, {"data": 16, "resource": 42, "channelNumber": 46})))
    if unit == "obj_seg007.c":
        views.extend((("AudioPayload", 48, {"shape": 8}),
                      ("AudioTimer", 76, {"payload": 28})))
    if unit in {"obj_seg000.c", "obj_seg001_complete.c", "obj_seg003.c", "obj_seg005.c"}:
        views.extend((("GAMESTATE", 1120, {"game_frame": 320, "game_inputmode": 1013}),
                      ("CARSTATE", 208, {}), ("SIMD", 776, {}), ("GAMEINFO", 26, {})))
    lines = ["\n/* PORT_BUILD: independently grounded scalar and shared-view contracts. */",
             "#include <stddef.h>",
             '_Static_assert(sizeof(void *) == 4, "SDL host requires i686 pointers");',
             '_Static_assert(sizeof(I16) == 2 && sizeof(U16) == 2, "DOS word width");',
             '_Static_assert(sizeof(I32) == 4 && sizeof(U32) == 4, "DOS dword width");']
    for name, size, fields in views:
        lines.append(f'_Static_assert(sizeof(struct {name}) == {size}, "{name} native stride");')
        for field, offset in fields.items():
            lines.append(f'_Static_assert(offsetof(struct {name}, {field}) == {offset}, '
                         f'"{name}.{field} native offset");')
    return "\n".join(lines) + "\n"
