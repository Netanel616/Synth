//
// Created by Netanel Seri on 24/02/2026.
//

#ifndef SYNTH1_AUDIO_H
#define SYNTH1_AUDIO_H

#include "common.h"

bool init_audio(AudioContext* ctx, SharedState* state);
void destroy_audio(AudioContext* ctx);

// Audio Engine Control
void audio_note_on(AudioContext* ctx, int midiNoteNumber, SDL_Scancode sc);
void audio_note_off(AudioContext* ctx, SDL_Scancode sc);

#endif //SYNTH1_AUDIO_H