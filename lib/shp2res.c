#include "files.h"
#include "types.h"
/* Cache or load shape resources, parse their run-length encoded image data, and size the resulting pages. */
/* Local view: SHAPE2D; struct.count is 10; another unit uses 6. */
struct SHAPE2D { I16 width,height,s2d_hotspot_x,s2d_hotspot_y,pos_x,pos_y; U8 s2d_plane_color0,s2d_plane_color1_bg,s2d_plane_color2_flip,s2d_plane_color3_flags; };
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "memory.h"
#include "video.h"


/* Load a shape resource and report failures as fatal.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* loadandshapepack(I8* resname) {
	return loadandshapepacka(resname, 1);
}
/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* loadandshapepackz(I8* resname) {
	return loadandshapepacka(resname, 0);
}
/* Reuse a cached shape resource or load and parse it into managed pages.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* loadandshapepacka(I8* resname, I16 fatal) {
    void FAR* memchunk;
    {
        void FAR* cached = checkmodule(mmgr_path_to_name(resname));
        if (cached) return cached;
    }
    {
        I16 chunksize;
        void FAR* pages;
        memchunk = file_load_shape2d(resname, fatal);
        if (!memchunk) return memchunk;
        chunksize = modulesize(memchunk);
        pages = reservememory(resname, chunksize);
        shapepack(memchunk, pages);
        purgememory(memchunk);
        return packmemory(pages);
    }
}
/* Expand each packed shape stream into destination pages and resize the allocation.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void shapepack(void FAR *memchunk, void FAR *mempages)
{
    I16 shape_data_num;
    U32 FAR *pagescnt;
    U8 FAR *input_read;
    U8 FAR *dest;
    U8 FAR *src;
    I16 shape_index;
    I16 index;
    U8 FAR *literal_from;
    U8 FAR *baseaddr;
    I32 output_size;
    I32 page_position;
    I16 repeat;
    I16 literal_cnt;
    I32 baseOff;
    U16 page_remaining;
    U8 FAR *page_data;
    struct SHAPE2D FAR *shape;

    index = file_get_res_shape_count(memchunk);
    pagescnt = (U32 FAR *)((U8 FAR *)mempages + index * 4 + RESOURCE_DIRECTORY_HEADER_BYTES);
    page_data = mempages;
    input_read = memchunk;
    for (shape_index = 0; shape_index < index * 4 + RESOURCE_DIRECTORY_HEADER_BYTES; shape_index++) {
        *page_data++ = *input_read++;
    }

    dest = (U8 FAR *)mempages + index * 8 + RESOURCE_DIRECTORY_HEADER_BYTES;
    baseaddr = dest;
    for (shape_index = 0; shape_index < index; shape_index++) {
        shape = shapepointer(memchunk, shape_index);
        page_position = segtoadr(dest);
        dest = adrtoseg(page_position);
        *pagescnt++ = segtoadr(dest) - segtoadr(baseaddr);
        src = (U8 FAR *)shape;
        for (shape_data_num = 0; shape_data_num < 16; shape_data_num++) {
            *dest++ = *src++;
        }
        literal_from = src;
        literal_cnt = 0;
        page_remaining = shape->width * shape->height;

        while (page_remaining != 0) {
            repeat = parse_shape2d_helper3((I8 FAR *)src);
            if (repeat > SHAPE2D_RLE_MIN_RUN_MINUS_ONE || literal_cnt >= page_remaining) {
                while (literal_cnt > SHAPE2D_RLE_MAX_PACKET_PIXELS) {
                    literal_cnt -= SHAPE2D_RLE_MAX_PACKET_PIXELS;
                    page_remaining -= SHAPE2D_RLE_MAX_PACKET_PIXELS;
                    *dest++ = SHAPE2D_RLE_LITERAL_127;
                    for (shape_data_num = 0; shape_data_num < SHAPE2D_RLE_MAX_PACKET_PIXELS; shape_data_num++)
                        *dest++ = *literal_from++;
                }
                if (literal_cnt != 0) {
                    *dest++ = -literal_cnt;
                    page_remaining -= literal_cnt;
                    for (shape_data_num = 0; shape_data_num < literal_cnt; shape_data_num++)
                        *dest++ = *literal_from++;
                }

                if (repeat > page_remaining) repeat = page_remaining;
                while (repeat > SHAPE2D_RLE_MAX_PACKET_PIXELS) {
                    repeat -= SHAPE2D_RLE_MAX_PACKET_PIXELS;
                    page_remaining -= SHAPE2D_RLE_MAX_PACKET_PIXELS;
                    *dest++ = SHAPE2D_RLE_MAX_PACKET_PIXELS;
                    *dest++ = *src;
                    src += SHAPE2D_RLE_MAX_PACKET_PIXELS;
                }
                if (repeat > SHAPE2D_RLE_MIN_RUN_MINUS_ONE) {
                    *dest++ = repeat;
                    page_remaining -= repeat;
                    *dest++ = *src;
                    src += repeat;
                }
                literal_from = src;
                literal_cnt = 0;
            }
            src++;
            literal_cnt++;
        }
        *dest++ = SHAPE2D_RLE_END;
    }

    output_size = segtoadr(dest) - segtoadr(mempages);
    if (output_size & DOS_PARAGRAPH_OFFSET_MASK) {
        output_size = (output_size >> 4) + 1;
    } else {
        output_size >>= 4;
    }
    resizememory(mempages, output_size);
}

/* Count the repeated byte run beginning at the source pointer.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
I16 parse_shape2d_helper3(I8 FAR *source)
{
    I8 value = *source;
    I16 count = 0;
    while (*source++ == value)
        ++count;
    return count;
}
