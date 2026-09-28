#include "stunts_types.h"
/* READABILITY: Cache or load shape resources, parse their run-length encoded image data, and size the resulting pages. */
/* PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port. */
struct SHAPE2D { I16 width,height,unknown1,unknown2,pos_x,pos_y; U8 unknown3,unknown4,unknown5,unknown6; };
extern I8 *mmgr_path_to_name(const I8 *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void FAR *mmgr_get_chunk_by_name(const I8 *);
    /* PLATFORM(file): load the named shape resource. */
extern void FAR *file_load_shape2d(I8 *, I16);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern U16S mmgr_get_chunk_size(void FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void FAR *mmgr_alloc_pages(const I8 *, U16S);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_release(void FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void FAR *mmgr_op_unk(void FAR *);
extern U16S file_get_res_shape_count(void FAR *);
extern struct SHAPE2D FAR *file_get_shape2d(U8 FAR *, I16);
extern I32 parse_shape2d_helper(U8 FAR *);
extern void FAR *parse_shape2d_helper2(I32);
extern I16 parse_shape2d_helper3(I8 FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_resize_memory(void FAR *, U16S);
void FAR *file_load_shape2d_res(I8 *, I16);
void parse_shape2d(void FAR *, void FAR *);
/* Load a shape resource and report failures as fatal.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a named shape resource with fatal-error behavior. */
void FAR* file_load_shape2d_res_fatal(I8* resname) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d_res(resname, 1);
}
/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a named shape resource without fatal-error behavior. */
void FAR* file_load_shape2d_res_nofatal(I8* resname) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d_res(resname, 0);
}
/* Reuse a cached shape resource or load and parse it into managed pages.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a shape resource through the shared file service. */
/* PLATFORM(memory): reuse or allocate game-managed resource pages. */
void FAR* file_load_shape2d_res(I8* resname, I16 fatal) {
    void FAR* memchunk;
    {
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        void FAR* cached = mmgr_get_chunk_by_name(mmgr_path_to_name(resname));
        if (cached) return cached;
    }
    {
        I16 chunksize;
        void FAR* pages;
    /* PLATFORM(file): load the named shape resource. */
        memchunk = file_load_shape2d(resname, fatal);
        if (!memchunk) return memchunk;
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        chunksize = mmgr_get_chunk_size(memchunk);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        pages = mmgr_alloc_pages(resname, chunksize);
        parse_shape2d(memchunk, pages);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        mmgr_release(memchunk);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        return mmgr_op_unk(pages);
    }
}
/* Expand each packed shape stream into destination pages and resize the allocation.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): read shape descriptors and packed image data from the loaded resource. */
/* PLATFORM(memory): resize game-managed pages to the expanded shape image data. */
void parse_shape2d(void FAR *memchunk, void FAR *mempages)
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

    /* PLATFORM(file): read the shape count from the loaded resource block. */
    index = file_get_res_shape_count(memchunk);
    pagescnt = (U32 FAR *)((U8 FAR *)mempages + index * 4 + 6);
    page_data = (U8 FAR *)mempages;
    input_read = (U8 FAR *)memchunk;
    for (shape_index = 0; shape_index < index * 4 + 6; shape_index++) {
        *page_data++ = *input_read++;
    }

    dest = (U8 FAR *)mempages + index * 8 + 6;
    baseaddr = dest;
    for (shape_index = 0; shape_index < index; shape_index++) {
    /* PLATFORM(file): read the current shape descriptor from the loaded resource block. */
        shape = file_get_shape2d((U8 FAR *)memchunk, shape_index);
        page_position = parse_shape2d_helper(dest);
        dest = (U8 FAR *)parse_shape2d_helper2(page_position);
        *pagescnt++ = parse_shape2d_helper(dest) - parse_shape2d_helper(baseaddr);
        src = (U8 FAR *)shape;
        for (shape_data_num = 0; shape_data_num < 16; shape_data_num++) {
            *dest++ = *src++;
        }
        literal_from = src;
        literal_cnt = 0;
        page_remaining = shape->width * shape->height;

        while (page_remaining != 0) {
            repeat = parse_shape2d_helper3((I8 FAR *)src);
            if (repeat > 3 || literal_cnt >= page_remaining) {
                while (literal_cnt > 127) {
                    literal_cnt -= 127;
                    page_remaining -= 127;
                    *dest++ = 0x81;
                    for (shape_data_num = 0; shape_data_num < 127; shape_data_num++)
                        *dest++ = *literal_from++;
                }
                if (literal_cnt != 0) {
                    *dest++ = (U8)(-literal_cnt);
                    page_remaining -= literal_cnt;
                    for (shape_data_num = 0; shape_data_num < literal_cnt; shape_data_num++)
                        *dest++ = *literal_from++;
                }

                if (repeat > page_remaining) repeat = page_remaining;
                while (repeat > 127) {
                    repeat -= 127;
                    page_remaining -= 127;
                    *dest++ = 0x7f;
                    *dest++ = *src;
                    src += 127;
                }
                if (repeat > 3) {
                    *dest++ = (U8)repeat;
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
        *dest++ = 0;
    }

    output_size = parse_shape2d_helper(dest) - parse_shape2d_helper((U8 FAR *)mempages);
    if (output_size & 15) {
        output_size = (output_size >> 4) + 1;
    } else {
        output_size >>= 4;
    }
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
    mmgr_resize_memory(mempages, (U16S)output_size);
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
