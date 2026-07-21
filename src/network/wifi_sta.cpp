#include "wifi_sta.h"

#include <WiFi.h>

#include "../storage/preferences_manager.h"

//------------------------------------------------------------

static const uint32_t CONNECT_TIMEOUT = 30000;

//------------------------------------------------------------

bool wifiSTAConnect()
{
    String ssid = prefGetWiFiSSID();
    String password = prefGetWiFiPassword();

    //--------------------------------------------------------
    // No WiFi configured
    //--------------------------------------------------------

    if (ssid.isEmpty())
    {
        Serial.println("WiFi: SSID not configured.");

        return false;
    }

    //--------------------------------------------------------
    // Static IP configuration (if DHCP disabled)
    //--------------------------------------------------------

    if (!prefGetWiFiDHCP())
    {
        IPAddress ip, subnet, gateway, dns;

        if (ip.fromString(prefGetWiFiIP()) &&
            subnet.fromString(prefGetWiFiSubnet()) &&
            gateway.fromString(prefGetWiFiGateway()) &&
            dns.fromString(prefGetWiFiDNS()))
        {
            WiFi.config(ip, gateway, subnet, dns);

            Serial.print("Static IP: ");
            Serial.println(ip);
        }
        else
        {
            Serial.println("WiFi: Invalid static IP config, using DHCP.");
        }
    }

    //--------------------------------------------------------
    // Connect
    //--------------------------------------------------------

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("Connecting to WiFi...");
    Serial.print("SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        ssid.c_str(),
        password.c_str());

    uint32_t startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");

        if (millis() - startTime > CONNECT_TIMEOUT)
        {
            Serial.println();
            Serial.println("WiFi connection timeout.");

            WiFi.disconnect(true);

            return false;
        }
    }

    Serial.println();
    Serial.println("WiFi connected.");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    return true;
}

//------------------------------------------------------------

void wifiSTADisconnect()
{
    WiFi.disconnect(true);
}

//------------------------------------------------------------

bool wifiSTAIsConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

//------------------------------------------------------------

String wifiSTAGetIP()
{
    if (!wifiSTAIsConnected())
        return "";

    return WiFi.localIP().toString();
}