#ifndef STUNTS_BOUNDARY_RECORD_H
#define STUNTS_BOUNDARY_RECORD_H
#include <stddef.h>
#include <stdint.h>
#define PORT_BOUNDARY_REGIONS 5
#define PORT_BOUNDARY_BYTES 514
typedef struct PortBoundaryRegion {
    const char *name;
    const void *data;
    uint16_t size;
    uint8_t read;
} PortBoundaryRegion;
typedef struct PortBoundaryCall {
    const char *name;
    uint64_t order;
    uint32_t frame;
    unsigned scope, active, argc, pointer_mask, region_count;
    unsigned ui_phase,ui_visit;uint64_t ui_key;
    uint32_t args[12];
    PortBoundaryRegion regions[PORT_BOUNDARY_REGIONS];
    uint8_t before[PORT_BOUNDARY_REGIONS][PORT_BOUNDARY_BYTES];
} PortBoundaryCall;
int port_boundary_open(const char *path);
void port_boundary_close(void);
int port_boundary_begin(PortBoundaryCall *,const char *,const uint32_t *,unsigned,unsigned,
                        const PortBoundaryRegion *,unsigned);
void port_boundary_end(PortBoundaryCall *,uint32_t,int);
void port_boundary_sim_begin(uint16_t frame);
void port_boundary_sim_end(void);
void port_boundary_frame_limit(uint16_t frame);
void port_boundary_simulation_only(int enabled);
unsigned port_boundary_scope(void);
uint32_t port_boundary_frame(void);
void port_boundary_graphics_enable(int enabled);
int port_boundary_graphics_enabled(void);
const uint8_t *port_boundary_graphics_row9(void);
uint16_t port_boundary_graphics_row9_size(void);
uint64_t port_boundary_graphics_corner(void);
uint64_t port_boundary_graphics_source(const void *shape);
#endif
