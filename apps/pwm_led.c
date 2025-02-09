#include "pwm_led.h"
#include "hardware/pwm.h"
#include "joystick_led.h"
#include "pico/stdlib.h"
#include <stdio.h>

const uint8_t LED_PULSE_SPEED = 50; // The higher the number, the lower the speed
uint16_t LED_START_LEVEL = 100;     // Nível inicial do PWM (duty cycle)

void setup_pwm() {
  uint slice;
  gpio_set_function(LED, GPIO_FUNC_PWM);    // Configura o pino do LED para função PWM
  slice = pwm_gpio_to_slice_num(LED);       // Obtém o slice do PWM associado ao pino do LED
  pwm_set_clkdiv(slice, DIVIDER_PWM);       // Define o divisor de clock do PWM
  pwm_set_wrap(slice, LED_PWM_PERIOD);      // Configura o valor máximo do contador (período do PWM)
  pwm_set_gpio_level(LED, LED_START_LEVEL); // Define o nível inicial do PWM para o pino do LED
  pwm_set_enabled(slice, true);             // Habilita o PWM no slice correspondente
}

int run_pwm_led() {
  uint up_down = 1; // Variável para controlar se o nível do LED aumenta ou diminui
  setup_pwm();      // Configura o PWM
  while (true) {
    if (gpio_get(SW_PIN) == 1) {
      pwm_set_gpio_level(LED, LED_START_LEVEL); // Define o nível atual do PWM (duty cycle)
      for (int i = 0; i < LED_PULSE_SPEED; i++) {
        if (gpio_get(SW_PIN) == 1) {
          sleep_ms(1);
        }
      }
      if (up_down) {
        LED_START_LEVEL += LED_STEP; // Incrementa o nível do LED
        if (LED_START_LEVEL >= LED_PWM_PERIOD)
          up_down = 0; // Muda direção para diminuir quando atingir o período máximo
      } else {
        LED_START_LEVEL -= LED_STEP; // Decrementa o nível do LED
        if (LED_START_LEVEL <= 0)
          up_down = 1; // Muda direção para aumentar quando atingir o nível mínimo
      }
    } else {
      pwm_set_gpio_level(LED, 0);
      break;
    }
  }
  return 0;
}