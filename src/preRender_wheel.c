#include "stunts_types.h"
/* READABILITY: Generate, rotate, and submit the front/back wheel outline and side faces for rendering. */

#define WHEEL_RING_POINT_COUNT 16
#define WHEEL_SECOND_RING_OFFSET 16
#define WHEEL_OUTER_RING_OFFSET 32
#define WHEEL_POINT_BUFFER_COUNT 48
#define WHEEL_DRAW_FACE_POINT_COUNT 18
#define WHEEL_HALF_RING_POINT_COUNT 8
struct Point { I16 x; I16 y; };
struct WheelRect { I16 left, top, unused0, unused1, unused2, unused3, x, y; };
extern void FAR preRender_wheel_helper(struct WheelRect *, I16 *, I16);
extern void FAR preRender_wheel_helper4(I16, I16, ...);
extern void FAR preRender_default_alt(I16, I16, I8 *);
/* Construct the wheel perimeter and submit front, back, and side vertices for drawing.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): submit wheel faces and point buffers to the renderer. */
void FAR preRender_wheel(struct WheelRect *rect, I16 count, I16 color,
    I16 styleA, I16 styleB)
{
    struct Point *pointPtr;
    struct Point *destination;
    struct Point *firstPoint;
    struct Point drawVertices[WHEEL_DRAW_FACE_POINT_COUNT];
    I16 position;
    struct Point vertices[WHEEL_POINT_BUFFER_COUNT];
    I16 vertexIndex;
    struct Point *secondSet;
    struct Point *outBack;
    I16 topY;
    I16 topPoint;

    preRender_wheel_helper(rect, (I16 *)vertices, count);
    pointPtr = vertices;
    for (position = 0; position < WHEEL_RING_POINT_COUNT - 1; ++position) {
    /* PLATFORM(video): submit generated wheel geometry to the renderer. */
        preRender_wheel_helper4(color, 4,
            pointPtr[0].x, pointPtr[0].y,
            pointPtr[1].x, pointPtr[1].y,
            pointPtr[WHEEL_OUTER_RING_OFFSET + 1].x, pointPtr[WHEEL_OUTER_RING_OFFSET + 1].y,
            pointPtr[WHEEL_OUTER_RING_OFFSET].x, pointPtr[WHEEL_OUTER_RING_OFFSET].y);
        ++pointPtr;
    }
    /* PLATFORM(video): submit generated wheel geometry to the renderer. */
    preRender_wheel_helper4(color, 4,
        pointPtr[0].x, pointPtr[0].y,
        vertices[0].x, vertices[0].y,
        vertices[WHEEL_OUTER_RING_OFFSET].x, vertices[WHEEL_OUTER_RING_OFFSET].y,
        pointPtr[WHEEL_OUTER_RING_OFFSET].x, pointPtr[WHEEL_OUTER_RING_OFFSET].y);

    firstPoint = vertices + 1;
    topY = vertices[0].y;
    topPoint = 0;
    for (position = 1; position < WHEEL_RING_POINT_COUNT; ++position) {
        if (firstPoint->y < topY) {
            topY = firstPoint->y;
            topPoint = position;
        }
        ++firstPoint;
    }

    firstPoint = vertices + topPoint;
    secondSet = vertices + WHEEL_SECOND_RING_OFFSET + topPoint;
    destination = drawVertices;
    outBack = drawVertices + WHEEL_DRAW_FACE_POINT_COUNT - 1;
    vertexIndex = topPoint;
    for (position = 0; position <= WHEEL_HALF_RING_POINT_COUNT; ++destination, --outBack, ++position) {
        destination->x = firstPoint->x;
        destination->y = firstPoint->y;
        outBack->x = secondSet->x;
        outBack->y = secondSet->y;
        if (++vertexIndex >= WHEEL_RING_POINT_COUNT) {
            firstPoint = vertices;
            secondSet = vertices + WHEEL_SECOND_RING_OFFSET;
            vertexIndex = 0;
        } else {
            ++firstPoint;
            ++secondSet;
        }
    }
    /* PLATFORM(video): submit generated wheel geometry to the renderer. */
    preRender_default_alt(styleA, 0x12, (I8 *)drawVertices);

    firstPoint = vertices + topPoint;
    secondSet = vertices + WHEEL_SECOND_RING_OFFSET + topPoint;
    destination = drawVertices;
    outBack = drawVertices + WHEEL_DRAW_FACE_POINT_COUNT - 1;
    vertexIndex = topPoint;
    for (position = 0; position < WHEEL_HALF_RING_POINT_COUNT + 1; ++destination, --outBack, ++position) {
        destination->x = firstPoint->x;
        destination->y = firstPoint->y;
        outBack->x = secondSet->x;
        outBack->y = secondSet->y;
        if (--vertexIndex < 0) {
            firstPoint = vertices + WHEEL_RING_POINT_COUNT - 1;
            secondSet = vertices + WHEEL_SECOND_RING_OFFSET + WHEEL_RING_POINT_COUNT - 1;
            vertexIndex = 16;
        } else {
            --firstPoint;
            --secondSet;
        }
    }
    /* PLATFORM(video): submit generated wheel geometry to the renderer. */
    preRender_default_alt(styleA, 0x12, (I8 *)drawVertices);
    /* PLATFORM(video): submit generated wheel geometry to the renderer. */
    preRender_default_alt(styleB, 0x10, (I8 *)(vertices + WHEEL_SECOND_RING_OFFSET));
}
