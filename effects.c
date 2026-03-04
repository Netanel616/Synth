//
// Created by Netanel Seri on 24/02/2026.
//
#include "effects.h"
#include <math.h>
#include <stdlib.h>

void init_effects(AppContext* ctx) {
    // Default BPM
    ctx->state.bpm = 120.0f;

    // Distortion
    ctx->state.effects.distEnabled = false;
    ctx->state.effects.distAmount = 0.5f;

    // Filter
    ctx->state.effects.filterEnabled = false;
    ctx->state.effects.filterCutoff = 0.8f; // Open
    ctx->state.effects.filterResonance = 0.2f;

    // Delay
    ctx->state.effects.delayEnabled = false;
    ctx->state.effects.delayNoteDiv = 4; // Quarter note
    ctx->state.effects.delayFeedback = 0.5f;
    ctx->state.effects.delayMix = 0.5f;

    // Initialize Filter State
    ctx->audio.lpfLow[0] = 0.0f;
    ctx->audio.lpfLow[1] = 0.0f;
    ctx->audio.lpfBand[0] = 0.0f;
    ctx->audio.lpfBand[1] = 0.0f;

    // Initialize Delay Buffer
    ctx->audio.delayBufferSize = (int)(SAMPLE_RATE * MAX_DELAY_SECONDS);
    ctx->audio.delayBuffer = (float*)calloc(ctx->audio.delayBufferSize, sizeof(float));
    ctx->audio.delayBufferIndex = 0;
}

const char* get_effect_name(EffectType type) {
    switch (type) {
        case FX_DISTORTION: return "Distortion";
        case FX_FILTER: return "Filter";
        case FX_DELAY: return "Delay";
        default: return "Unknown";
    }
}

// --- Effect Algorithms ---

static float apply_distortion(float sample, float amount) {
    float drive = 1.0f + (amount * 20.0f);
    float x = sample * drive;
    return x / (1.0f + fabsf(x));
}

static void apply_filter(AudioContext* ctx, float* left, float* right) {
    // State Variable Filter (Chamberlin)
    // Cutoff mapping: 0.0-1.0 -> 20Hz-15kHz (approx)
    float cutoff = ctx->state->effects.filterCutoff;
    float freq = 20.0f * powf(1000.0f, cutoff); // Exponential mapping

    // Calculate filter coefficient 'f'
    // f = 2 * sin(pi * freq / sampleRate)
    float f = 2.0f * sinf((float)M_PI * freq / (float)SAMPLE_RATE);

    // Resonance mapping: 0.0-1.0 -> Q factor
    float q = 1.0f - ctx->state->effects.filterResonance;

    // Process Left Channel
    ctx->lpfLow[0] += f * ctx->lpfBand[0];
    float highL = *left - ctx->lpfLow[0] - (q * ctx->lpfBand[0]);
    ctx->lpfBand[0] += f * highL;

    // Process Right Channel
    ctx->lpfLow[1] += f * ctx->lpfBand[1];
    float highR = *right - ctx->lpfLow[1] - (q * ctx->lpfBand[1]);
    ctx->lpfBand[1] += f * highR;

    // Output Low Pass
    *left = ctx->lpfLow[0];
    *right = ctx->lpfLow[1];
}

static void apply_delay(AudioContext* ctx, float* left, float* right) {
    // Calculate delay time based on BPM
    // 60 / BPM = seconds per beat
    // seconds per beat * 4 / noteDiv = delay time
    // e.g., 120 BPM, Quarter note (4): 0.5 * 4/4 = 0.5s
    float secondsPerBeat = 60.0f / ctx->state->bpm;
    float delayTime = secondsPerBeat * (4.0f / (float)ctx->state->effects.delayNoteDiv);

    // Clamp delay time
    if (delayTime > MAX_DELAY_SECONDS) delayTime = MAX_DELAY_SECONDS;

    int delayFrames = (int)(delayTime * SAMPLE_RATE);
    int delayOffset = delayFrames * 2; // Stereo interleaved

    int readIndex = ctx->delayBufferIndex - delayOffset;
    while (readIndex < 0) readIndex += ctx->delayBufferSize;

    float delayedL = ctx->delayBuffer[readIndex];
    float delayedR = ctx->delayBuffer[readIndex + 1];

    float feedback = ctx->state->effects.delayFeedback;
    float mix = ctx->state->effects.delayMix;

    // Write back with feedback
    ctx->delayBuffer[ctx->delayBufferIndex] = *left + (delayedL * feedback);
    ctx->delayBuffer[ctx->delayBufferIndex + 1] = *right + (delayedR * feedback);

    ctx->delayBufferIndex += 2;
    if (ctx->delayBufferIndex >= ctx->delayBufferSize) ctx->delayBufferIndex = 0;

    *left = (*left * (1.0f - mix)) + (delayedL * mix);
    *right = (*right * (1.0f - mix)) + (delayedR * mix);
}

void process_effects(AudioContext* ctx, float* left, float* right) {
    // 1. Distortion
    if (ctx->state->effects.distEnabled) {
        *left = apply_distortion(*left, ctx->state->effects.distAmount);
        *right = apply_distortion(*right, ctx->state->effects.distAmount);
    }

    // 2. Filter
    if (ctx->state->effects.filterEnabled) {
        apply_filter(ctx, left, right);
    }

    // 3. Delay
    if (ctx->state->effects.delayEnabled) {
        apply_delay(ctx, left, right);
    }
}