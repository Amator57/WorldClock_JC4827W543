#include "audio_manager.h"
#include "phrase_ura.h"
#include "explosion_data.h"

#include <driver/i2s_std.h>
#include <esp_heap_caps.h>

//------------------------------------------------------------
// Hardware / I2S configuration
//------------------------------------------------------------

#define I2S_PORT         I2S_NUM_1
#define I2S_BCK_PIN      42
#define I2S_WS_PIN       2
#define I2S_DATA_OUT_PIN 41

#define SAMPLE_RATE      22050

//------------------------------------------------------------
// State
//------------------------------------------------------------

static i2s_chan_handle_t g_txHandle   = nullptr;
static bool            g_initialised = false;
static bool            g_muted       = false;
static uint8_t         g_volume      = 80;   // %
static TaskHandle_t    g_task        = nullptr;

// Last status from audioInit(). Reported in audioPlayExplosion() so
// the cause is visible even if the boot log was missed.
static char g_initStatus[64] = "audioInit never called";

//----- Command codes for the audio task -----
#define AUDIO_CMD_NONE      0
#define AUDIO_CMD_EXPLOSION 1
#define AUDIO_CMD_PHRASE    2

// Phrase data set by audioPlayPhrase() before notifying the task.
static const int16_t *g_phraseData    = nullptr;
static size_t         g_phraseSamples = 0;

//------------------------------------------------------------
// Audio task: waits for a trigger, then streams audio through I2S.

