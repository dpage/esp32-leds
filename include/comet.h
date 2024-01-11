//+--------------------------------------------------------------------------
//
// Original copyright (https://github.com/davepl/DavesGarageLEDSeries):
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// Later modified by Dave Page.
//
//+--------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

extern CRGB g_LEDs[];

class CometEffect
{
public:
    void Draw()
    {
        const byte fadeAmt = 128;
        const int cometSize = 5;
        const int deltaHue = 4;

        static byte hue = HUE_RED;
        static int iDirection = 1;
        static int iPos = 0;

        hue += deltaHue;

        iPos += iDirection;
        if (iPos == (NUM_LEDS - cometSize) || iPos == 0)
            iDirection *= -1;

        for (int i = 0; i < cometSize; i++)
            g_LEDs[iPos + i].setHue(hue);

        // Randomly fade the LEDs
        for (int j = 0; j < NUM_LEDS; j++)
            if (random(10) > 5)
                g_LEDs[j] = g_LEDs[j].fadeToBlackBy(fadeAmt);
    }
};
