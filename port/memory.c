#include "port_runtime.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define PORT_MAX_ALLOCS 2048u
#define PORT_FIRST_HEAP_LINEAR 0x40000u
#define PORT_CONVENTIONAL_END 0xA0000u
#define PORT_HANDLE_BASE 0xC000u

typedef struct PortAllocation {
    uint8_t *host;
    size_t size;
    size_t padded_size;
    uint32_t linear;
    uint16_t handle_segment;
    uint8_t space;
    uint8_t live;
    char owner[32];
} PortAllocation;

static uint8_t s_dos_memory[PORT_DOS_ADDRESS_BYTES];
extern uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];
static PortAllocation s_allocations[PORT_MAX_ALLOCS];
static uint32_t s_next_handle = PORT_HANDLE_BASE;
static PortMemoryStats s_stats;

static int find_real_span(size_t size, uint32_t *linear_out)
{
    uint32_t candidate = PORT_FIRST_HEAP_LINEAR;
    if (size > PORT_CONVENTIONAL_END - PORT_FIRST_HEAP_LINEAR)
        return 0;
    for (;;) {
        uint32_t next_candidate = candidate;
        size_t i;
        if (candidate > PORT_CONVENTIONAL_END ||
            size > PORT_CONVENTIONAL_END - candidate)
            return 0;
        for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
            const PortAllocation *entry = &s_allocations[i];
            uint32_t end;
            if (!entry->live || entry->space != PORT_FAR_REAL)
                continue;
            end = entry->linear + (uint32_t)entry->padded_size;
            if (candidate < end && candidate + size > entry->linear &&
                end > next_candidate)
                next_candidate = end;
        }
        if (next_candidate == candidate) {
            *linear_out = candidate;
            return 1;
        }
        candidate = (next_candidate + 15u) & ~15u;
    }
}

static uint32_t real_heap_high_water(void)
{
    uint32_t high_water = PORT_FIRST_HEAP_LINEAR;
    size_t i;
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        const PortAllocation *entry = &s_allocations[i];
        uint32_t end;
        if (!entry->live || entry->space != PORT_FAR_REAL)
            continue;
        end = entry->linear + (uint32_t)entry->padded_size;
        if (end > high_water)
            high_water = end;
    }
    return high_water;
}

static PortAllocation *find_allocation(const void *pointer)
{
    size_t i;
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        if (s_allocations[i].live && s_allocations[i].host == pointer)
            return &s_allocations[i];
    }
    return NULL;
}

void port_memory_init(void)
{
    memset(s_dos_memory, 0, sizeof(s_dos_memory));
    memset(s_allocations, 0, sizeof(s_allocations));
    s_next_handle = PORT_HANDLE_BASE;
    memset(&s_stats, 0, sizeof(s_stats));
}

