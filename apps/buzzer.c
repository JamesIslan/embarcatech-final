#include "buzzer.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

uint duty_divider = 16; // Increase to low volume
const uint TIME = 100;
const uint WHOLE_NOTE = 60000 * 4 / TIME;

int notes_mapped[sizeof(buzzer_notes) / sizeof(buzzer_notes[0])];
size_t note_count = sizeof(notes_mapped) / sizeof(notes_mapped[0]);

// Initialize PWM at the buzzer pin
void init_pwm_buzzer(uint pin) {
  gpio_set_function(pin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(pin);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 4.0f); // Adjusts clock divisor
  pwm_init(slice_num, &config, true);
  pwm_set_gpio_level(pin, 0); // PWM starts up OFF
}

// Play a note with a certain frequency and duration value
void play_tone(uint pin, uint frequency, uint duration_ms) {
  uint slice_num = pwm_gpio_to_slice_num(pin);
  uint32_t clock_freq = clock_get_hz(clk_sys);
  uint32_t duty_max = clock_freq / frequency - 2;

  pwm_set_wrap(slice_num, duty_max);
  pwm_set_gpio_level(pin, duty_max / duty_divider);

  sleep_ms(duration_ms);

  pwm_set_gpio_level(pin, 0);
  sleep_ms(50);
}

int get_min_freq(const int notes[], uint array_count) {
  int min_freq = notes[0];
  for (int i = 1; i < array_count; i++) {
    min_freq = (notes[i] < min_freq && notes[i] != 0) ? notes[i] : min_freq;
  }
  return min_freq;
}

int get_max_freq(const int notes[], uint array_count) {
  int max_freq = notes[0];
  for (int i = 1; i < array_count; i++) {
    max_freq = (notes[i] > max_freq) ? notes[i] : max_freq;
  }
  return max_freq;
}

uint map_frequency(uint frequency, uint min_freq, uint max_freq) {
  uint new_min_freq = 2000;
  uint new_max_freq = 5000;
  return ((frequency - min_freq) * (new_max_freq - new_min_freq)) / (max_freq - min_freq) + new_min_freq;
}

void map_notes() {
  uint min_freq = get_min_freq(buzzer_notes, note_count);
  uint max_freq = get_max_freq(buzzer_notes, note_count);
  for (int i = 0; i < note_count; i++) {
    if (buzzer_notes[i] != REST) {
      notes_mapped[i] = map_frequency(buzzer_notes[i], min_freq, max_freq);
    } else {
      notes_mapped[i] = REST;
    }
  }
}

void play_song(uint pin, uint buzzer_notes[], const int buzzer_note_durations[], size_t note_count) {
  for (int i = 0; i < note_count; i++) {
    uint note = buzzer_notes[i];
    uint note_duration = WHOLE_NOTE / abs(buzzer_note_durations[i]);
    if (buzzer_note_durations[i] < 0) {
      note_duration *= 1.5;
    }
    if (buzzer_notes[i] == 0) {
      sleep_ms(note_duration);
    } else {
      play_tone(pin, note, note_duration);
    }
  }
}

int run_buzzer() {
  stdio_init_all();
  init_pwm_buzzer(BUZZER_PIN);
  map_notes();
  while (true) {
    play_song(BUZZER_PIN, notes_mapped, buzzer_note_durations, note_count);
  }
  return 0;
}