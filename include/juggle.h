#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

extern CRGB fbLEDs[];

class JuggleEffect
{
private:
    int _nLeds;

public:
    JuggleEffect(int leds)
        : _nLeds(leds)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        fadeToBlackBy(fbLEDs, _nLeds, 20);
        uint8_t dothue = 0;
        for (int i = 0; i < 8; i++)
        {
            fbLEDs[beatsin16(i + 7, 0, _nLeds - 1)] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
    }
};
