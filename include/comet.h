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

extern CRGB fbLEDs[];

class CometEffect
{
private:
    int _nLeds;

public:
    CometEffect()
        : _nLeds(GetNumLeds())
    {
    }

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
        if (iPos == (_nLeds - cometSize) || iPos == 0)
            iDirection *= -1;

        for (int i = 0; i < cometSize; i++)
            fbLEDs[iPos + i].setHue(hue);

        // Randomly fade the LEDs
        for (int j = 0; j < _nLeds; j++)
            if (random(10) > 5)
                fbLEDs[j] = fbLEDs[j].fadeToBlackBy(fadeAmt);
    }
};
