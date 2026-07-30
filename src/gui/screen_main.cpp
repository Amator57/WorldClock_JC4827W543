#include "screen_main.h"
#include <Arduino.h>
#include <math.h>
#include <stdlib.h>
#include "../fonts/font_clock.h"

//------------------------------------------------------------
// Objects
//------------------------------------------------------------

static lv_obj_t *lblTitle;

static lv_obj_t *lblCity1;
static lv_obj_t *lblTime1;

static lv_obj_t *lblCity2;
static lv_obj_t *lblTime2;

static lv_obj_t *lblCity3;
static lv_obj_t *lblTime3;

static lv_obj_t *lblCity4;
static lv_obj_t *lblTime4;

static lv_obj_t *lblCity5;
static lv_obj_t *lblTime5;

static lv_obj_t *lblDate;

static lv_obj_t *lblWiFi;
static lv_obj_t *lblNTP;
static lv_obj_t *lblIP;
static lv_obj_t *lblFPS;
static lv_obj_t *lblCPU;

static lv_obj_t *markerClocks[5];
static lv_obj_t *markerBars[5];

//------------------------------------------------------------
// Environment view objects
//------------------------------------------------------------

static lv_obj_t *lblEnvTitle;
static lv_obj_t *lblTempLabel;
static lv_obj_t *lblTempValue;
static lv_obj_t *lblHumLabel;
static lv_obj_t *lblHumValue;
static lv_obj_t *lblPresLabel;
static lv_obj_t *lblPresValue;

// True when the active sensor has a humidity channel (BME280).
static bool envHasHumidity = true;

//------------------------------------------------------------
// Battery / power status (IP5306) widget
//------------------------------------------------------------

static lv_obj_t *lblBattery;

//------------------------------------------------------------

