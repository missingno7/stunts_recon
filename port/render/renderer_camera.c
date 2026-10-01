#include "renderer_camera.h"

#include <math.h>
#include <string.h>

#define PORT_CAMERA_LINK_CAPACITY 400u
#define PORT_CAMERA_CLIP_POINTS (PORT_CAMERA_MAX_POINTS * 2u)

static const uint8_t s_index_counts[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2, 6, 3, 0, 0
};
static const uint8_t s_primitive_types[16] = {
    0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 0
};

static int16_t s16(uint16_t bits)
{
    return bits < 0x8000u ? (int16_t)bits : (int16_t)((int32_t)bits - 0x10000L);
}

static int16_t wrap_add16(int16_t left, int16_t right)
{
    return s16((uint16_t)((uint16_t)left + (uint16_t)right));
}

static int16_t trunc_div16(int32_t numerator, int16_t denominator)
{
    int32_t quotient;
    if (denominator == 0)
        return 0;
    quotient = numerator / denominator;
    return s16((uint16_t)quotient);
}

/* Semantic copy of vector_op_unk's 16-bit SUB/SHR/IMUL/IDIV width behavior. */
static PortCameraVec3i near_intersection(PortCameraVec3i first,
                                         PortCameraVec3i second)
{
    uint16_t numerator = (uint16_t)((uint16_t)PORT_CAMERA_NEAR_Z -
                                    (uint16_t)second.z);
    uint16_t denominator = (uint16_t)((uint16_t)first.z -
                                      (uint16_t)second.z);
    PortCameraVec3i output;
    int16_t signed_numerator;
    int16_t signed_denominator;

    if (s16(denominator) < 0) {
        numerator >>= 1;
        denominator >>= 1;
    }
    signed_numerator = s16(numerator);
    signed_denominator = s16(denominator);
    output.z = PORT_CAMERA_NEAR_Z;
    output.x = wrap_add16(second.x, trunc_div16(
        (int32_t)s16((uint16_t)((uint16_t)first.x - (uint16_t)second.x)) *
        signed_numerator, signed_denominator));
    output.y = wrap_add16(second.y, trunc_div16(
        (int32_t)s16((uint16_t)((uint16_t)first.y - (uint16_t)second.y)) *
        signed_numerator, signed_denominator));
    return output;
}

static PortCameraVec3i vertex_at(const PortCameraShapeCapture *capture,
                                 uint8_t index)
{
    const int16_t *xyz = capture->camera_xyz + (size_t)index * 3u;
    PortCameraVec3i result = {xyz[0], xyz[1], xyz[2]};
    return result;
}

static void command_point(PortCameraCommand *command, uint8_t at,
                          PortCameraVec3i point)
{
    command->xyz[at].x = (double)point.x;
    command->xyz[at].y = (double)point.y;
    command->xyz[at].z = (double)point.z;
}

