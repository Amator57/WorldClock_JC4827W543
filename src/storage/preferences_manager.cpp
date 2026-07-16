#include "preferences_manager.h"

//------------------------------------------------------------

static Preferences prefs;

static const char *NAMESPACE = "worldclock";

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------

bool preferencesInit()
{
    return prefs.begin(NAMESPACE, false);
}

//------------------------------------------------------------

void preferencesEnd()
{
    prefs.end();
}
//------------------------------------------------------------
// Load default values
//------------------------------------------------------------

void prefLoadDefaults()
{
    //--------------------------------------------------------
    // WiFi
    //--------------------------------------------------------

    if (prefGetWiFiSSID().isEmpty())
    {
        prefSetWiFiSSID("YOUR_WIFI");
        prefSetWiFiPassword("YOUR_PASSWORD");
    }

    //--------------------------------------------------------
    // Number of clocks
    //--------------------------------------------------------

    if (prefGetClockCount() < 1 ||
        prefGetClockCount() > MAX_WORLD_CLOCKS)
    {
        prefSetClockCount(5);
    }

    //--------------------------------------------------------
    // Cities
    //--------------------------------------------------------

    if (prefGetCity(0).isEmpty())
        prefSetCity(0, "Kyiv");

    if (prefGetCity(1).isEmpty())
        prefSetCity(1, "Valencia");

    if (prefGetCity(2).isEmpty())
        prefSetCity(2, "Delhi");

    if (prefGetCity(3).isEmpty())
        prefSetCity(3, "Washington");

    if (prefGetCity(4).isEmpty())
        prefSetCity(4, "Tokyo");

    //--------------------------------------------------------
    // Time zones
    //--------------------------------------------------------

    for (uint8_t i = 0; i < MAX_WORLD_CLOCKS; i++)
    {
        if (prefGetTimeZone(i) == 0)
        {
            prefSetTimeZone(i, i);
        }
    }

    //--------------------------------------------------------
    // NTP
    //--------------------------------------------------------

    if (prefGetNTPServer().isEmpty())
    {
        prefSetNTPServer("pool.ntp.org");
    }

    //--------------------------------------------------------
    // Language
    //--------------------------------------------------------

    if (prefGetLanguage().isEmpty())
    {
        prefSetLanguage("EN");
    }

    //--------------------------------------------------------
    // Brightness
    //--------------------------------------------------------

    if (prefGetBrightness() == 0)
    {
        prefSetBrightness(100);
    }
}
//------------------------------------------------------------
// WiFi
//------------------------------------------------------------

void prefSetWiFiSSID(const String &ssid)
{
    prefs.putString("wifi_ssid", ssid);
}

String prefGetWiFiSSID()
{
    return prefs.getString("wifi_ssid", "");
}

//------------------------------------------------------------

void prefSetWiFiPassword(const String &password)
{
    prefs.putString("wifi_pass", password);
}

String prefGetWiFiPassword()
{
    return prefs.getString("wifi_pass", "");
}

//------------------------------------------------------------
// Number of clocks
//------------------------------------------------------------

void prefSetClockCount(uint8_t count)
{
    if (count < 1)
        count = 1;

    if (count > MAX_WORLD_CLOCKS)
        count = MAX_WORLD_CLOCKS;

    prefs.putUChar("clk_count", count);
}

//------------------------------------------------------------

uint8_t prefGetClockCount()
{
    return prefs.getUChar("clk_count", 3);
}

//------------------------------------------------------------
// Cities
//------------------------------------------------------------

void prefSetCity(uint8_t index, const String &city)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[10];

    sprintf(key, "city%d", index);

    prefs.putString(key, city);
}

//------------------------------------------------------------

String prefGetCity(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return "";

    char key[10];

    sprintf(key, "city%d", index);

    return prefs.getString(key, "");
}
//------------------------------------------------------------
// Time zones
//------------------------------------------------------------

void prefSetTimeZone(uint8_t index, uint16_t tzIndex)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[10];

    sprintf(key, "tz%d", index);

    prefs.putUShort(key, tzIndex);
}

//------------------------------------------------------------

uint16_t prefGetTimeZone(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return 0;

    char key[10];

    sprintf(key, "tz%d", index);

    return prefs.getUShort(key, index);
}

//------------------------------------------------------------
// NTP
//------------------------------------------------------------

void prefSetNTPServer(const String &server)
{
    prefs.putString("ntp_srv", server);
}

//------------------------------------------------------------

String prefGetNTPServer()
{
    return prefs.getString(
        "ntp_srv",
        "pool.ntp.org");
}

//------------------------------------------------------------
// Language
//------------------------------------------------------------

void prefSetLanguage(const String &language)
{
    prefs.putString(
        "language",
        language);
}

//------------------------------------------------------------

String prefGetLanguage()
{
    return prefs.getString(
        "language",
        "EN");
}

//------------------------------------------------------------
// Brightness
//------------------------------------------------------------

void prefSetBrightness(uint8_t brightness)
{
    prefs.putUChar(
        "brightness",
        brightness);
}

//------------------------------------------------------------

uint8_t prefGetBrightness()
{
    return prefs.getUChar(
        "brightness",
        100);
}

//------------------------------------------------------------
// Factory reset
//------------------------------------------------------------

void prefClearAll()
{
    prefs.clear();
}