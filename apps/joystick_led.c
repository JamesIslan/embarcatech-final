#include "joystick_led.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

uint slice_led_b, slice_led_r; // Storage PWM slices for LEDs

void setup_pwm_led(uint led_pin, uint *slice) {
  gpio_set_function(led_pin, GPIO_FUNC_PWM);
  *slice = pwm_gpio_to_slice_num(led_pin);
  pwm_set_clkdiv(*slice, PWM_LED_DIVIDER);
  pwm_set_wrap(*slice, PWM_LED_PERIOD);
  pwm_set_enabled(*slice, true);
}

void setup() {
  setup_pwm_led(LED_B, &slice_led_b);
  setup_pwm_led(LED_R, &slice_led_r);
}

void read_joystick_axis(uint16_t *vrx_value, uint16_t *vry_value) {
  adc_select_input(ADC_CHANNEL_VRX); // X axis
  sleep_us(2);
  *vrx_value = adc_read();

  adc_select_input(ADC_CHANNEL_VRY); // Y axis
  sleep_us(2);
  *vry_value = adc_read();
}

void run_joystick_led() {
  setup();
  uint16_t vrx_value, vry_value;
  while (true) {
    if (gpio_get(SW_PIN)) {
      read_joystick_axis(&vrx_value, &vry_value);
      pwm_set_gpio_level(LED_B, vrx_value);
      pwm_set_gpio_level(LED_R, vry_value);
    } else {
      pwm_set_gpio_level(LED_R, 0);
      pwm_set_gpio_level(LED_B, 0);
      break;
    }
  }
}