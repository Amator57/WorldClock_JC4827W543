#include "meteo_archive.h"

#include <LittleFS.h>
#include <time.h>
#include <algorithm>

#include "preferences_manager.h"
#include "meteo_log.h"
#include "../clock/clock_engine.h"

static const char *ARCHIVE_DIR  = "/archive";
static const char *LOG_FILE     = "/meteo.bin";

// Same lower bound as meteo_log: detects an unsynchronised clock.
static const uint32_t MIN_VALID_TIME = 1700000000; // ~Nov 2023

static bool     s_initialised = false;
static bool     s_running     = false;
static uint32_t s_lastCheckMs = 0;
static uint32_t s_lastLocalDay = 0; // local day (first clock) last seen

//------------------------------------------------------------
// List archive file names (basenames) sorted ascending. Because
// the names are meteo_YYYYMMDD.bin, lexical order equals
// chronological order. Returns the number of files found.
//------------------------------------------------------------

static uint8_t listArchiveNames(String *names, uint8_t maxNames)
{
    uint8_t count = 0;

    File dir = LittleFS.open(ARCHIVE_DIR);
    if (!dir || !dir.isDirectory())
        return 0;

    File f;
    while ((f = dir.openNextFile()))
    {
        String name = f.name();
        f.close();

        // Some core versions return the full path instead of
        // the basename; keep only the file name part.
        int slash = name.lastIndexOf('/');
        if (slash >= 0)
            name = name.substring(slash + 1);

        if (!name.startsWith("meteo_") || !name.endsWith(".bin"))
            continue;

        // Insertion sort ascending.
        uint8_t pos = count;
        while (pos > 0 && names[pos - 1] > name)
        {
            if (pos < maxNames)
                names[pos] = names[pos - 1];
            pos--;
        }

        if (count < maxNames)
            names[pos] = name;
        count++;
    }

    dir.close();
    return count; // may exceed maxNames; extra entries are dropped
}

//------------------------------------------------------------
// Delete the oldest archives so that at most
// METEO_ARCHIVE_MAX_FILES remain in /archive.
//------------------------------------------------------------

static void rotateArchives()
{
    String names[METEO_ARCHIVE_MAX_FILES + 4];
    uint8_t count = listArchiveNames(
        names, METEO_ARCHIVE_MAX_FILES + 4);

    if (count <= METEO_ARCHIVE_MAX_FILES)
        return;

    uint8_t toDelete = count - METEO_ARCHIVE_MAX_FILES;
    for (uint8_t i = 0; i < toDelete && i < METEO_ARCHIVE_MAX_FILES + 4; i++)
    {
        String path = String(ARCHIVE_DIR) + "/" + names[i];
        LittleFS.remove(path);
        Serial.printf("MeteoArchive: rotated out %s\n", path.c_str());
    }
}

//------------------------------------------------------------
// Copy /meteo.bin -> /archive/meteo_YYYYMMDD.bin in chunks.
//------------------------------------------------------------

static bool copyToArchive(const char *destName)
{
    String destPath = String(ARCHIVE_DIR) + "/" + destName;

    File in = LittleFS.open(LOG_FILE, "r");
    if (!in)
        return false;

    File out = LittleFS.open(destPath, "w");
    if (!out)
    {
        in.close();
        return false;
    }

    static uint8_t buf[4096];
    size_t total = 0;
    while (in.available())
    {
        size_t n = in.read(buf, sizeof(buf));
        if (n == 0)
            break;
        out.write(buf, n);
        total += n;
    }

    in.close();
    out.close();

    Serial.printf("MeteoArchive: copied %u bytes to %s\n",
                  (unsigned)total, destPath.c_str());
    return total > 0;
}

//============================================================

bool meteoArchiveInit()
{
    if (!LittleFS.exists(ARCHIVE_DIR))
        LittleFS.mkdir(ARCHIVE_DIR);

    s_initialised = true;
    return true;
}

//------------------------------------------------------------