void *port_memory_alloc(size_t size, const char *owner, PortFarPtr *address_out)
{
    PortAllocation *entry = NULL;
    size_t i;
    size_t padded;
    size_t capacity;
    uint8_t *host;
    uint32_t linear;
    uint32_t handle_segments;
    int use_handle;

    if (size == 0)
        size = 1;
    if (size > 0xFFFFFFFFu - 15u)
        return NULL;
    padded = (size + 15u) & ~(size_t)15u;
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        if (!s_allocations[i].live) {
            entry = &s_allocations[i];
            break;
        }
    }
    if (entry == NULL)
        return NULL;

    /* A DOS page allocation owns only its requested paragraph extent. The
       earlier blanket 4 KiB growth reserve made memory queries report less
       conventional memory than the guest had actually consumed. Real blocks
       can grow in place on an explicit resize below. */
    capacity = padded;
    use_handle = !find_real_span(capacity, &linear);
    if (use_handle) {
        /* Leave one paragraph window of stable growth room for the legacy
           resize API. The public extent remains the requested byte count. */
        capacity = padded;
        if (capacity <= SIZE_MAX - 0x10000u)
            capacity += 0x10000u;
        handle_segments = (uint32_t)(((uint64_t)capacity + 0xFFFFu) >> 16);
        if (handle_segments == 0)
            handle_segments = 1;
        if (s_next_handle + handle_segments > 0x10000u)
            return NULL;
        host = (uint8_t *)calloc(1, capacity);
        if (host == NULL)
            return NULL;
        linear = 0;
        entry->space = PORT_FAR_HANDLE;
        entry->handle_segment = (uint16_t)s_next_handle;
        s_next_handle += handle_segments;
    } else {
        host = &s_dos_memory[linear];
        memset(host, 0, size);
        entry->space = PORT_FAR_REAL;
        entry->handle_segment = 0;
    }
    entry->host = host;
    entry->size = size;
    entry->padded_size = capacity;
    entry->linear = linear;
    entry->live = 1;
    if (owner != NULL) {
        strncpy(entry->owner, owner, sizeof(entry->owner) - 1u);
        entry->owner[sizeof(entry->owner) - 1u] = '\0';
    } else {
        entry->owner[0] = '\0';
    }
    s_stats.allocations++;
    s_stats.live_bytes += (uint32_t)size;
    if (s_stats.live_bytes > s_stats.high_water_bytes)
        s_stats.high_water_bytes = s_stats.live_bytes;

    if (address_out != NULL) {
        if (entry->space == PORT_FAR_REAL) {
            address_out->segment = (uint16_t)(linear >> 4);
            address_out->offset = (uint16_t)(linear & 0x0Fu);
            address_out->space = PORT_FAR_REAL;
        } else {
            address_out->segment = entry->handle_segment;
            address_out->offset = 0;
            address_out->space = PORT_FAR_HANDLE;
        }
    }
    return host;
}

void port_memory_free(void *pointer)
{
    PortAllocation *entry = find_allocation(pointer);
    if (entry == NULL)
        return;
    if (entry->space == PORT_FAR_HANDLE)
        free(entry->host);
    s_stats.live_bytes -= (uint32_t)entry->size;
    memset(entry, 0, sizeof(*entry));
}

PortFarPtr port_far_normalize(PortFarPtr pointer)
{
    uint32_t linear;
    if (pointer.space == PORT_FAR_HANDLE) {
        return pointer;
    }
    if (pointer.space != PORT_FAR_REAL)
        return pointer;
    linear = ((((uint32_t)pointer.segment << 4) + pointer.offset) & 0xFFFFFu);
    pointer.segment = (uint16_t)(linear >> 4);
    pointer.offset = (uint16_t)(linear & 0x0Fu);
    pointer.space = PORT_FAR_REAL;
    return pointer;
}

PortFarPtr port_far_add(PortFarPtr pointer, uint32_t amount)
{
    if (pointer.space == PORT_FAR_HANDLE) {
        uint64_t combined = (uint64_t)pointer.offset + amount;
        uint64_t segment = (uint64_t)pointer.segment + (combined >> 16);
        if (segment > 0xFFFFu) {
            pointer.space = 0xFFu;
            return pointer;
        }
        pointer.segment = (uint16_t)segment;
        pointer.offset = (uint16_t)combined;
        return pointer;
    }
    if (pointer.space != PORT_FAR_REAL)
        return pointer;
    {
        uint32_t linear = (((uint32_t)pointer.segment << 4) + pointer.offset + amount) &
                          0xFFFFFu;
        pointer.segment = (uint16_t)(linear >> 4);
        pointer.offset = (uint16_t)(linear & 0x0Fu);
        return pointer;
    }
}

