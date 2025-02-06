#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include <stdio.h>       // Biblioteca padrão de entrada e saída

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

static const uint LED = 12;            // Pino do LED conectado
static const uint16_t LED_PWM_PERIOD = 2000;   // Período do PWM (valor máximo do contador)
static const float DIVIDER_PWM = 16.0; // Divisor fracional do clock para o PWM
static const uint16_t LED_STEP = 100;  // Passo de incremento/decremento para o duty cycle do LED

extern void setup_pwm();
extern int run_pwm_led();