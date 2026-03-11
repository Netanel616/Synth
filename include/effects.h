//
// Created by Netanel Seri on 24/02/2026.
//

#ifndef SYNTH1_EFFECTS_H
#define SYNTH1_EFFECTS_H

#include "common.h"

// Initialize effects state
void init_effects(AppContext* ctx);

// Process a stereo frame through the effects chain
void process_effects(AudioContext* ctx, float* left, float* right);

// Helper to get effect name
const char* get_effect_name(EffectType type);

#endif //SYNTH1_EFFECTS_H