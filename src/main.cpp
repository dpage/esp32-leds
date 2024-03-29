//+--------------------------------------------------------------------------
//
// Original copyright (https://github.com/davepl/DavesGarageLEDSeries):
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// Later modified by Dave Page.
//
//+--------------------------------------------------------------------------

#include <Arduino.h>
#include <ArduinoOTA.h>

#define FASTLED_INTERNAL
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <Preferences.h>

#include "main.h"

#include "oled.h"
#include "network.h"
#include "leds.h"
#include "server.h"


// Threads
TaskHandle_t tLeds;
TaskHandle_t tServer;

// Frame buffer for FastLED
int nLeds = DEFAULT_NUM_LEDS;
CRGB fbLEDs[DEFAULT_NUM_LEDS] = {0};

// Power management
boolean bExternalPower = false;
boolean bPowerManagement = true;

// Button state tracking
boolean btnModeState = 0;
boolean lastBtnModeState = 0;


// Access to the fframe buffer

// How many LEDs?
int GetNumLeds()
{
    return nLeds;
}


void SetNumLeds(int leds)
{
    Preferences preferences;
    preferences.begin("ESP32-LEDs", false);
    preferences.putInt("Leds", leds);
    preferences.end();

    nLeds = leds;
}


// Are we in power management mode?
boolean GetPowerManagement()
{
    return bPowerManagement;
}


void SetPowerManagement(boolean enabled)
{
    Preferences preferences;
    preferences.begin("ESP32-LEDs", false);
    preferences.putBool("PwrMgmt", enabled);
    preferences.end();

    bPowerManagement = enabled;
}


// Are we on external power?
boolean GetExternalPower()
{
    return bExternalPower;
}


// Check to see if we're running on external power or not, and enable safety
// measures as appropriate
void checkPowerState()
{
    if (bPowerManagement)
    {
        boolean externalPower = digitalRead(EPWR_PIN);

        if (externalPower == bExternalPower)
            return;

        if (externalPower == LOW)
        {
            bExternalPower = false;
            set_max_power_indicator_LED(LED_BUILTIN);
            FastLED.setMaxPowerInMilliWatts(900);
        }
        else
        {
            digitalWrite(LED_BUILTIN, LOW);
            bExternalPower = true;
            set_max_power_indicator_LED(0);
            FastLED.setMaxPowerInMilliWatts(GetMaxPower());
        }
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        bExternalPower = true;
        set_max_power_indicator_LED(0);
        FastLED.setMaxPowerInMilliWatts(GetMaxPower());
    }
}


// Check and update the button state
void checkButtons()
{
    static unsigned long timer = 0;
    unsigned long interval = 20;
    if (millis() - timer >= interval)
    {
        timer = millis();

        btnModeState = digitalRead(BTN_MODE_PIN);

        // Change in state? If so, and the new state is low, we have a push.
        if (btnModeState != lastBtnModeState)
        {
            if (btnModeState == LOW)
                NextEffect(); 

            // Save the current state as the last state, for next time through the loop
            lastBtnModeState = btnModeState;
        }
    }
}


// Setup the microcontroller
void setup()
{
    delay(5);

    // Number of LEDs
    Preferences preferences;
    preferences.begin("ESP32-LEDs", true);
    nLeds = preferences.getInt("Leds", DEFAULT_NUM_LEDS);
    bPowerManagement = preferences.getBool("PwrMgmt", bPowerManagement);
    preferences.end();

    // Serial
    Serial.begin(115200);
    while (!Serial) { }

    // Setup the GPIO pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BTN_MODE_PIN, INPUT_PULLUP);
    pinMode(EPWR_PIN, INPUT_PULLDOWN);

    initOLED();

    // Pause for WPS setup
    int countdown = 3;
    while (countdown >= 0)
    {
        char msg[12];
        snprintf(msg, 12, "Waiting: %d", countdown);

        oledDisplay("Hold PRG for WPS", msg);

        delay(1000);
        countdown--;
    }

    // Network
    initWiFi();

    // LEDs
    checkPowerState();
    initLEDs();

    // Setup for OTA
    ArduinoOTA.setHostname(GetHostname().c_str());
    ArduinoOTA
        .onStart([]()
                 {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("ESP32 : Start updating " + type); })
        .onEnd([]()
               { Serial.println("\nESP32 : End"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("ESP32 : Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)
          Serial.println("ESP32 : Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
          Serial.println("ESP32 : Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
          Serial.println("ESP32 : Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
          Serial.println("ESP32 : Receive Failed");
      else if (error == OTA_END_ERROR)
          Serial.println("ESP32 : End Failed"); });

    ArduinoOTA.begin();

    // Create the threads
    xTaskCreatePinnedToCore(LedLoop, "LEDs", 10000, NULL, 1, &tLeds, 0);
    xTaskCreatePinnedToCore(ServerLoop, "Server", 10000, NULL, 1, &tServer, 1);
}


// User interaction handled here.
void loop()
{
    // Check power supply state
    checkPowerState();

    // Update the OLED display
    EVERY_N_MILLISECONDS(250)
    {
        char msg1[64], msg2[64], msg3[64], msg4[64], msg5[64], msg6[64];

        snprintf(msg1, 64, "Stats: %u LEDs, %ufps",
                 nLeds,
                 FastLED.getFPS());
                 
        if (bPowerManagement)
            snprintf(msg2, 64, "PWR  : %s, %umW",
                     bExternalPower ? "External" : "USB",
                     calculate_unscaled_power_mW(fbLEDs, 4));
        else
            snprintf(msg2, 64, "PWR  : Unmanaged, %umW",
                     calculate_unscaled_power_mW(fbLEDs, 4));

        snprintf(msg3, 64, "SSID : %s", GetSSID());
        snprintf(msg4, 64, "IP   : %s", GetLocalIP());
        snprintf(msg5, 64, "mDNS : %s.local", GetHostname());
        snprintf(msg6, 64, "Ver  : %s", FIRMWARE_VERSION);

        oledDisplay(GetEffectName(), msg1, msg2, msg3, msg4, msg5, msg6);
    }

    // Have we had any button presses?
    checkButtons();

    // OTA?
    ArduinoOTA.handle();
}
