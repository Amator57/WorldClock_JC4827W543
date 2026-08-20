#include "wifi_manager.h"

#include "wifi_sta.h"
#include "wifi_ap.h"

#include "../storage/preferences_manager.h"

//------------------------------------------------------------

static bool apMode = false;

// Reconnect handling: after the connection drops, give the WiFi
// stack a grace period to auto-reconnect on its own; if it does
// not recover, trigger a full reconnect attempt.
static const uint32_t RECONNECT_GRACE_MS = 15000;
static uint32_t s_disconnectMs = 0; // 0 = connected / not seen yet

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
    if (apMode)
        return; // AP mode is a setup fallback: no auto-switching

    if (wifiSTAIsConnected())
    {
        s_disconnectMs = 0;
        return;
    }

    // Connection just lost: let the stack try auto-reconnect first.
    if (s_disconnectMs == 0)
    {
        s_disconnectMs = millis();
        Serial.println("WiFi: connection lost, waiting for auto-reconnect...");
        return;
    }

    // Still down after the grace period: reconnect manually.
    if (millis() - s_disconnectMs < RECONNECT_GRACE_MS)
        return;

    Serial.println("WiFi: reconnecting...");
    s_disconnectMs = 0;

    if (wifiSTAConnect())
        Serial.println("WiFi: reconnected.");
    else
        Serial.println("WiFi: reconnect failed, will retry.");
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