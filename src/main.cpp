//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 08
//
// Description:
//
//   Draws sample effects on a an addressable strip using FastLED
//
// History:     Sep-15-2020     davepl      Created
//              Oct-05-2020     davepl      Revised for Episode 07
//              Oct-11-2020     davepl      Revised for Episode 08
//              Oct-16-2020     davepl      Revised for Episode 09
//---------------------------------------------------------------------------

#include <Arduino.h>     // Arduino Framework
#include <U8g2lib.h>     // For text on the little on-chip OLED
#define FASTLED_INTERNAL // Suppress build banner
#include <FastLED.h>

#define OLED_CLOCK 18 // Pins for the OLED display
#define OLED_DATA 17
#define OLED_RESET 21

#define NUM_LEDS 300 // FastLED definitions
#define LED_PIN 5

#define BTN_MODE_PIN 2
#define BTN_B_UP_PIN 4
#define BTN_B_DOWN_PIN 5

#include "ledgfx.h"
#include "bounce.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

CRGB g_LEDs[NUM_LEDS] = {0}; // Frame buffer for FastLED

// OLED display on the heltec_wifi_kit_32_V3
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;

int g_Brightness = 255;  // 0-255 LED brightness scale
int g_PowerLimit = 3000; // 900mW Power Limit

// Button state tracking
int btnModePushCounter = 0;
boolean btnModeState = 0;
boolean lastBtnModeState = 0;

int btnBUpPushCounter = 0;
boolean btnBUpState = 0;
boolean lastBtnBUpState = 0;

int btnBDownPushCounter = 0;
boolean btnBDownState = 0;
boolean lastBtnBDownState = 0;

// Check and update the button states
void checkButtons()
{
    static unsigned long timer = 0;
    unsigned long interval = 20;
    if (millis() - timer >= interval)
    {
        timer = millis();

        // read the pushbutton input pin:
        btnModeState = digitalRead(BTN_MODE_PIN);

        // compare the buttonState to its previous state
        if (btnModeState != lastBtnModeState)
        {
            // if the state has changed, increment the counter
            if (btnModeState == LOW)
            {
                // if the current state is LOW then the button went from off to on:
                btnModePushCounter++;
                if (btnModePushCounter == 17)
                    btnModePushCounter = 0;
            }
            // save the current state as the last state, for next time through the loop
            lastBtnModeState = btnModeState;
        }
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BTN_MODE_PIN, INPUT_PULLUP);
    pinMode(BTN_B_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_B_DOWN_PIN, INPUT_PULLUP);

    Serial.begin(115200);
    while (!Serial)
    {
    }
    Serial.println("ESP32 Startup");

    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont15_tf);
    g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // Descent is a negative number so we add it to the total

    FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS); // Add our LED strip to the FastLED library
    FastLED.setBrightness(g_Brightness);
    set_max_power_indicator_LED(LED_BUILTIN);      // Light the builtin LED if we power throttle
    FastLED.setMaxPowerInMilliWatts(g_PowerLimit); // Set the power limit, above which brightness will be throttled
}

void DrawMarqueeComparison()
{
    static float scroll = 0.0f;
    scroll += 0.1f;
    if (scroll > 5.0)
        scroll -= 5.0;

    for (float i = scroll; i < NUM_LEDS / 2 - 1; i += 5)
    {
        DrawPixels(i, 3, CRGB::Green);
        DrawPixels(NUM_LEDS - 1 - (int)i, 3, CRGB::Red);
    }
}

void loop()
{
    BouncingBallEffect balls(NUM_LEDS, 3, 0, false, 8.0);
    BouncingBallEffect ballsFade(NUM_LEDS, 3, 64, false, 8.0);
    BouncingBallEffect ballsM(NUM_LEDS, 3, 0, true, 8.0);
    BouncingBallEffect ballsMFade(NUM_LEDS, 3, 64, true, 8.0);

    ClassicFireEffect fireOut(NUM_LEDS, 30, 100, 3, 2, false, true);
    ClassicFireEffect fireIn(NUM_LEDS, 30, 100, 3, 2, true, true);
    ClassicFireEffect fireStart(NUM_LEDS, 20, 100, 3, 4, true, false);
    ClassicFireEffect fireEnd(NUM_LEDS, 20, 100, 3, 4, false, false); 

    ClassicFireEffect fireXOut(NUM_LEDS, 50, 300, 30, 12, false, true);
    ClassicFireEffect fireXIn(NUM_LEDS, 50, 300, 30, 12, true, true);
    ClassicFireEffect fireXStart(NUM_LEDS, 30, 300, 30, 24, true, false);
    ClassicFireEffect fireXEnd(NUM_LEDS, 30, 300, 30, 24, false, false);

    while (true)
    {
        const char *effect;

        // Handle LEDs
        switch (btnModePushCounter)
        {
        case 0:
            balls.Draw();
            effect = "Balls";
            break;
        case 1:
            ballsFade.Draw();
            effect = "Balls Fade";
            break;
        case 2:
            ballsM.Draw();
            effect = "BallsM";
            break;
        case 3:
            ballsMFade.Draw();
            effect = "BallsM Fade";
            break;
        case 4:
            DrawComet();
            effect = "Comet";
            break;
        case 5:
            DrawMarquee();
            effect = "Marquee 1";
            break;
        case 6:
            DrawMarqueeMirrored();
            effect = "Marquee 2";
            break;
        case 7:
            fireOut.DrawFire();
            effect = "Fire Out";
            break;
        case 8:
            fireIn.DrawFire();
            effect = "Fire In";
            break;
        case 9:
            fireStart.DrawFire();
            effect = "Fire Start";
            break;
        case 10:
            fireEnd.DrawFire();
            effect = "Fire End";
            break;
        case 11:
            fireXOut.DrawFire();
            effect = "FireX Out";
            break;
        case 12:
            fireXIn.DrawFire();
            effect = "FireX In";
            break;
        case 13:
            fireXStart.DrawFire();
            effect = "FireX Start";
            break;
        case 14:
            fireXEnd.DrawFire();
            effect = "FireX End";
            break;
        case 15:
            DrawTwinkle();
            effect = "Twinkle";
            break;
        }

        FastLED.show(g_Brightness);

        checkButtons();

        EVERY_N_MILLISECONDS(250)
        {
            g_OLED.clearBuffer();
            g_OLED.setCursor(0, g_lineHeight);
            g_OLED.printf("FPS  : %u", FastLED.getFPS());
            g_OLED.setCursor(0, g_lineHeight * 2);
            g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));
            g_OLED.setCursor(0, g_lineHeight * 3);
            g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
            g_OLED.setCursor(0, g_lineHeight * 4);
            g_OLED.printf("Mode : %s", effect);
            g_OLED.sendBuffer();
        }
    }
}
