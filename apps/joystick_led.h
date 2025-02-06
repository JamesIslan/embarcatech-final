#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Definição dos pinos usados para o joystick e LEDs
static const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
static const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
static const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
static const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
static const int SW = 22;           // Pino de leitura do botão do joystick

static const int LED_B = 12;           // Pino para controle do LED azul via PWM
static const int LED_R = 13;           // Pino para controle do LED vermelho via PWM
static const float PWM_DIVIDER = 16.0; // Divisor fracional do clock para o PWM
static const uint16_t PERIOD = 4096;   // Período do PWM (valor máximo do contador)

extern void setup_joystick();
extern void setup_pwm_led(uint led, uint *slice, uint16_t level);
extern void setup();
extern void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);
extern int run_joystick_led();