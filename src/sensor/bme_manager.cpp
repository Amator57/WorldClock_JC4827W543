#include "bme_manager.h"

#include <Wire.h>
#include <Adafruit_BMP280.h>

//------------------------------------------------------------
// Note: the module is named bme_manager for historical reasons,
// but the actual hardware is a BMP280 (chipID 0x58). The BMP280
// has no humidity sensor, so humidity is always reported as NAN.
//------------------------------------------------------------

static Adafruit_BMP280 bmp;
static bool sensorReady = false;

static float tempBuf[BME_AVG_SAMPLES];
static float presBuf[BME_AVG_SAMPLES];

static uint8_t bufIndex = 0;
static uint8_t bufCount = 0;

static uint32_t lastSample = 0;

//------------------------------------------------------------

bool bmeInit()
{
    Wire.begin(BME_SDA, BME_SCL);
    Wire.setClock(100000); // 100 kHz, safe for long wires / cheap modules

    //--------------------------------------------------------
    // I2C diagnostic scan
    //--------------------------------------------------------
    Serial.println("BMP280: scanning I2C bus...");
    Serial.printf("BMP280: SDA=%d SCL=%d\n", BME_SDA, BME_SCL);

    uint8_t foundAddr = 0;
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();

        if (err == 0)
        {
            // Read chip ID from register 0xD0 to identify the chip.
            // BMP280 -> 0x58, BME280 -> 0x60.
            Wire.beginTransmission(addr);
            Wire.write(0xD0);
            Wire.endTransmission();
            uint8_t chipId = 0;
            if (Wire.requestFrom((int)addr, 1) == 1)
                chipId = Wire.read();

            Serial.printf("BMP280: found device at 0x%02X  chipID=0x%02X\n",
                          addr, chipId);

            if (foundAddr == 0)
                foundAddr = addr;
        }
    }

    if (foundAddr == 0)
    {
        Serial.println("BMP280: no I2C devices found! Check wiring/power/pull-ups.");
        sensorReady = false;
        return false;
    }

    //--------------------------------------------------------
    // Try to initialise the BMP280 driver. Adafruit_BMP280::begin
    // uses the global Wire object (set up above with our pins).
    //--------------------------------------------------------
    bool ok = bmp.begin(foundAddr);
    if (!ok && foundAddr != 0x76)
        ok = bmp.begin(0x76);
    if (!ok && foundAddr != 0x77)
        ok = bmp.begin(0x77);

    if (!ok)
    {
        Serial.println("BMP280: device present but driver init failed.");
        sensorReady = false;
        return false;
    }

    // Weather monitoring: low-power forced mode, 1x oversampling.
    bmp.setSampling(
        Adafruit_BMP280::MODE_FORCED,
        Adafruit_BMP280::SAMPLING_X1,
        Adafruit_BMP280::SAMPLING_X1,
        Adafruit_BMP280::FILTER_OFF);

    sensorReady = true;

    Serial.println("BMP280: initialized");

    // Prime the buffer so the environment view has data to show
    // during the first display cycle. The 1-second time gate in
    // bmeLoop() lets only the first sample through here; further
    // samples accumulate once per second during normal operation.
    for (uint8_t i = 0; i < BME_AVG_SAMPLES; i++)
        bmeLoop();

    return true;
}

//------------------------------------------------------------

void bmeLoop()
{
    if (!sensorReady)
        return;

    if (millis() - lastSample < BME_SAMPLE_INTERVAL_MS)
        return;

    lastSample = millis();

    // Trigger one conversion and wait for completion.
    bmp.takeForcedMeasurement();

    float t = bmp.readTemperature();
    float p = bmp.readPressure() / 100.0F; // Pa -> hPa

    // Filter out obvious read errors.
    if (isnan(t) || isnan(p))
        return;

    tempBuf[bufIndex] = t;
    presBuf[bufIndex] = p;

    bufIndex = (bufIndex + 1) % BME_AVG_SAMPLES;
    if (bufCount < BME_AVG_SAMPLES)
        bufCount++;
}

//------------------------------------------------------------

bool bmeIsAvailable()
{
    return sensorReady;
}

//------------------------------------------------------------

bool bmeGetAverage(float &temperatureC,
                   float &humidityPct,
                   float &pressureHpa)
{
    // The BMP280 has no humidity sensor.
    humidityPct = NAN;

    if (!sensorReady || bufCount == 0)
    {
        temperatureC = NAN;
        pressureHpa  = NAN;
        return false;
    }

    float sumT = 0.0F;
    float sumP = 0.0F;

    for (uint8_t i = 0; i < bufCount; i++)
    {
        sumT += tempBuf[i];
        sumP += presBuf[i];
    }

    temperatureC = sumT / bufCount;
    pressureHpa  = sumP / bufCount;

    return true;
}
