#pragma once

#include <Arduino.h>
#include <Preferences.h>

//------------------------------------------------------------
// Constants
//------------------------------------------------------------

#define MAX_WORLD_CLOCKS 5

//------------------------------------------------------------
// Display mode
//------------------------------------------------------------

#define DISPLAY_MODE_ALTERNATE  0   // Clock and Meteo alternate every 30 s
#define DISPLAY_MODE_CLOCK_ONLY 1   // Only world clocks
#define DISPLAY_MODE_ENV_ONLY   2   // Only meteo (environment) data

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

void prefSetWiFiDHCP(bool enabled);
bool prefGetWiFiDHCP();

void prefSetWiFiIP(const String &ip);
String prefGetWiFiIP();

void prefSetWiFiSubnet(const String &subnet);
String prefGetWiFiSubnet();

void prefSetWiFiGateway(const String &gateway);
String prefGetWiFiGateway();

void prefSetWiFiDNS(const String &dns);
String prefGetWiFiDNS();

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

// Display mode: see DISPLAY_MODE_* constants above.
void prefSetDisplayMode(uint8_t mode);
uint8_t prefGetDisplayMode();

//------------------------------------------------------------
// Preferences version
//------------------------------------------------------------

uint16_t prefGetVersion();

void prefSetVersion(uint16_t version);

//------------------------------------------------------------
// Factory reset
//------------------------------------------------------------

void prefClearAll();

//------------------------------------------------------------
// Business hours
//------------------------------------------------------------

void prefSetWorkStart(uint8_t index, uint16_t minutes);

uint16_t prefGetWorkStart(uint8_t index);

void prefSetWorkEnd(uint8_t index, uint16_t minutes);

uint16_t prefGetWorkEnd(uint8_t index);

void prefSetWorkSaturday(uint8_t index, bool enabled);

bool prefGetWorkSaturday(uint8_t index);

void prefSetWorkSunday(uint8_t index, bool enabled);

bool prefGetWorkSunday(uint8_t index);

//------------------------------------------------------------
// Reference city
//------------------------------------------------------------

void prefSetReferenceCity(uint8_t index);

uint8_t prefGetReferenceCity();

//------------------------------------------------------------
// Overlap warning
//------------------------------------------------------------

void prefSetOverlapWarning(uint16_t minutes);

uint16_t prefGetOverlapWarning();