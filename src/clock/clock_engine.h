#pragma once

#include <cstdint>
#include <Arduino.h>

//------------------------------------------------------------
// Clock Engine
//------------------------------------------------------------

void clockEngineInit();

void clockEngineUpdate();

// Play the yearly notification (phrase + 60 s salute).
// Can be called from the web API for testing.
void triggerYearlyNotification();
void triggerSeptemberNotification();

// Current total UTC offset (seconds, including DST) of the clock
// at the given index, computed from its timezone settings.
// Returns 0 when the zone is unknown. Exposed for the web API
// (the meteo web UI displays time in the first clock's zone).
int32_t clockEngineClockUtcOffsetSec(uint8_t clockIndex);