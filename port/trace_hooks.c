#include "port_runtime.h"
#include "boundary_record.h"

/* carsim.c is renamed by the generated PORT_BUILD overlay so
   this wrapper traces only after the real update commits. */
extern void stunts_update_gamestate(void);

void movesim(void)
{
    uint8_t state[PORT_GAMESTATE_BYTES];
    size_t state_size;
    uint64_t step_id;
    port_boundary_sim_begin(port_game_frame_snapshot());
    stunts_update_gamestate();
    port_boundary_sim_end();
    state_size = port_game_state_copy(state, sizeof(state));
    step_id = port_trace_sim_step(state, state_size);
    port_guest_note_sim_step(step_id);
}
