#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

extern void setup_joystick();
extern void setup_pwm_led(uint led, uint *slice, uint16_t level);
extern void setup();
extern void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);
extern int run_joystick_led();