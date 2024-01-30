#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "main.h"

extern CRGB fbLEDs[];

class ConfettiEffect
{
private:
    int _nLeds;
    uint8_t _uHue = 0;

public:
    ConfettiEffect(int leds)
        : _nLeds(leds)
    {
    }

public:
    void Draw()
    {
        FastLED.clear();

        EVERY_N_MILLISECONDS(20) 
        { 
            _uHue++; 
        }

        fadeToBlackBy(fbLEDs, _nLeds, 10);
        int pos = random16(_nLeds);
        fbLEDs[pos] += CHSV(_uHue + random8(64), 255, 255);

        delay(50);
    }
};
