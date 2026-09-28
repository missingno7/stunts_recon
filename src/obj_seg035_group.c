/* READABILITY: Cache or load shape resources, parse their run-length encoded image data, and size the resulting pages. */
struct SHAPE2D { int width,height,unknown1,unknown2,pos_x,pos_y; unsigned char unknown3,unknown4,unknown5,unknown6; };
extern char *mmgr_path_to_name(const char *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void far *mmgr_get_chunk_by_name(const char *);
    /* PLATFORM(file): load the named shape resource. */
extern void far *file_load_shape2d(char *, int);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern unsigned short mmgr_get_chunk_size(void far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void far *mmgr_alloc_pages(const char *, unsigned short);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_release(void far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void far *mmgr_op_unk(void far *);
extern unsigned short file_get_res_shape_count(void far *);
extern struct SHAPE2D far *file_get_shape2d(unsigned char far *, int);
extern long parse_shape2d_helper(unsigned char far *);
extern void far *parse_shape2d_helper2(long);
extern int parse_shape2d_helper3(char far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_resize_memory(void far *, unsigned short);
void far *file_load_shape2d_res(char *, int);
void parse_shape2d(void far *, void far *);
/* Load a shape resource and report failures as fatal.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a named shape resource with fatal-error behavior. */
void far* file_load_shape2d_res_fatal(char* resname) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d_res(resname, 1);
}
/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a named shape resource without fatal-error behavior. */
void far* file_load_shape2d_res_nofatal(char* resname) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d_res(resname, 0);
}
/* Reuse a cached shape resource or load and parse it into managed pages.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load a shape resource through the shared file service. */
/* PLATFORM(memory): reuse or allocate game-managed resource pages. */
void far* file_load_shape2d_res(char* resname, int fatal) {
    void far* memchunk;
    {
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
        void far* cached = mmgr_get_chunk_by_name(mmgr_path_to_name(resname));
        if (cached) return cached;
    }
    {
        int chunksize;
        void far* pages;
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
void parse_shape2d(void far *memchunk, void far *mempages)
{
    int shape_data_num;
    unsigned long far *pagescnt;
    unsigned char far *input_read;
    unsigned char far *dest;
    unsigned char far *src;
    int shape_index;
    int index;
    unsigned char far *literal_from;
    unsigned char far *baseaddr;
    long output_size;
    long page_position;
    int repeat;
    int literal_cnt;
    long baseOff;
    unsigned int page_remaining;
    unsigned char far *page_data;
    struct SHAPE2D far *shape;

    /* PLATFORM(file): read the shape count from the loaded resource block. */
    index = file_get_res_shape_count(memchunk);
    pagescnt = (unsigned long far *)((unsigned char far *)mempages + index * 4 + 6);
    page_data = (unsigned char far *)mempages;
    input_read = (unsigned char far *)memchunk;
    for (shape_index = 0; shape_index < index * 4 + 6; shape_index++) {
        *page_data++ = *input_read++;
    }

    dest = (unsigned char far *)mempages + index * 8 + 6;
    baseaddr = dest;
    for (shape_index = 0; shape_index < index; shape_index++) {
    /* PLATFORM(file): read the current shape descriptor from the loaded resource block. */
        shape = file_get_shape2d((unsigned char far *)memchunk, shape_index);
        page_position = parse_shape2d_helper(dest);
        dest = (unsigned char far *)parse_shape2d_helper2(page_position);
        *pagescnt++ = parse_shape2d_helper(dest) - parse_shape2d_helper(baseaddr);
        src = (unsigned char far *)shape;
        for (shape_data_num = 0; shape_data_num < 16; shape_data_num++) {
            *dest++ = *src++;
        }
        literal_from = src;
        literal_cnt = 0;
        page_remaining = shape->width * shape->height;

        while (page_remaining != 0) {
            repeat = parse_shape2d_helper3((char far *)src);
            if (repeat > 3 || literal_cnt >= page_remaining) {
                while (literal_cnt > 127) {
                    literal_cnt -= 127;
                    page_remaining -= 127;
                    *dest++ = 0x81;
                    for (shape_data_num = 0; shape_data_num < 127; shape_data_num++)
                        *dest++ = *literal_from++;
                }
                if (literal_cnt != 0) {
                    *dest++ = (unsigned char)(-literal_cnt);
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
                    *dest++ = (unsigned char)repeat;
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

    output_size = parse_shape2d_helper(dest) - parse_shape2d_helper((unsigned char far *)mempages);
    if (output_size & 15) {
        output_size = (output_size >> 4) + 1;
    } else {
        output_size >>= 4;
    }
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
    mmgr_resize_memory(mempages, (unsigned short)output_size);
}

/* Count the repeated byte run beginning at the source pointer.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
int parse_shape2d_helper3(char far *source)
{
    char value = *source;
    int count = 0;
    while (*source++ == value)
        ++count;
    return count;
}
