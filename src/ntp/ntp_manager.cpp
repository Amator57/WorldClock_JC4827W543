#include "ntp_manager.h"

#include <WiFi.h>

//------------------------------------------------------------
// Configuration
//------------------------------------------------------------

static const char *NTP_SERVER_1 = "pool.ntp.org";
static const char *NTP_SERVER_2 = "time.nist.gov";
static const char *NTP_SERVER_3 = "time.google.com";

// UTC (тимчасово)
// Пізніше буде братися з Preferences

static const long GMT_OFFSET_SEC = 0;
static const int DAYLIGHT_OFFSET_SEC = 0;

//------------------------------------------------------------

static bool synchronized = false;

static char statusText[24] = "Waiting";

//------------------------------------------------------------

bool ntpInit()
{
    synchronized = false;

    strcpy(statusText, "Waiting");

    if (WiFi.status() != WL_CONNECTED)
        return false;

    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET_SEC,
        NTP_SERVER_1,
        NTP_SERVER_2,
        NTP_SERVER_3);

    strcpy(statusText, "Syncing");

    return true;
}

//------------------------------------------------------------
void ntpLoop()
{
    if (synchronized)
        return;

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("NTP: WiFi not connected");
        return;
    }

    Serial.println("NTP: waiting...");

    struct tm timeinfo;

    if (getLocalTime(&timeinfo, 100))
    {
        synchronized = true;

        strcpy(statusText, "OK");

        Serial.println("NTP synchronized!");
    }
}
/*
void ntpLoop()
{
    if (synchronized)
        return;

    if (WiFi.status() != WL_CONNECTED)
        return;

    struct tm timeinfo;

    if (getLocalTime(&timeinfo, 100))
    {
        synchronized = true;

        strcpy(statusText, "OK");
    }
}
*/
//------------------------------------------------------------

bool ntpIsSynchronized()
{
    return synchronized;
}

//------------------------------------------------------------

const char *ntpStatusString()
{
    return statusText;
}

//------------------------------------------------------------

bool ntpGetUTCTime(struct tm *timeinfo)
{
    if (!synchronized)
        return false;

    return getLocalTime(timeinfo, 10);
}

//------------------------------------------------------------

void ntpGetDateString(char *buffer, size_t len)
{
    struct tm tm;

    if (!ntpGetUTCTime(&tm))
    {
        strncpy(buffer, "-- --- ----", len);
        buffer[len - 1] = 0;
        return;
    }

    strftime(
        buffer,
        len,
        "%d %b %Y",
        &tm);
}

//------------------------------------------------------------

void ntpGetTimeString(char *buffer, size_t len)
{
    struct tm tm;

    if (!ntpGetUTCTime(&tm))
    {
        strncpy(buffer, "--:--:--", len);
        buffer[len - 1] = 0;
        return;
    }

    strftime(
        buffer,
        len,
        "%H:%M:%S",
        &tm);
}