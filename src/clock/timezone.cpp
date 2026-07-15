#include "timezone.h"

//------------------------------------------------------------
// Time zones
//------------------------------------------------------------

// Київ (літній час поки що не враховуємо)
const TimeZone tzKyiv =
{
    "Kyiv",
    2,
    0
};

// Делі
const TimeZone tzDelhi =
{
    "Delhi",
    5,
    30
};

// Вашингтон
const TimeZone tzWashington =
{
    "Washington",
    -5,
    0
};

//------------------------------------------------------------

void convertTime(
    int utcHour,
    int utcMinute,
    int utcSecond,
    const TimeZone &tz,
    int &hour,
    int &minute,
    int &second)
{
    hour   = utcHour;
    minute = utcMinute;
    second = utcSecond;

    //--------------------------------------------------------
    // Minutes
    //--------------------------------------------------------

    minute += tz.utcMinute;

    while (minute >= 60)
    {
        minute -= 60;
        hour++;
    }

    while (minute < 0)
    {
        minute += 60;
        hour--;
    }

    //--------------------------------------------------------
    // Hours
    //--------------------------------------------------------

    hour += tz.utcHour;

    while (hour >= 24)
        hour -= 24;

    while (hour < 0)
        hour += 24;
}