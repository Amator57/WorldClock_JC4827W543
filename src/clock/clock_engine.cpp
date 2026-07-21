#include "clock_engine.h"

#include <Arduino.h>
#include <WiFi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "dst_engine.h"
#include "timezones_db.h"
#include "../gui/screen_main.h"
#include "../ntp/ntp_manager.h"
#include "../storage/preferences_manager.h"

//------------------------------------------------------------

static uint32_t lastUpdate = 0;

//------------------------------------------------------------

static bool isDstActive(
    const TimeZoneInfo *tz,
    const struct tm &utc)
{
    switch (tz->dstRule)
    {
        case TZ_DST_EUROPE:
            return dstEurope(utc);

        case TZ_DST_USA_CANADA:
            return dstUSA(utc, tz->utcHour * 60 + tz->utcMinute);

        case TZ_DST_AUSTRALIA:
            return dstAustralia(utc, tz->utcHour * 60 + tz->utcMinute);

        case TZ_DST_NEW_ZEALAND:
            return dstNewZealand(utc, tz->utcHour * 60 + tz->utcMinute);

        case TZ_DST_EGYPT:
            return dstEgypt(utc);

        case TZ_DST_NONE:
        default:
            return false;
    }
}

//------------------------------------------------------------

static void formatCityTime(
    const TimeZoneInfo *tz,
    const struct tm &utc,
    char *buffer,
    size_t len)
{
    if (tz == nullptr)
    {
        strncpy(buffer, "--:--:--", len);
        buffer[len - 1] = 0;
        return;
    }

    int totalMinutes = utc.tm_hour * 60 + utc.tm_min;

    totalMinutes += tz->utcHour * 60 + tz->utcMinute;

    if (isDstActive(tz, utc))
        totalMinutes += 60;

    while (totalMinutes < 0)
        totalMinutes += 24 * 60;

    while (totalMinutes >= 24 * 60)
        totalMinutes -= 24 * 60;

    snprintf(
        buffer,
        len,
        "%02d:%02d:%02d",
        totalMinutes / 60,
        totalMinutes % 60,
        utc.tm_sec);
}

//------------------------------------------------------------

void clockEngineInit()
{
    lastUpdate = 0;
}

//------------------------------------------------------------

void clockEngineUpdate()
{
    if (millis() - lastUpdate < 1000)
        return;

    lastUpdate = millis();

    struct tm utc;

    if (!ntpGetUTCTime(&utc))
    {
        screenMainUpdate(
            "-----", "--:--:--",
            "-----", "--:--:--",
            "-----", "--:--:--",
            "-----", "--:--:--",
            "-----", "--:--:--",
            "-- --- ----",
            "WiFi : Waiting",
            ntpStatusString(),
            "IP : ---");
        return;
    }

    char dateString[24];
    strftime(dateString, sizeof(dateString), "%d %b %Y", &utc);

    String cities[5] =
    {
        prefGetCity(0), prefGetCity(1), prefGetCity(2),
        prefGetCity(3), prefGetCity(4)
    };

    const TimeZoneInfo *zones[5];
    char times[5][16];
    const char *names[5];

    for (uint8_t i = 0; i < 5; i++)
    {
        zones[i] = findTimeZoneByEnglish(cities[i].c_str());
        formatCityTime(zones[i], utc, times[i], sizeof(times[i]));
        names[i] = zones[i] ? zones[i]->nameEN : cities[i].c_str();
    }

    char wifiText[32];
    strcpy(wifiText, WiFi.status() == WL_CONNECTED ?
        "WiFi : Connected" : "WiFi : Offline");

    char ntpText[32];
    snprintf(ntpText, sizeof(ntpText), "NTP : %s", ntpStatusString());

    char ipText[40];
    if (WiFi.status() == WL_CONNECTED)
        snprintf(ipText, sizeof(ipText), "IP : %s",
            WiFi.localIP().toString().c_str());
    else
        strcpy(ipText, "IP : ---");

    screenMainUpdate(
        names[0], times[0], names[1], times[1], names[2], times[2],
        names[3], times[3], names[4], times[4], dateString,
        wifiText, ntpText, ipText);
}
