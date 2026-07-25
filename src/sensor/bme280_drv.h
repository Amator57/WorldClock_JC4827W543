#pragma once

#include <Arduino.h>

//============================================================
// Thin wrapper around the Adafruit_BME280 driver.
//
// Kept in a separate translation unit because Adafruit_BMP280.h
// and Adafruit_BME280.h both declare global enums (sensor_mode,
// sensor_sampling, ...) which would clash if both headers were
// included in the same .cpp.
//
// The caller is responsible for Wire.begin(...) with the proper
// SDA/SCL pins before calling init().
//============================================================

namespace bme280_drv
{
    // Initialise the BME280 on the already-started Wire bus.
    // Tries preferredAddr first, then 0x76 and 0x77.
    // Returns true if a BME280 chip is found.
    bool init(uint8_t preferredAddr);

    // Weather mode: forced, 1x oversampling on all channels.
    void setForcedWeather();

    // Trigger one conversion and read temperature (C), pressure
    // (hPa) and humidity (%). Returns false on a read error.
    bool readForced(float &temperatureC,
                    float &pressureHpa,
                    float &humidityPct);
}
