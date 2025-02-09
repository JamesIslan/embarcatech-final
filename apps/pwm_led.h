#include "pico/stdlib.h"
#include <stdio.h>

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

static const uint8_t LED_PIN = 12;
static const uint16_t LED_PWM_PERIOD = 2000; // PWM period (max value)
static const float DIVIDER_PWM = 16.0;       // Divisor fracional do clock para o PWM
static const int16_t LED_DUTY_STEP = 100;

extern void run_pwm_led();