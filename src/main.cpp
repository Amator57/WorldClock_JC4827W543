#include <Arduino.h>

#include "display.h"
#include "lvgl_port.h"
#include "gui/screen_main.h"

void setup()
{
    Serial.begin(115200);

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

    Serial.println("Ready.");
}

void loop()
{
    lvglLoop();
}