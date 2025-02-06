#include "joystick_led.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

// uint16_t led_b_level, led_r_level = 100; // Default PWM levels for LEDs
uint slice_led_b, slice_led_r; // Variáveis para armazenar os slices de PWM correspondentes aos LEDs

void setup_joystick() {
  adc_init();
  adc_gpio_init(VRX_PIN); // Set VRX_PIN to ADC input
  adc_gpio_init(VRY_PIN); // Set VRY_PIN to ADC input

  gpio_init(SW_PIN);             // Initialize joystick button pin
  gpio_set_dir(SW_PIN, GPIO_IN); // Set joystick button pin as input
  gpio_pull_up(SW_PIN);          // Activate pull up on joystick button to reduce bouncing
}

void setup_pwm_led(uint led_pin, uint *slice) {
  gpio_set_function(led_pin, GPIO_FUNC_PWM); // Configura o pino do LED como saída PWM
  *slice = pwm_gpio_to_slice_num(led_pin);   // Obtém o slice do PWM associado ao pino do LED
  pwm_set_clkdiv(*slice, PWM_LED_DIVIDER);   // Define o divisor de clock do PWM
  pwm_set_wrap(*slice, PWM_LED_PERIOD);      // Configura o valor máximo do contador (período do PWM)
  pwm_set_enabled(*slice, true);             // Habilita o PWM no slice correspondente ao LED
}

void setup() {
  stdio_init_all();
  setup_joystick();
  setup_pwm_led(LED_B, &slice_led_b); // Configura o PWM para o LED azul
  setup_pwm_led(LED_R, &slice_led_r); // Configura o PWM para o LED vermelho
}

void read_joystick_axis(uint16_t *vrx_value, uint16_t *vry_value) {
  adc_select_input(ADC_CHANNEL_VRX); // X axis
  sleep_us(2);
  *vrx_value = adc_read();

  adc_select_input(ADC_CHANNEL_VRY); // Y axis
  sleep_us(2);
  *vry_value = adc_read();
}

int run_joystick_led() {
  setup();
  uint16_t vrx_value, vry_value, sw_value;

  while (true) {
    read_joystick_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick
    // Ajusta os níveis PWM dos LEDs de acordo com os valores do joystick
    pwm_set_gpio_level(LED_B, vrx_value); // Ajusta o brilho do LED azul com o valor do eixo X
    pwm_set_gpio_level(LED_R, vry_value); // Ajusta o brilho do LED vermelho com o valor do eixo Y

    // Pequeno delay antes da próxima leitura
    sleep_ms(100); // Espera 100 ms antes de repetir o ciclo
  }
}