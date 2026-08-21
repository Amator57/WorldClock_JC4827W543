#pragma once

#include <Arduino.h>

//============================================================
// Automatic archiving of the meteorological log.
//
// Every METEO_ARCHIVE_INTERVAL_SEC (15 days), at 00:00 local
// time of the FIRST clock (index 0), the current /meteo.bin is
// copied to /archive/meteo_YYYYMMDD.bin (LittleFS). Only the
// newest METEO_ARCHIVE_MAX_FILES copies are kept on flash;
// older ones are deleted.
//
// Archives are transferred to an external network device
// manually: the web UI lists the stored files and the user
// downloads them through the built-in web server at
// http://<device-ip>/archive/<file name>.
//============================================================

#ifndef METEO_ARCHIVE_INTERVAL_SEC
#define METEO_ARCHIVE_INTERVAL_SEC (15UL * 24UL * 3600UL) // TEMP: 1 day for testing (was 15 days)
#endif

#ifndef METEO_ARCHIVE_MAX_FILES
#define METEO_ARCHIVE_MAX_FILES 10 // TEMP: 2 files for testing (was 10)
#endif

//------------------------------------------------------------
// Result of one archive run (used by the web UI "now" action).
//------------------------------------------------------------

struct MeteoArchiveResult
{
    bool copied;                 // local copy in /archive written
    char fileName[32];
};

//------------------------------------------------------------
// Lifecycle
//------------------------------------------------------------

// Create /archive on first boot. Call once after meteoLogInit().
bool meteoArchiveInit();

// Periodic check (throttled internally to once a minute). Call
// from loop(). Runs the archive when a new local day of the
// first clock starts (00:00), the clock is synchronised, the
// log has data and METEO_ARCHIVE_INTERVAL_SEC has elapsed
// since the last successful run.
void meteoArchiveTick();

// Run the archive right now regardless of the schedule.
// Returns true when the local copy was written; details are
// reported through the optional result struct.
bool meteoArchiveRun(MeteoArchiveResult *res = nullptr);

// JSON array of stored archives: [ {"name":..,"size":..} ]
void meteoArchiveListJson(String &out);

// Render one stored archive (the .bin base name, e.g.
// "meteo_20260818.bin") as CSV: unix_time,date_time,temp_c,
// hum_pct,pres_hpa. Timestamps use the local time of the first
// clock; missing sensor values become empty fields. Returns
// false when the file does not exist or has an unknown format.
bool meteoArchiveToCsv(const char *binName, String &out);
