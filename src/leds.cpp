// LEDs

#include <Preferences.h>

#include "main.h"

#include "leds.h"

// Effect info
const char *effects[] = {
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
    "Rainbow Solid",
    "Rainbow Ping Pong",
    "All Off"};
int effectId = 0;

#include "bounce.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"
#include "solid.h"
#include "pingpong.h"

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

    Serial.printf("Server: Saving last effect: %s\n", GetEffectName());

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


// Initialise the LEDs
void initLEDs()
{
    // Setup the LED strip and power management (warning LED and throttling)
    // Note: Power management should be removed if using an external PSU for
    //       the LED strips. This is just to prevent blowing out USB power.
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, g_NumLeds);
    FastLED.setBrightness(255);

    // Load the last effect
    Preferences preferences;
    preferences.begin("ESP32-LEDs", true);
    effectId = preferences.getInt("effectId", 0);
    preferences.end();

    // Clear everything
    FastLED.show();

    Serial.printf("Server: Loaded last effect: %s\n", GetEffectName());
}


// The main rendering loop
void LedLoop(void *pvParameters)
{
    // Instantiate our effects
    BouncingBallEffect fxBalls(g_NumLeds, 3, 0, false, 8.0);
    BouncingBallEffect fxBallsFade(g_NumLeds, 3, 64, false, 8.0);
    BouncingBallEffect fxBallsMirror(g_NumLeds, 3, 0, true, 8.0);
    BouncingBallEffect fxBallsMirrorFade(g_NumLeds, 3, 64, true, 8.0);

    CometEffect fxComet(g_NumLeds);

    MarqueeEffect fxMarquee(g_NumLeds, false);
    MarqueeEffect fxMarqueeMirror(g_NumLeds, true);

    FireEffect fxFireOut(g_NumLeds, 30, 100, 3, 2, false, true);
    FireEffect fxFireIn(g_NumLeds, 30, 100, 3, 2, true, true);

    FireEffect fxFireXOut(g_NumLeds, 50, 300, 30, 12, false, true);
    FireEffect fxFireXIn(g_NumLeds, 50, 300, 30, 12, true, true);

    TwinkleEffect fxTwinkle(g_NumLeds);

    SolidEffect fxSolid(g_NumLeds);

    PingPongEffect fxPingPong(g_NumLeds);

    for (;;)
    {
        // Draw the appropriate pattern
        switch (effectId)
        {
        case 0:
            fxBalls.Draw();
            break;
        case 1:
            fxBallsFade.Draw();
            break;
        case 2:
            fxBallsMirror.Draw();
            break;
        case 3:
            fxBallsMirrorFade.Draw();
            break;
        case 4:
            fxComet.Draw();
            break;
        case 5:
            fxMarquee.Draw();
            break;
        case 6:
            fxMarqueeMirror.Draw();
            break;
        case 7:
            fxFireOut.Draw();
            break;
        case 8:
            fxFireIn.Draw();
            break;
        case 9:
            fxFireXOut.Draw();
            break;
        case 10:
            fxFireXIn.Draw();
            break;
        case 11:
            fxTwinkle.Draw();
            break;
        case 12:
            fxSolid.Draw();
            break;
        case 13:
            fxPingPong.Draw();
            break;
        case 14:
            FastLED.clear();
            break;
        default:
            SetEffectId(0);
            break;
        }

        // Display the frame
        FastLED.show();
    }
}
