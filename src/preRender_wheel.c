struct Point { int x; int y; };
struct WheelRect { int left, top, unused0, unused1, unused2, unused3, x, y; };
extern void far preRender_wheel_helper(struct WheelRect *, int *, int);
extern void far preRender_wheel_helper4(int, int, ...);
extern void far preRender_default_alt(int, int, char *);
void far preRender_wheel(struct WheelRect *rect, int count, int color,
    int styleA, int styleB)
{
    struct Point *pointPtr;
    struct Point *destination;
    struct Point *firstPoint;
    struct Point drawVertices[18];
    int position;
    struct Point vertices[48];
    int vertexIndex;
    struct Point *secondSet;
    struct Point *outBack;
    int topY;
    int topPoint;

    preRender_wheel_helper(rect, (int *)vertices, count);
    pointPtr = vertices;
    for (position = 0; position < 15; ++position) {
        preRender_wheel_helper4(color, 4,
            pointPtr[0].x, pointPtr[0].y,
            pointPtr[1].x, pointPtr[1].y,
            pointPtr[33].x, pointPtr[33].y,
            pointPtr[32].x, pointPtr[32].y);
        ++pointPtr;
    }
    preRender_wheel_helper4(color, 4,
        pointPtr[0].x, pointPtr[0].y,
        vertices[0].x, vertices[0].y,
        vertices[32].x, vertices[32].y,
        pointPtr[32].x, pointPtr[32].y);

    firstPoint = vertices + 1;
    topY = vertices[0].y;
    topPoint = 0;
    for (position = 1; position < 16; ++position) {
        if (firstPoint->y < topY) {
            topY = firstPoint->y;
            topPoint = position;
        }
        ++firstPoint;
    }

    firstPoint = vertices + topPoint;
    secondSet = vertices + 16 + topPoint;
    destination = drawVertices;
    outBack = drawVertices + 17;
    vertexIndex = topPoint;
    for (position = 0; position <= 8; ++destination, --outBack, ++position) {
        destination->x = firstPoint->x;
        destination->y = firstPoint->y;
        outBack->x = secondSet->x;
        outBack->y = secondSet->y;
        if (++vertexIndex >= 16) {
            firstPoint = vertices;
            secondSet = vertices + 16;
            vertexIndex = 0;
        } else {
            ++firstPoint;
            ++secondSet;
        }
    }
    preRender_default_alt(styleA, 0x12, (char *)drawVertices);

    firstPoint = vertices + topPoint;
    secondSet = vertices + 16 + topPoint;
    destination = drawVertices;
    outBack = drawVertices + 17;
    vertexIndex = topPoint;
    for (position = 0; position < 9; ++destination, --outBack, ++position) {
        destination->x = firstPoint->x;
        destination->y = firstPoint->y;
        outBack->x = secondSet->x;
        outBack->y = secondSet->y;
        if (--vertexIndex < 0) {
            firstPoint = vertices + 15;
            secondSet = vertices + 31;
            vertexIndex = 16;
        } else {
            --firstPoint;
            --secondSet;
        }
    }
    preRender_default_alt(styleA, 0x12, (char *)drawVertices);
    preRender_default_alt(styleB, 0x10, (char *)(vertices + 16));
}
