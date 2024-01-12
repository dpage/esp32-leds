// Webserver

#include <Arduino.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <ESPmDNS.h>

#include "server.h"
#include "secrets.h"

#include "leds.h"

WebServer server(80);


// 404's
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

// Home page
void handleRoot()
{
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

// Change the effect
void handleEffect()
{
    int effectId = server.pathArg(0).toInt();

    if (effectId >= GetNumEffects())
        handleNotFound();

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
    server.send(200, "text/html", webApp);
}

// The server loop
void ServerLoop(void *pvParameters)
{
    if (MDNS.begin(WIFI_HOSTNAME))
    {
        MDNS.addService("http", "tcp", 80);
    }

    for (;;)
    {
        server.enableCORS();
        server.on("/", handleRoot);
        server.on(UriBraces("/e/{}"), handleEffect);
        server.onNotFound(handleNotFound);
        server.begin();

        for (;;)
        {
            server.handleClient();
            // Add a small delay to let the watchdog process
            // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
            delay(25);
        }
    }
}