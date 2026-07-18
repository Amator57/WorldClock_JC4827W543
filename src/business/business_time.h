#pragma once

#include <Arduino.h>
#include <time.h>

//------------------------------------------------------------
// Business state
//------------------------------------------------------------

enum BusinessState
{
    BUSINESS_OFF = 0,
    BUSINESS_WARNING,
    BUSINESS_ACTIVE
};

//------------------------------------------------------------
// Working schedule
//------------------------------------------------------------

struct BusinessHours
{
    uint8_t startHour;
    uint8_t startMinute;

    uint8_t endHour;
    uint8_t endMinute;

    bool workSaturday;
    bool workSunday;
};

//------------------------------------------------------------
// API
//------------------------------------------------------------

BusinessState businessGetState(
    const tm &localTime,
    const BusinessHours &schedule);

BusinessState businessOverlapState(
    const tm &referenceTime,
    const BusinessHours &referenceSchedule,

    const tm &remoteTime,
    const BusinessHours &remoteSchedule,

    uint16_t warningMinutes);