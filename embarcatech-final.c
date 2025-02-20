#include "apps/buzzer.h"
#include "apps/joystick_led.h"
#include "apps/pwm_led.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/bitmaps.h"
#include "inc/ssd1306.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

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

void setup_joystick() {
  adc_init();
  adc_gpio_init(VRX_PIN); // Set VRX_PIN to ADC input
  adc_gpio_init(VRY_PIN); // Set VRY_PIN to ADC input

  gpio_init(SW_PIN);             // Initialize joystick button pin
  gpio_set_dir(SW_PIN, GPIO_IN); // Set joystick button pin as input
  gpio_pull_up(SW_PIN);          // Activate pull up on joystick button to reduce bouncing
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
    if (gpio_get(SW_PIN) == 0) { // Button pressed?
      sleep_ms(200);
      ssd1306_draw_bitmap(&ssd_bm, menu_option_back);
      if (index == 0) {
        run_joystick_led();
      } else if (index == 1) {
        run_buzzer();
      } else if (index == 2) {
        run_pwm_led();
      }
      index = 0;
      sleep_ms(200);
    }
    sleep_ms(100);
    read_joystick_axis(&vrx_value, &vry_value);
    // OBS: NESTA ATIVIDADE, FOI USADO COMO REFERÊNCIA DE
    // MOVIMENTO DO JOYSTICK O EIXO "X", AO INVÉS DO EIXO "Y".
    // ISSO SE DEU PELO FATO DE O ANALÓGICO ESTAR EM UMA
    // POSIÇÃO INVERTIDA EM RELAÇÃO AO NORTE DA PLACA.
    // SENDO ASSIM, OPTEI POR REALIZAR ESSA TROCA PARA
    // DEIXAR A UTILIZAÇÃO DO PROGRAMA MAIS FLUIDO E INTUITIVO.
    if (vrx_value >= 4000) { // Joystick up?
      index = (index == 0) ? 2 : --index;
    } else if (vrx_value <= 100) { // Joystick down?
      index = (index == 2) ? 0 : ++index;
    }
  }

  return 0;
}