//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "audio.h"

void handle_input(AppContext* ctx)
{
    while (SDL_PollEvent(&ctx->event)) {
        if (ctx->event.type == SDL_QUIT) {
            ctx->state.running = false;
        }

        else if (ctx->event.type == SDL_KEYDOWN) {
            if (ctx->event.key.repeat) continue; // Prevent "machine gun" effect on long press

            SDL_Scancode code = ctx->event.key.keysym.scancode;

            // Octave control
            if (code == SDL_SCANCODE_UP) { ctx->state.octaveOffset++; continue; }
            if (code == SDL_SCANCODE_DOWN) { ctx->state.octaveOffset--; continue; }

            // Waveform control
            if (code == SDL_SCANCODE_1) { ctx->state.waveform = WAVE_SINE; printf("Waveform: Sine\n"); continue; }
            if (code == SDL_SCANCODE_2) { ctx->state.waveform = WAVE_SAW; printf("Waveform: Sawtooth\n"); continue; }
            if (code == SDL_SCANCODE_3) { ctx->state.waveform = WAVE_SQUARE; printf("Waveform: Square\n"); continue; }
            if (code == SDL_SCANCODE_4) { ctx->state.waveform = WAVE_TRIANGLE; printf("Waveform: Triangle\n"); continue; }

            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->gui.notes[i].key) {
                    ctx->gui.notes[i].isVisualPressed = true;
                    // Each octave is 12 semitones
                    // The base note is now C3 (48).
                    // i=0 is C3, i=12 is C4, i=24 is C5.
                    // The octaveOffset shifts everything by 12 semitones.
                    int midiNote = i + BASE_NOTE + (12 * ctx->state.octaveOffset);
                    audio_note_on(&ctx->audio, midiNote, code);
                }
            }
        }

        else if (ctx->event.type == SDL_KEYUP) {
            SDL_Scancode code = ctx->event.key.keysym.scancode;
            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->gui.notes[i].key) {
                    ctx->gui.notes[i].isVisualPressed = false;
                    audio_note_off(&ctx->audio, code);
                }
            }
        }
    }
}