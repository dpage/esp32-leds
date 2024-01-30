#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

extern CRGB fbLEDs[];

class BeatsEffect
{
private:
    int _nLeds;
    uint8_t _uHue = 0;

public:
    BeatsEffect(int leds)
        : _nLeds(leds)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        uint8_t BeatsPerMinute = 62;

        CRGBPalette16 palette = PartyColors_p;
        uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
        for (int i = 0; i < _nLeds; i++)
        {
            fbLEDs[i] = ColorFromPalette(palette, _uHue + (i * 2), beat - _uHue + (i * 10));
        }
    }
};