bool meteoArchiveRun(MeteoArchiveResult *res)
{
    if (!s_initialised || s_running)
        return false;

    if (meteoLogCount() == 0)
    {
        Serial.println("MeteoArchive: nothing to archive (empty log)");
        return false;
    }

    s_running = true;

    MeteoArchiveResult local;
    local.copied      = false;
    local.fileName[0] = '\0';

    // Build the destination name from the local date.
    time_t now = time(nullptr);
    struct tm t;
    localtime_r(&now, &t);
    snprintf(local.fileName, sizeof(local.fileName),
             "meteo_%04d%02d%02d.bin",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

    if (copyToArchive(local.fileName))
    {
        rotateArchives();

        // The archive folder is now full (the last of the
        // METEO_ARCHIVE_MAX_FILES slots is used): raise the
        // reminder flag so the clock shows the "archive needed"
        // notice until the files are downloaded to a computer.
        String names[METEO_ARCHIVE_MAX_FILES + 4];
        if (listArchiveNames(names, METEO_ARCHIVE_MAX_FILES + 4)
                >= METEO_ARCHIVE_MAX_FILES)
            prefSetArchiveRemind(true);

        prefSetArchiveLast((uint32_t)now);
        local.copied = true;
    }

    if (res)
        *res = local;

    s_running = false;
    return local.copied;
}

//------------------------------------------------------------

void meteoArchiveTick()
{
    if (!s_initialised || s_running)
        return;

    uint32_t nowMs = millis();
    if (s_lastCheckMs != 0 && nowMs - s_lastCheckMs < 60000)
        return;
    s_lastCheckMs = nowMs;

    uint32_t now = (uint32_t)time(nullptr);
    if (now < MIN_VALID_TIME)
        return; // clock not synchronised yet

    // Local time of the FIRST clock (index 0): the archive is
    // scheduled at 00:00 in that zone, detected as a local-day
    // rollover (robust even if a check is missed right at 00:00).
    int32_t offset = clockEngineClockUtcOffsetSec(0);
    uint32_t localDay = (now + (uint32_t)offset) / 86400UL;

    // First valid check: just remember the current day so no
    // archive is created in the middle of the day at boot.
    if (s_lastLocalDay == 0)
    {
        s_lastLocalDay = localDay;
        return;
    }

    if (localDay == s_lastLocalDay)
        return;
    s_lastLocalDay = localDay; // new local day just started

    uint32_t last = prefGetArchiveLast();
    if (last != 0 && now - last < METEO_ARCHIVE_INTERVAL_SEC)
        return;

    meteoArchiveRun();
}

//------------------------------------------------------------

void meteoArchiveListJson(String &out)
{
    String names[METEO_ARCHIVE_MAX_FILES + 4];
    uint8_t count = listArchiveNames(
        names, METEO_ARCHIVE_MAX_FILES + 4);

    out = "[";
    for (uint8_t i = 0; i < count && i < METEO_ARCHIVE_MAX_FILES + 4; i++)
    {
        String path = String(ARCHIVE_DIR) + "/" + names[i];
        size_t size = 0;

        File f = LittleFS.open(path, "r");
        if (f)
        {
            size = f.size();
            f.close();
        }

        if (i) out += ",";
        out += "{\"name\":\"" + names[i] +
               "\",\"size\":" + String((unsigned long)size) + "}";
    }
    out += "]";
}

//------------------------------------------------------------
// Render one stored archive as CSV (see meteo_archive.h).
//
// The file layout mirrors meteo_log: a 6-byte header (magic +
// capacity) followed by capacity packed records. The ring order
// on flash is not chronological, so valid records are collected,
// sorted by time and emitted oldest-first.
//------------------------------------------------------------

bool meteoArchiveToCsv(const char *binName, String &out)
{
    String path = String(ARCHIVE_DIR) + "/" + binName;

    File f = LittleFS.open(path, "r");
    if (!f)
        return false;

    struct
    {
        uint32_t magic;    // "METF" (new) or "METE" (legacy)
        uint16_t capacity;
    } __attribute__((packed)) hdr;

    if (f.read((uint8_t *)&hdr, sizeof(hdr)) != sizeof(hdr) ||
        (hdr.magic != 0x4D455446 && hdr.magic != 0x4D455445))
    {
        f.close();
        return false;
    }

    uint16_t count = hdr.capacity;
    size_t avail = (f.size() - sizeof(hdr)) / sizeof(MeteoRecord);
    if (count > avail)
        count = avail;

    out = "unix_time,date_time,temp_c,hum_pct,pres_hpa";

    if (count == 0)
    {
        f.close();
        return true;
    }

    MeteoRecord *recs = new MeteoRecord[count];
    if (!recs)
    {
        f.close();
        return false;
    }

    uint16_t n = 0;
    for (uint16_t i = 0; i < count; i++)
    {
        MeteoRecord r;
        if (f.read((uint8_t *)&r, sizeof(r)) != sizeof(r))
            break;
        if (r.time >= MIN_VALID_TIME) // skip empty ring slots
            recs[n++] = r;
    }
    f.close();

    std::sort(recs, recs + n,
              [](const MeteoRecord &a, const MeteoRecord &b)
              { return a.time < b.time; });

    // Wall time of the first clock, like the web UI charts.
    int32_t offset = clockEngineClockUtcOffsetSec(0);

    out.reserve((uint32_t)n * 48 + 64);

    char line[96];
    for (uint16_t i = 0; i < n; i++)
    {
        const MeteoRecord &r = recs[i];

        time_t t = (time_t)((uint32_t)r.time + (uint32_t)offset);
        struct tm lt;
        gmtime_r(&t, &lt);

        char dt[24];
        snprintf(dt, sizeof(dt), "%04d-%02d-%02d %02d:%02d",
                 lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
                 lt.tm_hour, lt.tm_min);

        int len = snprintf(line, sizeof(line), "\n%u,%s,",
                           (unsigned)r.time, dt);
        if (r.temp != 0x7FFF)
            len += snprintf(line + len, sizeof(line) - len,
                            "%.1f", r.temp / 100.0F);
        len += snprintf(line + len, sizeof(line) - len, ",");
        if (r.hum != 0xFFFF)
            len += snprintf(line + len, sizeof(line) - len,
                            "%.1f", r.hum / 10.0F);
        len += snprintf(line + len, sizeof(line) - len, ",");
        if (r.pres != 0xFFFF)
            snprintf(line + len, sizeof(line) - len,
                     "%.1f", r.pres / 10.0F);

        out += line;
    }

    delete[] recs;
    return true;
}
