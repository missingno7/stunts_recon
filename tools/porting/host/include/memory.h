#ifndef H1_HOST_MEMORY_H
#define H1_HOST_MEMORY_H
#include <stddef.h>
void *_fmemcpy(void *dst, const void *src, size_t count);
void *_fmemmove(void *dst, const void *src, size_t count);
void *_fmemset(void *dst, int value, size_t count);
int _fmemcmp(const void *left, const void *right, size_t count);
#endif
