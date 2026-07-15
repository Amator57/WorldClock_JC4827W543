#pragma once

#include <lvgl.h>

//------------------------------------------------------------
// Ініціалізація головного екрана
//------------------------------------------------------------
void screenMainCreate();

//------------------------------------------------------------
// Оновлення часу
//------------------------------------------------------------
void screenMainUpdate(
    const char *city1,
    const char *time1,
    const char *city2,
    const char *time2,
    const char *city3,
    const char *time3,
    const char *date,
    const char *wifi,
    const char *ntp,
    const char *ip
);