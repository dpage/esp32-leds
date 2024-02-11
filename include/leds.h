#ifndef _leds_h
#define _leds_h

// LEDs

void initLEDs();
void LedLoop(void *pvParameters);
int GetNumEffects();
int GetEffectId();
const char *GetEffectName();
const char *GetEffectNameById(int id);
void SetEffectId(int id);
void NextEffect();
int GetBrightness();
void SetBrightness(int brightness);
int GetMaxPower();
void SetMaxPower(int power);

#endif