#include <Arduino.h>

#include "display.h"
#include "lvgl_port.h"

lv_obj_t *labelTitle;
lv_obj_t *labelVersion;
lv_obj_t *labelStatus;

void createGUI()
{
    lv_obj_set_style_bg_color(lv_scr_act(),
                              lv_color_black(),
                              LV_PART_MAIN);

    //--------------------------------------------
    // Заголовок
    //--------------------------------------------

    labelTitle = lv_label_create(lv_scr_act());

    lv_label_set_text(labelTitle,
                      "WORLD CLOCK");

    lv_obj_set_style_text_font(labelTitle,
                               &lv_font_montserrat_28,
                               0);

    lv_obj_align(labelTitle,
                 LV_ALIGN_TOP_MID,
                 0,
                 20);

    //--------------------------------------------
    // Version
    //--------------------------------------------

    labelVersion = lv_label_create(lv_scr_act());

    lv_label_set_text(labelVersion,
                      "Version 1.0");

    lv_obj_set_style_text_font(labelVersion,
                               &lv_font_montserrat_18,
                               0);

    lv_obj_align(labelVersion,
                 LV_ALIGN_TOP_MID,
                 0,
                 70);

    //--------------------------------------------
    // Status
    //--------------------------------------------

    labelStatus = lv_label_create(lv_scr_act());

    lv_label_set_text(labelStatus,
                      "Initializing...");

    lv_obj_set_style_text_font(labelStatus,
                               &lv_font_montserrat_16,
                               0);

    lv_obj_align(labelStatus,
                 LV_ALIGN_BOTTOM_MID,
                 0,
                 -20);
}

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("WORLD CLOCK");
    Serial.println("--------------------------------");

    if (!displayInit())
    {
        Serial.println("Display error");

        while (true)
            delay(100);
    }

    if (!lvglInit())
    {
        Serial.println("LVGL error");

        while (true)
            delay(100);
    }

    createGUI();

    Serial.println("Ready.");
}

void loop()
{
    lvglLoop();
}