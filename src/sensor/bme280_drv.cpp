#include "bme280_drv.h"

#include <Wire.h>
#include <Adafruit_BME280.h>

static Adafruit_BME280 bme;

bool bme280_drv::init(uint8_t preferredAddr)
{
    // This library version exposes begin(uint8_t addr, TwoWire*).
    // It does not auto-detect the address, so try the scanned one
    // first and fall back to the two common Bosch addresses.
    const uint8_t addrs[3] = { preferredAddr, 0x76, 0x77 };

    for (uint8_t a : addrs)
    {
        if (a == 0)
            continue;

        if (bme.begin(a, &Wire))
            return true;
    }

    return false;
}

void bme280_drv::setForcedWeather()
{
    bme.setSampling(
        Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::FILTER_OFF,
        Adafruit_BME280::STANDBY_MS_1000);
}

bool bme280_drv::readForced(float &temperatureC,
                            float &pressureHpa,
                            float &humidityPct)
{
    bme.takeForcedMeasurement();

    float t = bme.readTemperature();
    float p = bme.readPressure() / 100.0F;   // Pa -> hPa
    float h = bme.readHumidity();

    if (isnan(t) || isnan(p))
        return false;

    temperatureC = t;
    pressureHpa = p;
    humidityPct = h;
    return true;
}