void *port_far_resolve(PortFarPtr pointer, size_t extent)
{
    size_t i;
    pointer = port_far_normalize(pointer);
    if (pointer.space != PORT_FAR_REAL && pointer.space != PORT_FAR_HANDLE)
        return NULL;
    if (pointer.space == PORT_FAR_REAL) {
        uint32_t linear = (((uint32_t)pointer.segment << 4) + pointer.offset) & 0xFFFFFu;
        if (extent > PORT_DOS_ADDRESS_BYTES - linear)
            return NULL;
        if (linear >= 0xA0000u && linear < 0xB0000u) {
            uint32_t video_offset = linear - 0xA0000u;
            if (extent > PORT_VIDEO_MEMORY_BYTES - video_offset)
                return NULL;
            return &port_framebuffer[video_offset];
        }
        return &s_dos_memory[linear];
    }
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        PortAllocation *entry = &s_allocations[i];
        uint32_t delta;
        if (!entry->live || entry->space != PORT_FAR_HANDLE ||
            pointer.segment < entry->handle_segment)
            continue;
        delta = ((uint32_t)pointer.segment - entry->handle_segment) * 0x10000u + pointer.offset;
        if (delta <= entry->size && extent <= entry->size - delta)
            return entry->host + delta;
    }
    return NULL;
}

int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out)
{
    size_t i;
    uintptr_t p = (uintptr_t)pointer;
    uintptr_t video_base = (uintptr_t)port_framebuffer;
    if (p >= video_base && p - video_base < PORT_VIDEO_MEMORY_BYTES) {
        uint32_t delta = (uint32_t)(p - video_base);
        if (remaining_out != NULL)
            *remaining_out = PORT_VIDEO_MEMORY_BYTES - delta;
        if (address_out != NULL) {
            address_out->segment = 0xA000u;
            address_out->offset = (uint16_t)delta;
            address_out->space = PORT_FAR_REAL;
        }
        return 1;
    }
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        PortAllocation *entry = &s_allocations[i];
        uintptr_t base = (uintptr_t)entry->host;
        if (!entry->live || p < base || p - base >= entry->size)
            continue;
        if (remaining_out != NULL)
            *remaining_out = entry->size - (size_t)(p - base);
        if (address_out != NULL) {
            if (entry->space == PORT_FAR_REAL) {
                uint32_t linear = entry->linear + (uint32_t)(p - base);
                address_out->segment = (uint16_t)(linear >> 4);
                address_out->offset = (uint16_t)(linear & 0x0Fu);
                address_out->space = PORT_FAR_REAL;
            } else {
                uint32_t delta = (uint32_t)(p - base);
                address_out->segment = (uint16_t)(entry->handle_segment + (delta >> 16));
                address_out->offset = (uint16_t)delta;
                address_out->space = PORT_FAR_HANDLE;
            }
        }
        return 1;
    }
    return 0;
}

PortMemoryStats port_memory_stats(void)
{
    return s_stats;
}

uint16_t mmgr_get_ofs_diff(void)
{
    uint32_t high_water = real_heap_high_water();
    uint32_t paragraphs = (PORT_CONVENTIONAL_END - high_water) >> 4;
    return paragraphs > 0xFFFFu ? 0xFFFFu : (uint16_t)paragraphs;
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    size_t i;
    uintptr_t p;
    if (pointer == NULL)
        return 0;
    p = (uintptr_t)pointer;
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        PortAllocation *entry = &s_allocations[i];
        uintptr_t base;
        size_t offset;
        if (!entry->live || entry->host == NULL)
            continue;
        base = (uintptr_t)entry->host;
        if (p < base || p - base >= entry->size)
            continue;
        offset = (size_t)(p - base);
        if (remaining_out != NULL)
            *remaining_out = entry->size - offset;
        return 1;
    }
    {
        uintptr_t video = (uintptr_t)port_framebuffer;
        if (p >= video && p - video < PORT_VIDEO_MEMORY_BYTES) {
            if (remaining_out != NULL)
                *remaining_out = PORT_VIDEO_MEMORY_BYTES - (size_t)(p - video);
            return 1;
        }
    }
    return 0;
}

