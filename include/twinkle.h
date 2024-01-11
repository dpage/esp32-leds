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
#include "ledgfx.h"

static const CRGB TwinkleColors [] = 
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow
};

class TwinkleEffect
{
public:
    void Draw()
    {
        static int passCount = 0;

        FastLED.clear();

        if (passCount++ == NUM_LEDS / 4)
        {
            passCount = 0;
            FastLED.clear(false);
        }
        FastLED.leds()[random(NUM_LEDS)] = TwinkleColors[random(0, ARRAYSIZE(TwinkleColors))];
        delay(50);
    }
};
