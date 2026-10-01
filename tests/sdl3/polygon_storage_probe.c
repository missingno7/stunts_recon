/* The include is extracted verbatim from the production obj_seg006 overlay. */
#include <stdint.h>
#define I8 char
#define I16 int16_t
#define U16 uint16_t
#define U8 uint8_t
#define far
struct POINT2D { I16 x, y; };
struct POLYINFO { I16 depth; };
#include "polygon_storage_excerpt.inc"

_Static_assert(sizeof(poly_link_list) / sizeof(poly_link_list[0]) ==
               POLYINFO_CAPACITY + 1,
               "the polygon-list head needs its own host storage word");

int main(int argc, char **argv)
{
    static char arena[128];
    static struct POLYINFO records[3];

    (void)argc;
    (void)argv;

    polyinfoptr = arena;
    polyinfo_reset();
    if (polyinfoptr != arena || poly_link_list[POLYINFO_CAPACITY] != -1)
        return 1;
    if (polygonnumber != 0 || polyinfo_offset != 0 ||
        facenodeiterator != POLYINFO_CAPACITY)
        return 2;

    poly_cursor1 = POLYINFO_CAPACITY;
    poly_link_listit4 = POLYINFO_CAPACITY;
    transformed_vert_count = 3;
    records[0].depth = 100;
    records[1].depth = 20;
    records[2].depth = 150;

    poly_info_ptrs[0] = &records[0];
    if (insert_newest_poly_in_poly_linked_list_40ED6(100, 1) != 0)
        return 3;
    poly_info_ptrs[1] = &records[1];
    if (insert_newest_poly_in_poly_linked_list_40ED6(20, 1) != 0)
        return 4;
    poly_info_ptrs[2] = &records[2];
    if (insert_newest_poly_in_poly_linked_list_40ED6(150, 1) != 0)
        return 5;

    if (polyinfoptr != arena || polygonnumber != 3 ||
        polyinfo_offset != 3 * (3 * sizeof(struct POINT2D) + 6))
        return 6;
    if (poly_link_list[POLYINFO_CAPACITY] != 2 ||
        poly_link_list[2] != 0 || poly_link_list[0] != 1 ||
        poly_link_list[1] != -1 || facenodeiterator != 1)
        return 7;

    polyinfo_reset();
    if (polyinfoptr != arena || poly_link_list[POLYINFO_CAPACITY] != -1 ||
        polygonnumber != 0 || polyinfo_offset != 0)
        return 8;
    return 0;
}
