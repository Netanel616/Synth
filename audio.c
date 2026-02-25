//
// Created by נתנאל סרי on 24/02/2026.
//
#include "common.h"

#define SINE_TABLE_SIZE 2048
float sine_lookup_table[SINE_TABLE_SIZE];

void init_sine_table() {
    for (int i = 0; i < SINE_TABLE_SIZE; i++) {
        // אנחנו מחשבים מחזור אחד שלם (0 עד 2 פאי)
        float angle = ((float)i / (float)SINE_TABLE_SIZE) * 2.0f * (float)M_PI;
        sine_lookup_table[i] = sinf(angle);
    }
}

// קבועים להגדרת המהירות (אפשר לשנות לפי הטעם)
#define ATTACK_RATE 0.001f  // כמה מהר הצליל עולה
#define RELEASE_RATE 0.0005f // כמה מהר הצליל דועך

void audio_callback(void* userdata, Uint8* stream, int len) {
    AppContext* ctx = (AppContext*)userdata;
    float* f_stream = (float*)stream;
    int sample_count = len / sizeof(float);

    SDL_memset(stream, 0, len);

    for (int v = 0; v < NUM_VOICES; v++) {
        if (ctx->voices[v].state == OFF) continue;

        float freq = ctx->voices[v].frequency;
        double step = (double)freq * SINE_TABLE_SIZE / 44100.0;

        for (int i = 0; i < sample_count; i++) {
            // ניהול ה-Envelope
            if (ctx->voices[v].state == ATTACK) {
                ctx->voices[v].amplitude += ATTACK_RATE;
                if (ctx->voices[v].amplitude >= 1.0f) {
                    ctx->voices[v].amplitude = 1.0f;
                    ctx->voices[v].state = SUSTAIN;
                }
            }
            else if (ctx->voices[v].state == RELEASE) {
                ctx->voices[v].amplitude -= RELEASE_RATE;
                if (ctx->voices[v].amplitude <= 0.0f) {
                    ctx->voices[v].amplitude = 0.0f;
                    ctx->voices[v].state = OFF;
                    ctx->voices[v].isActive = false; // הכיבוי הסופי קורה כאן
                    ctx->voices[v].ownerKey = SDL_SCANCODE_UNKNOWN;
                    break;
                }
            }

            // Mixing (חיבור גלים)
            int index = (int)ctx->voices[v].phase;
            float sample = sine_lookup_table[index % SINE_TABLE_SIZE];

            // 0.15f הוא ה-Master Gain כדי למנוע Clipping
            f_stream[i] += sample * ctx->voices[v].amplitude * 0.15f;

            ctx->voices[v].phase += step;
            if (ctx->voices[v].phase >= SINE_TABLE_SIZE) ctx->voices[v].phase -= SINE_TABLE_SIZE;
        }
    }
}

bool init_audio(AppContext* ctx)
{
    init_sine_table();
    SDL_AudioSpec desired;

    // איפוס המבנה (חובה ב-C!)
    SDL_zero(desired);

    // audio parameters
    desired.freq = BASE_FREQUENCY;
    desired.format = AUDIO_F32;
    desired.channels = 2;
    desired.samples = BUFFER_SIZE;
    desired.callback = audio_callback;
    desired.userdata = ctx;

    // open device
    ctx->audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);

    if (ctx->audioDevice == 0) {
        printf("SDL_OpenAudioDevice Error: %s\n", SDL_GetError());
        return false;
    }

    // start stream
    SDL_PauseAudioDevice(ctx->audioDevice, 0);
    return true;
}
void destroy_audio(AppContext* ctx)
{
    SDL_CloseAudioDevice(ctx->audioDevice);
}