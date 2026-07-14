#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

constexpr uint16_t SCREEN_WIDTH  = 480;
constexpr uint16_t SCREEN_HEIGHT = 272;

extern Arduino_GFX *gfx;

bool displayInit();

#endif