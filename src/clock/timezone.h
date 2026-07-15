#pragma once

#include <stdint.h>

//------------------------------------------------------------
// Time zone
//------------------------------------------------------------

struct TimeZone
{
    const char *city;

    int8_t utcHour;
    int8_t utcMinute;
};

//------------------------------------------------------------

extern const TimeZone tzKyiv;
extern const TimeZone tzDelhi;
extern const TimeZone tzWashington;

//------------------------------------------------------------

void convertTime(
    int utcHour,
    int utcMinute,
    int utcSecond,
    const TimeZone &tz,
    int &hour,
    int &minute,
    int &second);