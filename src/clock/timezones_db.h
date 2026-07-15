#pragma once

#include <Arduino.h>

//------------------------------------------------------------
// Daylight Saving Time rules
//------------------------------------------------------------
/*
enum DSTRule
{
    TZ_DST_NONE = 0,

    TZ_DST_EUROPE,
    TZ_DST_USA_CANADA,
    TZ_DST_AUSTRALIA,
    TZ_DST_NEW_ZEALAND,

    TZ_DST_EGYPT
};
*/
//------------------------------------------------------------
// Time zone database
//------------------------------------------------------------
struct TimeZoneInfo
{
    const char *nameEN;
    const char *nameUA;

    int8_t utcHour;
    int8_t utcMinute;

    bool useDST;
};
/*
struct TimeZoneInfo
{
    const char *nameEN;
    const char *nameUA;

    int8_t utcHour;
    int8_t utcMinute;

    DSTRule dstRule;
};
*/
//------------------------------------------------------------
// Database
//------------------------------------------------------------

extern const TimeZoneInfo timeZones[];

extern const uint16_t timeZonesCount;

//------------------------------------------------------------
// Search
//------------------------------------------------------------

const TimeZoneInfo *findTimeZoneByEnglish(const char *name);

const TimeZoneInfo *findTimeZoneByUkrainian(const char *name);

const TimeZoneInfo *getTimeZone(uint16_t index);