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


class FireEffect
{
private:
    int     _iSize;
    int     _iCooling;
    int     _iSparks;
    int     _iSparkHeight;
    int     _iSparking;
    bool    _bReversed;
    bool    _bMirrored;

    byte  * heat;

    // When diffusing the fire upwards, these control how much to blend in from
    // the cells below (ie: downward neighbors).
    // You can tune these coefficients to control how quickly and smoothly the 
    // fire spreads.  
    static const byte BlendSelf = 2;
    static const byte BlendNeighbor1 = 3;
    static const byte BlendNeighbor2 = 2;
    static const byte BlendNeighbor3 = 1;
    static const byte BlendTotal = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);

public:
    
    // Lower sparking -> more flicker. Higher sparking -> more consistent flame
    FireEffect(int size, int cooling = 80, int sparking = 50, int sparks = 3, int sparkHeight = 4, bool reversed = true, bool mirrored = true) 
        : _iSize(size),
          _iCooling(cooling),
          _iSparks(sparks),
          _iSparkHeight(sparkHeight),
          _iSparking(sparking),
          _bReversed(reversed),
          _bMirrored(mirrored)
    {
        if (_bMirrored)
            _iSize = _iSize / 2;

        heat = new byte[size] { 0 };
    }

    ~FireEffect()
    {
        delete [] heat;
    }

    void Draw()
    {
        FastLED.clear();
        
        // First cool each cell by a little bit
        for (int i = 0; i < _iSize; i++)
            heat[i] = max(0L, heat[i] - random(0, ((_iCooling * 10) / _iSize) + 2));

        // Next drift heat up and diffuse it a little but
        for (int i = 0; i < _iSize; i++)
            heat[i] = (heat[i] * BlendSelf +
                       heat[(i + 1) % _iSize] * BlendNeighbor1 +
                       heat[(i + 2) % _iSize] * BlendNeighbor2 +
                       heat[(i + 3) % _iSize] * BlendNeighbor3) /
                      BlendTotal;

        // Randomly ignite new sparks down in the flame kernel
        for (int i = 0; i < _iSparks; i++)
        {
            if (random(255) < _iSparking)
            {
                int y = _iSize - 1 - random(_iSparkHeight);
                heat[y] = heat[y] + random(160, 255); // This randomly rolls over sometimes of course, and that's essential to the effect
            }
        }

        // Finally convert heat to a color
        for (int i = 0; i < _iSize; i++)
        {
            CRGB color = HeatColor(heat[i]);
            int j = _bReversed ? (_iSize - 1 - i) : i;
            DrawPixels(j, 1, color);
            if (_bMirrored)
            {
                int j2 = !_bReversed ? (2 * _iSize - 1 - i) : _iSize + i;
                DrawPixels(j2, 1, color);
            }
        }
    }
};
