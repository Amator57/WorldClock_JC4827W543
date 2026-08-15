#pragma once

//------------------------------------------------------------
// Clock Engine
//------------------------------------------------------------

void clockEngineInit();

void clockEngineUpdate();

// Play the yearly notification (phrase + 60 s salute).
// Can be called from the web API for testing.
void triggerYearlyNotification();
void triggerSeptemberNotification();