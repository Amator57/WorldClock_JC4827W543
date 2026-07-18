#include "screen_main.h"

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
        15,
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
        15,
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
        15,
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
        15,
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
        15,
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

    const char *ip)
{
    if (lblCity1) lv_label_set_text(lblCity1, city1);
    if (lblTime1) lv_label_set_text(lblTime1, time1);

    if (lblCity2) lv_label_set_text(lblCity2, city2);
    if (lblTime2) lv_label_set_text(lblTime2, time2);

    if (lblCity3) lv_label_set_text(lblCity3, city3);
    if (lblTime3) lv_label_set_text(lblTime3, time3);

    if (lblCity4) lv_label_set_text(lblCity4, city4);
    if (lblTime4) lv_label_set_text(lblTime4, time4);

    if (lblCity5) lv_label_set_text(lblCity5, city5);
    if (lblTime5) lv_label_set_text(lblTime5, time5);

    if (lblDate) lv_label_set_text(lblDate, date);

    if (lblWiFi) lv_label_set_text(lblWiFi, wifi);

    if (lblNTP) lv_label_set_text(lblNTP, ntp);

    if (lblIP) lv_label_set_text(lblIP, ip);
}