static void audioTask(void *arg)
{
    (void)arg;
    static uint32_t s_played = 0;
    static int16_t streamBuf[512 * 2]; // phrase streaming buffer

    while (true)
    {
        uint32_t cmd = 0;
        xTaskNotifyWait(0, 0xFFFFFFFF, &cmd, portMAX_DELAY);
        if (cmd == AUDIO_CMD_NONE) continue;

        if (!g_initialised || g_muted || !g_txHandle) continue;

        esp_err_t err = i2s_channel_enable(g_txHandle);
        if (err != ESP_OK) continue;

        if (cmd == AUDIO_CMD_EXPLOSION)
        {
            const int32_t vol = g_volume;
            for (size_t pos = 0; pos < EXPLOSION_DATA_SAMPLES; pos += 512)
            {
                size_t chunk = EXPLOSION_DATA_SAMPLES - pos;
                if (chunk > 512) chunk = 512;
                for (size_t j = 0; j < chunk; j++)
                {
                    int16_t s = (int16_t)((EXPLOSION_DATA[pos + j] * vol) / 100);
                    streamBuf[j * 2]     = s;
                    streamBuf[j * 2 + 1] = s;
                }
                size_t written = 0;
                i2s_channel_write(g_txHandle, streamBuf, chunk * 4,
                                  &written, portMAX_DELAY);
            }

            s_played++;
            if (s_played <= 3 || (s_played % 20) == 0)
                Serial.printf("Audio: explosion #%u vol=%u\n",
                              (unsigned)s_played, g_volume);

            vTaskDelay(pdMS_TO_TICKS(200));  // I2S DMA flush gap
        }
        else if (cmd == AUDIO_CMD_PHRASE && g_phraseData && g_phraseSamples > 0)
        {
            const int32_t vol = g_volume;
            Serial.printf("Audio: phrase %u samples\n",
                          (unsigned)g_phraseSamples);

            for (size_t pos = 0; pos < g_phraseSamples; pos += 512)
            {
                size_t chunk = g_phraseSamples - pos;
                if (chunk > 512) chunk = 512;
                for (size_t j = 0; j < chunk; j++)
                {
                    int16_t s = (int16_t)((g_phraseData[pos + j] * vol) / 100);
                    streamBuf[j * 2]     = s;
                    streamBuf[j * 2 + 1] = s;
                }
                size_t written = 0;
                i2s_channel_write(g_txHandle, streamBuf, chunk * 4,
                                  &written, portMAX_DELAY);
            }
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        i2s_channel_disable(g_txHandle);
    }
}

//------------------------------------------------------------
// Public API
//------------------------------------------------------------

bool audioInit()
{
    if (g_initialised)
        return true;

    Serial.println("Audio: initialising...");
    snprintf(g_initStatus, sizeof(g_initStatus), "starting");

    //----- Temporarily drain PSRAM so the I2S channel handle gets
    // allocated in internal RAM. On IDF 5.3 with PSRAM enabled,
    // heap_caps_calloc(MALLOC_CAP_DEFAULT) happily returns PSRAM
    // pointers, but GDMA requires the callback context (the channel
    // handle) to live in internal RAM. Without this drain, init
    // fails with "user context not in internal RAM".
    Serial.println("Audio: draining PSRAM...");
    static void *drains[2048];
    int ndrains = 0;
    const size_t drainSizes[] = {65536, 8192, 1024, 64, 8};
    for (int phase = 0; phase < 5 && ndrains < 2048; phase++)
    {
        while (ndrains < 2048)
        {
            void *p = heap_caps_malloc(drainSizes[phase], MALLOC_CAP_SPIRAM);
            if (!p) break;
            drains[ndrains++] = p;
        }
    }
    Serial.printf("Audio: drained %d blocks, free PSRAM=%u\n",
                  ndrains, (unsigned)ESP.getFreePsram());

    //----- Allocate the TX channel -----
    i2s_chan_config_t chanCfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT,
                                                           I2S_ROLE_MASTER);
    chanCfg.auto_clear_after_cb = true;   // silence between explosions

    esp_err_t err = i2s_new_channel(&chanCfg, &g_txHandle, nullptr);
    if (err != ESP_OK)
    {
        Serial.printf("Audio: i2s_new_channel FAILED err=0x%x\n", err);
        snprintf(g_initStatus, sizeof(g_initStatus),
                 "i2s_new_channel FAILED err=0x%x", err);
        g_txHandle = nullptr;
        // Free PSRAM drain even on failure
        for (int i = 0; i < ndrains; i++) free(drains[i]);
        return false;
    }
    Serial.println("Audio: channel allocated");
    Serial.printf("Audio: handle=%p internal=%d\n",
                  g_txHandle, esp_ptr_internal(g_txHandle));
    snprintf(g_initStatus, sizeof(g_initStatus), "channel allocated");

    i2s_chan_info_t info;
    if (i2s_channel_get_info(g_txHandle, &info) == ESP_OK)
        Serial.printf("Audio: port=%d role=%d dir=%d\n",
                      (int)info.id, (int)info.role, (int)info.dir);

    //----- Configure standard (Philips) mode -----
    // NB: I2S_STD_CLK_DEFAULT_CONFIG() macro on IDF 5.3 lists
    // mclk_multiple before ext_clk_freq_hz, but on ESP32-S3
    // (SOC_I2S_HW_VERSION_2) the struct declaration order is the
    // opposite. C++20 designated initialisers require matching
    // order, so we build clk_cfg by hand.
    i2s_std_config_t stdCfg;
    stdCfg.clk_cfg.sample_rate_hz  = SAMPLE_RATE;
    stdCfg.clk_cfg.clk_src         = I2S_CLK_SRC_DEFAULT;
    stdCfg.clk_cfg.ext_clk_freq_hz = 0;
    stdCfg.clk_cfg.mclk_multiple   = I2S_MCLK_MULTIPLE_256;

    stdCfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);

    stdCfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    stdCfg.gpio_cfg.bclk = (gpio_num_t)I2S_BCK_PIN;
    stdCfg.gpio_cfg.ws   = (gpio_num_t)I2S_WS_PIN;
    stdCfg.gpio_cfg.dout = (gpio_num_t)I2S_DATA_OUT_PIN;
    stdCfg.gpio_cfg.din  = I2S_GPIO_UNUSED;
    stdCfg.gpio_cfg.invert_flags.mclk_inv = false;
    stdCfg.gpio_cfg.invert_flags.bclk_inv = false;
    stdCfg.gpio_cfg.invert_flags.ws_inv   = false;

    err = i2s_channel_init_std_mode(g_txHandle, &stdCfg);
    if (err != ESP_OK)
    {
        Serial.printf("Audio: i2s_channel_init_std_mode FAILED err=0x%x\n", err);
        snprintf(g_initStatus, sizeof(g_initStatus),
                 "init_std_mode FAILED err=0x%x", err);
        return false;
    }
    Serial.println("Audio: std mode configured");
    snprintf(g_initStatus, sizeof(g_initStatus), "std mode configured");

    // NOTE: channel is NOT enabled here. I2S clocks (BCLK/LRCK)
    // run continuously while the channel is enabled and would be
    // picked up by the NS4168 amplifier as an audible constant
    // whistle. Instead, we enable lazily for each playback and
    // disable afterwards so the speaker is silent between
    // explosions.

    // Free PSRAM drain now that the I2S channel handle is safely in
    // internal RAM.
    for (int i = 0; i < ndrains; i++)
        free(drains[i]);
    Serial.printf("Audio: PSRAM drain freed (%d blocks)\n", ndrains);

    BaseType_t ok = xTaskCreatePinnedToCore(audioTask,
                                            "audio",
                                            4096,
                                            nullptr,
                                            5,
                                            &g_task,
                                            0);
    if (ok != pdPASS || !g_task)
    {
        Serial.printf("Audio: xTaskCreatePinnedToCore FAILED ok=%d\n", ok);
        snprintf(g_initStatus, sizeof(g_initStatus), "xTaskCreate FAILED");
        return false;
    }

    g_initialised = true;
    snprintf(g_initStatus, sizeof(g_initStatus), "OK");
    Serial.println("Audio: I2S DAC initialised (BCLK=42 LRCK=2 DOUT=41)");

    // Generate the placeholder phrase data.
    phraseInit();

    return true;
}

void audioPlayExplosion()
{
    if (!g_initialised || !g_task)
        return;
    xTaskNotify(g_task, AUDIO_CMD_EXPLOSION, eSetValueWithOverwrite);
}

void audioPlayPhrase(const int16_t *data, size_t sampleCount)
{
    if (!g_initialised || !g_task || !data || sampleCount == 0)
        return;
    g_phraseData    = data;
    g_phraseSamples = sampleCount;
    xTaskNotify(g_task, AUDIO_CMD_PHRASE, eSetValueWithOverwrite);
}

void audioSetMuted(bool muted)
{
    g_muted = muted;
}

bool audioIsMuted()
{
    return g_muted;
}

void audioSetVolume(uint8_t percent)
{
    if (percent > 100)
        percent = 100;
    g_volume = percent;
}

uint8_t audioGetVolume()
{
    return g_volume;
}
