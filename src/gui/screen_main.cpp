#include "screen_main.h"

static lv_obj_t *lblTitle;

static lv_obj_t *lblCity1;
static lv_obj_t *lblTime1;

static lv_obj_t *lblCity2;
static lv_obj_t *lblTime2;

static lv_obj_t *lblCity3;
static lv_obj_t *lblTime3;

static lv_obj_t *lblDate;
static lv_obj_t *lblWiFi;
static lv_obj_t *lblNTP;
static lv_obj_t *lblIP;

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
        &lv_font_montserrat_28,
        0);

    lv_obj_align(
        lblTitle,
        LV_ALIGN_TOP_MID,
        0,
        8);

    //--------------------------------------------------------
    // Kyiv
    //--------------------------------------------------------

    lblCity1 = lv_label_create(scr);

    lv_label_set_text(
        lblCity1,
        "Kyiv");

    lv_obj_set_style_text_font(
        lblCity1,
        &lv_font_montserrat_18,
        0);

    lv_obj_align(
        lblCity1,
        LV_ALIGN_TOP_LEFT,
        15,
        55);

    lblTime1 = lv_label_create(scr);

    lv_label_set_text(
        lblTime1,
        "00:00:00");

    lv_obj_set_style_text_font(
        lblTime1,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(lblTime1, 300, 48);

    //--------------------------------------------------------
    // Delhi
    //--------------------------------------------------------

    lblCity2 = lv_label_create(scr);

    lv_label_set_text(
        lblCity2,
        "Delhi");

    lv_obj_set_style_text_font(
        lblCity2,
        &lv_font_montserrat_18,
        0);

    lv_obj_align(
        lblCity2,
        LV_ALIGN_TOP_LEFT,
        15,
        105);

    lblTime2 = lv_label_create(scr);

    lv_label_set_text(
        lblTime2,
        "00:00:00");

    lv_obj_set_style_text_font(
        lblTime2,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(lblTime2, 300, 98);

    //--------------------------------------------------------
    // Washington
    //--------------------------------------------------------

    lblCity3 = lv_label_create(scr);

    lv_label_set_text(
        lblCity3,
        "Washington");

    lv_obj_set_style_text_font(
        lblCity3,
        &lv_font_montserrat_18,
        0);

    lv_obj_align(
        lblCity3,
        LV_ALIGN_TOP_LEFT,
        15,
        155);

    lblTime3 = lv_label_create(scr);

    lv_label_set_text(
        lblTime3,
        "00:00:00");

    lv_obj_set_style_text_font(
        lblTime3,
        &lv_font_montserrat_24,
        0);

    lv_obj_set_pos(lblTime3, 300, 148);

    //--------------------------------------------------------
    // Date
    //--------------------------------------------------------

    lblDate = lv_label_create(scr);

    lv_label_set_text(
        lblDate,
        "15 Jul 2026");

    lv_obj_set_style_text_font(
        lblDate,
        &lv_font_montserrat_16,
        0);

    lv_obj_align(
        lblDate,
        LV_ALIGN_BOTTOM_LEFT,
        15,
        -45);

    //--------------------------------------------------------
    // WiFi
    //--------------------------------------------------------

    lblWiFi = lv_label_create(scr);

    lv_label_set_text(
        lblWiFi,
        "WiFi : Offline");

    lv_obj_set_style_text_font(
        lblWiFi,
        &lv_font_montserrat_16,
        0);

    lv_obj_align(
        lblWiFi,
        LV_ALIGN_BOTTOM_LEFT,
        15,
        -25);

    //--------------------------------------------------------
    // NTP
    //--------------------------------------------------------

    lblNTP = lv_label_create(scr);

    lv_label_set_text(
        lblNTP,
        "NTP : Waiting");

    lv_obj_set_style_text_font(
        lblNTP,
        &lv_font_montserrat_16,
        0);

    lv_obj_align(
        lblNTP,
        LV_ALIGN_BOTTOM_MID,
        0,
        -25);

    //--------------------------------------------------------
    // IP
    //--------------------------------------------------------

    lblIP = lv_label_create(scr);

    lv_label_set_text(
        lblIP,
        "IP : ---");

    lv_obj_set_style_text_font(
        lblIP,
        &lv_font_montserrat_16,
        0);

    lv_obj_align(
        lblIP,
        LV_ALIGN_BOTTOM_RIGHT,
        -15,
        -25);
}
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
    const char *date,
    const char *wifi,
    const char *ntp,
    const char *ip)
{
    if (lblCity1)
        lv_label_set_text(lblCity1, city1);

    if (lblTime1)
        lv_label_set_text(lblTime1, time1);

    if (lblCity2)
        lv_label_set_text(lblCity2, city2);

    if (lblTime2)
        lv_label_set_text(lblTime2, time2);

    if (lblCity3)
        lv_label_set_text(lblCity3, city3);

    if (lblTime3)
        lv_label_set_text(lblTime3, time3);

    if (lblDate)
        lv_label_set_text(lblDate, date);

   if (lblWiFi)
    lv_label_set_text(lblWiFi, wifi);

if (lblNTP)
    lv_label_set_text(lblNTP, ntp);

if (lblIP)
    lv_label_set_text(lblIP, ip);
}