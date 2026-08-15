#pragma once

#include <stdint.h>
#include <stddef.h>

//============================================================
// Pre-recorded firework explosion sound.
//
// Mono, 22050 Hz, 16-bit signed. Stored in flash and streamed
// to the I2S DAC in chunks (no large RAM buffer needed).
//============================================================

extern const int16_t *EXPLOSION_DATA;
extern const size_t   EXPLOSION_DATA_SAMPLES;
