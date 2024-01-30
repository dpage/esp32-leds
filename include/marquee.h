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

#include "main.h"

extern CRGB fbLEDs[];

class MarqueeEffect
{
private:
    int _nLeds;
    bool _bMirrored;

public:
    MarqueeEffect(boolean bmirrored = false)
        : _nLeds(GetNumLeds()),
          _bMirrored(bmirrored)
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
                for (int i = 0; i < (_nLeds + 1) / 2; i++)
                {
                    fbLEDs[i] = c.setHue(k);
                    fbLEDs[_nLeds - 1 - i] = c.setHue(k);
                    k += 8;
                }
            }
            else
            {
                for (int i = 0; i < _nLeds; i++)
                    fbLEDs[i] = c.setHue(k += 8);
            }

            static int scroll = 0;
            scroll++;

            if (_bMirrored)
            {
                for (int i = scroll % 5; i < _nLeds / 2; i += 5)
                {
                    fbLEDs[i] = CRGB::Black;
                    fbLEDs[_nLeds - 1 - i] = CRGB::Black;
                }
            }
            else
            {
                for (int i = scroll % 5; i < _nLeds - 1; i += 5)
                {
                    fbLEDs[i] = CRGB::Black;
                }
            }

            delay(50);
    }
};
