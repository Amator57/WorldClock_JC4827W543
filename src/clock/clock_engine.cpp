#include "clock_engine.h"

#include <Arduino.h>
#include <WiFi.h>
#include <stdio.h>
#include <time.h>

#include "../gui/screen_main.h"
#include "../ntp/ntp_manager.h"
#include "../storage/preferences_manager.h"
#include "timezones_db.h"

//------------------------------------------------------------

static uint32_t lastUpdate = 0;

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

    //--------------------------------------------------------
    // Convert UTC to minutes
    //--------------------------------------------------------

    int totalMinutes =
        utc.tm_hour * 60 +
        utc.tm_min;

    //--------------------------------------------------------
    // Apply UTC offset
    //--------------------------------------------------------

    totalMinutes +=
        tz->utcHour * 60 +
        tz->utcMinute;

    //--------------------------------------------------------
    // Wrap
    //--------------------------------------------------------

    while (totalMinutes < 0)
        totalMinutes += 24 * 60;

    while (totalMinutes >= 24 * 60)
        totalMinutes -= 24 * 60;

    //--------------------------------------------------------

    int hh = totalMinutes / 60;
    int mm = totalMinutes % 60;

    snprintf(
        buffer,
        len,
        "%02d:%02d:%02d",
        hh,
        mm,
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

    //--------------------------------------------------------
    // UTC time from NTP
    //--------------------------------------------------------

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

    //--------------------------------------------------------
    // Date
    //--------------------------------------------------------

    char dateString[24];

    strftime(
        dateString,
        sizeof(dateString),
        "%d %b %Y",
        &utc);

    //--------------------------------------------------------
    // Read cities from Preferences
    //--------------------------------------------------------

    String city1 = prefGetCity(0);
    String city2 = prefGetCity(1);
    String city3 = prefGetCity(2);
    String city4 = prefGetCity(3);
    String city5 = prefGetCity(4);

    //--------------------------------------------------------
    // Find in database
    //--------------------------------------------------------

    const TimeZoneInfo *tz1 =
        findTimeZoneByEnglish(city1.c_str());

    const TimeZoneInfo *tz2 =
        findTimeZoneByEnglish(city2.c_str());

    const TimeZoneInfo *tz3 =
        findTimeZoneByEnglish(city3.c_str());

    const TimeZoneInfo *tz4 =
        findTimeZoneByEnglish(city4.c_str());

    const TimeZoneInfo *tz5 =
        findTimeZoneByEnglish(city5.c_str());

    //--------------------------------------------------------
    // Time strings
    //--------------------------------------------------------

    char time1[16];
    char time2[16];
    char time3[16];
    char time4[16];
    char time5[16];

    formatCityTime(tz1, utc, time1, sizeof(time1));
    formatCityTime(tz2, utc, time2, sizeof(time2));
    formatCityTime(tz3, utc, time3, sizeof(time3));
    formatCityTime(tz4, utc, time4, sizeof(time4));
    formatCityTime(tz5, utc, time5, sizeof(time5));
        //--------------------------------------------------------
    // City names
    //--------------------------------------------------------

    const char *name1 = tz1 ? tz1->nameEN : city1.c_str();
    const char *name2 = tz2 ? tz2->nameEN : city2.c_str();
    const char *name3 = tz3 ? tz3->nameEN : city3.c_str();
    const char *name4 = tz4 ? tz4->nameEN : city4.c_str();
    const char *name5 = tz5 ? tz5->nameEN : city5.c_str();

    //--------------------------------------------------------
    // WiFi status
    //--------------------------------------------------------

    char wifiText[32];

    if (WiFi.status() == WL_CONNECTED)
        strcpy(wifiText, "WiFi : Connected");
    else
        strcpy(wifiText, "WiFi : Offline");

    //--------------------------------------------------------
    // NTP status
    //--------------------------------------------------------

    char ntpText[32];

    snprintf(
        ntpText,
        sizeof(ntpText),
        "NTP : %s",
        ntpStatusString());

    //--------------------------------------------------------
    // IP address
    //--------------------------------------------------------

    char ipText[40];

    if (WiFi.status() == WL_CONNECTED)
    {
        snprintf(
            ipText,
            sizeof(ipText),
            "IP : %s",
            WiFi.localIP().toString().c_str());
    }
    else
    {
        strcpy(ipText, "IP : ---");
    }

    //--------------------------------------------------------
    // Update screen
    //--------------------------------------------------------

    screenMainUpdate(

        name1,
        time1,

        name2,
        time2,

        name3,
        time3,

        name4,
        time4,

        name5,
        time5,

        dateString,

        wifiText,

        ntpText,

        ipText);
}