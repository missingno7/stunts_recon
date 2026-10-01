#ifndef STUNTS_PORT_AD15_DRIVER_H
#define STUNTS_PORT_AD15_DRIVER_H

#include <stddef.h>
#include <stdint.h>

typedef void (*Ad15WriteRegister)(void *userdata, uint8_t reg,
                                  uint8_t value);

/* Host state for the observed AD15 OPL2 register interface. The driver image
   is retained as inert data so its lookup tables remain the source of truth;
   its instructions are never executed. */
typedef struct Ad15Driver {
    const uint8_t *image;
    size_t image_size;
    Ad15WriteRegister write_register;
    void *write_userdata;
    uint8_t register_cache[256];
    uint8_t velocity[9];
    uint8_t initialized;
    uint8_t sample_timer_active;
    uint32_t unsupported_sample_calls;
} Ad15Driver;

void ad15_driver_bind(Ad15Driver *driver, const uint8_t *image,
                      size_t image_size, Ad15WriteRegister write_register,
                      void *write_userdata);
uint8_t ad15_driver_00(Ad15Driver *driver);
void ad15_driver_03(Ad15Driver *driver);
void ad15_driver_06(Ad15Driver *driver);
void ad15_driver_09(Ad15Driver *driver, int16_t channel, void *voice,
                    void *chunk, int16_t note, int16_t velocity,
                    const void *sample);
void ad15_driver_0c(Ad15Driver *driver, int16_t channel, void *voice);
void ad15_driver_0f(Ad15Driver *driver, int16_t channel, void *voice);
void ad15_driver_12(Ad15Driver *driver, int16_t channel, void *voice,
                    int16_t value);
void ad15_driver_15(Ad15Driver *driver, int16_t channel, void *voice,
                    int16_t type, int16_t value);
void ad15_driver_21(Ad15Driver *driver, int16_t channel, void *voice,
                    void *chunk, const void *sample);
void ad15_driver_24(Ad15Driver *driver, int16_t channel, void *voice,
                    uint16_t value);
void ad15_driver_27(Ad15Driver *driver, uint16_t channel, void *voice,
                    void *chunk, const void *sample);
void ad15_driver_1e(Ad15Driver *driver, int16_t channel);
uint16_t ad15_driver_36(const Ad15Driver *driver, int16_t channel);
void ad15_driver_stop_unknown(Ad15Driver *driver);

#endif
