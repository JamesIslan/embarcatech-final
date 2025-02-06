#include "buzzer.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
  gpio_set_function(pin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(pin);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
  pwm_init(slice_num, &config, true);
  pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms) {
  uint slice_num = pwm_gpio_to_slice_num(pin);
  uint32_t clock_freq = clock_get_hz(clk_sys);
  uint32_t top = clock_freq / frequency - 1;

  pwm_set_wrap(slice_num, top);
  pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

  sleep_ms(duration_ms);

  pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
  sleep_ms(50);               // Pausa entre notas
}

// Função principal para tocar a música
void play_star_wars(uint pin) {
  for (int i = 0; i < sizeof(buzzer_notes) / sizeof(buzzer_notes[0]); i++) {
    if (buzzer_notes[i] == 0) {
      sleep_ms(note_duration[i]);
    } else {
      play_tone(pin, buzzer_notes[i], note_duration[i]);
    }
  }
}

int run_buzzer() {
  stdio_init_all();
  pwm_init_buzzer(BUZZER_PIN);
  while (1) {
    play_star_wars(BUZZER_PIN);
  }

  return 0;
}