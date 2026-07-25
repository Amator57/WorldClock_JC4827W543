#include <Arduino.h>

#include "display.h"
#include "lvgl_port.h"
#include "gui/screen_main.h"
#include "clock/clock_engine.h"
#include "ntp/ntp_manager.h"
#include "network/wifi_manager.h"
#include "storage/preferences_manager.h"
#include "storage/meteo_log.h"
#include "sensor/bme_manager.h"
#include "web/filesystem.h"
#include "web/web_server.h"

void setup()
{
    Serial.begin(115200);
    delay(2000);

Serial.println();
Serial.println("===== SETUP START =====");
 //--------------------------------------------------------
// Preferences
//--------------------------------------------------------

if (!preferencesInit())
{
    Serial.println("Preferences initialization failed.");

    while (true)
        delay(100);
}

prefLoadDefaults();   
    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("WORLD CLOCK");
    Serial.println("--------------------------------");

    //--------------------------------------------------------
    // Display
    //--------------------------------------------------------

    if (!displayInit())
    {
        Serial.println("Display error");

        while (true)
            delay(100);
    }

    //--------------------------------------------------------
    // LVGL
    //--------------------------------------------------------

    if (!lvglInit())
    {
        Serial.println("LVGL error");

        while (true)
            delay(100);
    }

    //--------------------------------------------------------
// LittleFS
//--------------------------------------------------------

if (!filesystemInit())
{
    Serial.println("Filesystem initialization failed.");

    while (true)
        delay(100);
}

    //--------------------------------------------------------
    // Meteo data logger
    //--------------------------------------------------------

    meteoLogInit();

    //--------------------------------------------------------
    // GUI
    //--------------------------------------------------------

    screenMainCreate();

   /*
    screenMainUpdate(

    "Kyiv",
    "12:34:56",
    0,

    "Delhi",
    "16:04:56",
    0,

    "Washington",
    "05:34:56",
    0,

    "Valencia",
    "11:34:56",
    0,

    "Tokyo",
    "19:34:56",
    0,

    "15 Jul 2026",

    "WiFi : Offline",

    "NTP : Waiting",

    "IP : ---");
    */
    //--------------------------------------------------------
    // BME280 sensor
    //--------------------------------------------------------

    if (!bmeInit())
    {
        Serial.println("BME280 init failed (sensor offline).");
    }

        clockEngineInit();
//========================================================
// WiFi
//========================================================

// Тимчасово.
// Пізніше ці параметри будуть читатися з Preferences.

wifiManagerInit();

if (!webServerInit())
{
    Serial.println("Web Server init failed.");
}

if (!ntpInit())
{
    Serial.println("NTP init deferred (no network).");
}

Serial.println("Ready.");
}

void loop()
{

wifiManagerLoop();

webServerLoop();

ntpLoop();

bmeLoop();

clockEngineUpdate();

lvglLoop();

}
