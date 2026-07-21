#include "web_server.h"

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "web_api.h"
#include "../network/wifi_manager.h"

//------------------------------------------------------------

static AsyncWebServer server(80);

//------------------------------------------------------------

bool webServerInit()
{
    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("WEB SERVER");
    Serial.println("--------------------------------");

    //--------------------------------------------------------
    // LittleFS
    //--------------------------------------------------------

    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed.");

        return false;
    }

    Serial.println("LittleFS mounted.");

    //--------------------------------------------------------
    // Main page
    //--------------------------------------------------------

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(
                LittleFS,
                "/index.html",
                "text/html");
        });

    //--------------------------------------------------------
    // Static files
    //--------------------------------------------------------

    server.serveStatic("/", LittleFS, "/");

    //--------------------------------------------------------
    // 404
    //--------------------------------------------------------

    server.onNotFound(
        [](AsyncWebServerRequest *request)
        {
            request->send(
                404,
                "text/plain",
                "404 Not Found");
        });

        //--------------------------------------------------------
    // REST API
    //--------------------------------------------------------

    webApiInit(server);

    //--------------------------------------------------------

    server.begin();

    Serial.println("Web server started.");

    return true;
}

//------------------------------------------------------------

void webServerLoop()
{
    // AsyncWebServer
}