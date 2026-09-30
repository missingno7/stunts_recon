#include "port_runtime.h"

/* Semantic translation of load_09dc6, the 34-byte
   update_rpm_from_speed entry in seg002. context.py verifies instruction
   boundaries but has no strict recipe; the Restunts statecar.c source lead
   describes the high-word speed-times-ratio update. */
uint16_t update_rpm_from_speed(uint16_t current_rpm, uint16_t speed,
                               uint16_t gear_ratio, int16_t changing_gear,
                               uint16_t idle_rpm)
{
    if (changing_gear == 0)
        current_rpm = (uint16_t)(((uint32_t)speed * gear_ratio) >> 16);
    return current_rpm >= idle_rpm ? current_rpm : idle_rpm;
}
