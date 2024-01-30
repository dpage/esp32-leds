#include <Arduino.h>
#include <U8g2lib.h>

#include "main.h"

#include "oled.h"


// OLED display on the heltec_wifi_kit_32_V3
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight10 = 0;
int g_lineHeight12 = 0;
int g_width = 0;
int g_height = 0;


// Initialise the OLED
void initOLED()
{
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont10_tf);
    g_lineHeight10 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_OLED.setFont(u8g2_font_profont12_tf);
    g_lineHeight12 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_width = g_OLED.getWidth();
    g_height = g_OLED.getHeight();
}


// Display a page of data on the OLED
void oledDisplay(const char *title, const char *line1, const char *line2, const char *line3, const char *line4, const char *line5, const char *line6)
{
    int linePos = 0;

    g_OLED.clearBuffer();

    g_OLED.setFont(u8g2_font_profont12_tf);

    linePos = g_lineHeight12;
    g_OLED.setCursor(0, linePos);
    g_OLED.printf(title);

    linePos = linePos + 1;
    g_OLED.drawHLine(0, linePos, g_width);

    g_OLED.setFont(u8g2_font_profont10_tf);

    linePos = linePos + g_lineHeight10 + 3;
    if (line1 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line1);
    }

    linePos = linePos + g_lineHeight10;
    if (line2 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line2);
    }

    linePos = linePos + g_lineHeight10;
    if (line3 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line3);
    }

    linePos = linePos + g_lineHeight10;
    if (line4 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line4);
    }

    linePos = linePos + g_lineHeight10;
    if (line5 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line5);
    }

    linePos = linePos + g_lineHeight10;
    if (line6 != NULL)
    {
        g_OLED.setCursor(0, linePos);
        g_OLED.printf(line6);
    }

    g_OLED.sendBuffer();
}
