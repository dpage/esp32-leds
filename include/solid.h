#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

class SolidEffect
{
private:
    int _nLeds;
    CRGB _colour;

public:
    SolidEffect(int leds, CRGB colour)
        : _nLeds(leds),
          _colour(colour)
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
    }
};
