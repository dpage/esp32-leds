#ifndef _main_h
#define _main_h

// Main

// Firmward version
#define FIRMWARE_VERSION "0.9"

// LED strip config
#define DEFAULT_NUM_LEDS 300
#define LED_PIN 5
#define EPWR_PIN 47

// Button config
#define BTN_MODE_PIN 0

// Functions
int GetNumLeds();
void SetNumLeds(int leds);
boolean GetPowerManagement();
void SetPowerManagement(boolean enabled);
boolean GetExternalPower();

#endif