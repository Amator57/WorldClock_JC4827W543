#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include <stdint.h>

bool lvglInit();
void lvglLoop();

// Estimated rendering frame rate over the last second.
// Returns 0 until at least one measurement window has elapsed.
uint32_t lvglGetFPS();

// Estimated CPU load (0..100) spent inside the LVGL refresh path
// over the last second.
uint8_t lvglGetCPU();

#endif
