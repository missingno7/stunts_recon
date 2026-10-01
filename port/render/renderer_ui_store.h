#ifndef PORT_RENDER_UI_STORE_H
#define PORT_RENDER_UI_STORE_H
#include "renderer_ui.h"
#include <string.h>
static inline void port_ui_store(uint8_t *base, size_t offset, uint8_t value)
{ base[offset]=value; PORT_UI_WRITE(base,offset,1); }
static inline void port_ui_and(uint8_t *base, size_t offset, uint8_t mask)
{ base[offset]&=mask; if (mask != 255) PORT_UI_WRITE(base,offset,1); }
static inline void port_ui_or(uint8_t *base, size_t offset, uint8_t mask)
{ base[offset]|=mask; if (mask != 0) PORT_UI_WRITE(base,offset,1); }
static inline void port_ui_xor(uint8_t *base, size_t offset, uint8_t mask)
{ base[offset]^=mask; if (mask != 0) PORT_UI_WRITE(base,offset,1); }
static inline void port_ui_fill(uint8_t *base, size_t offset, uint8_t value, size_t count)
{ memset(base+offset,value,count); PORT_UI_WRITE(base,offset,count); }
static inline void port_ui_blit(uint8_t *dst, size_t dofs, const uint8_t *src, size_t sofs, size_t count)
{ memcpy(dst+dofs,src+sofs,count); PORT_UI_COPY(dst,dofs,src,sofs,count); }
#endif
