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

enum ClockMarkerState
{
    MARKER_RED = 0,             // Non-working time
    MARKER_GREEN,               // Working time
    MARKER_YELLOW,              // Less than 30 minutes to end
    MARKER_BLUE                 // Less than 30 minutes to start
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

ClockMarkerState businessGetMarkerState(
    const tm &localTime,
    const BusinessHours &schedule);

BusinessState businessOverlapState(
    const tm &referenceTime,
    const BusinessHours &referenceSchedule,

    const tm &remoteTime,
    const BusinessHours &remoteSchedule,

    uint16_t warningMinutes);