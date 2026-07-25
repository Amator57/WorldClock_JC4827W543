#pragma once

#include <Arduino.h>
#include <stdint.h>

//============================================================
// Meteorological data logger.
//
// Stores one sample (temperature, humidity, pressure) every
// METEO_LOG_INTERVAL_SEC seconds and keeps the most recent
// METEO_LOG_RETENTION_DAYS days of history.
//
// At 5 min / 15 days this is 4320 records (~42 KB), kept both in
// RAM (ring buffer) and persisted to LittleFS as /meteo.bin.
//
// Humidity is included so the format is ready for a BME280; with
// the current BMP280 humidity is stored as "no data".
//============================================================

#ifndef METEO_LOG_INTERVAL_SEC
#define METEO_LOG_INTERVAL_SEC 300 // 5 minutes
#endif

#ifndef METEO_LOG_RETENTION_DAYS
#define METEO_LOG_RETENTION_DAYS 15
#endif

//------------------------------------------------------------
// Packed on-disk record. 10 bytes.
//------------------------------------------------------------
struct MeteoRecord
{
    uint32_t time;  // Unix timestamp (seconds, UTC)
    int16_t  temp;  // temperature  x 100, degC  (0x7FFF = no data)
    uint16_t hum;   // humidity     x 10,  %     (0xFFFF = no data)
    uint16_t pres;  // pressure     x 10,  hPa   (0xFFFF = no data)
} __attribute__((packed));

//------------------------------------------------------------
// Lifecycle
//------------------------------------------------------------

// Mounts/loads the existing log from flash. Must be called once
// after LittleFS is initialised.
bool meteoLogInit();

//------------------------------------------------------------
// Recording
//------------------------------------------------------------

// Append the current averaged sensor values. Timestamped with the
// system clock; ignored while the clock is not yet synchronised
// (NTP). Persists the ring buffer to flash.
void meteoLogAddSample(float temperatureC,
                       float humidityPct,
                       float pressureHpa);

//------------------------------------------------------------
// Access
//------------------------------------------------------------

uint16_t meteoLogCount();    // number of stored samples
uint16_t meteoLogCapacity(); // maximum number of samples kept

// Serialize all stored samples to a compact JSON string suitable
// for charting. The series are decimated to at most maxPoints points
// to keep the payload small. Returns the number of stored samples.
//
// Output format:
//   { "count":N, "shown":M, "interval":300, "capacity":4320,
//     "time":[t0,t1,...], "temp":[v|null,...],
//     "hum":[v|null,...], "pres":[v|null,...] }
uint16_t meteoLogSerializeJson(String &out, uint16_t maxPoints = 600);