void screenMainCreate()
{
    lv_obj_t *scr = lv_scr_act();

    lv_obj_set_style_bg_color(
        scr,
        lv_color_black(),
        LV_PART_MAIN);

    //--------------------------------------------------------
    // Title
    //--------------------------------------------------------

    lblTitle = lv_label_create(scr);

    lv_label_set_text(
        lblTitle,
        "WORLD CLOCK");

    lv_obj_set_style_text_font(
        lblTitle,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTitle,
        130,
        5);

    //--------------------------------------------------------
    // Clock 1
    //--------------------------------------------------------

    lblCity1 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity1,
        &lv_font_montserrat_18,
        0);

    lv_obj_set_pos(
        lblCity1,
        40,
        73);

    lblTime1 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime1,
        &font_clock,
        0);

    lv_obj_set_pos(
        lblTime1,
        260,
        75);

    //--------------------------------------------------------
    // Clock 2
    //--------------------------------------------------------

    lblCity2 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity2,
        &lv_font_montserrat_18,
        0);

    lv_obj_set_pos(
        lblCity2,
        40,
        108);

    lblTime2 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime2,
        &font_clock,
        0);

    lv_obj_set_pos(
        lblTime2,
        260,
        110);

    //--------------------------------------------------------
    // Clock 3
    //--------------------------------------------------------

    lblCity3 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity3,
        &lv_font_montserrat_18,
        0);

    lv_obj_set_pos(
        lblCity3,
        40,
        143);

    lblTime3 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime3,
        &font_clock,
        0);

    lv_obj_set_pos(
        lblTime3,
        260,
        145);
           //--------------------------------------------------------
    // Clock 4
    //--------------------------------------------------------

    lblCity4 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity4,
        &lv_font_montserrat_18,
        0);

    lv_obj_set_pos(
        lblCity4,
        40,
        178);

    lblTime4 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime4,
        &font_clock,
        0);

    lv_obj_set_pos(
        lblTime4,
        260,
        180);

    //--------------------------------------------------------
    // Clock 5
    //--------------------------------------------------------

    lblCity5 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity5,
        &lv_font_montserrat_18,
        0);

    lv_obj_set_pos(
        lblCity5,
        40,
        213);

    lblTime5 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime5,
        &font_clock,
        0);

    lv_obj_set_pos(
        lblTime5,
        260,
        215);

    //--------------------------------------------------------
    // Status Markers
    //--------------------------------------------------------

    for (int i = 0; i < 5; i++)
    {
        markerClocks[i] = lv_obj_create(scr);
        lv_obj_set_size(markerClocks[i], 12, 12);
        lv_obj_set_style_radius(markerClocks[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(markerClocks[i], LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(markerClocks[i], 0, 0);
        lv_obj_set_style_shadow_width(markerClocks[i], 0, 0);
        lv_obj_clear_flag(markerClocks[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(markerClocks[i], 15, 75 + i * 35);
        lv_obj_set_style_bg_color(markerClocks[i], lv_color_hex(0xFF0000), 0); // Default to Red
    }

    //--------------------------------------------------------
    // Status Bars (larger duplicate of the markers, left of the time)
    //--------------------------------------------------------

    for (int i = 0; i < 5; i++)
    {
        markerBars[i] = lv_obj_create(scr);
        lv_obj_set_size(markerBars[i], 18, 18);
        lv_obj_set_style_radius(markerBars[i], 2, 0);
        lv_obj_set_style_bg_opa(markerBars[i], LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(markerBars[i], 0, 0);
        lv_obj_set_style_shadow_width(markerBars[i], 0, 0);
        lv_obj_clear_flag(markerBars[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(markerBars[i], 236, 75 + i * 35);
        lv_obj_set_style_bg_color(markerBars[i], lv_color_hex(0xFF0000), 0); // Default to Red
    }

    //--------------------------------------------------------
    // Date
    //--------------------------------------------------------

    lblDate = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblDate,
        &lv_font_montserrat_24,
        0);

    lv_obj_align(
        lblDate,
        LV_ALIGN_TOP_MID,
        0,
        34);

    //--------------------------------------------------------
    // WiFi
    //--------------------------------------------------------

    lblWiFi = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblWiFi,
        &lv_font_montserrat_12,
        0);

    lv_obj_set_pos(
        lblWiFi,
        5,
        250);

    //--------------------------------------------------------
    // NTP
    //--------------------------------------------------------

    lblNTP = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblNTP,
        &lv_font_montserrat_12,
        0);

    lv_obj_set_pos(
        lblNTP,
        125,
        250);

    //--------------------------------------------------------
    // IP
    //--------------------------------------------------------

    lblIP = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblIP,
        &lv_font_montserrat_12,
        0);

    lv_obj_set_pos(
        lblIP,
        225,
        250);

    //--------------------------------------------------------
    // FPS / CPU (rendering performance, last second)
    //--------------------------------------------------------

    lblFPS = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblFPS,
        &lv_font_montserrat_12,
        0);

    lv_obj_set_pos(
        lblFPS,
        345,
        250);

    lblCPU = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCPU,
        &lv_font_montserrat_12,
        0);

    lv_obj_set_pos(
        lblCPU,
        410,
        250);

    //--------------------------------------------------------
    // Battery / power status (IP5306)
    //--------------------------------------------------------

    lblBattery = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblBattery,
        &lv_font_montserrat_14,
        0);

    lv_obj_set_pos(
        lblBattery,
        390,
        8);

    lv_label_set_text(lblBattery, "");
    lv_obj_add_flag(lblBattery, LV_OBJ_FLAG_HIDDEN);

    //--------------------------------------------------------
    // Initial values
    //--------------------------------------------------------

    lv_label_set_text(lblCity1, "Kyiv");
    lv_label_set_text(lblTime1, "00:00:00");

    lv_label_set_text(lblCity2, "Delhi");
    lv_label_set_text(lblTime2, "00:00:00");

    lv_label_set_text(lblCity3, "Washington");
    lv_label_set_text(lblTime3, "00:00:00");

    lv_label_set_text(lblCity4, "Valencia");
    lv_label_set_text(lblTime4, "00:00:00");

    lv_label_set_text(lblCity5, "Tokyo");
    lv_label_set_text(lblTime5, "00:00:00");

    lv_label_set_text(lblDate, "01 Jan 2026");

    lv_label_set_text(lblWiFi, "WiFi: Offline");

    lv_label_set_text(lblNTP, "NTP: Waiting");

    lv_label_set_text(lblIP, "IP: ---");

    lv_label_set_text(lblFPS, "FPS:--");
    lv_label_set_text(lblCPU, "CPU:--%");

    //--------------------------------------------------------
    // Environment view (hidden by default)
    //--------------------------------------------------------

    lblEnvTitle = lv_label_create(scr);
    lv_obj_set_style_text_font(lblEnvTitle, &lv_font_montserrat_24, 0);
    lv_obj_align(
        lblEnvTitle,
        LV_ALIGN_TOP_MID,
        0,
        5);
    lv_label_set_text(lblEnvTitle, "ENVIRONMENT");
    lv_obj_add_flag(lblEnvTitle, LV_OBJ_FLAG_HIDDEN);

    lblTempLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(lblTempLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lblTempLabel, lv_color_hex(0xFF0000), 0);
    lv_obj_set_pos(lblTempLabel, 40, 74);
    lv_label_set_text(lblTempLabel, "Temperature");
    lv_obj_add_flag(lblTempLabel, LV_OBJ_FLAG_HIDDEN);

    lblTempValue = lv_label_create(scr);
    lv_obj_set_style_text_font(lblTempValue, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lblTempValue, lv_color_hex(0xFF0000), 0);
    lv_obj_set_pos(lblTempValue, 260, 66);
    lv_label_set_text(lblTempValue, "--.- C");
    lv_obj_add_flag(lblTempValue, LV_OBJ_FLAG_HIDDEN);

    lblHumLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(lblHumLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lblHumLabel, lv_color_hex(0x00E676), 0);
    lv_obj_set_pos(lblHumLabel, 40, 194);
    lv_label_set_text(lblHumLabel, "Humidity");
    lv_obj_add_flag(lblHumLabel, LV_OBJ_FLAG_HIDDEN);

    lblHumValue = lv_label_create(scr);
    lv_obj_set_style_text_font(lblHumValue, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lblHumValue, lv_color_hex(0x00E676), 0);
    lv_obj_set_pos(lblHumValue, 260, 186);
    lv_label_set_text(lblHumValue, "--.- %");
    lv_obj_add_flag(lblHumValue, LV_OBJ_FLAG_HIDDEN);

    lblPresLabel = lv_label_create(scr);
    lv_obj_set_style_text_font(lblPresLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lblPresLabel, lv_color_hex(0x448AFF), 0);
    lv_obj_set_pos(lblPresLabel, 40, 134);
    lv_label_set_text(lblPresLabel, "Pressure");
    lv_obj_add_flag(lblPresLabel, LV_OBJ_FLAG_HIDDEN);

    lblPresValue = lv_label_create(scr);
    lv_obj_set_style_text_font(lblPresValue, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lblPresValue, lv_color_hex(0x448AFF), 0);
    lv_obj_set_pos(lblPresValue, 260, 126);
    lv_label_set_text(lblPresValue, "----.- hPa");
    lv_obj_add_flag(lblPresValue, LV_OBJ_FLAG_HIDDEN);
}
static void updateMarkerColor(lv_obj_t *marker, uint8_t state)
{
    if (!marker) return;
    lv_color_t color;
    switch (state)
    {
        case 1: // MARKER_GREEN (working time)
            color = lv_color_hex(0x388E3C);
            break;
        case 2: // MARKER_YELLOW (less than 30 mins to end)
            color = lv_color_hex(0xFBC02D);
            break;
        case 3: // MARKER_BLUE (less than 30 mins to start)
            color = lv_color_hex(0x0288D1);
            break;
        case 0: // MARKER_RED (non-working time)
        default:
            color = lv_color_hex(0xFF0000);
            break;
    }
    lv_obj_set_style_bg_color(marker, color, 0);
}

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

    const char *ip)
{
    if (lblCity1) lv_label_set_text(lblCity1, city1);
    if (lblTime1) lv_label_set_text(lblTime1, time1);
    updateMarkerColor(markerClocks[0], markerState1);
    updateMarkerColor(markerBars[0],   markerState1);

    if (lblCity2) lv_label_set_text(lblCity2, city2);
    if (lblTime2) lv_label_set_text(lblTime2, time2);
    updateMarkerColor(markerClocks[1], markerState2);
    updateMarkerColor(markerBars[1],   markerState2);

    if (lblCity3) lv_label_set_text(lblCity3, city3);
    if (lblTime3) lv_label_set_text(lblTime3, time3);
    updateMarkerColor(markerClocks[2], markerState3);
    updateMarkerColor(markerBars[2],   markerState3);

    if (lblCity4) lv_label_set_text(lblCity4, city4);
    if (lblTime4) lv_label_set_text(lblTime4, time4);
    updateMarkerColor(markerClocks[3], markerState4);
    updateMarkerColor(markerBars[3],   markerState4);

    if (lblCity5) lv_label_set_text(lblCity5, city5);
    if (lblTime5) lv_label_set_text(lblTime5, time5);
    updateMarkerColor(markerClocks[4], markerState5);
    updateMarkerColor(markerBars[4],   markerState5);

    if (lblDate) lv_label_set_text(lblDate, date);

    screenStatusUpdate(wifi, ntp, ip);
}

