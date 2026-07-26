#include "lvgl_port.h"
#include "display.h"
//#include "touch.h"

#include <esp_heap_caps.h>

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = nullptr;
static lv_disp_drv_t disp_drv;

//------------------------------------------------------
// Flush callback
//------------------------------------------------------

static void my_disp_flush(lv_disp_drv_t *disp,
                          const lv_area_t *area,
                          lv_color_t *color_p)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(
        area->x1,
        area->y1,
        (uint16_t *)&color_p->full,
        w,
        h);
#else
    gfx->draw16bitRGBBitmap(
        area->x1,
        area->y1,
        (uint16_t *)&color_p->full,
        w,
        h);
#endif

    lv_disp_flush_ready(disp);
}

//------------------------------------------------------
// Touch callback
//------------------------------------------------------
/*
static void my_touchpad_read(lv_indev_drv_t *indev,
                             lv_indev_data_t *data)
{
    if (touch_has_signal())
    {
        if (touch_touched())
        {
            data->state = LV_INDEV_STATE_PR;
            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
            return;
        }
    }

    data->state = LV_INDEV_STATE_REL;
}
*/
//------------------------------------------------------
// LVGL init
//------------------------------------------------------

bool lvglInit()
{
    lv_init();
/*
    touch_init(
        gfx->width(),
        gfx->height(),
        gfx->getRotation());
*/

    const uint32_t bufSize = gfx->width() * 40;

    disp_draw_buf =
        (lv_color_t *)heap_caps_malloc(
            sizeof(lv_color_t) * bufSize,
            MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (!disp_draw_buf)
    {
        disp_draw_buf =
            (lv_color_t *)heap_caps_malloc(
                sizeof(lv_color_t) * bufSize,
                MALLOC_CAP_8BIT);
    }

    if (!disp_draw_buf)
        return false;

    lv_disp_draw_buf_init(
        &draw_buf,
        disp_draw_buf,
        NULL,
        bufSize);

    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = gfx->width();
    disp_drv.ver_res = gfx->height();

    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = my_disp_flush;

    lv_disp_drv_register(&disp_drv);
/*
    static lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);

    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;

    lv_indev_drv_register(&indev_drv);
*/
    return true;
}

//------------------------------------------------------

// LVGL creates the perf-monitor label lazily on the first refresh
// (see lv_refr.c) with a hard-coded bright-white text color, which
// stands out against the otherwise dim UI. With LV_THEME_DEFAULT_DARK
// enabled the rest of the UI is also light, so the default perf label
// style now matches and no override is needed.

void lvglLoop()
{
    lv_timer_handler();

    gfx->flush();

    delay(5);
}