#include <stddef.h>
#include <stdint.h>
#include "stunts_scene_views.h"

struct SHAPE3D { uint16_t marker; };
extern struct SHAPE3D g_shapes3d[130];
extern struct TRACKOBJECT scene2[19];
extern struct TRACKOBJECT scene3[13];
extern size_t port_scene_producer_stride(void);
extern size_t port_scene_producer_shape_offset(void);

_Static_assert(sizeof(struct TRACKOBJECT) == 20, "renderer scene stride");
_Static_assert(offsetof(struct TRACKOBJECT, ss_shapePtr) == 6,
               "renderer shape pointer offset");

int main(void)
{
    if (port_scene_producer_stride() != sizeof(struct TRACKOBJECT) ||
        port_scene_producer_shape_offset() !=
            offsetof(struct TRACKOBJECT, ss_shapePtr))
        return 1;
    if (scene2[0].ss_shapePtr != &g_shapes3d[108] ||
        scene2[6].ss_shapePtr != &g_shapes3d[43] ||
        scene2[7].ss_shapePtr != &g_shapes3d[42] ||
        scene2[8].ss_shapePtr != &g_shapes3d[42] ||
        scene2[7].ss_loShapePtr != &g_shapes3d[42])
        return 2;
    if (scene2[7].ss_trkObjInfoPtr != NULL ||
        scene2[7].ss_rotY != 0 ||
        scene2[7].ss_ignoreZBias != 1 ||
        (uint8_t)scene2[7].ss_physicalModel != 255)
        return 3;
    if (scene3[0].ss_shapePtr != &g_shapes3d[112] ||
        scene3[12].ss_shapePtr != &g_shapes3d[110])
        return 4;
    return 0;
}
