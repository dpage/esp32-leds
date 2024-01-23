// Webserver

#include <Arduino.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include "main.h"

#include "server.h"
#include "network.h"
#include "leds.h"

WebServer server(80);
Preferences preferences;

// Format a message for debug/display
String getRequestMsg(int status)
{
    String msg;

    msg += String(status) + ": ";
    msg += String((server.method() == HTTP_GET) ? "GET" : "POST") + " ";
    msg += server.uri() + "\n";

    return msg;
}

// 404's
void handleNotFound()
{
    String message = getRequestMsg(404);

    Serial.print(String("Server: ") + message);
    server.send(404, "text/plain", message);
}

// Home page
void handleRoot()
{
    Serial.print(String("Server: ") + getRequestMsg(200));

    String webApp = "<html>";
    webApp += "<head><title>ESP32 LEDs</title>";
    webApp += "</head><body>";
    webApp += "<h1>Current effect: " + String(GetEffectName()) + "</h1>";

    for (int i = 0; i < GetNumEffects(); i++)
    {
        if (i == GetEffectId())
            webApp += "<b><a href=\"/e/" + String(i) + "\">" + String(GetEffectNameById(i)) + "</b><br/>";
        else
            webApp += "<a href=\"/e/" + String(i) + "\">" + String(GetEffectNameById(i)) + "<br/>";
    }

    webApp += "</body></html>";

    server.send(200, "text/html", webApp);
}

// Setup page
void handleSetup()
{
    boolean reboot = false;

    Serial.print(String("Server: ") + getRequestMsg(200));

    // Handle arguments
    preferences.begin("ESP32-LEDs", false);
    for (uint8_t i = 0; i < server.args(); i++)
    {
        if (server.hasArg("Leds") && atoi(server.arg("Leds").c_str()) != g_NumLeds)
        {
            Serial.printf("Server: Setting LED count to: %d\n", atoi(server.arg("Leds").c_str()));
            preferences.putInt("Leds", atoi(server.arg("Leds").c_str()));
            g_NumLeds = atoi(server.arg("Leds").c_str());
            reboot = true;
        }
        if (server.hasArg("Hostname") && server.arg("Hostname") != GetHostname())
        {
            Serial.printf("Server: Setting hostname to: %d\n", server.arg("Hostname").c_str());
            preferences.putString("Hostname", server.arg("Hostname"));
            reboot = true;
        }
    }

    String webApp = "<html>";
    webApp += "<head><title>ESP32 LEDs - Setup</title>";

    if (reboot)
        webApp += "<meta http-equiv=\"refresh\" content=\"10\">";

    webApp += "</head><body>";
    webApp += "<h1>ESP32 LEDs - Setup</h1>";

    webApp += "<form method=\"POST\">";
    webApp += "<label for=\"Leds\">Number of LEDs:</label><br>";
    webApp += "<input type=\"text\" id=\"Leds\" name=\"Leds\" value=\"" + String(g_NumLeds) + "\"><br>";
    webApp += "<label for=\"Hostname\">Hostname:</label><br>";
    webApp += "<input type=\"text\" id=\"Hostname\" name=\"Hostname\" value=\"" + GetHostname() + "\"><br>";
    webApp += "<input type = \"submit\" value = \"Submit\">";
    webApp += "</form>";

    if (reboot)
        webApp += "<h2>Rebooting...</h2>";

    webApp += "</body></html>";

    preferences.end();

    server.send(200, "text/html", webApp);
    if (reboot)
    {
        delay(3);
        Serial.println("Server: Rebooting to apply configuration changes...");
        ESP.restart();
    }
}


// Change the effect
void handleEffect()
{
    int effectId = server.pathArg(0).toInt();

    if (effectId >= GetNumEffects())
    {
        handleNotFound();
        return;
    }

    SetEffectId(effectId);

    String webApp = "<html>";
    webApp += "<head><title>ESP32 LEDs</title>";
    webApp += "</head><body>";
    webApp += "<h1>Effect set to: " + String(GetEffectName()) + "</h1>";

    for (int i = 0; i < GetNumEffects(); i++)
    {
        if (i == GetEffectId())
            webApp += "<b><a href=\"/e/" + String(i) + "\">" + String(GetEffectNameById(i)) + "</b><br/>";
        else
            webApp += "<a href=\"/e/" + String(i) + "\">" + String(GetEffectNameById(i)) + "<br/>";
    }

    webApp += "</body></html>";

    Serial.print(String("Server: ") + getRequestMsg(200));
    Serial.printf("Server: Effect link clicked. Now running: %s\n", GetEffectName());
    server.send(200, "text/html", webApp);
}

// The server loop
void ServerLoop(void *pvParameters)
{
    Serial.printf("Server: Starting mDNS with hostname: %s.local...\n", GetHostname());
    if (MDNS.begin(GetHostname().c_str()))
    {
        MDNS.addService("http", "tcp", 80);
    }

    for (;;)
    {
        Serial.printf("Server: Starting webserver...\n");
        server.enableCORS();
        server.on("/", handleRoot);
        server.on("/setup", handleSetup);
        server.on(UriBraces("/e/{}"), handleEffect);
        server.onNotFound(handleNotFound);
        server.begin();

        for (;;)
        {
            server.handleClient();
            // Add a small delay to let the watchdog process
            // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
            delay(2);
        }
    }
}
