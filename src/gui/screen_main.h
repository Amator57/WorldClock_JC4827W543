#pragma once

#include <lvgl.h>

//------------------------------------------------------------
// Create screen
//------------------------------------------------------------

void screenMainCreate();

void startSaluteAnimation();

//------------------------------------------------------------
// Update screen
//------------------------------------------------------------

void screenMainUpdate(
    const char *city1,
    const char *time1,
    uint8_t markerState1,

    const char *city2,
    const char *time2,
    uint8_t markerState2,

    const char *city3,
    const char *time3,
    uint8_t markerState3,

    const char *city4,
    const char *time4,
    uint8_t markerState4,

    const char *city5,
    const char *time5,
    uint8_t markerState5,

    const char *date,

    const char *wifi,

    const char *ntp,

    const char *ip);