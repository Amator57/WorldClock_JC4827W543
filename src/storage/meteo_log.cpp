#include "meteo_log.h"

#include <LittleFS.h>
#include <time.h>

//------------------------------------------------------------
//
// Storage strategy
// ----------------
// The log file is pre-allocated at full capacity on first init
// and never resized. Each new sample is written in-place at its
// slot offset (10 bytes only), instead of rewriting the whole
// file. The ring indices (head/count) are NOT persisted: on boot
// they are reconstructed by scanning the records. This way every
// flash block is hit evenly (each slot once per retention cycle),
// which extends flash life by roughly an order of magnitude
// compared to a full rewrite on every sample.
//
// Layout:  [Header][MeteoRecord x CAPACITY]
//------------------------------------------------------------

static const char *LOG_FILE = "/meteo.bin";
static const uint32_t MAGIC = 0x4D455446; // "METF" (new format; legacy was "METE")

// (24h * 60min / interval) * retentionDays
static const uint16_t CAPACITY =
    (uint16_t)((24UL * 60UL) / (METEO_LOG_INTERVAL_SEC / 60UL)) *
    METEO_LOG_RETENTION_DAYS; // 4320

// Earliest plausible Unix time; used to detect unsynchronised clock
// and to distinguish a written slot from an empty one.
static const uint32_t MIN_VALID_TIME = 1700000000; // ~Nov 2023

// Legacy "METE" magic, used only for one-shot migration.
static const uint32_t LEGACY_MAGIC = 0x4D455445;

//------------------------------------------------------------
// File header (magic + capacity). Head/count are NOT stored
// persistently; they are reconstructed from the data on boot.
//------------------------------------------------------------

