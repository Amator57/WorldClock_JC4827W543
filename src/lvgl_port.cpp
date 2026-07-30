#include "lvgl_port.h"
#include "display.h"
//#include "touch.h"

#include <lvgl.h>
#include <esp_heap_caps.h>

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = nullptr;
static lv_disp_drv_t disp_drv;

// Performance measurement, computed the same way as LVGL's built-in
// perf monitor (see lv_refr.c):
//   FPS = 1000 * frames / sum_of_render_times   (capped at refr limit)
//   CPU = 100 - lv_timer_get_idle()
static uint32_t frameCount = 0;   // completed frames in the window
static uint32_t refrMicros = 0;   // accumulated refresh time in the window
static uint32_t fpsValue   = 0;
static uint8_t  cpuPercent = 0;
static uint32_t measLastMs = 0;

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

    // NOTE: lv_disp_flush_ready() clears the "flushing_last" flag, so
    // we must sample it BEFORE signalling completion.
    bool last = lv_disp_flush_is_last(disp);

    lv_disp_flush_ready(disp);

    if (last)
        frameCount++;
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
    uint32_t framesBefore = frameCount;
    uint32_t t0 = micros();

    lv_timer_handler();

    gfx->flush();

    // Charge the elapsed time to rendering only when a frame was
    // actually produced during this handler call.
    if (frameCount > framesBefore)
        refrMicros += (micros() - t0);

    uint32_t now = millis();

    if (now - measLastMs >= 1000)
    {
        uint32_t renderMs = refrMicros / 1000UL;
        if (renderMs == 0)
            renderMs = 1;

        uint32_t fps = 1000UL * frameCount / renderMs;

        uint32_t fpsLimit = 1000UL / LV_DISP_DEF_REFR_PERIOD;
        if (fps > fpsLimit)
            fps = fpsLimit;

        fpsValue   = fps;
        cpuPercent = (uint8_t)(100 - lv_timer_get_idle());

        frameCount = 0;
        refrMicros = 0;
        measLastMs = now;
    }

    delay(5);
}

//------------------------------------------------------

uint32_t lvglGetFPS()
{
    return fpsValue;
}

uint8_t lvglGetCPU()
{
    return (uint8_t)cpuPercent;
}