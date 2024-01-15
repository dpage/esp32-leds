#include <Arduino.h>
#include <WiFi.h>
#include "esp_wps.h"

#include "main.h"

#include "network.h"
#include "oled.h"

// WiFi
#define ESP_WPS_MODE WPS_TYPE_PBC
#define ESP_MANUFACTURER "ESPRESSIF"
#define ESP_MODEL_NUMBER "ESP32"
#define ESP_MODEL_NAME "ESPRESSIF IOT"
#define ESP_DEVICE_NAME "ESP STATION"

static esp_wps_config_t config;


// Get a hex string representation of a byte
String getHexByte(byte x)
{
    char hex[2] = "";
    ltoa(x, hex, 16);

    return (String(hex));
}

// Generate a hostname
String GetHostname()
{
    byte mac[6];
    String hostname = "leds-";

    WiFi.macAddress(mac);

    hostname += getHexByte(mac[3]);
    hostname += getHexByte(mac[4]);
    hostname += getHexByte(mac[5]);

    return hostname;
}


// Get the local IP address
String GetLocalIP()
{
    return WiFi.localIP().toString();
}

// Get the SSID
String GetSSID()
{
    return WiFi.SSID();
}


// Initialise the config for WPS
void wpsInitConfig()
{
    config.wps_type = ESP_WPS_MODE;
    strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
    strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
    strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
    strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}


// Start the WPS process
boolean wpsStart()
{
    int res = esp_wifi_wps_enable(&config);
    if (res)
    {
        Serial.printf("ESP32 : WPS enable failed: %d\n", res);
        return false;
    }

    res = esp_wifi_wps_start(0);
    if (res)
    {
        Serial.printf("ESP32 : WPS start failed: %d\n", res);
        return false;
    }

    return true;
}


// Stop the WPS process
void wpsStop()
{
    int res = esp_wifi_wps_disable();
    if (res)
    {
        Serial.printf("ESP32 : WPS disable failed: %d\n", res);
    }
}

// Kick off the WPS WiFi configuration
void wpsSetup()
{
    Serial.printf("ESP32 : Entering WPS Setup mode...\n");

    wpsInitConfig();
    wpsStart();
}


// Handle WiFi events
void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("ESP32 : Station Mode Started");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.printf("ESP32 : SSID: %s, hostname: %s, IP: %s\n",
                      WiFi.SSID(), GetHostname(), WiFi.localIP().toString());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("ESP32 : Disconnected from station, attempting reconnection");
        WiFi.reconnect();
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        Serial.println("ESP32 : WPS Successful, stopping WPS and connecting to: " + String(WiFi.SSID()));
        wpsStop();
        delay(10);
        WiFi.begin();
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        Serial.println("ESP32 : WPS Failed, retrying");
        wpsStop();
        wpsStart();
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        Serial.println("ESP32 : WPS Timeout, retrying");
        wpsStop();
        wpsStart();
        break;
    default:
        break;
    }
}


// Setup WiFi
void initWiFi()
{
    int countdown = 10;
    boolean wps = false;

    Serial.println("ESP32 : Initialising WiFi...");

    WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_STA);

    // WPS setup mode?
    if (digitalRead(BTN_MODE_PIN) == LOW)
    {
        wps = true;
        wpsSetup();
    }
    else
        WiFi.begin();

    while (WiFi.status() != WL_CONNECTED && (wps || countdown >= 0))
    {
        char msg1[64];
        char msg2[13];

        snprintf(msg1, 64, "SSID   : %s", WiFi.SSID());

        if (!wps)
            snprintf(msg2, 13, "Waiting: %d", countdown);
        else
            snprintf(msg2, 13, "Waiting: WPS", countdown);

        oledDisplay("WiFi Connecting...", msg1, msg2);

        delay(1000);
        countdown--;
    }
}
