#pragma once

#include <Arduino.h>
#include <time.h>

//------------------------------------------------------------
// Europe
//------------------------------------------------------------

bool dstEurope(const struct tm &utc);

//------------------------------------------------------------
// USA / Canada
//------------------------------------------------------------

bool dstUSA(const struct tm &utc, int standardOffsetMinutes);

//------------------------------------------------------------
// Australia
//------------------------------------------------------------

bool dstAustralia(const struct tm &utc, int standardOffsetMinutes);

//------------------------------------------------------------
// New Zealand
//------------------------------------------------------------

bool dstNewZealand(const struct tm &utc, int standardOffsetMinutes);

//------------------------------------------------------------
// Egypt
//------------------------------------------------------------

bool dstEgypt(const struct tm &utc);
