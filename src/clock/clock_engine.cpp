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
#include "../business/business_time.h"

//------------------------------------------------------------

static uint32_t lastUpdate = 0;
static int lastSaluteYear = -1;
static int lastSaluteYday = -1;

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

static bool isLeapYear(int year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static time_t my_timegm(struct tm *tm)
{
    static const int daysInMonths[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int year = tm->tm_year + 1900;
    int month = tm->tm_mon;
    time_t seconds = 0;

    for (int y = 1970; y < year; y++)
    {
        seconds += (isLeapYear(y) ? 366 : 365) * 86400;
    }

    for (int m = 0; m < month; m++)
    {
        if (m == 1 && isLeapYear(year))
        {
            seconds += 29 * 86400;
        }
        else
        {
            seconds += daysInMonths[m] * 86400;
        }
    }

    seconds += (tm->tm_mday - 1) * 86400;
    seconds += tm->tm_hour * 3600;
    seconds += tm->tm_min * 60;
    seconds += tm->tm_sec;

    return seconds;
}

//------------------------------------------------------------

static tm getLocalTimeStruct(
    const TimeZoneInfo *tz,
    const tm &utc)
{
    if (tz == nullptr)
    {
        return utc;
    }

    time_t t = my_timegm(const_cast<struct tm*>(&utc));

    t += (tz->utcHour * 60 + tz->utcMinute) * 60;

    if (isDstActive(tz, utc))
    {
        t += 3600;
    }

    struct tm localTime;
    gmtime_r(&t, &localTime);
    return localTime;
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
            "-----", "--:--:--", 0,
            "-----", "--:--:--", 0,
            "-----", "--:--:--", 0,
            "-----", "--:--:--", 0,
            "-----", "--:--:--", 0,
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
    uint8_t states[5];

    for (uint8_t i = 0; i < 5; i++)
    {
        zones[i] = findTimeZoneByEnglish(cities[i].c_str());
        formatCityTime(zones[i], utc, times[i], sizeof(times[i]));
        names[i] = zones[i] ? zones[i]->nameEN : cities[i].c_str();

        struct tm localTime = getLocalTimeStruct(zones[i], utc);
        BusinessHours schedule;
        uint16_t workStart = prefGetWorkStart(i);
        uint16_t workEnd = prefGetWorkEnd(i);
        schedule.startHour = workStart / 60;
        schedule.startMinute = workStart % 60;
        schedule.endHour = workEnd / 60;
        schedule.endMinute = workEnd % 60;
        schedule.workSaturday = prefGetWorkSaturday(i);
        schedule.workSunday = prefGetWorkSunday(i);

        states[i] = (uint8_t)businessGetMarkerState(localTime, schedule);

        if (i == 0)
        {
            if (localTime.tm_wday == 5) // Friday
            {
                int nowMinutes = localTime.tm_hour * 60 + localTime.tm_min;
                int endMinutes = schedule.endHour * 60 + schedule.endMinute;

                if (nowMinutes >= endMinutes && (nowMinutes - endMinutes) < 5)
                {
                    if (localTime.tm_year != lastSaluteYear || localTime.tm_yday != lastSaluteYday)
                    {
                        lastSaluteYear = localTime.tm_year;
                        lastSaluteYday = localTime.tm_yday;
                        startSaluteAnimation();
                    }
                }
            }
        }
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
        names[0], times[0], states[0],
        names[1], times[1], states[1],
        names[2], times[2], states[2],
        names[3], times[3], states[3],
        names[4], times[4], states[4],
        dateString,
        wifiText, ntpText, ipText);
}
