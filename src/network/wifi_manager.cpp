#include "wifi_manager.h"

#include "wifi_sta.h"
#include "wifi_ap.h"

#include "../storage/preferences_manager.h"

//------------------------------------------------------------

static bool apMode = false;

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------

bool wifiManagerInit()
{
    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("WiFi Manager");
    Serial.println("--------------------------------");

    //--------------------------------------------------------
    // Try Station mode
    //--------------------------------------------------------

    if (wifiSTAConnect())
    {
        apMode = false;

        Serial.println("Running in STA mode.");

        return true;
    }

    //--------------------------------------------------------
    // Start Access Point
    //--------------------------------------------------------

    Serial.println("Starting Access Point...");

    if (!wifiAPStart())
    {
        Serial.println("Access Point failed.");

        return false;
    }

    apMode = true;

    Serial.println("Running in AP mode.");

    return true;
}

//------------------------------------------------------------

void wifiManagerLoop()
{
    // Reserved for future:
    // reconnect
    // captive portal
    // OTA
}
//------------------------------------------------------------
// Status
//------------------------------------------------------------

bool wifiIsConnected()
{
    if (apMode)
        return false;

    return wifiSTAIsConnected();
}

//------------------------------------------------------------

bool wifiIsAccessPoint()
{
    return apMode;
}

//------------------------------------------------------------

String wifiGetIPAddress()
{
    if (apMode)
        return wifiAPGetIP();

    return wifiSTAGetIP();
}

//------------------------------------------------------------

String wifiGetSSID()
{
    if (apMode)
    {
        return "WorldClock_Setup";
    }

    return prefGetWiFiSSID();
}