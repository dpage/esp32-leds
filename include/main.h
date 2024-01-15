#ifndef _main_h
#define _main_h

// Main

// Firmward version
#define FIRMWARE_VERSION "0.3"

// LED strip config
#define DEFAULT_NUM_LEDS 300
#define LED_PIN 5
extern int g_NumLeds;
extern int g_Brightness;
extern int g_PowerLimit;

// Button config
#define BTN_MODE_PIN 0

#endif