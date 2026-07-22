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

//------------------------------------------------------------

ClockMarkerState businessGetMarkerState(
    const tm &localTime,
    const BusinessHours &schedule)
{
    // 1. Check if it's a working day
    bool todayIsWork = true;
    if (localTime.tm_wday == 0) // Sunday
    {
        todayIsWork = schedule.workSunday;
    }
    else if (localTime.tm_wday == 6) // Saturday
    {
        todayIsWork = schedule.workSaturday;
    }

    uint16_t now = localTime.tm_hour * 60 + localTime.tm_min;
    uint16_t start = schedule.startHour * 60 + schedule.startMinute;
    uint16_t end = schedule.endHour * 60 + schedule.endMinute;

    if (todayIsWork && now >= start && now < end)
    {
        // Currently working time. Check if less than 30 minutes until end
        if (end - now < 30)
        {
            return MARKER_YELLOW;
        }
        return MARKER_GREEN;
    }

    // 2. Non-working time. Find minutes to next working time start.
    // Check next 7 days to find when working time next starts
    int minutesToStart = -1;
    for (int d = 0; d < 7; d++)
    {
        int wday = (localTime.tm_wday + d) % 7;
        bool isWork = true;
        if (wday == 0)
            isWork = schedule.workSunday;
        else if (wday == 6)
            isWork = schedule.workSaturday;

        if (!isWork)
            continue;

        if (d == 0)
        {
            if (now < start)
            {
                minutesToStart = start - now;
                break;
            }
        }
        else
        {
            minutesToStart = (d * 24 * 60) - now + start;
            break;
        }
    }

    if (minutesToStart >= 0 && minutesToStart < 30)
    {
        return MARKER_BLUE;
    }

    return MARKER_RED;
}