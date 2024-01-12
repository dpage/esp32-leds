// LEDs

void LedLoop(void *pvParameters);
int GetNumEffects();
int GetEffectId();
const char *GetEffectName();
const char *GetEffectNameById(int id);
void SetEffectId(int id);
void NextEffect();