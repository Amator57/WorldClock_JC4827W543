#include <Arduino.h>

#include "display.h"
#include "lvgl_port.h"
#include "gui/screen_main.h"
#include "clock/clock_engine.h"
#include "network/wifi_manager.h"
#include "storage/preferences_manager.h"

void setup()
{
    Serial.begin(115200);
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
//--------------------------------------------------------
// Preferences
//--------------------------------------------------------

if (!preferencesInit())
{
    Serial.println("Preferences error");

    while (true)
        delay(100);
}

//--------------------------------------------------------
// First start defaults
//--------------------------------------------------------

if (prefGetWiFiSSID().isEmpty())
{
    prefSetWiFiSSID("YOUR_WIFI");
    prefSetWiFiPassword("YOUR_PASSWORD");

    prefSetClockCount(5);

    prefSetCity(0, "Kyiv");
    prefSetCity(1, "Valencia");
    prefSetCity(2, "Delhi");
    prefSetCity(3, "Washington");
    prefSetCity(4, "Tokyo");

    prefSetNTPServer("pool.ntp.org");

    prefSetLanguage("EN");

    prefSetBrightness(100);
}
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
    // GUI
    //--------------------------------------------------------

    screenMainCreate();

    screenMainUpdate(
        "Kyiv",
        "12:34:56",

        "Delhi",
        "16:04:56",

        "Washington",
        "05:34:56",

        "15 Jul 2026",

        "WiFi : Offline",

        "NTP : Waiting",

        "IP : ---");
        clockEngineInit();
//========================================================
// WiFi
//========================================================

// Тимчасово.
// Пізніше ці параметри будуть читатися з Preferences.

wifiManagerInit();
    Serial.println("Ready.");
}

void loop()
{

    clockEngineUpdate();
    
   wifiManagerLoop();

    lvglLoop();

}
