#include "display.h"

#define GFX_BL 1

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
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);

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