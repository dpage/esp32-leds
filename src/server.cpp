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

    server.send(404, "text/plain", message);
}

// Home page
void handleRoot()
{
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
    webApp += "<p>Version: " + String(FIRMWARE_VERSION) + "</p>";
    webApp += "</center></body></html>";

    // Finish up
    server.send(200, "text/html", webApp);
}

// Setup page
void handleSetup()
{
    boolean reboot = false;

    // Handle arguments
    preferences.begin("ESP32-LEDs", false);
    if (server.hasArg("Leds") && atoi(server.arg("Leds").c_str()) != GetNumLeds())
    {
        SetNumLeds(atoi(server.arg("Leds").c_str()));
        reboot = true;
    }
    if (server.hasArg("Hostname") && server.arg("Hostname") != GetHostname())
    {
        preferences.putString("Hostname", server.arg("Hostname"));
        reboot = true;
    }
    if (server.hasArg("PwrMgmt") && atoi(server.arg("PwrMgmt").c_str()) != GetPowerManagement())
    {
        SetPowerManagement(atoi(server.arg("PwrMgmt").c_str()) ? true : false);
    }
    if (server.hasArg("MaxPower") && atoi(server.arg("MaxPower").c_str()) != GetMaxPower())
    {
        SetMaxPower(atoi(server.arg("MaxPower").c_str()));
    }
    if (server.hasArg("Brightness") && atoi(server.arg("Brightness").c_str()) != GetBrightness())
    {
        SetBrightness(atoi(server.arg("Brightness").c_str()));
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
        webApp += "<input type=\"text\" id=\"Leds\" name=\"Leds\" value=\"" + String(GetNumLeds()) + "\">";
        webApp += "<label for=\"Hostname\">Hostname:</label>";
        webApp += "<input type=\"text\" id=\"Hostname\" name=\"Hostname\" value=\"" + GetHostname() + "\">";
        webApp += "<label for=\"PwrMgmt\">Active Power Management:</label>";
        webApp += "<select name=\"PwrMgmt\" id=\"PwrMgmt\">";
        if (GetPowerManagement() == true)
        {
            webApp += "<option selected value=\"1\">Enabled</option>";
            webApp += "<option value=\"0\">Disabled</option>";
        }
        else
        {
            webApp += "<option value=\"1\">Enabled</option>";
            webApp += "<option selected value=\"0\">Disabled</option>";
        }
        webApp += "</select>";
        webApp += "<label for=\"MaxPower\">Maximum External Power (mW):</label>";
        webApp += "<input type=\"text\" id=\"MaxPower\" name=\"MaxPower\" value=\"" + String(GetMaxPower()) + "\">";
        webApp += "<label for=\"Brightness\">Brightness (0 - 255):</label>";
        webApp += "<input type=\"text\" id=\"Brightness\" name=\"Brightness\" value=\"" + String(GetBrightness()) + "\">";
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

    webApp += "<p>Version: " + String(FIRMWARE_VERSION) + "</p>";
    webApp += "</center></body></html>";

    // Finish up
    preferences.end();
    server.send(200, "text/html", webApp);

    if (reboot)
    {
        delay(3);
        ESP.restart();
    }
}


// The server loop
void ServerLoop(void *pvParameters)
{
    if (MDNS.begin(GetHostname().c_str()))
    {
        MDNS.addService("http", "tcp", 80);
    }

    for (;;)
    {
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
