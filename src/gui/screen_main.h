#pragma once

#include <lvgl.h>

//------------------------------------------------------------
// Create screen
//------------------------------------------------------------

void screenMainCreate();

void startSaluteAnimation();

//------------------------------------------------------------
// Display views
//------------------------------------------------------------

enum ScreenView : uint8_t
{
    SCREEN_VIEW_CLOCK = 0,
    SCREEN_VIEW_ENV   = 1
};

// Toggle between clock view and environment view.
void screenViewSet(uint8_t view);

// Update the environment view with averaged sensor data.
// Pass NAN for any value that is unavailable (shown as "--").
void screenEnvUpdate(float temperatureC,
                     float humidityPct,
                     float pressureHpa);

// Configure the environment view for the connected sensor.
// When hasHumidity is false (BMP280) the humidity row is hidden and
// the remaining two rows are re-centred. Call once after the sensor
// is initialised and the screen is created.
void screenEnvSetHumidity(bool hasHumidity);

//------------------------------------------------------------
// Battery / power status (IP5306)
//
// Shown in the top-right corner of the clock view.
//   present  -> false hides the widget entirely
//   level    -> 0..100, or -1 when unknown (shown as "--")
//   charging -> true draws the "+" (charge-in-progress) marker
//------------------------------------------------------------
void screenBatteryUpdate(bool present, int8_t level, bool charging);

//------------------------------------------------------------
// Update screen
//------------------------------------------------------------

void screenMainUpdate(
    const char *city1,
    const char *time1,
    uint8_t markerState1,

    const char *city2,
    const char *time2,
    uint8_t markerState2,

    const char *city3,
    const char *time3,
    uint8_t markerState3,

    const char *city4,
    const char *time4,
    uint8_t markerState4,

    const char *city5,
    const char *time5,
    uint8_t markerState5,

    const char *date,

    const char *wifi,

    const char *ntp,

    const char *ip);

//------------------------------------------------------------
// Update only the always-visible status row (WiFi / NTP / IP).
// Use this from views that otherwise skip screenMainUpdate() so
// the IP address and network state stay current on the display.
//------------------------------------------------------------

void screenStatusUpdate(const char *wifi,
                        const char *ntp,
                        const char *ip);

// Update the FPS/CPU readout shown at the right end of the status row.
void screenPerfUpdate(uint32_t fps, uint8_t cpu);