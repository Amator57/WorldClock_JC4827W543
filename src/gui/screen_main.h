#pragma once

#include <lvgl.h>

//------------------------------------------------------------
// Create screen
//------------------------------------------------------------

void screenMainCreate();

//------------------------------------------------------------
// Update screen
//------------------------------------------------------------

void screenMainUpdate(

    const char *city1,
    const char *time1,

    const char *city2,
    const char *time2,

    const char *city3,
    const char *time3,

    const char *city4,
    const char *time4,

    const char *city5,
    const char *time5,

    const char *date,

    const char *wifi,

    const char *ntp,

    const char *ip);