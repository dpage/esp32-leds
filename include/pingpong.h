#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

class PingPongEffect
{
private:
    int _nLeds;
    CHSV _colour = CHSV(0, 255, 255);
    int _pos = 0;
    boolean _reverse = false;

public:
    PingPongEffect(int leds)
        : _nLeds(leds)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        FastLED.leds()[_pos] = _colour;

        if (!_reverse)
        {
            _pos++;

            if (_pos >= _nLeds)
                _reverse = true;
        }
        else
        {
            _pos--;

            if (_pos == 0)
                _reverse = false;
        }

        // Next colour
        int hue = _colour.hue;
        if (hue == 255)
            hue = 0;
        else
            hue++;

        _colour.hue = hue;

        delay(20);
    }
};
