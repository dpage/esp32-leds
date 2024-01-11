// LEDs

#include <WiFi.h>

#include "leds.h"

// Effect info
extern const char *effects;
extern int effectId;

// LED power management constants
extern int g_Brightness;
extern int g_PowerLimit; // 900mW Power Limit


#include "bounce.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

// The main rendering loop
void LedLoop(void *pvParameters)
{
    // Instantiate our effects
    BouncingBallEffect fxBalls(NUM_LEDS, 3, 0, false, 8.0);
    BouncingBallEffect fxBallsFade(NUM_LEDS, 3, 64, false, 8.0);
    BouncingBallEffect fxBallsMirror(NUM_LEDS, 3, 0, true, 8.0);
    BouncingBallEffect fxBallsMirrorFade(NUM_LEDS, 3, 64, true, 8.0);

    CometEffect fxComet;

    MarqueeEffect fxMarquee(false);
    MarqueeEffect fxMarqueeMirror(true);

    FireEffect fxFireOut(NUM_LEDS, 30, 100, 3, 2, false, true);
    FireEffect fxFireIn(NUM_LEDS, 30, 100, 3, 2, true, true);

    FireEffect fxFireXOut(NUM_LEDS, 50, 300, 30, 12, false, true);
    FireEffect fxFireXIn(NUM_LEDS, 50, 300, 30, 12, true, true);

    TwinkleEffect fxTwinkle;

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
        }

        // Display the frame
        FastLED.show(g_Brightness);
    }
}
