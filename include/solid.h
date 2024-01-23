#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

class SolidEffect
{
private:
    int _nLeds;
    CHSV _colour = CHSV(0, 255, 255);

public:
    SolidEffect(int leds)
        : _nLeds(leds)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        for (int i = 0; i < _nLeds; i++)
        {
            FastLED.leds()[i] = _colour;
        }

        // Next colour
        int hue = _colour.hue;
        if (hue == 255)
            hue = 0;
        else
            hue++;

        _colour.hue = hue;
    }
};
