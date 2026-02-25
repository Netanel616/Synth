//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"

#define SINE_TABLE_SIZE 2048
float sine_lookup_table[SINE_TABLE_SIZE];

void init_sine_table() {
    for (int i = 0; i < SINE_TABLE_SIZE; i++) {
        // Calculate one full cycle (0 to 2*PI)
        float angle = ((float)i / (float)SINE_TABLE_SIZE) * 2.0f * (float)M_PI;
        sine_lookup_table[i] = sinf(angle);
    }
}

// Envelope rates
#define ATTACK_RATE 0.001f
#define RELEASE_RATE 0.0005f

// Helper function to generate sample based on waveform type
float get_sample(WaveformType type, double phase) {
    // Normalize phase to 0.0 - 1.0 range for non-sine waves
    double normalized_phase = phase / (double)SINE_TABLE_SIZE;

    switch (type) {
        case WAVE_SINE:
            return sine_lookup_table[(int)phase % SINE_TABLE_SIZE];

        case WAVE_SAW:
            // Sawtooth: linearly decreases from 1.0 to -1.0
            return (float)(1.0 - 2.0 * normalized_phase);

        case WAVE_SQUARE:
            // Square: 1.0 for first half, -1.0 for second half
            return (normalized_phase < 0.5) ? 1.0f : -1.0f;

        case WAVE_TRIANGLE:
            // Triangle: linear ramp up and down
            if (normalized_phase < 0.5) {
                return (float)(-1.0 + 4.0 * normalized_phase);
            } else {
                return (float)(3.0 - 4.0 * normalized_phase);
            }

        default:
            return 0.0f;
    }
}

void audio_callback(void* userdata, Uint8* stream, int len) {
    AppContext* ctx = (AppContext*)userdata;
    float* f_stream = (float*)stream;
    int sample_count = len / sizeof(float);

    SDL_memset(stream, 0, len);

    for (int v = 0; v < NUM_VOICES; v++) {
        if (ctx->voices[v].state == OFF) continue;

        float freq = ctx->voices[v].frequency;
        double step = (double)freq * SINE_TABLE_SIZE / (double)SAMPLE_RATE;

        for (int i = 0; i < sample_count; i++) {
            // Envelope management
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
                    ctx->voices[v].isActive = false;
                    ctx->voices[v].ownerKey = SDL_SCANCODE_UNKNOWN;
                    break;
                }
            }

            // Get sample from the selected waveform
            float sample = get_sample(ctx->waveform, ctx->voices[v].phase);

            // 0.15f is the Master Gain to prevent clipping
            f_stream[i] += sample * ctx->voices[v].amplitude * 0.15f;

            ctx->voices[v].phase += step;
            if (ctx->voices[v].phase >= SINE_TABLE_SIZE) ctx->voices[v].phase -= SINE_TABLE_SIZE;
        }
    }
}

void audio_note_on(AppContext* ctx, int midiNoteNumber, SDL_Scancode sc) {
    int stealCandidate = -1;
    float minAmplitude = 1.1f;

    // 1. Find a free voice or a candidate to steal
    for (int i = 0; i < NUM_VOICES; i++) {
        if (ctx->voices[i].state == OFF) {
            stealCandidate = i;
            break; // Found an empty slot
        }

        // Stealing mechanism: Find the voice with the lowest amplitude (usually in Release phase)
        if (ctx->voices[i].amplitude < minAmplitude) {
            minAmplitude = ctx->voices[i].amplitude;
            stealCandidate = i;
        }
    }

    // 2. Initialize the selected voice (new or stolen)
    if (stealCandidate != -1) {
        Voice* v = &ctx->voices[stealCandidate];

        // Lock audio to prevent sound glitches during modification
        SDL_LockAudioDevice(ctx->audioDevice);

        // Standard MIDI to Frequency formula: f = 440 * 2^((d-69)/12)
        v->frequency = A4_FREQUENCY * pow(2.0, (double)(midiNoteNumber - A4_MIDI_NUMBER) / 12.0);
        v->ownerKey = sc;
        v->midiNote = midiNoteNumber;
        v->phase = 0.0;
        v->state = ATTACK; // Reset to Attack (if stolen from Release)
        v->isActive = true;

        // If we stole a voice, keep its current amplitude to prevent a sharp "click",
        // or reset if it was OFF.
        if (v->state == OFF) v->amplitude = 0.0f;

        SDL_UnlockAudioDevice(ctx->audioDevice);
    }
}

void audio_note_off(AppContext* ctx, SDL_Scancode sc) {
    // Lock to ensure atomicity against the Audio Thread
    SDL_LockAudioDevice(ctx->audioDevice);

    for (int i = 0; i < NUM_VOICES; i++) {
        // Find a voice belonging to this key that is not already releasing
        if (ctx->voices[i].ownerKey == sc && ctx->voices[i].state != RELEASE) {
            ctx->voices[i].state = RELEASE;
        }
    }

    SDL_UnlockAudioDevice(ctx->audioDevice);
}

bool init_audio(AppContext* ctx)
{
    init_sine_table();
    SDL_AudioSpec desired;

    // Zero out the structure (mandatory in C)
    SDL_zero(desired);

    // Audio parameters
    desired.freq = SAMPLE_RATE;
    desired.format = AUDIO_F32;
    desired.channels = 2;
    desired.samples = BUFFER_SIZE;
    desired.callback = audio_callback;
    desired.userdata = ctx;

    // Open device
    ctx->audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);

    if (ctx->audioDevice == 0) {
        printf("SDL_OpenAudioDevice Error: %s\n", SDL_GetError());
        return false;
    }

    // Start stream
    SDL_PauseAudioDevice(ctx->audioDevice, 0);
    return true;
}

void destroy_audio(AppContext* ctx)
{
    SDL_CloseAudioDevice(ctx->audioDevice);
}