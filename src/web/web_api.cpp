#include "web_api.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../storage/preferences_manager.h"
#include "../storage/meteo_log.h"
#include "../clock/timezones_db.h"
#include "../gui/screen_main.h"
#include "../sensor/bme_manager.h"
#include "../display.h"

//------------------------------------------------------------
// Local functions
//------------------------------------------------------------

static void sendJson(
    AsyncWebServerRequest *request,
    JsonDocument &doc)
{
    String json;

    serializeJson(doc, json);

    request->send(
        200,
        "application/json",
        json);
}

//------------------------------------------------------------

static void sendError(
    AsyncWebServerRequest *request,
    int code,
    const char *message)
{
    JsonDocument doc;

    doc["status"] = "error";
    doc["message"] = message;

    String json;

    serializeJson(doc, json);

    request->send(
        code,
        "application/json",
        json);
}

//------------------------------------------------------------

static void sendOk(
    AsyncWebServerRequest *request)
{
    JsonDocument doc;

    doc["status"] = "ok";

    sendJson(request, doc);
}

//============================================================
// REST API
//============================================================

void webApiInit(AsyncWebServer &server)
{
   
    //--------------------------------------------------------
    // Health
    //--------------------------------------------------------

    server.on(
        "/api/ping",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", "OK");
        });

    //--------------------------------------------------------
    // System
    //--------------------------------------------------------

    server.on(
        "/api/system",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["chipModel"] = ESP.getChipModel();
            doc["chipRevision"] = ESP.getChipRevision();
            doc["cpuMHz"] = ESP.getCpuFreqMHz();
            doc["flashSize"] = ESP.getFlashChipSize();
            doc["freeHeap"] = ESP.getFreeHeap();
            doc["freePsram"] = ESP.getFreePsram();
            doc["meteoCount"] = meteoLogCount();
            doc["meteoCapacity"] = meteoLogCapacity();

            sendJson(request, doc);
        });

    //--------------------------------------------------------
    // Timezones
    //--------------------------------------------------------

    server.on(
        "/api/timezones",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;
            JsonArray arr = doc.to<JsonArray>();

            for (uint16_t i = 0; i < timeZonesCount; i++)
            {
                JsonObject obj = arr.add<JsonObject>();
                obj["index"] = i;
                obj["nameEN"] = timeZones[i].nameEN;
                obj["nameUA"] = timeZones[i].nameUA;
                obj["utcHour"] = timeZones[i].utcHour;
                obj["utcMinute"] = timeZones[i].utcMinute;
            }

            sendJson(request, doc);
        });

    //--------------------------------------------------------
    // Network (GET)
    //--------------------------------------------------------

    server.on(
        "/api/network",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["ssid"]     = prefGetWiFiSSID();
            doc["hasPassword"] = !prefGetWiFiPassword().isEmpty();
            doc["dhcp"]     = prefGetWiFiDHCP();
            doc["ip"]       = prefGetWiFiIP();
            doc["subnet"]   = prefGetWiFiSubnet();
            doc["gateway"]  = prefGetWiFiGateway();
            doc["dns"]      = prefGetWiFiDNS();

            sendJson(request, doc);
        });
    //--------------------------------------------------------
// Save world clocks
//--------------------------------------------------------

