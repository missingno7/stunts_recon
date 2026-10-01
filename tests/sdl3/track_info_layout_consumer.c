#include <stddef.h>
#include <stdint.h>
#include "stunts_track_views.h"

extern struct TRKOBJINFO shapeinfos[120];
extern struct TRACKOBJECT trklst[215];
extern unsigned char shapedata42_10[1236];
extern size_t port_track_producer_stride(void);
extern size_t port_track_producer_link_offset(void);

_Static_assert(sizeof(struct TRKOBJINFO) == 16, "track-info consumer stride");
_Static_assert(offsetof(struct TRKOBJINFO, si_cameraDataOffset) == 8,
               "track-info camera field");
_Static_assert(offsetof(struct TRKOBJINFO, link) == 12,
               "track-info DOS near-offset field");
_Static_assert(sizeof(struct TRACKOBJECT) == 20, "track-object consumer stride");

int main(void)
{
    unsigned index;
    if (port_track_producer_stride() != sizeof(struct TRKOBJINFO) ||
        port_track_producer_link_offset() !=
            offsetof(struct TRKOBJINFO, link))
        return 1;
    for (index = 104; index <= 109; ++index) {
        const struct TRKOBJINFO *info = &shapeinfos[index];
        const unsigned char *camera = (const unsigned char *)info->si_cameraDataOffset;
        const unsigned char *resolved = camera + 7 * sizeof(struct VECTOR);
        unsigned entry;
        int found = 0;
        if (info->link.dataPointer != (index <= 105 ? 0x1972 : 0x191e))
            return 2;
        if (camera != shapedata42_10 + (index <= 105 ? 1044 : 960) ||
            resolved != shapedata42_10 + (index <= 105 ? 1086 : 1002))
            return 3;
        for (entry = 0; entry < 215; ++entry)
            if (trklst[entry].ss_trkObjInfoPtr == info)
                found = 1;
        if (!found)
            return 4;
    }
    if (shapeinfos[103].link.dataPointer != 0 ||
        shapeinfos[110].link.dataPointer != 0)
        return 5;
    return 0;
}
