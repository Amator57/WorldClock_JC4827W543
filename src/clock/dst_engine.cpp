#include "dst_engine.h"

#include <stdint.h>

//------------------------------------------------------------
// Gregorian-calendar helpers. Month is zero-based, like tm_mon.
//------------------------------------------------------------

static int64_t daysFromCivil(int year, int month, int day)
{
    year -= month <= 1;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yearOfEra = static_cast<unsigned>(year - era * 400);
    const unsigned monthPrime = static_cast<unsigned>(month + (month > 1 ? -2 : 10));
    const unsigned dayOfYear = (153 * monthPrime + 2) / 5 + day - 1;
    const unsigned dayOfEra = yearOfEra * 365 + yearOfEra / 4 -
        yearOfEra / 100 + dayOfYear;

    return era * 146097 + static_cast<int>(dayOfEra) - 719468;
}

static int64_t utcMinutes(int year, int month, int day, int hour, int minute)
{
    return daysFromCivil(year, month, day) * 1440 + hour * 60 + minute;
}

static int weekday(int year, int month, int day)
{
    int result = static_cast<int>((daysFromCivil(year, month, day) + 4) % 7);
    return result < 0 ? result + 7 : result; // Sunday is 0.
}

static int daysInMonth(int year, int month)
{
    static const uint8_t days[] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 1 && leap ? 29 : days[month];
}

static int nthSunday(int year, int month, int occurrence)
{
    const int firstSunday = 1 + (7 - weekday(year, month, 1)) % 7;
    return firstSunday + (occurrence - 1) * 7;
}

static int lastSunday(int year, int month)
{
    const int lastDay = daysInMonth(year, month);
    return lastDay - weekday(year, month, lastDay);
}

static int lastWeekday(int year, int month, int wantedWeekday)
{
    const int lastDay = daysInMonth(year, month);
    return lastDay - (weekday(year, month, lastDay) - wantedWeekday + 7) % 7;
}

static int64_t asUtcMinutes(
    int year,
    int month,
    int day,
    int hour,
    int minute,
    int localOffsetMinutes)
{
    return utcMinutes(year, month, day, hour, minute) - localOffsetMinutes;
}

//------------------------------------------------------------
// Europe: last Sunday of March 01:00 UTC to last Sunday of
// October 01:00 UTC.
//------------------------------------------------------------

bool dstEurope(const struct tm &utc)
{
    const int year = utc.tm_year + 1900;
    const int64_t now = utcMinutes(year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min);
    const int64_t start = utcMinutes(year, 2, lastSunday(year, 2), 1, 0);
    const int64_t end = utcMinutes(year, 9, lastSunday(year, 9), 1, 0);
    return now >= start && now < end;
}

//------------------------------------------------------------
// USA / Canada: second Sunday in March 02:00 standard local
// time to first Sunday in November 02:00 daylight local time.
//------------------------------------------------------------

bool dstUSA(const struct tm &utc, int standardOffsetMinutes)
{
    const int year = utc.tm_year + 1900;
    const int64_t now = utcMinutes(year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min);
    const int64_t start = asUtcMinutes(year, 2, nthSunday(year, 2, 2), 2, 0,
        standardOffsetMinutes);
    const int64_t end = asUtcMinutes(year, 10, nthSunday(year, 10, 1), 2, 0,
        standardOffsetMinutes + 60);
    return now >= start && now < end;
}

//------------------------------------------------------------
// Australia: first Sunday in October 02:00 standard local time
// to first Sunday in April 03:00 daylight local time.
//------------------------------------------------------------

bool dstAustralia(const struct tm &utc, int standardOffsetMinutes)
{
    const int year = utc.tm_year + 1900;
    const int64_t now = utcMinutes(year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min);
    const int64_t end = asUtcMinutes(year, 3, nthSunday(year, 3, 1), 3, 0,
        standardOffsetMinutes + 60);
    const int64_t start = asUtcMinutes(year, 9, nthSunday(year, 9, 1), 2, 0,
        standardOffsetMinutes);
    return now < end || now >= start;
}

//------------------------------------------------------------
// New Zealand: last Sunday in September 02:00 standard local
// time to first Sunday in April 03:00 daylight local time.
//------------------------------------------------------------

bool dstNewZealand(const struct tm &utc, int standardOffsetMinutes)
{
    const int year = utc.tm_year + 1900;
    const int64_t now = utcMinutes(year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min);
    const int64_t end = asUtcMinutes(year, 3, nthSunday(year, 3, 1), 3, 0,
        standardOffsetMinutes + 60);
    const int64_t start = asUtcMinutes(year, 8, lastSunday(year, 8), 2, 0,
        standardOffsetMinutes);
    return now < end || now >= start;
}

//------------------------------------------------------------
// Egypt: last Friday of April 00:00 local standard time to the
// last Friday of October 00:00 local daylight time.
//------------------------------------------------------------

bool dstEgypt(const struct tm &utc)
{
    const int year = utc.tm_year + 1900;
    const int64_t now = utcMinutes(year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min);
    const int64_t start = asUtcMinutes(year, 3, lastWeekday(year, 3, 5), 0, 0, 120);
    const int64_t end = asUtcMinutes(year, 9, lastWeekday(year, 9, 5), 0, 0, 180);
    return now >= start && now < end;
}
