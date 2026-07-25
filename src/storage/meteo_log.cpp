#include "meteo_log.h"

#include <LittleFS.h>
#include <time.h>

//------------------------------------------------------------

static const char *LOG_FILE = "/meteo.bin";
static const uint32_t MAGIC = 0x4D455445; // "METE"

// (24h * 60min / interval) * retentionDays
static const uint16_t CAPACITY =
    (uint16_t)((24UL * 60UL) / (METEO_LOG_INTERVAL_SEC / 60UL)) *
    METEO_LOG_RETENTION_DAYS; // 4320

// Earliest plausible Unix time; used to detect unsynchronised clock.
static const uint32_t MIN_VALID_TIME = 1700000000; // ~Nov 2023

//------------------------------------------------------------
// File header
//------------------------------------------------------------

struct Header
{
    uint32_t magic;
    uint16_t capacity;
    uint16_t count;
} __attribute__((packed));

//------------------------------------------------------------
// Ring buffer (kept in RAM, flushed to flash on every sample)
//------------------------------------------------------------

static MeteoRecord g_ring[CAPACITY];
static volatile uint16_t g_head  = 0; // index of the oldest record
static volatile uint16_t g_count = 0;

//------------------------------------------------------------
// Encoding helpers (NAN -> sentinel)
//------------------------------------------------------------

static int16_t encTemp(float t)
{
    return isnan(t) ? 0x7FFF : (int16_t)lroundf(t * 100.0F);
}

static uint16_t encHum(float h)
{
    return isnan(h) ? 0xFFFF : (uint16_t)lroundf(h * 10.0F);
}

static uint16_t encPres(float p)
{
    return isnan(p) ? 0xFFFF : (uint16_t)lroundf(p * 10.0F);
}

//------------------------------------------------------------
// Load the log from flash into the RAM ring buffer.
//------------------------------------------------------------

static bool load()
{
    if (!LittleFS.exists(LOG_FILE))
        return false;

    File f = LittleFS.open(LOG_FILE, "r");
    if (!f)
        return false;

    Header h;
    if (f.readBytes((char *)&h, sizeof(h)) != sizeof(h) ||
        h.magic != MAGIC)
    {
        f.close();
        return false;
    }

    g_head = 0;
    g_count = 0;

    uint16_t n = h.count;
    if (n > CAPACITY)
        n = CAPACITY;

    for (uint16_t i = 0; i < n; i++)
    {
        MeteoRecord rec;
        if (f.readBytes((char *)&rec, sizeof(rec)) != sizeof(rec))
            break;
        g_ring[i] = rec;
        g_count++;
    }

    f.close();
    return true;
}

//------------------------------------------------------------
// Persist the whole ring buffer to flash (oldest first).
//------------------------------------------------------------

static bool flush()
{
    uint32_t t0 = millis();

    File f = LittleFS.open(LOG_FILE, "w");
    if (!f)
    {
        Serial.println("MeteoLog: failed to open file for writing");
        return false;
    }

    Header h;
    h.magic = MAGIC;
    h.capacity = CAPACITY;
    h.count = g_count;

    f.write((const uint8_t *)&h, sizeof(h));

    for (uint16_t i = 0; i < g_count; i++)
    {
        uint16_t idx = (g_head + i) % CAPACITY;
        f.write((const uint8_t *)&g_ring[idx], sizeof(MeteoRecord));
    }

    f.close();

    Serial.printf("MeteoLog: flushed %u records in %u ms\n",
                  (unsigned)g_count,
                  (unsigned)(millis() - t0));

    return true;
}

//============================================================

bool meteoLogInit()
{
    g_head = 0;
    g_count = 0;

    bool loaded = load();

    Serial.printf("MeteoLog: capacity=%u loaded=%u\n",
                  (unsigned)CAPACITY,
                  (unsigned)g_count);

    if (!loaded)
        Serial.println("MeteoLog: no previous data (starting empty)");

    return true;
}

//------------------------------------------------------------

void meteoLogAddSample(float temperatureC,
                       float humidityPct,
                       float pressureHpa)
{
    time_t now = time(nullptr);
    if ((uint32_t)now < MIN_VALID_TIME)
    {
        // Clock not synchronised yet; skip this sample.
        return;
    }

    uint16_t idx;
    if (g_count < CAPACITY)
    {
        idx = (g_head + g_count) % CAPACITY;
        g_count++;
    }
    else
    {
        // Buffer full: overwrite the oldest and advance head.
        idx = g_head;
        g_head = (g_head + 1) % CAPACITY;
    }

    g_ring[idx].time = (uint32_t)now;
    g_ring[idx].temp = encTemp(temperatureC);
    g_ring[idx].hum  = encHum(humidityPct);
    g_ring[idx].pres = encPres(pressureHpa);

    flush();
}

//------------------------------------------------------------

uint16_t meteoLogCount()
{
    return g_count;
}

uint16_t meteoLogCapacity()
{
    return CAPACITY;
}

//------------------------------------------------------------
// Serialize the log to a compact JSON document, decimated to at
// most maxPoints points. Reads a snapshot of head/count; the only
// concurrent writer is meteoLogAddSample (once every 5 minutes),
// so a transiently inconsistent read can at most shift one sample
// in the chart, which is harmless.
//------------------------------------------------------------

static void appendFloat(String &out, int16_t raw, int16_t sentinel,
                        float scale, uint8_t decimals)
{
    if (raw == sentinel)
    {
        out += "null";
        return;
    }
    char buf[16];
    dtostrf(raw * scale, 1, decimals, buf);
    out += buf;
}

uint16_t meteoLogSerializeJson(String &out, uint16_t maxPoints)
{
    uint16_t head = g_head;
    uint16_t count = g_count;

    uint16_t step = 1;
    if (count > maxPoints && maxPoints > 0)
        step = (uint16_t)((count + maxPoints - 1) / maxPoints);

    uint16_t shown = 0;
    for (uint16_t i = 0; i < count; i += step)
        shown++;

    out.reserve((uint32_t)shown * 40 + 128);

    char numbuf[16];

    out = "{\"count\":";
    out += count;
    out += ",\"shown\":";
    out += shown;
    out += ",\"interval\":";
    out += (unsigned long)METEO_LOG_INTERVAL_SEC;
    out += ",\"capacity\":";
    out += (unsigned long)CAPACITY;

    // time
    out += ",\"time\":[";
    for (uint16_t i = 0, k = 0; i < count; i += step, k++)
    {
        if (k) out += ",";
        out += (unsigned long)g_ring[(head + i) % CAPACITY].time;
    }
    out += "]";

    // temperature (x100 -> degC)
    out += ",\"temp\":[";
    for (uint16_t i = 0, k = 0; i < count; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, g_ring[(head + i) % CAPACITY].temp,
                    0x7FFF, 0.01F, 2);
    }
    out += "]";

    // humidity (x10 -> %)
    out += ",\"hum\":[";
    for (uint16_t i = 0, k = 0; i < count; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, (int16_t)g_ring[(head + i) % CAPACITY].hum,
                    (int16_t)0xFFFF, 0.1F, 1);
    }
    out += "]";

    // pressure (x10 -> hPa)
    out += ",\"pres\":[";
    for (uint16_t i = 0, k = 0; i < count; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, (int16_t)g_ring[(head + i) % CAPACITY].pres,
                    (int16_t)0xFFFF, 0.1F, 1);
    }
    out += "]}";

    return count;
}