void screenStatusUpdate(const char *wifi, const char *ntp, const char *ip)
{
    if (lblWiFi) lv_label_set_text(lblWiFi, wifi);
    if (lblNTP)  lv_label_set_text(lblNTP,  ntp);
    if (lblIP)   lv_label_set_text(lblIP,   ip);
}

void screenPerfUpdate(uint32_t fps, uint8_t cpu)
{
    char buf[16];

    if (lblFPS)
    {
        snprintf(buf, sizeof(buf), "FPS:%u", (unsigned)fps);
        lv_label_set_text(lblFPS, buf);
    }

    if (lblCPU)
    {
        snprintf(buf, sizeof(buf), "CPU:%u%%", (unsigned)cpu);
        lv_label_set_text(lblCPU, buf);
    }
}

//------------------------------------------------------------
// View switching helpers
//------------------------------------------------------------

static void setWidgetVisible(lv_obj_t *obj, bool visible)
{
    if (!obj)
        return;

    if (visible)
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

void screenViewSet(uint8_t view)
{
    bool clockVisible = (view == SCREEN_VIEW_CLOCK);

    // Clock view widgets
    setWidgetVisible(lblTitle, clockVisible);
    setWidgetVisible(lblBattery, clockVisible);

    setWidgetVisible(lblCity1, clockVisible);
    setWidgetVisible(lblTime1, clockVisible);
    setWidgetVisible(lblCity2, clockVisible);
    setWidgetVisible(lblTime2, clockVisible);
    setWidgetVisible(lblCity3, clockVisible);
    setWidgetVisible(lblTime3, clockVisible);
    setWidgetVisible(lblCity4, clockVisible);
    setWidgetVisible(lblTime4, clockVisible);
    setWidgetVisible(lblCity5, clockVisible);
    setWidgetVisible(lblTime5, clockVisible);

    for (int i = 0; i < 5; i++)
    {
        setWidgetVisible(markerClocks[i], clockVisible);
        setWidgetVisible(markerBars[i],   clockVisible);
    }

    // Environment view widgets
    setWidgetVisible(lblEnvTitle,  !clockVisible);
    setWidgetVisible(lblTempLabel, !clockVisible);
    setWidgetVisible(lblTempValue, !clockVisible);

    const bool humVisible = !clockVisible && envHasHumidity;
    setWidgetVisible(lblHumLabel, humVisible);
    setWidgetVisible(lblHumValue, humVisible);

    setWidgetVisible(lblPresLabel, !clockVisible);
    setWidgetVisible(lblPresValue, !clockVisible);
}

//------------------------------------------------------------
// Environment layout configuration
//------------------------------------------------------------

void screenEnvSetHumidity(bool hasHumidity)
{
    // The layout is fixed (Temperature / Pressure / Humidity) and is
    // set once in screenMainCreate. Here we only record whether the
    // humidity channel exists. For BMP280 the humidity row is hidden;
    // Temperature and Pressure keep their normal positions.
    envHasHumidity = hasHumidity;

    if (!hasHumidity)
    {
        setWidgetVisible(lblHumLabel, false);
        setWidgetVisible(lblHumValue, false);
    }
}

void screenEnvUpdate(float temperatureC,
                     float humidityPct,
                     float pressureHpa)
{
    char buf[20];

    if (lblTempValue)
    {
        if (isnan(temperatureC))
            snprintf(buf, sizeof(buf), "--.- C");
        else
            snprintf(buf, sizeof(buf), "%.1f C", temperatureC);
        lv_label_set_text(lblTempValue, buf);
    }

    if (lblHumValue)
    {
        if (isnan(humidityPct))
            snprintf(buf, sizeof(buf), "--.- %%");
        else
            snprintf(buf, sizeof(buf), "%.1f %%", humidityPct);
        lv_label_set_text(lblHumValue, buf);
    }

    if (lblPresValue)
    {
        if (isnan(pressureHpa))
            snprintf(buf, sizeof(buf), "----.- hPa");
        else
            snprintf(buf, sizeof(buf), "%.1f hPa", pressureHpa);
        lv_label_set_text(lblPresValue, buf);
    }
}

//------------------------------------------------------------
// Battery / power status (IP5306)
//------------------------------------------------------------

void screenBatteryUpdate(bool present, int8_t level, bool charging)
{
    if (!lblBattery)
        return;

    if (!present)
    {
        lv_obj_add_flag(lblBattery, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_clear_flag(lblBattery, LV_OBJ_FLAG_HIDDEN);

    char buf[20];

    if (level < 0)
        snprintf(buf, sizeof(buf), "BAT --%s", charging ? " +" : "");
    else
        snprintf(buf, sizeof(buf), "BAT %d%%%s", level, charging ? " +" : "");

    lv_label_set_text(lblBattery, buf);
}

//------------------------------------------------------------
// Fireworks (Salute) Animation
//------------------------------------------------------------

static lv_timer_t *saluteSpawnTimer = nullptr;
static uint32_t saluteStartTime = 0;

static void animReadyCb(lv_anim_t *a)
{
    lv_obj_t *obj = (lv_obj_t *)a->var;
    if (obj)
    {
        lv_obj_del_async(obj);
    }
}

static void spawnExplosion(int cx, int cy)
{
    static const uint32_t colors[] = {
        0xFF1744, // Red
        0xD500F9, // Magenta
        0x2979FF, // Blue
        0x00E5FF, // Cyan
        0x00E676, // Green
        0xFFEA00, // Yellow
        0xFF9100, // Orange
        0xFF3D00  // Deep Orange
    };

    int numParticles = 12 + rand() % 6; // 12 to 17 particles
    lv_obj_t *scr = lv_scr_act();

    for (int i = 0; i < numParticles; i++)
    {
        lv_obj_t *part = lv_obj_create(scr);
        if (!part) continue;

        lv_obj_set_size(part, 8, 8);
        lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(part, 0, 0);
        lv_obj_set_style_shadow_width(part, 0, 0);
        lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);

        lv_color_t color = lv_color_hex(colors[rand() % 8]);
        lv_obj_set_style_bg_color(part, color, 0);

        lv_obj_set_pos(part, cx - 4, cy - 4);

        // Compute destination
        double angle = (rand() % 360) * 3.14159265 / 180.0;
        int dist = 30 + rand() % 80;
        int targetX = cx + (int)(dist * cos(angle));
        int targetY = cy + (int)(dist * sin(angle));
        int duration = 800 + rand() % 400; // 800 to 1200 ms

        // X animation
        lv_anim_t a_x;
        lv_anim_init(&a_x);
        lv_anim_set_var(&a_x, part);
        lv_anim_set_values(&a_x, cx - 4, targetX - 4);
        lv_anim_set_time(&a_x, duration);
        lv_anim_set_exec_cb(&a_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_path_cb(&a_x, lv_anim_path_ease_out);

        // Y animation
        lv_anim_t a_y;
        lv_anim_init(&a_y);
        lv_anim_set_var(&a_y, part);
        lv_anim_set_values(&a_y, cy - 4, targetY - 4);
        lv_anim_set_time(&a_y, duration);
        lv_anim_set_exec_cb(&a_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a_y, lv_anim_path_ease_out);

        // Opacity animation (fade out). Ready-cb lives on the LAST
        // animation so x/y have already applied their final value;
        // async delete avoids use-after-free of the still-registered
        // sibling animations.
        lv_anim_t a_opa;
        lv_anim_init(&a_opa);
        lv_anim_set_var(&a_opa, part);
        lv_anim_set_values(&a_opa, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_time(&a_opa, duration);
        lv_anim_set_exec_cb(&a_opa, [](void *var, int32_t val) {
            lv_obj_set_style_opa((lv_obj_t *)var, val, 0);
        });
        lv_anim_set_path_cb(&a_opa, lv_anim_path_ease_out);
        lv_anim_set_ready_cb(&a_opa, animReadyCb);

        lv_anim_start(&a_x);
        lv_anim_start(&a_y);
        lv_anim_start(&a_opa);
    }
}

static void saluteSpawnCb(lv_timer_t *t)
{
    if (millis() - saluteStartTime >= 60000)
    {
        // 60 seconds finished, stop spawning
        lv_timer_del(t);
        saluteSpawnTimer = nullptr;
        return;
    }

    // Spawn a random explosion
    // Display coordinates range: X in [80, 400], Y in [60, 180]
    int cx = 80 + rand() % 320;
    int cy = 60 + rand() % 120;
    spawnExplosion(cx, cy);
}

void startSaluteAnimation()
{
    saluteStartTime = millis();

    // If there is already a running timer, delete it first to restart
    if (saluteSpawnTimer != nullptr)
    {
        lv_timer_del(saluteSpawnTimer);
    }

    // Spawn first explosion immediately
    spawnExplosion(240, 100);

    // Create a timer to spawn subsequent explosions every 1200 ms
    saluteSpawnTimer = lv_timer_create(saluteSpawnCb, 1200, nullptr);
}