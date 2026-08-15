#include "meteo_archive.h"

#include <LittleFS.h>
#include <time.h>

#include "preferences_manager.h"
#include "meteo_log.h"

static const char *ARCHIVE_DIR  = "/archive";
static const char *LOG_FILE     = "/meteo.bin";

// Same lower bound as meteo_log: detects an unsynchronised clock.
static const uint32_t MIN_VALID_TIME = 1700000000; // ~Nov 2023

static bool     s_initialised = false;
static bool     s_running     = false;
static uint32_t s_lastCheckMs = 0;

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

    out = "{\"files\":[";
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
    out += "]}";
}