struct Header
{
    uint32_t magic;
    uint16_t capacity;
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
// Reset the RAM ring to all-empty.
//------------------------------------------------------------

static void initRingEmpty()
{
    for (uint16_t i = 0; i < CAPACITY; i++)
    {
        g_ring[i].time = 0;
        g_ring[i].temp = 0x7FFF;
        g_ring[i].hum  = 0xFFFF;
        g_ring[i].pres = 0xFFFF;
    }
    g_head  = 0;
    g_count = 0;
}

//------------------------------------------------------------
// Reconstruct head/count by scanning the records in RAM.
//
// During the initial fill, records are written sequentially
// starting at slot 0; an empty slot (time < MIN_VALID_TIME)
// therefore means the buffer is not yet full and head == 0.
// Once the buffer is full, the newest sample sits right before
// the oldest, so head = (newest + 1) % CAPACITY.
//------------------------------------------------------------

static void reconstructIndex()
{
    g_head  = 0;
    g_count = 0;

    uint16_t  newest  = 0xFFFF;
    uint32_t  maxTime = 0;
    bool      anyEmpty = false;

    for (uint16_t i = 0; i < CAPACITY; i++)
    {
        if (g_ring[i].time < MIN_VALID_TIME)
        {
            anyEmpty = true;
        }
        else if (g_ring[i].time > maxTime)
        {
            maxTime = g_ring[i].time;
            newest  = i;
        }
    }

    if (newest == 0xFFFF)
        return; // no data at all

    if (anyEmpty)
    {
        uint16_t c = 0;
        for (uint16_t i = 0; i < CAPACITY; i++)
            if (g_ring[i].time >= MIN_VALID_TIME) c++;
        g_head  = 0;
        g_count = c;
    }
    else
    {
        g_head  = (uint16_t)((newest + 1) % CAPACITY);
        g_count = CAPACITY;
    }
}

//------------------------------------------------------------
// Write the header + all CAPACITY records from the RAM ring.
// Used once during init / migration, never on a normal sample.
//------------------------------------------------------------

static bool writeFullFile()
{
    uint32_t t0 = millis();

    File f = LittleFS.open(LOG_FILE, "w");
    if (!f)
    {
        Serial.println("MeteoLog: failed to create file");
        return false;
    }

    Header h;
    h.magic    = MAGIC;
    h.capacity = CAPACITY;
    f.write((const uint8_t *)&h, sizeof(h));

    for (uint16_t i = 0; i < CAPACITY; i++)
        f.write((const uint8_t *)&g_ring[i], sizeof(MeteoRecord));

    f.close();

    Serial.printf("MeteoLog: wrote full file (%u B) in %u ms\n",
                  (unsigned)(sizeof(Header) + CAPACITY * sizeof(MeteoRecord)),
                  (unsigned)(millis() - t0));
    return true;
}

//------------------------------------------------------------
// Load the whole file into the RAM ring. Returns false if the
// file is missing, short, or has the wrong magic.
//------------------------------------------------------------

static bool load()
{
    if (!LittleFS.exists(LOG_FILE))
        return false;

    File f = LittleFS.open(LOG_FILE, "r");
    if (!f)
        return false;

    Header h;
    if (f.readBytes((char *)&h, sizeof(h)) != sizeof(h) || h.magic != MAGIC)
    {
        f.close();
        return false;
    }

    initRingEmpty();

    for (uint16_t i = 0; i < CAPACITY; i++)
    {
        if (f.readBytes((char *)&g_ring[i], sizeof(MeteoRecord))
            != sizeof(MeteoRecord))
        {
            break; // short file: leave the rest empty
        }
    }

    f.close();
    return true;
}

//------------------------------------------------------------
// Read the legacy "METE" format into the RAM ring so it can be
// rewritten in the new format. Returns false if the file is not
// a legacy log.
//------------------------------------------------------------

static bool migrateLegacy()
{
    File f = LittleFS.open(LOG_FILE, "r");
    if (!f)
        return false;

    struct LegacyHeader
    {
        uint32_t magic;
        uint16_t capacity;
        uint16_t count;
    } __attribute__((packed));

    LegacyHeader lh;
    if (f.readBytes((char *)&lh, sizeof(lh)) != sizeof(lh) ||
        lh.magic != LEGACY_MAGIC)
    {
        f.close();
        return false;
    }

    initRingEmpty();

    uint16_t n = lh.count;
    if (n > CAPACITY) n = CAPACITY;

    for (uint16_t i = 0; i < n; i++)
    {
        MeteoRecord rec;
        if (f.readBytes((char *)&rec, sizeof(rec)) != sizeof(rec))
            break;
        g_ring[i] = rec;
    }

    f.close();

    Serial.printf("MeteoLog: migrating %u records from legacy format\n",
                  (unsigned)n);
    return true;
}

//------------------------------------------------------------
// Persist one slot in-place (10 bytes only). This is the only
// flash write performed on a normal sample.
//------------------------------------------------------------

static bool writeSlot(uint16_t idx)
{
    File f = LittleFS.open(LOG_FILE, "r+");
    if (!f)
    {
        Serial.println("MeteoLog: failed to open file for writing");
        return false;
    }

    f.seek(sizeof(Header) + (uint32_t)idx * sizeof(MeteoRecord), SeekSet);
    f.write((const uint8_t *)&g_ring[idx], sizeof(MeteoRecord));
    f.close();

    return true;
}

//============================================================

bool meteoLogInit()
{
    initRingEmpty();

    if (!load())
    {
        // Either first boot or legacy format: rebuild the file.
        if (!migrateLegacy())
            Serial.println("MeteoLog: no previous data (starting empty)");
        writeFullFile();
    }

    reconstructIndex();

    Serial.printf("MeteoLog: capacity=%u count=%u head=%u\n",
                  (unsigned)CAPACITY,
                  (unsigned)g_count,
                  (unsigned)g_head);

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

    writeSlot(idx);
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

uint16_t meteoLogSerializeJson(String &out, uint16_t maxPoints,
                               uint32_t from, uint32_t to)
{
    uint16_t head  = g_head;
    uint16_t count = g_count;

    // The ring is sorted ascending by time. Narrow [i0, i1) to the
    // requested window; from/to == 0 means "no bound on that side".
    uint16_t i0 = 0;
    uint16_t i1 = count;

    if (from > 0)
    {
        while (i0 < i1 && g_ring[(head + i0) % CAPACITY].time < from)
            i0++;
    }
    if (to > 0)
    {
        while (i1 > i0 && g_ring[(head + i1 - 1) % CAPACITY].time > to)
            i1--;
    }

    uint16_t n = (i1 > i0) ? (uint16_t)(i1 - i0) : 0;

    uint16_t step = 1;
    if (n > maxPoints && maxPoints > 0)
        step = (uint16_t)((n + maxPoints - 1) / maxPoints);

    uint16_t shown = 0;
    for (uint16_t i = 0; i < n; i += step)
        shown++;

    out.reserve((uint32_t)shown * 40 + 128);

    out = "{\"count\":";
    out += n;
    out += ",\"shown\":";
    out += shown;
    out += ",\"interval\":";
    out += (unsigned long)METEO_LOG_INTERVAL_SEC;
    out += ",\"capacity\":";
    out += (unsigned long)CAPACITY;

    // time
    out += ",\"time\":[";
    for (uint16_t i = 0, k = 0; i < n; i += step, k++)
    {
        if (k) out += ",";
        out += (unsigned long)g_ring[(head + i0 + i) % CAPACITY].time;
    }
    out += "]";

    // temperature (x100 -> degC)
    out += ",\"temp\":[";
    for (uint16_t i = 0, k = 0; i < n; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, g_ring[(head + i0 + i) % CAPACITY].temp,
                    0x7FFF, 0.01F, 2);
    }
    out += "]";

    // humidity (x10 -> %)
    out += ",\"hum\":[";
    for (uint16_t i = 0, k = 0; i < n; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, (int16_t)g_ring[(head + i0 + i) % CAPACITY].hum,
                    (int16_t)0xFFFF, 0.1F, 1);
    }
    out += "]";

    // pressure (x10 -> hPa)
    out += ",\"pres\":[";
    for (uint16_t i = 0, k = 0; i < n; i += step, k++)
    {
        if (k) out += ",";
        appendFloat(out, (int16_t)g_ring[(head + i0 + i) % CAPACITY].pres,
                    (int16_t)0xFFFF, 0.1F, 1);
    }
    out += "]}";

    return n;
}
