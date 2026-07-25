#include "ip5306_manager.h"

#include <Wire.h>

//------------------------------------------------------------
// All status is cached here after each poll so the GUI can read
// it without touching the I2C bus.
//------------------------------------------------------------

static bool     present     = false;
static uint8_t  statusByte  = 0x00;   // last READ0
static uint8_t  gaugeByte   = 0x00;   // last READ4 (4-level gauge)
static int8_t   level       = -1;
static uint32_t lastPoll    = 0;

//------------------------------------------------------------
// Low-level I2C helpers (bus is owned by bme_manager).
//------------------------------------------------------------

static bool readRegister(uint8_t reg, uint8_t &value)
{
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
        return false;

    if (Wire.requestFrom((int)IP5306_ADDR, 1) != 1)
        return false;

    value = Wire.read();
    return true;
}

//------------------------------------------------------------
// Detect a device at 0x75 with a plain ACK probe.
//------------------------------------------------------------

static bool probeAddress(uint8_t addr)
{
    Wire.beginTransmission(addr);
    return (Wire.endTransmission() == 0);
}

//------------------------------------------------------------
// Decode the cached status into a coarse 0-100 level.
//
//   * charge complete  -> 100 %
//   * otherwise        -> 4-level gauge from READ4 when available
//
// The READ4 mapping below matches the Injoinic datasheet: the top
// nibble reports 0..4 "gas-gauge" segments. If your module leaves
// READ4 at 0x00 / 0xFF, the function falls back to -1 (unknown)
// and the UI displays the charge state instead of a percentage.
//------------------------------------------------------------

static int8_t decodeLevel()
{
    if (statusByte & IP5306_BIT_CHARGE_FULL)
        return 100;

    const uint8_t gauge = (gaugeByte >> 4) & 0x0F;

    switch (gauge)
    {
        case 0: return 0;
        case 1: return 25;
        case 2: return 50;
        case 3: return 75;
        default: break;
    }

    return -1;
}

//------------------------------------------------------------

bool ip5306Init()
{
    present = false;

    if (!probeAddress(IP5306_ADDR))
    {
        Serial.println("IP5306: no device at 0x75 (disabled).");
        return false;
    }

    present = true;

    // Read the current register snapshot and dump it so the
    // developer can confirm the bit interpretation for the
    // specific module in use.
    uint8_t r0 = 0, r1 = 0, r4 = 0;
    readRegister(IP5306_REG_READ0, r0);
    readRegister(IP5306_REG_READ1, r1);
    readRegister(IP5306_REG_READ4, r4);

    statusByte = r0;
    gaugeByte  = r4;
    level      = decodeLevel();

    Serial.println("IP5306: detected at 0x75");
    Serial.printf("IP5306: READ0(0x70)=0x%02X READ1(0x71)=0x%02X READ4(0x78)=0x%02X\n",
                  r0, r1, r4);
    Serial.printf("IP5306: charging=%d full=%d level=%d\n",
                  (int)ip5306IsCharging(),
                  (int)ip5306IsChargeFull(),
                  (int)level);

    return true;
}

//------------------------------------------------------------

void ip5306Loop()
{
    if (!present)
        return;

    if (millis() - lastPoll < IP5306_POLL_INTERVAL_MS)
        return;

    lastPoll = millis();

    uint8_t r0 = 0;
    uint8_t r4 = 0;

    if (!readRegister(IP5306_REG_READ0, r0))
    {
        // Bus error -- keep the last known good state.
        return;
    }

    statusByte = r0;

    if (readRegister(IP5306_REG_READ4, r4))
        gaugeByte = r4;

    level = decodeLevel();
}

//------------------------------------------------------------

bool ip5306IsPresent()
{
    return present;
}

bool ip5306IsCharging()
{
    return present && (statusByte & IP5306_BIT_CHARGING);
}

bool ip5306IsChargeFull()
{
    return present && (statusByte & IP5306_BIT_CHARGE_FULL);
}

int8_t ip5306GetLevel()
{
    return present ? level : -1;
}
