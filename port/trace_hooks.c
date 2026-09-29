#include "port_runtime.h"

/* obj_seg001_complete.c is renamed by the generated PORT_BUILD overlay so
   this wrapper traces only after the real update commits. */
extern void stunts_update_gamestate(void);

void update_gamestate(void)
{
    stunts_update_gamestate();
    port_trace_sim_step();
}
