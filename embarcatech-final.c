#include "apps/buzzer.h"
#include "apps/joystick_led.h"
#include "apps/pwm_led.h"
#include "hardware/i2c.h"
#include "inc/bitmaps.h"
#include "inc/ssd1306.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define I2C_SDA 14
#define I2C_SCL 15

void run_peripherals_setup() {
  // INITIALIZE STDIO TYPES
  stdio_init_all();

  // START AND CONFIGURE LEDS
  gpio_init(LED_R_PIN);
  gpio_set_dir(LED_R_PIN, GPIO_OUT);
  gpio_init(LED_G_PIN);
  gpio_set_dir(LED_G_PIN, GPIO_OUT);
  gpio_init(LED_B_PIN);
  gpio_set_dir(LED_B_PIN, GPIO_OUT);

  // START AND CONFIGURE DISPLAY
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();
}

int main() {
  run_peripherals_setup();
  setup_joystick();
  uint16_t vrx_value, vry_value;

  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);

  ssd1306_t ssd_bm;
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);

  int index = 0;
  while (true) {
    ssd1306_draw_bitmap(&ssd_bm, display_options[index]);
    read_joystick_axis(&vrx_value, &vry_value);
    sleep_ms(50);
    if (vrx_value >= 4000) {
      index = (index == 0) ? 2 : --index;
    } else if (vrx_value <= 100) {
      index = (index == 2) ? 0 : ++index;
    }
    if (gpio_get(SW_PIN) == 0) {
      ssd1306_draw_bitmap(&ssd_bm, menu_option_back);
      while (true) {
        switch (index) {
        case 0:
          run_joystick_led();
        case 1:
          run_buzzer();
        case 2:
          run_pwm_led();
        }
        sleep_ms(50);
      }
    }
  }

  return 0;
}