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
// Preferences version
//------------------------------------------------------------

static const uint16_t PREF_VERSION = 2;

//------------------------------------------------------------

uint16_t prefGetVersion()
{
    return prefs.getUShort("version", 0);
}

//------------------------------------------------------------

void prefSetVersion(uint16_t version)
{
    prefs.putUShort("version", version);
}
//------------------------------------------------------------
// Load default values
//------------------------------------------------------------

void prefLoadDefaults()
{
        //--------------------------------------------------------
    // Preferences version
    //--------------------------------------------------------

    if (prefGetVersion() == PREF_VERSION)
        return;

    Serial.println("Initializing Preferences...");
    //--------------------------------------------------------
    // WiFi
    //--------------------------------------------------------

    if (prefGetWiFiSSID().isEmpty())
    {
        prefSetWiFiSSID("");
        prefSetWiFiPassword("");
    }

    if (prefGetWiFiIP().isEmpty())
    {
        prefSetWiFiDHCP(true);
        prefSetWiFiIP("192.168.1.100");
        prefSetWiFiSubnet("255.255.255.0");
        prefSetWiFiGateway("192.168.1.1");
        prefSetWiFiDNS("8.8.8.8");
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
 /*
    for (uint8_t i = 0; i < MAX_WORLD_CLOCKS; i++)
    {
        if (prefGetTimeZone(i) == 0)
        {
            prefSetTimeZone(i, i);
        }
    }
*/
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
        //--------------------------------------------------------
    // Business hours
    //--------------------------------------------------------

    for (uint8_t i = 0; i < MAX_WORLD_CLOCKS; i++)
    {
        if (prefGetWorkStart(i) > 1439)
            prefSetWorkStart(i, 9 * 60);

        if (prefGetWorkEnd(i) > 1439)
            prefSetWorkEnd(i, 18 * 60);

        prefSetWorkSaturday(i, false);
        prefSetWorkSunday(i, false);
    }

    //--------------------------------------------------------
    // Reference city
    //--------------------------------------------------------

    if (prefGetReferenceCity() >= MAX_WORLD_CLOCKS)
    {
        prefSetReferenceCity(0);
    }

    //--------------------------------------------------------
    // Overlap warning
    //--------------------------------------------------------

    if (prefGetOverlapWarning() == 0)
    {
        prefSetOverlapWarning(60);
    }
        //--------------------------------------------------------
    // Save version
    //--------------------------------------------------------

    prefSetVersion(PREF_VERSION);

    Serial.println("Preferences initialized.");
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
// WiFi Static IP
//------------------------------------------------------------

void prefSetWiFiDHCP(bool enabled)
{
    prefs.putBool("wifi_dhcp", enabled);
}

bool prefGetWiFiDHCP()
{
    return prefs.getBool("wifi_dhcp", true);
}

//------------------------------------------------------------

void prefSetWiFiIP(const String &ip)
{
    prefs.putString("wifi_ip", ip);
}

String prefGetWiFiIP()
{
    return prefs.getString("wifi_ip", "");
}

//------------------------------------------------------------

void prefSetWiFiSubnet(const String &subnet)
{
    prefs.putString("wifi_subnet", subnet);
}

String prefGetWiFiSubnet()
{
    return prefs.getString("wifi_subnet", "255.255.255.0");
}

//------------------------------------------------------------

void prefSetWiFiGateway(const String &gateway)
{
    prefs.putString("wifi_gw", gateway);
}

String prefGetWiFiGateway()
{
    return prefs.getString("wifi_gw", "");
}

//------------------------------------------------------------

void prefSetWiFiDNS(const String &dns)
{
    prefs.putString("wifi_dns", dns);
}

String prefGetWiFiDNS()
{
    return prefs.getString("wifi_dns", "8.8.8.8");
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
    return prefs.getUChar("clk_count", MAX_WORLD_CLOCKS);
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
//------------------------------------------------------------
// Business hours
//------------------------------------------------------------

void prefSetWorkStart(uint8_t index, uint16_t minutes)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[16];
    sprintf(key, "ws%u", index);

    prefs.putUShort(key, minutes);
}

//------------------------------------------------------------

uint16_t prefGetWorkStart(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return 540;      // 09:00

    char key[16];
    sprintf(key, "ws%u", index);

    return prefs.getUShort(key, 540);
}

//------------------------------------------------------------

void prefSetWorkEnd(uint8_t index, uint16_t minutes)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[16];
    sprintf(key, "we%u", index);

    prefs.putUShort(key, minutes);
}

//------------------------------------------------------------

uint16_t prefGetWorkEnd(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return 1080;     // 18:00

    char key[16];
    sprintf(key, "we%u", index);

    return prefs.getUShort(key, 1080);
}

//------------------------------------------------------------
// Saturday
//------------------------------------------------------------

void prefSetWorkSaturday(uint8_t index, bool enabled)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[16];
    sprintf(key, "sat%u", index);

    prefs.putBool(key, enabled);
}

//------------------------------------------------------------

bool prefGetWorkSaturday(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return false;

    char key[16];
    sprintf(key, "sat%u", index);

    return prefs.getBool(key, false);
}

//------------------------------------------------------------
// Sunday
//------------------------------------------------------------

void prefSetWorkSunday(uint8_t index, bool enabled)
{
    if (index >= MAX_WORLD_CLOCKS)
        return;

    char key[16];
    sprintf(key, "sun%u", index);

    prefs.putBool(key, enabled);
}

//------------------------------------------------------------

bool prefGetWorkSunday(uint8_t index)
{
    if (index >= MAX_WORLD_CLOCKS)
        return false;

    char key[16];
    sprintf(key, "sun%u", index);

    return prefs.getBool(key, false);
}

//------------------------------------------------------------
// Reference city
//------------------------------------------------------------

void prefSetReferenceCity(uint8_t index)
{
    prefs.putUChar("ref_city", index);
}

//------------------------------------------------------------

uint8_t prefGetReferenceCity()
{
    return prefs.getUChar("ref_city", 0);
}

//------------------------------------------------------------
// Overlap warning
//------------------------------------------------------------

void prefSetOverlapWarning(uint16_t minutes)
{
    prefs.putUShort("overlap", minutes);
}

//------------------------------------------------------------

uint16_t prefGetOverlapWarning()
{
    return prefs.getUShort("overlap", 60);
}