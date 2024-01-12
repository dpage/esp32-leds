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
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>

#include "main.h"
#include "secrets.h"

#include "leds.h"
#include "server.h"


// Threads
TaskHandle_t tLeds;
TaskHandle_t tServer;

// Frame buffer for FastLED
CRGB g_LEDs[NUM_LEDS] = {0};

// OLED display on the heltec_wifi_kit_32_V3
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight10 = 0;
int g_lineHeight12 = 0;
int g_lineHeight15 = 0;
int g_width = 0;
int g_height = 0;

// LED power management constants
int g_Brightness = 255;
int g_PowerLimit = 900; // 900mW Power Limit

// Button state tracking
boolean btnModeState = 0;
boolean lastBtnModeState = 0;


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
            {
                NextEffect(); 
                Serial.printf("ESP32 : Next effect button pushed. Now running: %s\n", GetEffectName());
            }

            // Save the current state as the last state, for next time through the loop
            lastBtnModeState = btnModeState;
        }
    }
}


// Setup WIFI
void initWiFi()
{
    int countdown = 10;

    Serial.printf("ESP32 : Initialising WIFI... ");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED && countdown >= 0)
    {
        int linePos = 0;

        g_OLED.clearBuffer();

        g_OLED.setFont(u8g2_font_profont15_tf);

        linePos = g_lineHeight15;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("WIFI Connecting...");

        linePos = linePos + 1;
        g_OLED.drawHLine(0, linePos, g_width);

        g_OLED.setFont(u8g2_font_profont12_tf);

        linePos = linePos + g_lineHeight12 + 3;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("SSID   : %s", WIFI_SSID);

        linePos = linePos + g_lineHeight12;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("Waiting: %d", countdown);

        g_OLED.sendBuffer();

        Serial.printf("%d ", countdown);

        delay(1000);
        countdown--;
    }

    Serial.printf("\nESP32 : SSID: %s, hostname: %s, IP: %s\n", WIFI_SSID, WIFI_HOSTNAME, WiFi.localIP().toString());
}


// Setup the microcontroller
void setup()
{
    // Serial
    Serial.begin(115200);
    while (!Serial) { }
    Serial.printf("ESP32 : Startup, firmware version %s.\n", FIRMWARE_VERSION);
    Serial.printf("ESP32 : Configured for %d LEDs on pin %d.\n", NUM_LEDS, LED_PIN);

    // Setup the GPIO pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BTN_MODE_PIN, INPUT_PULLUP);

    // Setup the OLED screen
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont10_tf);
    g_lineHeight10 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_OLED.setFont(u8g2_font_profont12_tf);
    g_lineHeight12 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_OLED.setFont(u8g2_font_profont15_tf);
    g_lineHeight15 = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    g_width = g_OLED.getWidth();
    g_height = g_OLED.getHeight();

    // Network
    initWiFi();

    // Setup the LED strip and power management (warning LED and throttling)
    // Note: Power management should be removed if using an external PSU for
    //       the LED strips. This is just to prevent blowing out USB power.
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);
    FastLED.setBrightness(g_Brightness);
    set_max_power_indicator_LED(LED_BUILTIN);
    FastLED.setMaxPowerInMilliWatts(g_PowerLimit);

    // Setup for OTA
    ArduinoOTA.setHostname(WIFI_HOSTNAME);
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
    // Update the OLED display
    EVERY_N_MILLISECONDS(250)
    {
        int linePos = 0;

        g_OLED.clearBuffer();

        g_OLED.setFont(u8g2_font_profont15_tf);

        linePos = g_lineHeight15;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("%s", GetEffectName());

        linePos = linePos + 1;
        g_OLED.drawHLine(0, linePos, g_width);

        g_OLED.setFont(u8g2_font_profont10_tf);

        linePos = linePos + g_lineHeight10 + 3;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("FPS  : %u", FastLED.getFPS());

        linePos = linePos + g_lineHeight10;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));

        linePos = linePos + g_lineHeight10;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));

        linePos = linePos + g_lineHeight10;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("IP   : %s", WiFi.localIP().toString());

        linePos = linePos + g_lineHeight10;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("mDNS : %s.local", WIFI_HOSTNAME);

        linePos = linePos + g_lineHeight10;
        g_OLED.setCursor(0, linePos);
        g_OLED.printf("Ver  : %s", FIRMWARE_VERSION);

        g_OLED.sendBuffer();
    }

    // Have we had any button presses?
    checkButtons();

    // OTA?
    ArduinoOTA.handle();
}


