//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "effects.h"
#include <stdlib.h>

void setup_init(AppContext* ctx)
{
    // Global Settings
    ctx->state.octaveOffset = 0;
    ctx->state.masterVolume = 0.5f;
    ctx->state.running = true;
    ctx->state.selectedOscIndex = 0;
    ctx->state.selectedEffect = FX_DISTORTION;

    // Initialize Effects
    init_effects(ctx);

    // Initialize Oscillator Array
    ctx->state.numOscillators = 1;
    ctx->state.oscCapacity = 4; // Start with capacity for 4
    ctx->state.oscillators = (Oscillator*)malloc(sizeof(Oscillator) * ctx->state.oscCapacity);

    // Default Oscillator 1
    ctx->state.oscillators[0].type = WAVE_SINE;
    ctx->state.oscillators[0].volume = 0.8f;
    ctx->state.oscillators[0].octaveOffset = 0;
    ctx->state.oscillators[0].detune = 1.0f;
    ctx->state.oscillators[0].enabled = true;

    // Initialize Voices
    for (int i = 0; i < NUM_VOICES; i++) {
        ctx->audio.voices[i].isActive = false;
        ctx->audio.voices[i].frequency = 0.0f;
        ctx->audio.voices[i].ownerKey = SDL_SCANCODE_UNKNOWN;
        ctx->audio.voices[i].state = OFF;

        // Allocate phase array for each voice
        ctx->audio.voices[i].phasesCapacity = ctx->state.oscCapacity;
        ctx->audio.voices[i].phases = (double*)calloc(ctx->state.oscCapacity, sizeof(double));
    }
}

void setup_destroy(AppContext* ctx)
{
    // Free Oscillator Array
    if (ctx->state.oscillators) {
        free(ctx->state.oscillators);
        ctx->state.oscillators = NULL;
    }

    // Free Voice Phase Arrays
    for (int i = 0; i < NUM_VOICES; i++) {
        if (ctx->audio.voices[i].phases) {
            free(ctx->audio.voices[i].phases);
            ctx->audio.voices[i].phases = NULL;
        }
    }

    // Free Delay Buffer
    if (ctx->audio.delayBuffer) {
        free(ctx->audio.delayBuffer);
        ctx->audio.delayBuffer = NULL;
    }
}