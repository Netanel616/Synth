//
// Created by Netanel Seri on 24/02/2026.
//

#ifndef SYNTH1_AUDIO_H
#define SYNTH1_AUDIO_H

#include "common.h"

bool init_audio(AppContext* ctx);
void destroy_audio(AppContext* ctx);

// Audio Engine Control
void audio_note_on(AppContext* ctx, int midiNoteNumber, SDL_Scancode sc);
void audio_note_off(AppContext* ctx, SDL_Scancode sc);

#endif //SYNTH1_AUDIO_H