static int same_point(PortCameraVec3i a, PortCameraVec3i b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static int16_t add_center_reference(uint16_t value, int16_t center)
{
    uint16_t center_word = (uint16_t)center;
    uint16_t sum = (uint16_t)(value + center_word);
    uint16_t same_sign = (uint16_t)~(value ^ center_word);
    uint16_t changed_sign = (uint16_t)(value ^ sum);
    if ((same_sign & changed_sign & 0x8000u) != 0)
        return (int16_t)((int16_t)sum < 0 ? 0x7D00 : 0x8300);
    return s16(sum);
}

static int16_t project_reference_x(const PortCameraProjection *projection,
                                   int16_t coordinate, int16_t depth)
{
    uint16_t magnitude = (uint16_t)coordinate;
    uint32_t product;
    uint16_t low, high, threshold, quotient, scaled;
    if (coordinate < 0)
        magnitude = (uint16_t)(0u - magnitude);
    product = (uint32_t)magnitude * (uint16_t)projection->scale_x;
    low = (uint16_t)product;
    high = (uint16_t)(product >> 16);
    threshold = (uint16_t)(high << 1);
    if ((int16_t)low < 0)
        ++threshold;
    if (depth <= (int16_t)threshold)
        return (int16_t)(coordinate < 0 ? 0x8300 : 0x7D00);
    quotient = (uint16_t)(product / (uint16_t)depth);
    scaled = coordinate < 0 ? (uint16_t)(0u - quotient) : quotient;
    return add_center_reference(scaled, projection->center_x);
}

static int16_t project_reference_y(const PortCameraProjection *projection,
                                   int16_t coordinate, int16_t depth)
{
    uint16_t magnitude = (uint16_t)coordinate;
    uint32_t product;
    uint16_t low, high, threshold, quotient, scaled;
    if (coordinate < 0)
        magnitude = (uint16_t)(0u - magnitude);
    product = (uint32_t)magnitude * (uint16_t)projection->scale_y;
    low = (uint16_t)product;
    high = (uint16_t)(product >> 16);
    threshold = (uint16_t)(high << 1);
    if ((int16_t)low < 0)
        ++threshold;
    if (depth <= (int16_t)threshold)
        return (int16_t)((int16_t)low < 0 ? 0x7D00 : 0x8300);
    quotient = (uint16_t)(product / (uint16_t)depth);
    scaled = coordinate < 0 ? quotient : (uint16_t)(0u - quotient);
    return add_center_reference(scaled, projection->center_y);
}

static PortCameraPoint2i project_reference(
    const PortCameraShapeCapture *capture, PortCameraVec3i point)
{
    PortCameraPoint2i result;
    if (point.z <= 0) {
        result.x = (int16_t)0x8000u;
        result.y = (int16_t)0x8000u;
    } else {
        result.x = project_reference_x(&capture->projection, point.x, point.z);
        result.y = project_reference_y(&capture->projection, point.y, point.z);
    }
    return result;
}

static int same_point2(PortCameraPoint2i a, PortCameraPoint2i b)
{
    return a.x == b.x && a.y == b.y;
}

static int reference_facing(const PortCameraPoint2i *points, uint8_t count)
{
    int32_t dx0, dx1, dy0, dy1;
    uint32_t cross_bits;
    if (count < 3u)
        return 0;
    dx0 = (int32_t)points[0].x - points[1].x;
    dx1 = (int32_t)points[2].x - points[1].x;
    if (dx0 == 0 && dx1 == 0)
        return 0;
    dy0 = (int32_t)points[0].y - points[1].y;
    dy1 = (int32_t)points[2].y - points[1].y;
    if (dy0 == 0 && dy1 == 0)
        return 0;
    /* x86 I32 multiply/subtract wraps before the signed comparison. */
    cross_bits = (uint32_t)dx1 * (uint32_t)dy0 -
                 (uint32_t)dx0 * (uint32_t)dy1;
    return (cross_bits & 0x80000000u) == 0 && cross_bits != 0;
}

static uint8_t reference_clip_polygon(
    const PortCameraShapeCapture *capture, const PortCameraVec3i *input,
    uint8_t input_count, PortCameraPoint2i *output)
{
    PortCameraVec3i previous;
    PortCameraPoint2i previous_projected;
    int previous_inside;
    uint8_t out_count = 0;
    uint8_t i;
    if (input_count == 0)
        return 0;
    previous = input[input_count - 1u];
    previous_inside = previous.z >= PORT_CAMERA_NEAR_Z;
    previous_projected = previous_inside ? project_reference(capture, previous)
                                         : (PortCameraPoint2i){0, 0};
    for (i = 0; i < input_count; ++i) {
        PortCameraVec3i current = input[i];
        int current_inside = current.z >= PORT_CAMERA_NEAR_Z;
        PortCameraPoint2i current_projected;
        if (current_inside) {
            current_projected = project_reference(capture, current);
            if (!previous_inside) {
                PortCameraVec3i crossing = near_intersection(current, previous);
                PortCameraPoint2i crossing_projected = project_reference(capture, crossing);
                if (!same_point2(current_projected, crossing_projected)) {
                    if (out_count >= PORT_CAMERA_CLIP_POINTS)
                        return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
                    output[out_count++] = crossing_projected;
                }
            }
            if (out_count >= PORT_CAMERA_CLIP_POINTS)
                return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
            output[out_count++] = current_projected;
        } else if (previous_inside) {
            PortCameraVec3i crossing = near_intersection(previous, current);
            PortCameraPoint2i crossing_projected = project_reference(capture, crossing);
            if (!same_point2(previous_projected, crossing_projected)) {
                if (out_count >= PORT_CAMERA_CLIP_POINTS)
                    return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
                output[out_count++] = crossing_projected;
            }
        }
        previous = current;
        previous_inside = current_inside;
        if (current_inside)
            previous_projected = current_projected;
    }
    return out_count;
}

static uint8_t point_outcode(PortCameraPoint2i point,
                             const PortCameraClipRect *clip)
{
    uint8_t result;
    if (point.y < clip->top)
        result = 1;
    else if (point.y > clip->bottom)
        result = 2;
    else
        result = 0;
    if (point.x < clip->left)
        result |= 4;
    else if (point.x > clip->right)
        result |= 8;
    return result;
}

static uint8_t reference_rectcode(const PortCameraPoint2i *points,
                                  uint8_t count,
                                  const PortCameraClipRect *clip)
{
    uint8_t result = 0x0F;
    uint8_t i;
    for (i = 0; i < count; ++i)
        result &= point_outcode(points[i], clip);
    return result;
}

static int project_winding(const PortCameraShapeCapture *capture,
                           const PortCameraVec3i *points, uint8_t count)
{
    PortCameraPoint2i projected[PORT_CAMERA_CLIP_POINTS];
    uint8_t i;
    if (count > PORT_CAMERA_CLIP_POINTS)
        return 0;
    for (i = 0; i < count; ++i)
        projected[i] = project_reference(capture, points[i]);
    return reference_facing(projected, count);
}

/* Keep the old integer projection and near-intersection count as reference
   metadata, while the command's XYZ list is clipped without pixel snapping. */
static uint8_t clipped_reference_polygon(
    const PortCameraShapeCapture *capture, const PortCameraVec3i *points,
    uint8_t count, PortCameraPoint2i *reference_points)
{
    return reference_clip_polygon(capture, points, count, reference_points);
}

static uint8_t clip_polygon_near(const PortCameraVec3i *input,
                                 uint8_t input_count,
                                 PortCameraVec3i *output)
{
    PortCameraVec3i previous;
    int previous_inside;
    uint8_t out_count = 0;
    uint8_t i;
    if (input_count == 0)
        return 0;
    previous = input[input_count - 1u];
    previous_inside = previous.z >= PORT_CAMERA_NEAR_Z;
    for (i = 0; i < input_count; ++i) {
        PortCameraVec3i current = input[i];
        int current_inside = current.z >= PORT_CAMERA_NEAR_Z;
        if (current_inside) {
            if (!previous_inside) {
                PortCameraVec3i cross = near_intersection(current, previous);
                if (out_count >= PORT_CAMERA_CLIP_POINTS)
                    return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
                if (out_count == 0 || !same_point(output[out_count - 1u], cross))
                    output[out_count++] = cross;
            }
            if (out_count >= PORT_CAMERA_CLIP_POINTS)
                return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
            if (previous_inside || out_count == 0 ||
                !same_point(output[out_count - 1u], current))
                output[out_count++] = current;
        } else if (previous_inside) {
            PortCameraVec3i cross = near_intersection(previous, current);
            if (out_count >= PORT_CAMERA_CLIP_POINTS)
                return (uint8_t)(PORT_CAMERA_CLIP_POINTS + 1u);
            if (out_count == 0 || !same_point(output[out_count - 1u], cross))
                output[out_count++] = cross;
        }
        previous = current;
        previous_inside = current_inside;
    }
    return out_count;
}

static int32_t sum_depth(const PortCameraVec3i *points, uint8_t count)
{
    int32_t result = 0;
    uint8_t i;
    for (i = 0; i < count; ++i)
        result += points[i].z;
    return result;
}

static int16_t average_depth(int32_t sum, uint8_t count)
{
    if (count == 0)
        return 0;
    return s16((uint16_t)(sum / count));
}

static void insert_in_shape_chain(PortCameraCommand *commands,
                                  uint16_t new_index,
                                  uint8_t object_flags,
                                  int16_t *links,
                                  int16_t *tail,
                                  int16_t *cursor,
                                  uint16_t *inserted_count)
{
    int16_t list_index;
    int16_t insertion_cursor;
    int32_t remaining;
    PortCameraCommand *command = &commands[new_index];
    int forced = (object_flags & 1u) != 0 || (command->primitive_flags & 2u) != 0;

    if (forced) {
        insertion_cursor = *cursor;
        list_index = links[insertion_cursor];
    } else {
        insertion_cursor = PORT_CAMERA_LINK_CAPACITY;
        list_index = links[insertion_cursor];
        remaining = *inserted_count;
        while (list_index >= 0) {
            if (remaining-- == 0)
                break;
            if (commands[list_index].depth < command->depth)
                break;
            insertion_cursor = list_index;
            list_index = links[list_index];
        }
    }

    links[new_index] = list_index;
    links[insertion_cursor] = (int16_t)new_index;
    if (list_index < 0)
        *tail = (int16_t)new_index;
    *cursor = links[insertion_cursor];
    ++*inserted_count;
}

static PortCameraBuildStatus append_unsupported(
    const PortCameraShapeCapture *capture, uint16_t ordinal, uint16_t offset,
    uint8_t code, uint8_t type, PortCameraUnsupported *unsupported,
    size_t capacity, size_t *count)
{
    if (*count >= capacity || unsupported == NULL)
        return PORT_CAMERA_BUILD_DIAGNOSTIC_CAPACITY;
    unsupported[*count].shape_identity = capture->shape_identity;
    unsupported[*count].primitive_ordinal = ordinal;
    unsupported[*count].primitive_offset = offset;
    unsupported[*count].source_code = code;
    unsupported[*count].recovered_type = type;
    ++*count;
    return PORT_CAMERA_BUILD_OK;
}

PortCameraBuildStatus port_camera_build_shape(
    const PortCameraShapeCapture *capture,
    PortCameraCommand *commands, size_t command_capacity,
    size_t *command_count_out,
    PortCameraUnsupported *unsupported, size_t unsupported_capacity,
    size_t *unsupported_count_out)
{
    int16_t links[PORT_CAMERA_LINK_CAPACITY + 1u];
    int16_t tail = PORT_CAMERA_LINK_CAPACITY;
    int16_t cursor = PORT_CAMERA_LINK_CAPACITY;
    uint16_t inserted_count = 0;
    uint16_t ordinal = 0;
    uint16_t offset = 0;
    size_t command_count = 0;
    size_t unsupported_count = 0;

    if (command_count_out != NULL) *command_count_out = 0;
    if (unsupported_count_out != NULL) *unsupported_count_out = 0;
    if (capture == NULL || capture->camera_xyz == NULL ||
        capture->primitive_stream == NULL || capture->cull1 == NULL ||
        capture->cull2 == NULL || commands == NULL || command_count_out == NULL ||
        capture->vertex_count > 255u ||
        (capture->accepted_only && capture->reference_accepted == NULL))
        return PORT_CAMERA_BUILD_INVALID;
    if (command_capacity > PORT_CAMERA_LINK_CAPACITY)
        command_capacity = PORT_CAMERA_LINK_CAPACITY;
    if (capture->primitive_count > 512u)
        return PORT_CAMERA_BUILD_BAD_STREAM;
    memset(links, 0xFF, sizeof(links));

    while (ordinal < capture->primitive_count) {
        uint8_t code = capture->primitive_stream[offset];
        uint8_t flags;
        uint8_t vertex_count;
        uint8_t type;
        size_t record_size;
        const uint8_t *paint_bytes;
        const uint8_t *indices;
        PortCameraVec3i source_points[PORT_CAMERA_MAX_POINTS];
        PortCameraVec3i output_points[PORT_CAMERA_CLIP_POINTS];
        PortCameraPoint2i source_reference[PORT_CAMERA_MAX_POINTS];
        PortCameraPoint2i output_reference[PORT_CAMERA_CLIP_POINTS];
        uint8_t output_count = 0;
        uint8_t reference_count = 0;
        uint8_t material;
        uint8_t i;
        int has_front_vertex = 0;
        int has_near_clipped_vertex = 0;
        int reference_accepted = capture->reference_accepted != NULL &&
                                 capture->reference_accepted[ordinal] != 0;
        int added = 0;
        PortCameraCommand command;

        if (code == 0)
            break;
        if (code >= 16u)
            return PORT_CAMERA_BUILD_BAD_STREAM;
        flags = capture->primitive_stream[offset + 1u];
        vertex_count = s_index_counts[code];
        type = s_primitive_types[code];
        record_size = 2u + capture->paint_count + vertex_count;
        paint_bytes = capture->primitive_stream + offset + 2u;
        indices = paint_bytes + capture->paint_count;
        if (record_size > (size_t)(UINT16_MAX - offset))
            return PORT_CAMERA_BUILD_BAD_STREAM;
        if ((size_t)ordinal >= capture->primitive_count)
            return PORT_CAMERA_BUILD_BAD_CULL_COUNT;
        if (capture->accepted_only && !reference_accepted)
            goto rejected_primitive;
        if (!(capture->cull1[ordinal] & capture->cullmask))
            goto rejected_primitive;
        if (reference_accepted &&
            (capture->reference_depth == NULL ||
             capture->reference_point_count == NULL))
            return PORT_CAMERA_BUILD_INVALID;

        for (i = 0; i < vertex_count; ++i) {
            if (indices[i] >= capture->vertex_count)
                return PORT_CAMERA_BUILD_BAD_INDEX;
            source_points[i] = vertex_at(capture, indices[i]);
            if (source_points[i].z < PORT_CAMERA_NEAR_Z) {
                has_near_clipped_vertex = 1;
            } else {
                has_front_vertex = 1;
                source_reference[i] = project_reference(capture, source_points[i]);
            }
        }
        if (!has_front_vertex)
            goto rejected_primitive;
        if (capture->filter_legacy_clip && !reference_accepted &&
            !has_near_clipped_vertex &&
            reference_rectcode(source_reference, vertex_count,
                               &capture->legacy_clip) != 0)
            goto rejected_primitive;
        material = capture->selected_paint < capture->paint_count
            ? paint_bytes[capture->selected_paint] : 0;
        if (material == 0x2Du)
            material = (uint8_t)capture->backlight_override;
        memset(&command, 0, sizeof(command));
        command.shape_identity = capture->shape_identity;
        command.primitive_ordinal = ordinal;
        command.primitive_offset = offset;
        command.source_code = code;
        command.primitive_flags = flags;
        command.material = material;

        switch (type) {
        case 0: /* Filled polygon, clipped at the same z=12 plane. */
            output_count = clip_polygon_near(source_points, vertex_count, output_points);
            if (output_count > PORT_CAMERA_MAX_POINTS)
                return PORT_CAMERA_BUILD_TOO_MANY_POINTS;
            if (output_count < 3u)
                goto rejected_primitive;
            reference_count = clipped_reference_polygon(capture, source_points,
                                                        vertex_count, output_reference);
            if (reference_count > PORT_CAMERA_MAX_POINTS)
                return PORT_CAMERA_BUILD_TOO_MANY_REFERENCE_POINTS;
            if (capture->filter_legacy_clip && !reference_accepted &&
                reference_rectcode(output_reference, reference_count,
                                   &capture->legacy_clip) != 0)
                goto rejected_primitive;
            if (!reference_accepted && (flags & 1u) == 0 &&
                (capture->cull2[ordinal] & capture->cullbits2) == 0 &&
                !reference_facing(output_reference, reference_count))
                goto rejected_primitive;
            command.type = PORT_CAMERA_FILLED_POLYGON;
            command.depth = average_depth(sum_depth(source_points, vertex_count),
                                          reference_count);
            added = 1;
            break;
        case 1: /* Line. */
            if (vertex_count != 2u ||
                (source_points[0].z < PORT_CAMERA_NEAR_Z &&
                 source_points[1].z < PORT_CAMERA_NEAR_Z))
                goto rejected_primitive;
            if (source_points[0].z < PORT_CAMERA_NEAR_Z) {
                output_points[0] = near_intersection(source_points[1], source_points[0]);
                output_points[1] = source_points[1];
            } else if (source_points[1].z < PORT_CAMERA_NEAR_Z) {
                output_points[0] = source_points[0];
                output_points[1] = near_intersection(source_points[0], source_points[1]);
            } else {
                output_points[0] = source_points[0];
                output_points[1] = source_points[1];
            }
            output_count = 2;
            reference_count = 2;
            output_reference[0] = project_reference(capture, output_points[0]);
            output_reference[1] = project_reference(capture, output_points[1]);
            command.type = PORT_CAMERA_LINE;
            command.depth = average_depth(sum_depth(source_points, 2), 2);
            added = 1;
            break;
        case 2: /* Sphere: keep center and radius-defining camera vertex. */
            if (vertex_count != 2u || source_points[0].z < PORT_CAMERA_NEAR_Z ||
                source_points[1].z < PORT_CAMERA_NEAR_Z)
                goto rejected_primitive;
            output_points[0] = source_points[0];
            output_points[1] = source_points[1];
            output_count = 2;
            reference_count = 2;
            output_reference[0] = source_reference[0];
            output_reference[1] = source_reference[1];
            command.type = PORT_CAMERA_SPHERE;
            command.depth = average_depth(sum_depth(source_points, 2), 2);
            command.radius = sqrt(
                (double)(source_points[1].x - source_points[0].x) *
                (double)(source_points[1].x - source_points[0].x) +
                (double)(source_points[1].y - source_points[0].y) *
                (double)(source_points[1].y - source_points[0].y) +
                (double)(source_points[1].z - source_points[0].z) *
                (double)(source_points[1].z - source_points[0].z));
            added = 1;
            break;
        case 3: /* Wheel: original path rejects any near-clipped vertex. */
            if (vertex_count != 6u)
                goto rejected_primitive;
            for (i = 0; i < 6u; ++i)
                if (source_points[i].z < PORT_CAMERA_NEAR_Z)
                    goto rejected_primitive;
            if (project_winding(capture, source_points, 4u)) {
                for (i = 0; i < 4u; ++i) {
                    output_points[i] = source_points[i];
                    output_reference[i] = source_reference[i];
                }
            } else {
                output_points[0] = source_points[3];
                output_points[1] = source_points[4];
                output_points[2] = source_points[5];
                output_points[3] = source_points[0];
                output_reference[0] = source_reference[3];
                output_reference[1] = source_reference[4];
                output_reference[2] = source_reference[5];
                output_reference[3] = source_reference[0];
            }
            output_count = 4;
            reference_count = 4;
            command.type = PORT_CAMERA_WHEEL;
            command.depth = output_points[0].z;
            command.extra_materials[0] = (uint8_t)(material + 1u);
            command.extra_materials[1] = (uint8_t)(material + 2u);
            added = 1;
            break;
        case 5: /* Pixel special. */
            if (vertex_count != 1u || source_points[0].z < PORT_CAMERA_NEAR_Z)
                goto rejected_primitive;
            output_points[0] = source_points[0];
            output_count = 1;
            reference_count = 1;
            output_reference[0] = source_reference[0];
            command.type = PORT_CAMERA_PIXEL;
            command.depth = source_points[0].z;
            added = 1;
            break;
        default:
            if (append_unsupported(capture, ordinal, offset, code, type,
                                   unsupported, unsupported_capacity,
                                   &unsupported_count) != PORT_CAMERA_BUILD_OK)
                return PORT_CAMERA_BUILD_DIAGNOSTIC_CAPACITY;
            goto rejected_primitive;
        }

        if (added) {
            if (command_count >= command_capacity)
                return PORT_CAMERA_BUILD_COMMAND_CAPACITY;
            if (reference_accepted) {
                reference_count = capture->reference_point_count[ordinal];
                command.depth = capture->reference_depth[ordinal];
            }
            if (reference_count > PORT_CAMERA_MAX_POINTS)
                return PORT_CAMERA_BUILD_TOO_MANY_REFERENCE_POINTS;
            command.point_count = output_count;
            command.reference_point_count = reference_count;
            command.reference_accepted = reference_accepted ? 1u : 0u;
            for (i = 0; i < output_count; ++i)
                command_point(&command, i, output_points[i]);
            for (i = 0; i < reference_count; ++i)
                command.reference_xy[i] = output_reference[i];
            commands[command_count] = command;
            insert_in_shape_chain(commands, (uint16_t)command_count,
                                  capture->object_flags, links, &tail, &cursor,
                                  &inserted_count);
            ++command_count;
        }
        ++ordinal;
        offset = (uint16_t)(offset + record_size);
        continue;

    rejected_primitive:
        ++ordinal;
        offset = (uint16_t)(offset + record_size);
        if ((flags & 2u) == 0) {
            while (ordinal < capture->primitive_count &&
                   (capture->primitive_stream[offset + 1u] & 2u) != 0) {
                uint8_t skipped_code = capture->primitive_stream[offset];
                if (skipped_code == 0 || skipped_code >= 16u)
                    return PORT_CAMERA_BUILD_BAD_STREAM;
                offset = (uint16_t)(offset + 2u + capture->paint_count +
                                    s_index_counts[skipped_code]);
                ++ordinal;
            }
        }
    }

    {
        size_t out_index = 0;
        int16_t link = links[PORT_CAMERA_LINK_CAPACITY];
        PortCameraCommand ordered[PORT_CAMERA_LINK_CAPACITY];
        while (link >= 0) {
            if (out_index >= command_count)
                return PORT_CAMERA_BUILD_BAD_STREAM;
            ordered[out_index++] = commands[link];
            link = links[link];
        }
        if (out_index != command_count)
            return PORT_CAMERA_BUILD_BAD_STREAM;
        memcpy(commands, ordered, command_count * sizeof(commands[0]));
    }
    *command_count_out = command_count;
    if (unsupported_count_out != NULL)
        *unsupported_count_out = unsupported_count;
    return PORT_CAMERA_BUILD_OK;
}
