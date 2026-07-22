#include "screen_main.h"
#include <Arduino.h>
#include <math.h>
#include <stdlib.h>

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

static lv_obj_t *markerClocks[5];

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
        &lv_font_montserrat_16,
        0);

    lv_obj_set_pos(
        lblCity1,
        40,
        40);

    lblTime1 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime1,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTime1,
        260,
        34);

    //--------------------------------------------------------
    // Clock 2
    //--------------------------------------------------------

    lblCity2 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity2,
        &lv_font_montserrat_16,
        0);

    lv_obj_set_pos(
        lblCity2,
        40,
        75);

    lblTime2 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime2,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTime2,
        260,
        69);

    //--------------------------------------------------------
    // Clock 3
    //--------------------------------------------------------

    lblCity3 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity3,
        &lv_font_montserrat_16,
        0);

    lv_obj_set_pos(
        lblCity3,
        40,
        110);

    lblTime3 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime3,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTime3,
        260,
        104);
           //--------------------------------------------------------
    // Clock 4
    //--------------------------------------------------------

    lblCity4 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity4,
        &lv_font_montserrat_16,
        0);

    lv_obj_set_pos(
        lblCity4,
        40,
        145);

    lblTime4 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime4,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTime4,
        260,
        139);

    //--------------------------------------------------------
    // Clock 5
    //--------------------------------------------------------

    lblCity5 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblCity5,
        &lv_font_montserrat_16,
        0);

    lv_obj_set_pos(
        lblCity5,
        40,
        180);

    lblTime5 = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblTime5,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(
        lblTime5,
        260,
        174);

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
        lv_obj_set_pos(markerClocks[i], 15, 42 + i * 35);
        lv_obj_set_style_bg_color(markerClocks[i], lv_color_hex(0xD32F2F), 0); // Default to Red
    }

    //--------------------------------------------------------
    // Date
    //--------------------------------------------------------

    lblDate = lv_label_create(scr);

    lv_obj_set_style_text_font(
        lblDate,
        &lv_font_montserrat_14,
        0);

    lv_obj_set_pos(
        lblDate,
        15,
        220);

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
        15,
        245);

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
        170,
        245);

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
        310,
        245);

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
            color = lv_color_hex(0xD32F2F);
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

    if (lblCity2) lv_label_set_text(lblCity2, city2);
    if (lblTime2) lv_label_set_text(lblTime2, time2);
    updateMarkerColor(markerClocks[1], markerState2);

    if (lblCity3) lv_label_set_text(lblCity3, city3);
    if (lblTime3) lv_label_set_text(lblTime3, time3);
    updateMarkerColor(markerClocks[2], markerState3);

    if (lblCity4) lv_label_set_text(lblCity4, city4);
    if (lblTime4) lv_label_set_text(lblTime4, time4);
    updateMarkerColor(markerClocks[3], markerState4);

    if (lblCity5) lv_label_set_text(lblCity5, city5);
    if (lblTime5) lv_label_set_text(lblTime5, time5);
    updateMarkerColor(markerClocks[4], markerState5);

    if (lblDate) lv_label_set_text(lblDate, date);

    if (lblWiFi) lv_label_set_text(lblWiFi, wifi);

    if (lblNTP) lv_label_set_text(lblNTP, ntp);

    if (lblIP) lv_label_set_text(lblIP, ip);
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
        lv_obj_del(obj);
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
        lv_anim_set_ready_cb(&a_x, animReadyCb);

        // Y animation
        lv_anim_t a_y;
        lv_anim_init(&a_y);
        lv_anim_set_var(&a_y, part);
        lv_anim_set_values(&a_y, cy - 4, targetY - 4);
        lv_anim_set_time(&a_y, duration);
        lv_anim_set_exec_cb(&a_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a_y, lv_anim_path_ease_out);

        // Opacity animation (fade out)
        lv_anim_t a_opa;
        lv_anim_init(&a_opa);
        lv_anim_set_var(&a_opa, part);
        lv_anim_set_values(&a_opa, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_time(&a_opa, duration);
        lv_anim_set_exec_cb(&a_opa, [](void *var, int32_t val) {
            lv_obj_set_style_opa((lv_obj_t *)var, val, 0);
        });
        lv_anim_set_path_cb(&a_opa, lv_anim_path_ease_out);

        lv_anim_start(&a_x);
        lv_anim_start(&a_y);
        lv_anim_start(&a_opa);
    }
}

static void saluteSpawnCb(lv_timer_t *t)
{
    if (millis() - saluteStartTime >= 60000)
    {
        // 160 seconds finished, stop spawning
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