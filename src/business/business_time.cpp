#include "business_time.h"

//------------------------------------------------------------

static uint16_t toMinutes(
    uint8_t hour,
    uint8_t minute)
{
    return hour * 60 + minute;
}

//------------------------------------------------------------

BusinessState businessGetState(
    const tm &localTime,
    const BusinessHours &schedule)
{
    //--------------------------------------------------------
    // Weekend
    //--------------------------------------------------------

    if (localTime.tm_wday == 0 &&
        !schedule.workSunday)
    {
        return BUSINESS_OFF;
    }

    if (localTime.tm_wday == 6 &&
        !schedule.workSaturday)
    {
        return BUSINESS_OFF;
    }

    //--------------------------------------------------------
    // Time
    //--------------------------------------------------------

    uint16_t now =
        toMinutes(
            localTime.tm_hour,
            localTime.tm_min);

    uint16_t start =
        toMinutes(
            schedule.startHour,
            schedule.startMinute);

    uint16_t end =
        toMinutes(
            schedule.endHour,
            schedule.endMinute);

    if (now >= start &&
        now < end)
    {
        return BUSINESS_ACTIVE;
    }

    return BUSINESS_OFF;
}

//------------------------------------------------------------

BusinessState businessOverlapState(
    const tm &referenceTime,
    const BusinessHours &referenceSchedule,

    const tm &remoteTime,
    const BusinessHours &remoteSchedule,

    uint16_t warningMinutes)
{
    BusinessState ref =
        businessGetState(
            referenceTime,
            referenceSchedule);

    BusinessState remote =
        businessGetState(
            remoteTime,
            remoteSchedule);

    if (ref != BUSINESS_ACTIVE)
        return BUSINESS_OFF;

    if (remote != BUSINESS_ACTIVE)
        return BUSINESS_OFF;

    return BUSINESS_ACTIVE;
}