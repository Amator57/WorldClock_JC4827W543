#include "display.h"

#define GFX_BL 1

// LEDC PWM parameters for the backlight.
static const uint32_t BL_FREQ       = 5000;   // 5 kHz
static const uint8_t  BL_RESOLUTION = 8;      // 8-bit -> 0..255
static const uint8_t  BL_MIN_DUTY   = 16;     // below this the panel flickers / goes dark

//------------------------------------------------------------
// QSPI BUS
//------------------------------------------------------------

static Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    45,     // CS
    47,     // SCK
    21,     // D0
    48,     // D1
    40,     // D2
    39      // D3
);

//------------------------------------------------------------
// LCD
//------------------------------------------------------------

static Arduino_GFX *panel =
    new Arduino_NV3041A(
        bus,
        GFX_NOT_DEFINED,
        0,
        true
    );

//------------------------------------------------------------
// Canvas
//------------------------------------------------------------

Arduino_GFX *gfx =
    new Arduino_Canvas(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        panel
    );

//------------------------------------------------------------

bool displayInit()
{
    ledcAttach(GFX_BL, BL_FREQ, BL_RESOLUTION);
    displaySetBrightness(100);

    if (!gfx->begin())
    {
        Serial.println("Display init FAILED");
        return false;
    }

    gfx->fillScreen(gfx->color565(0,0,0));
gfx->setTextColor(gfx->color565(255,255,255));

//gfx->fillScreen(black);
//gfx->setTextColor(white);
    gfx->setTextSize(2);

    gfx->setCursor(20,30);
    gfx->println("WORLD CLOCK");

    gfx->setTextSize(1);

    gfx->setCursor(20,70);
    gfx->println("Display initialized");

    gfx->setCursor(20,90);
    gfx->println("ESP32-S3");

    gfx->setCursor(20,110);
    gfx->println("480 x 272");

    return true;
}

//------------------------------------------------------------
// Backlight brightness (LEDC PWM)
//------------------------------------------------------------

void displaySetBrightness(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    // Apply a simple gamma curve so low percentages feel linear.
    // x_norm in [0..1] -> duty in [BL_MIN_DUTY..255].
    float norm = percent / 100.0f;
    float gamma = norm * norm;        // y = x^2

    uint32_t duty = BL_MIN_DUTY + (uint32_t)((255 - BL_MIN_DUTY) * gamma);

    // percent == 0 -> backlight fully off.
    if (percent == 0)
        duty = 0;

    ledcWrite(GFX_BL, duty);
}