#ifndef _oled_h
#define _oled_h

// OLED

// OLED config
#define OLED_CLOCK 18
#define OLED_DATA 17
#define OLED_RESET 21

void initOLED();
void oledDisplay(const char *title, const char *line1 = 0, const char *line2 = 0, const char *line3 = 0, const char *line4 = 0, const char *line5 = 0, const char *line6 = 0);

#endif