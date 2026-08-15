#pragma once

#include <Arduino.h>

//============================================================
// Audio output via the on-board I2S DAC (NS4168 amplifier).
//
// Hardware (JC4827W543):
//   I2S1 BCLK  -> GPIO 42
//   I2S1 LRCK  -> GPIO  2
//   I2S1 DOUT  -> GPIO 41
//
// The driver is intentionally minimal: a single pre-computed
// "explosion" buffer is played back through I2S NUM_1 whenever
// audioPlayExplosion() is triggered. A dedicated FreeRTOS task
// drains the I2S DMA queue so the LVGL animation never blocks.
//============================================================

//------------------------------------------------------------
// Lifecycle
//------------------------------------------------------------

// Initialise I2S NUM_1 (22050 Hz, 16-bit, stereo) and pre-compute
// the explosion sample. Must be called once from setup().
bool audioInit();

//------------------------------------------------------------
// Playback
//------------------------------------------------------------

// Trigger a firework explosion sound. Non-blocking: if a previous
// explosion is still playing, the request collapses into the
// pending one. Honours the muted flag.
void audioPlayExplosion();

// Play an arbitrary mono PCM phrase (22050 Hz, 16-bit signed).
// The caller must keep the data alive for the duration of playback.
// Non-blocking; honours the muted flag.
void audioPlayPhrase(const int16_t *data, size_t sampleCount);

//------------------------------------------------------------
// Mute (persistent via Preferences)
//------------------------------------------------------------

void audioSetMuted(bool muted);
bool audioIsMuted();

//------------------------------------------------------------
// Volume (0..100 %, persistent via Preferences)
//------------------------------------------------------------

void audioSetVolume(uint8_t percent);
uint8_t audioGetVolume();
