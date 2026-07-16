#include "wifi_ap.h"

#include <WiFi.h>

//------------------------------------------------------------

static bool apRunning = false;

//------------------------------------------------------------

bool wifiAPStart()
{
    if (apRunning)
        return true;

    //--------------------------------------------------------
    // Configure AP
    //--------------------------------------------------------

    WiFi.mode(WIFI_AP);

    IPAddress ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(ip, gateway, subnet);

    //--------------------------------------------------------
    // Start AP
    //--------------------------------------------------------

    bool ok = WiFi.softAP(
        "WorldClock_Setup",
        "");

    if (!ok)
    {
        Serial.println("AP start failed.");

        return false;
    }

    apRunning = true;

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("Access Point started");
    Serial.println("--------------------------------");

    Serial.print("SSID : ");
    Serial.println("WorldClock_Setup");

    Serial.print("IP   : ");
    Serial.println(WiFi.softAPIP());

    return true;
}

//------------------------------------------------------------

void wifiAPStop()
{
    if (!apRunning)
        return;

    WiFi.softAPdisconnect(true);

    apRunning = false;

    Serial.println("Access Point stopped.");
}

//------------------------------------------------------------

bool wifiAPIsRunning()
{
    return apRunning;
}

//------------------------------------------------------------

String wifiAPGetIP()
{
    if (!apRunning)
        return "";

    return WiFi.softAPIP().toString();
}