#include "bme_manager.h"

#include <Wire.h>
#include <Adafruit_BMP280.h>

#include "bme280_drv.h"
#include "../storage/meteo_log.h"

//------------------------------------------------------------
// Supports both BMP280 (temp + pressure) and BME280
// (temp + humidity + pressure). The chip is detected automatically
// via the chip ID register (0x58 = BMP280, 0x60 = BME280); no
// manual configuration is needed.
//------------------------------------------------------------

enum SensorKind
{
    KIND_NONE   = 0,
    KIND_BMP280 = 1,
    KIND_BME280 = 2
};

static Adafruit_BMP280 bmp;

static bool sensorReady = false;
static uint8_t activeKind = KIND_NONE;
static bool hasHumidity = false;

static float tempBuf[BME_AVG_SAMPLES];
static float humBuf[BME_AVG_SAMPLES];
static float presBuf[BME_AVG_SAMPLES];

static uint8_t bufIndex = 0;
static uint8_t bufCount = 0;

static uint32_t lastSample = 0;

// 5-minute meteo logger trigger.
static uint32_t lastLog = 0;

//------------------------------------------------------------

static uint8_t detectKindByChipId(uint8_t addr)
{
    Wire.beginTransmission(addr);
    Wire.write(0xD0); // chip ID register
    Wire.endTransmission();

    uint8_t chipId = 0;
    if (Wire.requestFrom((int)addr, 1) == 1)
        chipId = Wire.read();

    // 0x60 -> BME280, anything else (e.g. 0x58) -> BMP280.
    return (chipId == 0x60) ? KIND_BME280 : KIND_BMP280;
}

//------------------------------------------------------------

bool bmeInit()
{
    Wire.begin(BME_SDA, BME_SCL);
    Wire.setClock(100000); // 100 kHz, safe for long wires / cheap modules

    //--------------------------------------------------------
    // I2C diagnostic scan
    //--------------------------------------------------------
    Serial.println("BME/BMP280: scanning I2C bus...");
    Serial.printf("BME/BMP280: SDA=%d SCL=%d\n", BME_SDA, BME_SCL);

    uint8_t foundAddr = 0;
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();

        if (err == 0)
        {
            Wire.beginTransmission(addr);
            Wire.write(0xD0);
            Wire.endTransmission();

            uint8_t chipId = 0;
            if (Wire.requestFrom((int)addr, 1) == 1)
                chipId = Wire.read();

            Serial.printf("BME/BMP280: found device at 0x%02X  chipID=0x%02X\n",
                          addr, chipId);

            if (foundAddr == 0)
                foundAddr = addr;
        }
    }

    if (foundAddr == 0)
    {
        Serial.println("BME/BMP280: no I2C devices found! Check wiring/power/pull-ups.");
        sensorReady = false;
        activeKind = KIND_NONE;
        hasHumidity = false;
        return false;
    }

    //--------------------------------------------------------
    // Detect the chip by its ID.
    //--------------------------------------------------------
    uint8_t kind = detectKindByChipId(foundAddr);

    bool ok = false;

    if (kind == KIND_BME280)
    {
        ok = bme280_drv::init(foundAddr);

        // Re-assert our I2C pins in case the library touched Wire.
        Wire.begin(BME_SDA, BME_SCL);
        Wire.setClock(100000);

        if (ok)
        {
            bme280_drv::setForcedWeather();
            hasHumidity = true;
        }
    }
    else // BMP280
    {
        ok = bmp.begin(foundAddr);
        if (!ok && foundAddr != 0x76)
            ok = bmp.begin(0x76);
        if (!ok && foundAddr != 0x77)
            ok = bmp.begin(0x77);

        if (ok)
        {
            bmp.setSampling(
                Adafruit_BMP280::MODE_FORCED,
                Adafruit_BMP280::SAMPLING_X1,
                Adafruit_BMP280::SAMPLING_X1,
                Adafruit_BMP280::FILTER_OFF);
            hasHumidity = false;
        }
    }

    if (!ok)
    {
        Serial.println("BME/BMP280: device present but driver init failed.");
        sensorReady = false;
        activeKind = KIND_NONE;
        hasHumidity = false;
        return false;
    }

    activeKind   = kind;
    sensorReady  = true;

    Serial.printf("%s: initialized (humidity %s)\n",
                  bmeGetActiveSensorName(),
                  hasHumidity ? "enabled" : "disabled");

    // Prime the buffer so the environment view has data to show
    // during the first display cycle.
    for (uint8_t i = 0; i < BME_AVG_SAMPLES; i++)
        bmeLoop();

    // Start the 5-minute logger countdown from boot so the first
    // sample is recorded 5 minutes after start, not immediately.
    lastLog = millis();

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

    float t = NAN;
    float p = NAN;
    float h = NAN;

    if (activeKind == KIND_BME280)
    {
        if (!bme280_drv::readForced(t, p, h))
            return;
    }
    else
    {
        bmp.takeForcedMeasurement();

        t = bmp.readTemperature();
        p = bmp.readPressure() / 100.0F; // Pa -> hPa

        if (isnan(t) || isnan(p))
            return;

        h = NAN;
    }

    tempBuf[bufIndex] = t;
    humBuf[bufIndex]  = h;
    presBuf[bufIndex] = p;

    bufIndex = (bufIndex + 1) % BME_AVG_SAMPLES;
    if (bufCount < BME_AVG_SAMPLES)
        bufCount++;

    //--------------------------------------------------------
    // Meteorological logger: store the running average every
    // 5 minutes.
    //--------------------------------------------------------
    if (millis() - lastLog >= (uint32_t)METEO_LOG_INTERVAL_SEC * 1000UL)
    {
        lastLog = millis();

        float avgT, avgH, avgP;
        bmeGetAverage(avgT, avgH, avgP);
        meteoLogAddSample(avgT, avgH, avgP);
    }
}

//------------------------------------------------------------

bool bmeIsAvailable()
{
    return sensorReady;
}

//------------------------------------------------------------

bool bmeHasHumidity()
{
    return sensorReady && hasHumidity;
}

//------------------------------------------------------------

const char *bmeGetActiveSensorName()
{
    if (!sensorReady)
        return "none";

    if (activeKind == KIND_BME280)
        return "BME280";

    if (activeKind == KIND_BMP280)
        return "BMP280";

    return "none";
}

//------------------------------------------------------------

bool bmeGetAverage(float &temperatureC,
                   float &humidityPct,
                   float &pressureHpa)
{
    humidityPct = NAN;

    if (!sensorReady || bufCount == 0)
    {
        temperatureC = NAN;
        pressureHpa  = NAN;
        return false;
    }

    float sumT = 0.0F;
    float sumP = 0.0F;
    float sumH = 0.0F;
    uint8_t hCount = 0;

    for (uint8_t i = 0; i < bufCount; i++)
    {
        sumT += tempBuf[i];
        sumP += presBuf[i];

        if (!isnan(humBuf[i]))
        {
            sumH += humBuf[i];
            hCount++;
        }
    }

    temperatureC = sumT / bufCount;
    pressureHpa  = sumP / bufCount;

    if (hasHumidity && hCount > 0)
        humidityPct = sumH / hCount;

    return true;
}
