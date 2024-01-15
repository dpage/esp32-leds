#ifndef _main_h
#define _main_h

// Main

// Firmward version
#define FIRMWARE_VERSION "0.1"

// OLED config
#define OLED_CLOCK 18
#define OLED_DATA 17
#define OLED_RESET 21

// LED strip config
#define NUM_LEDS 300
#define LED_PIN 5

// Button config
#define BTN_MODE_PIN 0

void oledDisplay(const char *title, const char *line1 = 0, const char *line2 = 0, const char *line3 = 0, const char *line4 = 0, const char *line5 = 0, const char *line6 = 0);

#endif