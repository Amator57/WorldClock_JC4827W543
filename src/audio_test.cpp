//============================================================
// Standalone I2S audio test for JC4827W543 (ESP32-S3).
//
// Plays a continuous 440 Hz sine wave on the on-board NS4168
// amplifier via the I2S DAC. If you hear a steady tone -> pins
// are correct. If not -> change PIN_* below and reflash.
//
// Built via the [env:audio_test] PlatformIO environment so the
// main project firmware is not touched.
//============================================================

#include <Arduino.h>
#include <driver/i2s_std.h>
#include <math.h>
#include <esp_heap_caps.h>
#include <esp_attr.h>

// ====== PIN CONFIGURATION — change here if needed ======
static const int PIN_BCLK  = 42;
static const int PIN_LRCLK = 2;
static const int PIN_DOUT  = 41;
// ======================================================

static const int      SAMPLE_RATE  = 22050;
static const int      TONE_FREQ    = 440;
static const int      BUF_SAMPLES  = 256;
static const uint16_t TONE_AMP     = 5000;   // 0..32000

static i2s_chan_handle_t s_tx       = nullptr;
static int16_t           s_buf[BUF_SAMPLES * 2];  // stereo L,R
static uint32_t          s_phase    = 0;

// Pointers to drained PSRAM blocks (kept allocated to keep PSRAM
// full for the lifetime of the test).
static void *s_psramBlocks[2048];
static int   s_psramCount = 0;

// Fill PSRAM so the I2S driver can't allocate the channel handle
// there. The GDMA driver requires the callback context to live in
// internal RAM, but on IDF 5.3 with PSRAM enabled, heap_caps_calloc
// with MALLOC_CAP_DEFAULT happily returns PSRAM pointers, which
// trips "user context not in internal RAM" in
// gdma_register_rx_event_callbacks. Draining PSRAM completely
// (including the small tail left by big-block drain) forces the
// I2S driver's small allocations into internal RAM.
static void drainPsram()
{
    Serial.println("Draining PSRAM...");
    const size_t sizes[] = {8192, 1024, 64, 8};
    for (size_t s = 0; s < sizeof(sizes) / sizeof(sizes[0]); s++)
    {
        while (s_psramCount < 2048)
        {
            void *p = heap_caps_malloc(sizes[s], MALLOC_CAP_SPIRAM);
            if (!p) break;
            s_psramBlocks[s_psramCount++] = p;
        }
        Serial.printf("  after %u-byte pass: %d blocks, free PSRAM=%u\n",
                      (unsigned)sizes[s],
                      s_psramCount,
                      (unsigned)ESP.getFreePsram());
    }
    Serial.printf("  total drained %d blocks, free PSRAM=%u\n",
                  s_psramCount,
                  (unsigned)ESP.getFreePsram());
}

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("===== I2S AUDIO TEST =====");
    Serial.printf("Pins: BCLK=%d  LRCLK=%d  DOUT=%d\n",
                  PIN_BCLK, PIN_LRCLK, PIN_DOUT);
    Serial.printf("Tone:  %d Hz @ %d Hz sample rate\n",
                  TONE_FREQ, SAMPLE_RATE);
    Serial.printf("Mem:   internal=%u  PSRAM=%u\n",
                  (unsigned)ESP.getFreeHeap(),
                  (unsigned)ESP.getFreePsram());

    // Workaround for IDF 5.3 GDMA bug with PSRAM-enabled boards.
    drainPsram();

    //------------------------------------------------------
    // 1) Allocate a TX channel on any free I2S controller
    //------------------------------------------------------
    i2s_chan_config_t chanCfg =
        I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);

    esp_err_t err = i2s_new_channel(&chanCfg, &s_tx, nullptr);
    if (err != ESP_OK)
    {
        Serial.printf("FAIL i2s_new_channel: 0x%x\n", err);
        Serial.println(">>> No free I2S controller. Halting.");
        while (true) delay(1000);
    }
    Serial.println("[OK] channel allocated");
    Serial.printf("      handle=%p  esp_ptr_internal=%d\n",
                  s_tx, esp_ptr_internal(s_tx));

    i2s_chan_info_t info;
    if (i2s_channel_get_info(s_tx, &info) == ESP_OK)
        Serial.printf("      port=%d role=%d dir=%d\n",
                      (int)info.id, (int)info.role, (int)info.dir);

    //------------------------------------------------------
    // 2) Configure standard Philips I2S mode
    //------------------------------------------------------
    i2s_std_config_t cfg;
    cfg.clk_cfg.sample_rate_hz  = SAMPLE_RATE;
    cfg.clk_cfg.clk_src         = I2S_CLK_SRC_DEFAULT;
    cfg.clk_cfg.ext_clk_freq_hz = 0;
    cfg.clk_cfg.mclk_multiple   = I2S_MCLK_MULTIPLE_256;

    cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);

    cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    cfg.gpio_cfg.bclk = (gpio_num_t)PIN_BCLK;
    cfg.gpio_cfg.ws   = (gpio_num_t)PIN_LRCLK;
    cfg.gpio_cfg.dout = (gpio_num_t)PIN_DOUT;
    cfg.gpio_cfg.din  = I2S_GPIO_UNUSED;
    cfg.gpio_cfg.invert_flags.mclk_inv = false;
    cfg.gpio_cfg.invert_flags.bclk_inv = false;
    cfg.gpio_cfg.invert_flags.ws_inv   = false;

    err = i2s_channel_init_std_mode(s_tx, &cfg);
    if (err != ESP_OK)
    {
        Serial.printf("FAIL i2s_channel_init_std_mode: 0x%x\n", err);
        Serial.println(">>> Bad config or pin conflict. Halting.");
        while (true) delay(1000);
    }
    Serial.println("[OK] std mode configured");

    //------------------------------------------------------
    // 3) Enable clocks and start playback
    //------------------------------------------------------
    err = i2s_channel_enable(s_tx);
    if (err != ESP_OK)
    {
        Serial.printf("FAIL i2s_channel_enable: 0x%x\n", err);
        while (true) delay(1000);
    }
    Serial.println("[OK] channel enabled");
    Serial.println();
    Serial.println(">>> A 440 Hz tone should now be playing. <<<");
    Serial.println();
}

void loop()
{
    // Fill the buffer with one chunk of the sine wave.
    const float twoPi = 2.0f * PI;
    for (int i = 0; i < BUF_SAMPLES; i++)
    {
        float t = (float)s_phase / SAMPLE_RATE;
        int16_t sample = (int16_t)(sinf(twoPi * TONE_FREQ * t) * TONE_AMP);
        s_buf[i * 2]     = sample;   // L
        s_buf[i * 2 + 1] = sample;   // R
        s_phase++;
    }

    size_t written = 0;
    i2s_channel_write(s_tx, s_buf, sizeof(s_buf),
                      &written, portMAX_DELAY);

    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 1000)
    {
        Serial.printf("playing... bytes=%u  free_int=%u\n",
                      (unsigned)written,
                      (unsigned)ESP.getFreeHeap());
        lastPrint = millis();
    }
}
