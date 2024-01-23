// Webserver

#include <Arduino.h>
#include <WebServer.h>
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

    // Handle arguments
    if (server.hasArg("Effect") && atoi(server.arg("Effect").c_str()) != GetEffectId())
        SetEffectId(atoi(server.arg("Effect").c_str()));

    // Render page
    String webApp = "<html color-mode=\"user\">";
    webApp += "<head><title>ESP32 LEDs</title>";
    webApp += "<link rel=\"stylesheet\" href=\"https://unpkg.com/mvp.css\">";
    webApp += "</head><body style=\"margin: 25px;\"><center>";
    webApp += "<h1>Current effect: " + String(GetEffectName()) + "</h1>";

    webApp += "<form method=\"POST\">";
    webApp += "<label for=\"Effect\">Select an effect:</label>";
    webApp += "<select name=\"Effect\" id=\"Effect\">";
    for (int i = 0; i < GetNumEffects(); i++)
    {
        if (i == GetEffectId())
            webApp += "<option selected value=\"" + String(i) + "\">" + String(GetEffectNameById(i)) + "</option>";
        else
            webApp += "<option value=\"" + String(i) + "\">" + String(GetEffectNameById(i)) + "</option>";
    }
    webApp += "</select>";
    webApp += "<input type = \"submit\" value = \"Submit\">";
    webApp += "</form>";

    webApp += "<p><a href=\"/setup\">Setup</a></p>";
    webApp += "</center></body></html>";

    // Finish up
    server.send(200, "text/html", webApp);
}

// Setup page
void handleSetup()
{
    boolean reboot = false;

    Serial.print(String("Server: ") + getRequestMsg(200));

    // Handle arguments
    preferences.begin("ESP32-LEDs", false);
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

    // Render page
    String webApp = "<html color-mode=\"user\">";
    webApp += "<head><title>ESP32 LEDs - Setup</title>";
    webApp += "<link rel=\"stylesheet\" href=\"https://unpkg.com/mvp.css\">";

    if (reboot)
        webApp += "<meta http-equiv=\"refresh\" content=\"10\">";

    webApp += "</head><body style=\"margin: 25px;\"><center>";
    webApp += "<h1>ESP32 LEDs - Setup</h1>";

    if (!reboot) 
    {
        webApp += "<form method=\"POST\">";
        webApp += "<label for=\"Leds\">Number of LEDs:</label>";
        webApp += "<input type=\"text\" id=\"Leds\" name=\"Leds\" value=\"" + String(g_NumLeds) + "\">";
        webApp += "<label for=\"Hostname\">Hostname:</label>";
        webApp += "<input type=\"text\" id=\"Hostname\" name=\"Hostname\" value=\"" + GetHostname() + "\">";
        webApp += "<input type = \"submit\" value = \"Submit\">";
        webApp += "</form>";
    }

    if (reboot)
    {
        webApp += "<h2>Rebooting...</h2>";
        webApp += "<p>This page will reload in 10 seconds.</p>";
    }
    else
        webApp += "<p><a href=\"/\">Home</a></p>";
    webApp += "</center></body></html>";

    // Finish up
    preferences.end();
    server.send(200, "text/html", webApp);

    if (reboot)
    {
        delay(3);
        Serial.println("Server: Rebooting to apply configuration changes...");
        ESP.restart();
    }
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
