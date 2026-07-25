#pragma once

#include <Arduino.h>

//------------------------------------------------------------
// IP5306 power-management IC driver (I2C, slave address 0x75).
//
// The chip is commonly used on battery-equipped ESP32 boards
// (M5Stack, LilyGO, etc.). It exposes:
//   * charge status  (charging / charge complete)
//   * a coarse 4-level battery gauge
//
// This driver REUSES the I2C bus initialised by bme_manager
// (SDA = GPIO17, SCL = GPIO18, 100 kHz). Do not call Wire.begin()
// here -- call ip5306Init() AFTER bmeInit().
//------------------------------------------------------------

#ifndef IP5306_ADDR
#define IP5306_ADDR 0x75
#endif

// READ registers (high addresses, I2C read-only).
#ifndef IP5306_REG_READ0
#define IP5306_REG_READ0 0x70   // charger / power status
#endif

#ifndef IP5306_REG_READ1
#define IP5306_REG_READ1 0x71
#endif

#ifndef IP5306_REG_READ4
#define IP5306_REG_READ4 0x78   // used by some revisions for the
                                // 4-level battery gauge
#endif

//------------------------------------------------------------
// READ0 (0x70) bit map.
//
// These are the bits documented in the Injoinic datasheet and
// confirmed in M5Stack / LilyGO products. Different chip
// revisions exist: if your module behaves differently, check
// the raw register dump printed by ip5306Init() at boot and
// adjust the masks below.
//------------------------------------------------------------

#ifndef IP5306_BIT_CHARGING
#define IP5306_BIT_CHARGING     0x10   // bit4: 1 = charge in progress
#endif

#ifndef IP5306_BIT_CHARGE_FULL
#define IP5306_BIT_CHARGE_FULL  0x80   // bit7: 1 = charge complete
#endif

// Sampling cadence (ms). The IP5306 status changes slowly, so a
// 2-second poll is plenty and keeps the I2C bus free.
#ifndef IP5306_POLL_INTERVAL_MS
#define IP5306_POLL_INTERVAL_MS 2000
#endif

// Initialise the driver: probe address 0x75, read READ0 and dump
// the raw register values to Serial for verification. Returns
// false if no chip answers at 0x75 (the rest of the API will
// then safely report "not present").
bool ip5306Init();

// Non-blocking poll. Call from loop(). Refreshes the cached
// status every IP5306_POLL_INTERVAL_MS.
void ip5306Loop();

// True when a chip answered at 0x75 during ip5306Init().
bool ip5306IsPresent();

// True when the battery is being charged right now.
bool ip5306IsCharging();

// True when the charge is complete (battery full).
bool ip5306IsChargeFull();

// Coarse battery level in percent: 0 / 25 / 50 / 75 / 100.
// Returns -1 when the chip is absent or the level cannot be
// determined (then the UI shows "--").
int8_t ip5306GetLevel();
