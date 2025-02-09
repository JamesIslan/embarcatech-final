#include "pwm_led.h"
#include "hardware/pwm.h"
#include "joystick_led.h"
#include "pico/stdlib.h"
#include <stdio.h>

const uint8_t LED_PULSE_SPEED = 50; // The higher the number, the lower the speed
uint16_t LED_START_LEVEL = 100;     // Nível inicial do PWM (duty cycle)

// Configures the PWM function on the LED pin
void setup_pwm() {
  uint slice;
  gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
  slice = pwm_gpio_to_slice_num(LED_PIN);
  pwm_set_clkdiv(slice, DIVIDER_PWM);
  pwm_set_wrap(slice, LED_PWM_PERIOD);
  pwm_set_gpio_level(LED_PIN, LED_START_LEVEL);
  pwm_set_enabled(slice, true);
}

void run_pwm_led() {
  bool go_up = true;
  setup_pwm();
  while (true) {
    if (gpio_get(SW_PIN) == 1) {
      pwm_set_gpio_level(LED_PIN, LED_START_LEVEL); // Updates PWM level
      for (int i = 0; i < LED_PULSE_SPEED; i++) {
        if (gpio_get(SW_PIN) == 1) { // Button is not pressed?
          sleep_ms(1);
        }
      }
      LED_START_LEVEL += (go_up) ? +LED_DUTY_STEP : -LED_DUTY_STEP;                                              // Updates duty cycle level
      go_up = (go_up && LED_START_LEVEL >= LED_PWM_PERIOD) || (!go_up && LED_START_LEVEL <= 0) ? !go_up : go_up; // Controls if duty cycle will increase or decrease
    } else {
      pwm_set_gpio_level(LED_PIN, 0);
      break;
    }
  }
}