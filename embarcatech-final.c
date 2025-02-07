#include "apps/buzzer.h"
#include "apps/joystick_led.h"
#include "apps/pwm_led.h"
#include "hardware/i2c.h"
#include "inc/bitmaps.h"
#include "inc/ssd1306.h"
#include "pico/binary_info.h"
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

void reset_display(struct render_area frame_area) {
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}

int main() {
  run_peripherals_setup();
  setup_joystick();
  uint16_t vrx_value, vry_value, sw_value;

  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);

  reset_display(frame_area);

  ssd1306_t ssd_bm;
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);

  uint8_t *display_now = display_options[0];
  int index = 0;
  while (true) {
    ssd1306_draw_bitmap(&ssd_bm, display_options[index]);
    read_joystick_axis(&vrx_value, &vry_value);
    if (vrx_value >= 3000) {
      index = ((index - 1) < 0) ? 2 : index - 1;
      printf("%i", index);
    } else if (vrx_value <= 1000) {
      index = ((index + 1) > 2) ? 0 : index + 1;
    } else {
      continue;
    }
    sleep_ms(500);
  }

  run_joystick_led();
  sleep_ms(1000);
  while (true) {
    sleep_ms(500);
  }

  return 0;
}