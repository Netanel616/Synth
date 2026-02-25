//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"

void setup_init(AppContext* ctx)
{
    ctx->state.octaveOffset = 0;
    ctx->state.masterVolume = 0.5f;
    ctx->state.running = true;
    ctx->state.waveform = WAVE_SINE; // Default waveform

    // Initialize all voices to OFF
    for (int i = 0; i < NUM_VOICES; i++) {
        ctx->audio.voices[i].isActive = false;
        ctx->audio.voices[i].frequency = 0.0f;
        ctx->audio.voices[i].ownerKey = SDL_SCANCODE_UNKNOWN;
    }
}

void setup_destroy(AppContext* ctx)
{
    // Cleanup logic if needed
}