#pragma once

#include <Arduino.h>

//------------------------------------------------------------
// WiFi Station
//------------------------------------------------------------

bool wifiSTAConnect();

void wifiSTADisconnect();

bool wifiSTAIsConnected();

String wifiSTAGetIP();