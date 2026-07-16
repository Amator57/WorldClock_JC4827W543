#pragma once

#include <Arduino.h>
#include <Preferences.h>

//------------------------------------------------------------
// Constants
//------------------------------------------------------------

#define MAX_WORLD_CLOCKS 5

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------

bool preferencesInit();

void preferencesEnd();

void prefLoadDefaults();

//------------------------------------------------------------
// WiFi
//------------------------------------------------------------

void prefSetWiFiSSID(const String &ssid);
String prefGetWiFiSSID();

void prefSetWiFiPassword(const String &password);
String prefGetWiFiPassword();

//------------------------------------------------------------
// World clocks
//------------------------------------------------------------

void prefSetClockCount(uint8_t count);
uint8_t prefGetClockCount();

void prefSetCity(uint8_t index, const String &city);
String prefGetCity(uint8_t index);

void prefSetTimeZone(uint8_t index, uint16_t tzIndex);
uint16_t prefGetTimeZone(uint8_t index);

//------------------------------------------------------------
// NTP
//------------------------------------------------------------

void prefSetNTPServer(const String &server);
String prefGetNTPServer();

//------------------------------------------------------------
// Language
//------------------------------------------------------------

void prefSetLanguage(const String &language);
String prefGetLanguage();

//------------------------------------------------------------
// Display
//------------------------------------------------------------

void prefSetBrightness(uint8_t brightness);
uint8_t prefGetBrightness();

//------------------------------------------------------------
// Factory reset
//------------------------------------------------------------

void prefClearAll();