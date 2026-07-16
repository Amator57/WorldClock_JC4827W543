#pragma once

#include <Arduino.h>

//------------------------------------------------------------
// WiFi Access Point
//------------------------------------------------------------

bool wifiAPStart();

void wifiAPStop();

bool wifiAPIsRunning();

String wifiAPGetIP();