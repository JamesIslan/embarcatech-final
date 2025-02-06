#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Configuração do pino do buzzer
#define BUZZER_PIN 21

extern void pwm_init_buzzer(uint pin);
extern void play_tone(uint pin, uint frequency, uint duration_ms);
extern void play_star_wars(uint pin);
extern int run_buzzer();
