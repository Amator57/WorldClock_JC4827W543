#include "clock_engine.h"

#include <Arduino.h>
#include <stdio.h>

#include "../gui/screen_main.h"

//------------------------------------------------------------

static uint32_t lastUpdate = 0;

static int sec = 0;
static int minute = 34;
static int hourKyiv = 12;

//------------------------------------------------------------

void clockEngineInit()
{
    lastUpdate = millis();
}

//------------------------------------------------------------

void clockEngineUpdate()
{
    if (millis() - lastUpdate < 1000)
        return;

    lastUpdate += 1000;

    //--------------------------------------------------------
    // Test clock
    //--------------------------------------------------------

    sec++;

    if (sec >= 60)
    {
        sec = 0;
        minute++;

        if (minute >= 60)
        {
            minute = 0;
            hourKyiv++;

            if (hourKyiv >= 24)
                hourKyiv = 0;
        }
    }

    //--------------------------------------------------------
    // Time strings
    //--------------------------------------------------------

    char kyiv[16];
    char delhi[16];
    char washington[16];

    sprintf(kyiv,
            "%02d:%02d:%02d",
            hourKyiv,
            minute,
            sec);

    int hourDelhi = (hourKyiv + 3) % 24;

    sprintf(delhi,
            "%02d:%02d:%02d",
            hourDelhi,
            (minute + 30) % 60,
            sec);

    int hourWashington = (hourKyiv + 17) % 24;

    sprintf(washington,
            "%02d:%02d:%02d",
            hourWashington,
            minute,
            sec);

    //--------------------------------------------------------
    // Update screen
    //--------------------------------------------------------

    screenMainUpdate(
        "Kyiv",
        kyiv,

        "Delhi",
        delhi,

        "Washington",
        washington,

        "15 Jul 2026",

        "WiFi : Offline",

        "NTP : Waiting",

        "IP : ---");
}