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
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

class MarqueeEffect
{
private:
    bool _bMirrored;

public:
    MarqueeEffect(boolean bmirrored = false)
        : _bMirrored(bmirrored)
    {
    }

    virtual void Draw()
        {
            static byte j = 0;
            j += 4;
            byte k = j;

            CRGB c;

            FastLED.clear();

            if (_bMirrored)
            {
                for (int i = 0; i < (NUM_LEDS + 1) / 2; i++)
                {
                    g_LEDs[i] = c.setHue(k);
                    g_LEDs[NUM_LEDS - 1 - i] = c.setHue(k);
                    k += 8;
                }
            }
            else
            {
                for (int i = 0; i < NUM_LEDS; i++)
                    g_LEDs[i] = c.setHue(k += 8);
            }

            static int scroll = 0;
            scroll++;

            if (_bMirrored)
            {
                for (int i = scroll % 5; i < NUM_LEDS / 2; i += 5)
                {
                    g_LEDs[i] = CRGB::Black;
                    g_LEDs[NUM_LEDS - 1 - i] = CRGB::Black;
                }
            }
            else
            {
                for (int i = scroll % 5; i < NUM_LEDS - 1; i += 5)
                {
                    g_LEDs[i] = CRGB::Black;
                }
            }

            delay(50);
    }
};
