#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display/ssd1306.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "inc/bitmaps.h"
#include "inc/notes.h"
#include "pico/stdlib.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

#define SW_PIN 22

#define VRX_PIN 26
#define VRY_PIN 27

#define VRX_ADC_CHANNEL 0
#define VRY_ADC_CHANNEL 1

#define BUZZER_PIN 21

#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define RED_LED_PIN 13

#define LED_STEP 100

#define PWM_WRAP 4000
#define LED_PWM_DIVIDER 16.0
#define BUZZER_PWM_DIVIDER 4.0

#define TIME 100
#define WHOLE_NOTE 240000 / TIME

volatile bool program_running;
volatile bool is_sw_released;

uint16_t led_level = 100;

int notes_mapped[sizeof(notes) / sizeof(notes[0])];
size_t note_count = sizeof(notes_mapped) / sizeof(notes_mapped[0]);

void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t wrap = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pin, wrap / 2);

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0);
}

int get_min_value(int arr[], uint array_count) {
    int min_value = arr[0];
    for (int i = 1; i < array_count; i++) {
        if (arr[i] < min_value && arr[i] != 0) {
            min_value = arr[i];
        }
    }
    return min_value;
}

int get_max_value(int arr[], uint array_count) {
    int max_value = arr[0];
    for (int i = 1; i < array_count; i++) {
        if (arr[i] > max_value) {
            max_value = arr[i];
        }
    }
    return max_value;
}

uint map_frequency(uint frequency, uint min_freq, uint max_freq) {
    uint new_min_freq = 2000;
    uint new_max_freq = 6000;
    return ((frequency - min_freq) * (new_max_freq - new_min_freq)) / (max_freq - min_freq) + new_min_freq;
}

void map_notes() {
    uint min_freq = get_min_value(notes, note_count);
    uint max_freq = get_max_value(notes, note_count);

    for (int i = 0; i < note_count; i++) {
        if (notes[i] != REST) {
            notes_mapped[i] = map_frequency(notes[i], min_freq, max_freq);
        } else {
            notes_mapped[i] = REST;
        }
    }
}

void play_song(uint pin, int notes[], int note_durations[], size_t note_count) {
    for (int i = 0; i < note_count; i++) {
        if (!program_running) {
            break;
        }

        uint note = notes[i];
        uint note_duration = WHOLE_NOTE / abs(note_durations[i]);

        if (note_durations[i] < 0) {
            note_duration *= 1.5;
        }

        if (notes[i] == 0) {
            sleep_ms(note_duration);
        } else {
            play_tone(pin, note, note_duration);
        }

        sleep_ms(50);
    }
}

void setup_led_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_set_clkdiv(slice_num, LED_PWM_DIVIDER);
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_enabled(slice_num, true);

    pwm_set_gpio_level(pin, 0);
}

void setup_buzzer_pwm() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    pwm_set_clkdiv(slice_num, BUZZER_PWM_DIVIDER);
    pwm_set_enabled(slice_num, true);

    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void setup_joystick() {
    adc_init();

    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);
}

void setup_display() {
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void read_joystick_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(VRX_ADC_CHANNEL);
    sleep_us(5);
    *vrx_value = adc_read();

    adc_select_input(VRY_ADC_CHANNEL);
    sleep_us(5);
    *vry_value = adc_read();
}

bool get_sw_value() {
    return !gpio_get(SW_PIN);
}

bool watch_sw_callback(struct repeating_timer *t) {
    if (get_sw_value()) return true;
    is_sw_released = true;
    return false;
}

void press_sw_callback(uint pin, uint32_t events) {
    if (is_sw_released) {
        program_running = false;
        gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, false);
    }
}

void run_buzzer() {
    while (program_running) {
        play_song(BUZZER_PIN, notes_mapped, note_durations, note_count);
        sleep_ms(50);
    }
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void run_joystick_led() {
    uint16_t vrx_value, vry_value;
    while (program_running) {
        read_joystick_axis(&vrx_value, &vry_value);

        pwm_set_gpio_level(BLUE_LED_PIN, vrx_value);
        pwm_set_gpio_level(RED_LED_PIN, vry_value);

        sleep_ms(100);
    }
    pwm_set_gpio_level(BLUE_LED_PIN, 0);
    pwm_set_gpio_level(RED_LED_PIN, 0);
}

void run_led_pwm() {
    bool increase = true;

    while (program_running) {
        pwm_set_gpio_level(GREEN_LED_PIN, led_level);
        sleep_ms(500);

        if (increase) {
            led_level += LED_STEP;
            if (led_level >= PWM_WRAP) {
                increase = false;
            }
        } else {
            led_level -= LED_STEP;
            if (led_level <= LED_STEP) {
                increase = true;
            }
        }
    }

    led_level = 100;
    pwm_set_gpio_level(GREEN_LED_PIN, 0);
}

void (*program_functions[])() = {
    run_buzzer,
    run_joystick_led,
    run_led_pwm};

void run_program(uint option) {
    if (option >= 0 && option <= 2) {
        program_functions[option]();
    }
}

int main() {
    stdio_init_all();

    setup_joystick();

    setup_led_pwm(BLUE_LED_PIN);
    setup_led_pwm(RED_LED_PIN);
    setup_led_pwm(GREEN_LED_PIN);

    setup_display();

    setup_buzzer_pwm();

    map_notes();

    ssd1306_init();

    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, I2C_PORT);
    ssd1306_config(&ssd_bm);

    uint16_t vrx_value, vry_value;
    bool sw_value;

    bool joystick_released = true;

    struct repeating_timer sw_timer;

    uint option = 0;

    while (true) {
        ssd1306_draw_bitmap(&ssd_bm, options[option]);
        read_joystick_axis(&vrx_value, &vry_value);

        if (joystick_released) {
            if (vrx_value > 3000) {
                option = (option == 0) ? 0 : --option;
                joystick_released = false;
            } else if (vrx_value < 1000) {
                option = (option == 2) ? 2 : ++option;
                joystick_released = false;
            }
        } else if (vrx_value >= 1000 && vrx_value <= 3000) {
            joystick_released = true;
        }

        sw_value = get_sw_value();

        if (sw_value) {
            ssd1306_draw_bitmap(&ssd_bm, go_back);

            add_repeating_timer_ms(50, watch_sw_callback, NULL, &sw_timer);
            gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, press_sw_callback);

            program_running = true;
            is_sw_released = false;

            run_program(option);

            while (get_sw_value()) {
                sleep_ms(10);
            }
        }

        sleep_ms(50);
    }

    return 0;
}
