// LEDs

#include <Preferences.h>

#include "main.h"

#include "leds.h"

// Effect info
const char *effects[] = {
    "All Off",
    "Balls",
    "Balls Fade",
    "Balls Mirror",
    "Balls Mirror Fade",
    "Comet",
    "Marquee",
    "Marquee Mirror",
    "Fire Out",
    "Fire In",
    "FireX Out",
    "FireX In",
    "Twinkle",
    "Solid",
    "Ping Pong",
    "Rainbow Short",
    "Rainbow Full",
    "Rainbow Short Glitter",
    "Rainbow Full Glitter",
    "Confetti",
    "Juggle",
    "Beats"
};

int effectId = 0;

#include "balls.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"
#include "solid.h"
#include "pingpong.h"
#include "rainbow.h"
#include "confetti.h"
#include "juggle.h"
#include "beats.h"


int GetNumEffects()
{
    return sizeof(effects) / sizeof(effects[0]);
}

int GetEffectId()
{
    return effectId;
}


const char *GetEffectName()
{
    return effects[effectId];
}


const char *GetEffectNameById(int id)
{
    return effects[id];
}


void saveEffect() 
{
    Preferences preferences;

    preferences.begin("ESP32-LEDs", false);
    preferences.putInt("effectId", effectId);
    preferences.end();
}


void SetEffectId(int id) 
{
    effectId = id;
    saveEffect();
}


void NextEffect()
{
    effectId++;
    if (effectId >= GetNumEffects())
        effectId = 0;

    saveEffect();
}


// Brightness
int GetBrightness()
{
    Preferences preferences;
    int brightness;

    preferences.begin("ESP32-LEDs", true);
    brightness = preferences.getInt("Brightness", 255);
    preferences.end();

    return brightness;
}


void SetBrightness(int brightness)
{
    Preferences preferences;

    if (brightness < 0)
        brightness = 0;

    if (brightness > 255)
        brightness = 255;

    preferences.begin("ESP32-LEDs", false);
    preferences.putInt("Brightness", brightness);
    preferences.end();

    FastLED.setBrightness(brightness);
}


// Max Power
int GetMaxPower()
{
    Preferences preferences;
    int brightness;

    preferences.begin("ESP32-LEDs", true);
    brightness = preferences.getInt("MaxPower", 10000);
    preferences.end();

    return brightness;
}


void SetMaxPower(int power)
{
    Preferences preferences;

    preferences.begin("ESP32-LEDs", false);
    preferences.putInt("MaxPower", power);
    preferences.end();

    if (GetExternalPower())
        FastLED.setMaxPowerInMilliWatts(power);
}


// Initialise the LEDs
void initLEDs()
{
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(fbLEDs, GetNumLeds());

    // Load the last effect & brightness
    Preferences preferences;
    preferences.begin("ESP32-LEDs", true);
    effectId = preferences.getInt("effectId", 0);
    FastLED.setBrightness(preferences.getInt("Brightness", 255));
    preferences.end();

    // Clear everything
    FastLED.show();
}


// The main rendering loop
void LedLoop(void *pvParameters)
{
    // Instantiate our effects
    BallsEffect fxBalls(3, 0, false, 8.0);
    BallsEffect fxBallsFade(3, 64, false, 8.0);
    BallsEffect fxBallsMirror(3, 0, true, 8.0);
    BallsEffect fxBallsMirrorFade(3, 64, true, 8.0);

    CometEffect fxComet;

    MarqueeEffect fxMarquee(false);
    MarqueeEffect fxMarqueeMirror(true);

    FireEffect fxFireOut(30, 100, 3, 2, false, true);
    FireEffect fxFireIn(30, 100, 3, 2, true, true);
    FireEffect fxFireXOut(50, 300, 30, 12, false, true);
    FireEffect fxFireXIn(50, 300, 30, 12, true, true);

    TwinkleEffect fxTwinkle;

    SolidEffect fxSolid;

    PingPongEffect fxPingPong;

    RainbowEffect fxRainbowFull(true, false);
    RainbowEffect fxRainbowShort(false, false);
    RainbowEffect fxRainbowFullGlitter(true, true);
    RainbowEffect fxRainbowShortGlitter(false, true);

    ConfettiEffect fxConfetti;

    JuggleEffect fxJuggle;

    BeatsEffect fxBeats;

    for (;;)
    {
        // Draw the appropriate pattern
        switch (effectId)
        {
        case 0:
            FastLED.clear();
            break;
        case 1:
            fxBalls.Draw();
            break;
        case 2:
            fxBallsFade.Draw();
            break;
        case 3:
            fxBallsMirror.Draw();
            break;
        case 4:
            fxBallsMirrorFade.Draw();
            break;
        case 5:
            fxComet.Draw();
            break;
        case 6:
            fxMarquee.Draw();
            break;
        case 7:
            fxMarqueeMirror.Draw();
            break;
        case 8:
            fxFireOut.Draw();
            break;
        case 9:
            fxFireIn.Draw();
            break;
        case 10:
            fxFireXOut.Draw();
            break;
        case 11:
            fxFireXIn.Draw();
            break;
        case 12:
            fxTwinkle.Draw();
            break;
        case 13:
            fxSolid.Draw();
            break;
        case 14:
            fxPingPong.Draw();
            break;
        case 15:
            fxRainbowShort.Draw();
            break;
        case 16:
            fxRainbowFull.Draw();
            break;
        case 17:
            fxRainbowShortGlitter.Draw();
            break;
        case 18:
            fxRainbowFullGlitter.Draw();
            break;
        case 19:
            fxConfetti.Draw();
            break;
        case 20:
            fxJuggle.Draw();
            break;
        case 21:
            fxBeats.Draw();
            break;
        default:
            SetEffectId(0);
            break;
        }

        // Display the frame
        FastLED.show();
    }
}
