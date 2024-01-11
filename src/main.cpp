//+--------------------------------------------------------------------------
//
// Original copyright (https://github.com/davepl/DavesGarageLEDSeries):
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// Later modified by Dave Page.
//
//+--------------------------------------------------------------------------

#include <Arduino.h>
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

// OLED config
#define OLED_CLOCK 18
#define OLED_DATA 17
#define OLED_RESET 21

// LED strip config
#define NUM_LEDS 300
#define LED_PIN 5

// Button config
#define BTN_MODE_PIN 0

// Include the effect classes
// Note: These must be after the LED strip config
//#include "ledgfx.h"
#include "bounce.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

// Frame buffer for FastLED
CRGB g_LEDs[NUM_LEDS] = {0};

// OLED display on the heltec_wifi_kit_32_V3
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight12 = 0;
int g_lineHeight15 = 0;
int g_width = 0;
int g_height = 0;

// LED power management constants
int g_Brightness = 255;
int g_PowerLimit = 900; // 900mW Power Limit

// Button state tracking
int patternMode = 0;
boolean btnModeState = 0;
boolean lastBtnModeState = 0;


// Check and update the button state
void checkButtons()
{
    static unsigned long timer = 0;
    unsigned long interval = 20;
    if (millis() - timer >= interval)
    {
        timer = millis();

        btnModeState = digitalRead(BTN_MODE_PIN);

        // Change in state? If so, and the new state is low, we have a push.
        if (btnModeState != lastBtnModeState)
        {
            if (btnModeState == LOW)
            {
                patternMode++;
                if (patternMode == 12)
                    patternMode = 0;
            }
            // save the current state as the last state, for next time through the loop
            lastBtnModeState = btnModeState;
        }
    }
}


// Setup the microcontroller
void setup()
{
    // Setup the GPIO pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BTN_MODE_PIN, INPUT_PULLUP);

    // Setup the OLED screen
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont12_tf);
    g_lineHeight12 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_OLED.setFont(u8g2_font_profont15_tf);
    g_lineHeight15 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_width = g_OLED.getWidth();
    g_height = g_OLED.getHeight();

    // Setup the LED strip and power management (warning LED and throttling)
    // Note: Power management should be removed if using an external PSU for
    //       the LED strips. This is just to prevent blowing out USB power.
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);
    FastLED.setBrightness(g_Brightness);
    set_max_power_indicator_LED(LED_BUILTIN);
    FastLED.setMaxPowerInMilliWatts(g_PowerLimit); 
}


// The main rendering loop
void loop()
{
    // Instantiate our effects
    BouncingBallEffect fxBalls(NUM_LEDS, 3, 0, false, 8.0);
    BouncingBallEffect fxBallsFade(NUM_LEDS, 3, 64, false, 8.0);
    BouncingBallEffect fxBallsMirror(NUM_LEDS, 3, 0, true, 8.0);
    BouncingBallEffect fxBallsMirrorFade(NUM_LEDS, 3, 64, true, 8.0);

    CometEffect fxComet;

    MarqueeEffect fxMarquee(false);
    MarqueeEffect fxMarqueeMirror(true);

    FireEffect fxFireOut(NUM_LEDS, 30, 100, 3, 2, false, true);
    FireEffect fxFireIn(NUM_LEDS, 30, 100, 3, 2, true, true);

    FireEffect fxFireXOut(NUM_LEDS, 50, 300, 30, 12, false, true);
    FireEffect fxFireXIn(NUM_LEDS, 50, 300, 30, 12, true, true);

    TwinkleEffect fxTwinkle;

    while (true)
    {
        const char *effect;

        // Draw the appropriate pattern
        switch (patternMode)
        {
        case 0:
            fxBalls.Draw();
            effect = "Balls";
            break;
        case 1:
            fxBallsFade.Draw();
            effect = "Balls Fade";
            break;
        case 2:
            fxBallsMirror.Draw();
            effect = "Balls Mirror";
            break;
        case 3:
            fxBallsMirrorFade.Draw();
            effect = "Balls Mirror Fade";
            break;
        case 4:
            fxComet.Draw();
            effect = "Comet";
            break;
        case 5:
            fxMarquee.Draw();
            effect = "Marquee";
            break;
        case 6:
            fxMarqueeMirror.Draw();
            effect = "Marquee Mirror";
            break;
        case 7:
            fxFireOut.DrawFire();
            effect = "Fire Out";
            break;
        case 8:
            fxFireIn.DrawFire();
            effect = "Fire In";
            break;
        case 9:
            fxFireXOut.DrawFire();
            effect = "FireX Out";
            break;
        case 10:
            fxFireXIn.DrawFire();
            effect = "FireX In";
            break;
        case 11:
            fxTwinkle.Draw();
            effect = "Twinkle";
            break;
        }

        // Display the frame
        FastLED.show(g_Brightness);

        // Have we had any button presses?
        checkButtons();

        // Update the OLED display
        EVERY_N_MILLISECONDS(250)
        {
            int linePos = 0;

            g_OLED.clearBuffer();

            g_OLED.setFont(u8g2_font_profont15_tf);

            linePos = g_lineHeight15;
            g_OLED.setCursor(0, linePos);
            g_OLED.printf("%s", effect);

            linePos = linePos + 1;
            g_OLED.drawHLine(0, linePos, g_width);

            g_OLED.setFont(u8g2_font_profont12_tf);

            linePos = linePos + g_lineHeight12 + 3;
            g_OLED.setCursor(0, linePos);
            g_OLED.printf("FPS  : %u", FastLED.getFPS());

            linePos = linePos + g_lineHeight12;
            g_OLED.setCursor(0, linePos);
            g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));

            linePos = linePos + g_lineHeight12;
            g_OLED.setCursor(0, linePos);
            g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));

            g_OLED.sendBuffer();
        }
    }
}