server.on(
    "/api/clocks",
    HTTP_POST,

    [](AsyncWebServerRequest *request)
    {
    },

    NULL,

    [](AsyncWebServerRequest *request,
       uint8_t *data,
       size_t len,
       size_t index,
       size_t total)
    {
        JsonDocument doc;

        DeserializationError err =
            deserializeJson(doc, data, len);

        if (err)
        {
            sendError(
                request,
                400,
                "JSON parse error");

            return;
        }

        //----------------------------------------------------
        // Clock count
        //----------------------------------------------------

        if (doc["clockCount"].is<uint8_t>())
        {
            uint8_t count = doc["clockCount"];

            if (count > MAX_WORLD_CLOCKS)
                count = MAX_WORLD_CLOCKS;

            prefSetClockCount(count);
        }

        //----------------------------------------------------
        // Clocks
        //----------------------------------------------------

        if (doc["clocks"].is<JsonArray>())
        {
            JsonArray clocks = doc["clocks"];

            uint8_t i = 0;

            for (JsonObject clock : clocks)
            {
                if (i >= MAX_WORLD_CLOCKS)
                    break;

                if (clock["city"].is<String>())
                    prefSetCity(i, clock["city"].as<String>());

                if (clock["timezone"].is<uint16_t>())
                    prefSetTimeZone(i, clock["timezone"]);

                if (clock["workStart"].is<uint16_t>())
                    prefSetWorkStart(i, clock["workStart"]);

                if (clock["workEnd"].is<uint16_t>())
                    prefSetWorkEnd(i, clock["workEnd"]);

                if (clock["workSaturday"].is<bool>())
                    prefSetWorkSaturday(i, clock["workSaturday"]);

                if (clock["workSunday"].is<bool>())
                    prefSetWorkSunday(i, clock["workSunday"]);

                i++;
            }
        }

        //----------------------------------------------------
        // Reference city
        //----------------------------------------------------

        if (doc["referenceCity"].is<uint8_t>())
            prefSetReferenceCity(doc["referenceCity"]);

        //----------------------------------------------------
        // Overlap warning
        //----------------------------------------------------

        if (doc["overlapWarning"].is<uint16_t>())
            prefSetOverlapWarning(doc["overlapWarning"]);

        //----------------------------------------------------

        sendOk(request);
    });   

    //--------------------------------------------------------
    // Network (POST)
    //--------------------------------------------------------

    server.on(
        "/api/network",
        HTTP_POST,

        [](AsyncWebServerRequest *request)
        {
            // empty
        },

        NULL,

        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total)
        {
            JsonDocument doc;

            DeserializationError err =
                deserializeJson(doc, data, len);

            if (err)
            {
                sendError(
                    request,
                    400,
                    "JSON parse error");

                return;
            }

            if (doc["ssid"].is<String>())
                prefSetWiFiSSID(doc["ssid"].as<String>());

            if (doc["password"].is<String>())
                prefSetWiFiPassword(doc["password"].as<String>());

            if (doc["dhcp"].is<bool>())
                prefSetWiFiDHCP(doc["dhcp"].as<bool>());

            if (doc["ip"].is<String>())
                prefSetWiFiIP(doc["ip"].as<String>());

            if (doc["subnet"].is<String>())
                prefSetWiFiSubnet(doc["subnet"].as<String>());

            if (doc["gateway"].is<String>())
                prefSetWiFiGateway(doc["gateway"].as<String>());

            if (doc["dns"].is<String>())
                prefSetWiFiDNS(doc["dns"].as<String>());

            sendOk(request);
        });

    //--------------------------------------------------------
    // World clocks
    //--------------------------------------------------------

    server.on(
        "/api/clocks",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["maxClocks"] = MAX_WORLD_CLOCKS;
            doc["clockCount"] = prefGetClockCount();

            JsonArray clocks = doc["clocks"].to<JsonArray>();

            for (uint8_t i = 0; i < MAX_WORLD_CLOCKS; i++)
            {
                JsonObject clock = clocks.add<JsonObject>();

                clock["index"] = i;
                clock["enabled"] = (i < prefGetClockCount());

                clock["city"] = prefGetCity(i);
                clock["timezone"] = prefGetTimeZone(i);

                clock["workStart"] = prefGetWorkStart(i);
                clock["workEnd"] = prefGetWorkEnd(i);

                clock["workSaturday"] = prefGetWorkSaturday(i);
                clock["workSunday"] = prefGetWorkSunday(i);
            }

            doc["referenceCity"] = prefGetReferenceCity();
            doc["overlapWarning"] = prefGetOverlapWarning();

            sendJson(request, doc);
        });
    //--------------------------------------------------------
    // Display settings (brightness + view mode)
    //--------------------------------------------------------

    server.on(
        "/api/display",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["brightness"] = prefGetBrightness();
            doc["mode"]       = prefGetDisplayMode();

            sendJson(request, doc);
        });

    server.on(
        "/api/display",
        HTTP_POST,

        [](AsyncWebServerRequest *request)
        {
            // empty
        },

        NULL,

        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total)
        {
            JsonDocument doc;

            DeserializationError err =
                deserializeJson(doc, data, len);

            if (err)
            {
                sendError(
                    request,
                    400,
                    "JSON parse error");

                return;
            }

            if (doc["brightness"].is<uint8_t>())
            {
                uint8_t b = doc["brightness"].as<uint8_t>();

                if (b > 100)
                    b = 100;

                prefSetBrightness(b);
                displaySetBrightness(b);
            }

            if (doc["mode"].is<uint8_t>())
            {
                uint8_t m = doc["mode"].as<uint8_t>();

                if (m > DISPLAY_MODE_ENV_ONLY)
                    m = DISPLAY_MODE_ALTERNATE;

                prefSetDisplayMode(m);
                // The clock engine picks the change up on its next
                // 1-second tick, no extra notification needed.
            }

            sendOk(request);
        });

    //--------------------------------------------------------
    // Trigger Salute
    //--------------------------------------------------------

    server.on(
        "/api/salute",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            startSaluteAnimation();
            JsonDocument doc;
            doc["status"] = "ok";
            sendJson(request, doc);
        });

    //--------------------------------------------------------
    // Meteorological history (for charts)
    //--------------------------------------------------------

    server.on(
        "/api/meteo",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            String json;
            uint16_t count = meteoLogSerializeJson(json);

            (void)count;

            request->send(
                200,
                "application/json",
                json);
        });

    //--------------------------------------------------------
    // Sensor info (auto-detected, read-only)
    //--------------------------------------------------------

    server.on(
        "/api/sensor",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["active"]      = bmeGetActiveSensorName();
            doc["hasHumidity"] = bmeHasHumidity();

            sendJson(request, doc);
        });

    //--------------------------------------------------------
    // API information
    //--------------------------------------------------------

    server.on(
        "/api",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            JsonDocument doc;

            doc["project"] = "World Clock";
            doc["api"] = "1.0";
            doc["status"] = "ok";

            sendJson(request, doc);
        });
 
}