#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

static const int VRX_PIN = 26; // Joystick's X axis pin connected to ADC
static const int VRY_PIN = 27; // Joystick's Y axis pin connected to ADC
static const int SW_PIN = 22;  // Joystick's button pin

static const int ADC_CHANNEL_VRX = 0;
static const int ADC_CHANNEL_VRY = 1;

static const int LED_B = 12; // Blue LED pin
static const int LED_R = 13; // Red LED pin

static const float PWM_DIVIDER = 16.0;
static const uint16_t PERIOD = 4096; // Period in milisseconds

extern void setup_joystick();
extern void setup_pwm_led(uint led, uint *slice, uint16_t level);
extern void setup();
extern void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);
extern int run_joystick_led();