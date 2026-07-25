#pragma once

#include <Arduino.h>

//------------------------------------------------------------
// BME280 sensor manager
// I2C: SDA = GPIO17, SCL = GPIO18
// Maintains a rolling average over the last BME_AVG_SAMPLES
// measurements.
//------------------------------------------------------------

#ifndef BME_SDA
#define BME_SDA 17
#endif

#ifndef BME_SCL
#define BME_SCL 18
#endif

#ifndef BME_AVG_SAMPLES
#define BME_AVG_SAMPLES 10
#endif

// Sampling cadence (ms). One measurement is taken every second.
#ifndef BME_SAMPLE_INTERVAL_MS
#define BME_SAMPLE_INTERVAL_MS 1000
#endif

bool bmeInit();

void bmeLoop();

bool bmeIsAvailable();

// True when the active sensor provides humidity (BME280). BMP280
// reports no humidity.
bool bmeHasHumidity();

// Human-readable active sensor: "BME280", "BMP280" or "none".
const char *bmeGetActiveSensorName();

// Returns averaged values over the last BME_AVG_SAMPLES samples.
// Returns false (and NAN values) when sensor is not available or
// no samples have been collected yet. humidityPct is NAN when the
// active sensor has no humidity channel.
bool bmeGetAverage(float &temperatureC,
                   float &humidityPct,
                   float &pressureHpa);
