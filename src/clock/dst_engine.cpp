#include "dst_engine.h"

//------------------------------------------------------------
// Helpers
//------------------------------------------------------------

static bool isLeapYear(int year)
{
    year += 1900;

    return ((year % 4 == 0) &&
           (year % 100 != 0)) ||
           (year % 400 == 0);
}

//------------------------------------------------------------

static int daysInMonth(int month, int year)
{
    static const uint8_t days[] =
    {
        31,28,31,30,31,30,
        31,31,30,31,30,31
    };

    if (month == 1 && isLeapYear(year))
        return 29;

    return days[month];
}

//------------------------------------------------------------
// Last Sunday of month
//------------------------------------------------------------

static int lastSunday(
    int month,
    int year)
{
    struct tm t = {};

    t.tm_year = year;
    t.tm_mon  = month;
    t.tm_mday = daysInMonth(month, year);

    mktime(&t);

    return t.tm_mday - t.tm_wday;
}

//------------------------------------------------------------
// Europe
//------------------------------------------------------------

bool dstEurope(const struct tm &utc)
{
    int year = utc.tm_year;

    int marchSunday =
        lastSunday(2, year);

    int octoberSunday =
        lastSunday(9, year);

    //--------------------------------------------------------

    if (utc.tm_mon < 2)
        return false;

    if (utc.tm_mon > 9)
        return false;

    if (utc.tm_mon > 2 &&
        utc.tm_mon < 9)
        return true;

    //--------------------------------------------------------
    // March
    //--------------------------------------------------------

    if (utc.tm_mon == 2)
    {
        if (utc.tm_mday > marchSunday)
            return true;

        if (utc.tm_mday < marchSunday)
            return false;

        return utc.tm_hour >= 1;
    }

    //--------------------------------------------------------
    // October
    //--------------------------------------------------------

    if (utc.tm_mon == 9)
    {
        if (utc.tm_mday < octoberSunday)
            return true;

        if (utc.tm_mday > octoberSunday)
            return false;

        return utc.tm_hour < 1;
    }

    return false;
}

//------------------------------------------------------------
// USA / Canada
//------------------------------------------------------------

bool dstUSA(const struct tm &utc)
{
    // Реалізуємо на наступному етапі

    return false;
}

//------------------------------------------------------------
// Australia
//------------------------------------------------------------

bool dstAustralia(const struct tm &utc)
{
    return false;
}

//------------------------------------------------------------
// New Zealand
//------------------------------------------------------------

bool dstNewZealand(const struct tm &utc)
{
    return false;
}

//------------------------------------------------------------
// Egypt
//------------------------------------------------------------

bool dstEgypt(const struct tm &utc)
{
    return false;
}