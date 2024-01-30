#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

extern CRGB fbLEDs[];

class RainbowEffect
{
private:
    int _nLeds;
    boolean _bCircular;
    boolean _bGlitter;

public:
    RainbowEffect(boolean circular, boolean glitter)
        : _nLeds(GetNumLeds()),
          _bCircular(circular),
          _bGlitter(glitter)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        uint8_t hue = beat8(15, 255);

        if (_bCircular)
            fill_rainbow_circular(fbLEDs, _nLeds, hue);
        else
            fill_rainbow(fbLEDs, _nLeds, hue, 10);

        if (_bGlitter)
        {
            if (random8() < 80)
            {
                fbLEDs[random16(_nLeds)] += CRGB::White;
            }
        }
    }
};