int port_memory_resize(void *pointer, size_t size)
{
    PortAllocation *entry = find_allocation(pointer);
    size_t padded;
    if (entry == NULL || size == 0 || size > SIZE_MAX - 15u)
        return 0;
    padded = (size + 15u) & ~(size_t)15u;
    if (padded > entry->padded_size) {
        size_t i;
        uint32_t end;
        if (entry->space != PORT_FAR_REAL ||
            padded > PORT_CONVENTIONAL_END - entry->linear)
            return 0;
        end = entry->linear + (uint32_t)padded;
        for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
            const PortAllocation *other = &s_allocations[i];
            uint32_t other_end;
            if (other == entry || !other->live || other->space != PORT_FAR_REAL)
                continue;
            other_end = other->linear + (uint32_t)other->padded_size;
            if (entry->linear < other_end && end > other->linear)
                return 0;
        }
        memset(entry->host + entry->size, 0, size - entry->size);
        s_stats.live_bytes += (uint32_t)(size - entry->size);
        entry->padded_size = padded;
        entry->size = size;
        if (s_stats.live_bytes > s_stats.high_water_bytes)
            s_stats.high_water_bytes = s_stats.live_bytes;
        return 1;
    }
    if (size > entry->size)
        memset(entry->host + entry->size, 0, size - entry->size);
    else
        s_stats.live_bytes -= (uint32_t)(entry->size - size);
    if (size > entry->size)
        s_stats.live_bytes += (uint32_t)(size - entry->size);
    entry->size = size;
    entry->padded_size = padded;
    if (s_stats.live_bytes > s_stats.high_water_bytes)
        s_stats.high_water_bytes = s_stats.live_bytes;
    return 1;
}

void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{
    return port_memory_alloc((size_t)paragraphs * 16u, name, NULL);
}

void *mmgr_alloc_farmem(uint32_t bytes)
{
    return port_memory_alloc((size_t)bytes, "farmem", NULL);
}

void mmgr_alloc_a000(void)
{
    /* The A000 segment is the VGA window at DOS linear address 0xA0000. */
}

void mmgr_free(void *pointer)
{
    port_memory_free(pointer);
}

void mmgr_release(void *pointer)
{
    port_memory_free(pointer);
}

uint16_t mmgr_get_chunk_size(void *pointer)
{
    size_t extent;
    size_t paragraphs;
    if (!port_memory_extent(pointer, &extent))
        return 0;
    paragraphs = (extent + 15u) >> 4;
    return paragraphs > 0xFFFFu ? 0xFFFFu : (uint16_t)paragraphs;
}

void mmgr_resize_memory(void *pointer, uint16_t paragraphs)
{
    if (!port_memory_resize(pointer, (size_t)paragraphs * 16u))
        port_guest_unwind("memory block resize exceeded its reserved segment span");
}

void *mmgr_op_unk(void *pointer)
{
    return pointer;
}

static void normalized_name(const char *path, char *out, size_t capacity)
{
    const char *base = path != NULL ? path : "";
    const char *p;
    size_t n = 0;
    for (p = path; p != NULL && *p != '\0'; ++p)
        if (*p == '/' || *p == '\\')
            base = p + 1;
    for (p = base; *p != '\0' && n + 1u < capacity; ++p) {
        unsigned char c = (unsigned char)*p;
        out[n++] = (char)tolower(c);
    }
    out[n] = '\0';
}

char *mmgr_path_to_name(const char *path)
{
    const char *base = path;
    const char *p;
    if (path == NULL)
        return NULL;
    for (p = path; *p != '\0'; ++p)
        if (*p == ':' || *p == '\\')
            base = p + 1;
    return (char *)base;
}

void *mmgr_get_chunk_by_name(const char *name)
{
    char wanted[32];
    size_t i;
    normalized_name(name, wanted, sizeof(wanted));
    for (i = 0; i < PORT_MAX_ALLOCS; ++i) {
        char owner[32];
        if (!s_allocations[i].live)
            continue;
        normalized_name(s_allocations[i].owner, owner, sizeof(owner));
        if (wanted[0] != '\0' && strcmp(wanted, owner) == 0)
            return s_allocations[i].host;
    }
    return NULL;
}
