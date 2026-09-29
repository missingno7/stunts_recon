#ifndef H1_HOST_MALLOC_H
#define H1_HOST_MALLOC_H
#include <stddef.h>
void *_fmalloc(size_t size);
void *_fcalloc(size_t count, size_t size);
void *_frealloc(void *ptr, size_t size);
void _ffree(void *ptr);
void *_halloc(long count, size_t size);
void _hfree(void *ptr);
#endif
