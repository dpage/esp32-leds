//+--------------------------------------------------------------------------
//
// Original copyright (https://github.com/davepl/DavesGarageLEDSeries):
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// Later modified by Dave Page.
//
//+--------------------------------------------------------------------------

#include <sys/time.h> // For time-of-day

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

using namespace std;
#include <vector> // Use the C++ resizable array

#include "main.h"
#include "ledgfx.h"

extern CRGB fbLEDs[];

// Draws a set of N bouncing balls using a simple little kinematics formula.  
static const CRGB ballColors[] =
{
    CRGB::Green,
    CRGB::Red,
    CRGB::Blue,
    CRGB::Orange,
    CRGB::Indigo,
    CRGB::Cyan
};

class BallsEffect
{
private:
    double InitialBallSpeed(double height) const
    {
        return sqrt(-2 * Gravity * height);
    }

    size_t _cLength;
    size_t _cBalls;
    byte _byteFade;
    bool _bMirrored;
    double _dSpeedKnob;

    const double Gravity = -9.81;   // Because PHYSICS!
    const double StartHeight = 1;   // Drop balls from max height to start

    vector<double> _ClockTimeAtLastBounce, _Height, _BallSpeed, _Dampening; // When the ball last bounced

    // Return current time in floating form for easier calcs than ms
    double Time() const
    {
        timeval tv = {0};
        gettimeofday(&tv, nullptr);
        return (double)(tv.tv_usec / (double)1000000 + (double)tv.tv_sec);
    }

public:
    BallsEffect(size_t ballCount = 3, byte fade = 0, bool mirrored = false, double speedKnob = 4.0)
        : _cLength(GetNumLeds() - 1), // Reserve one LED for floating point fraction draw
          _cBalls(ballCount),
          _byteFade(fade),
          _bMirrored(mirrored),
          _dSpeedKnob(speedKnob),
          _ClockTimeAtLastBounce(ballCount),
          _Height(ballCount),
          _BallSpeed(ballCount),
          _Dampening(ballCount)
    {

        for (size_t i = 0; i < ballCount; i++)
        {
            _Height[i] = StartHeight;                    // Current ball height
            _ClockTimeAtLastBounce[i] = Time();          // When the last time it hit ground was
            _Dampening[i] = 1.0 - i / pow(_cBalls, 2);   // Each ball bounces differently
            _BallSpeed[i] = InitialBallSpeed(_Height[i]); // Don't dampen initial launch to they go together
        }
    }

    // Draw each of the balls.  When any ball gets too little energy it would 
    // just sit at the base so it is re-kicked with new energy.
    void Draw()
    {
        if (_byteFade)
        {
            for (size_t i = 0; i < _cLength; i++)
            {
                fbLEDs[i].fadeToBlackBy(_byteFade);
            }
        }
        else
        {
            FastLED.clear();
        }

        // Draw each of the three balls
        for (size_t i = 0; i < _cBalls; i++)
        {
            double TimeSinceLastBounce = (Time() - _ClockTimeAtLastBounce[i]) / _dSpeedKnob;
            _Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce, 2.0) + _BallSpeed[i] * TimeSinceLastBounce;

            if (_Height[i] < 0)
            {
                _Height[i] = 0;
                _BallSpeed[i] = _Dampening[i] * _BallSpeed[i];
                _ClockTimeAtLastBounce[i] = Time();

                if (_BallSpeed[i] < 1.0)
                    _BallSpeed[i] = InitialBallSpeed(StartHeight) * _Dampening[i];
            }

            static const CRGB ballColors[] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Orange, CRGB::Violet};
            CRGB color = ballColors[i % ARRAYSIZE(ballColors)];

            double position = (_Height[i] * (_cLength - 1.0) / StartHeight);
            DrawPixels(position, 1, color);
            if (_bMirrored)
                DrawPixels(_cLength - 1 - position, 1, color);
        }
        delay(20);
    }
};
