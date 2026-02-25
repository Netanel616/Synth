//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "audio.h"

void handle_input(AppContext* ctx)
{
    while (SDL_PollEvent(&ctx->event)) {
        if (ctx->event.type == SDL_QUIT) {
            ctx->running = false;
        }

        else if (ctx->event.type == SDL_KEYDOWN) {
            if (ctx->event.key.repeat) continue; // Prevent "machine gun" effect on long press

            SDL_Scancode code = ctx->event.key.keysym.scancode;

            // Octave control
            if (code == SDL_SCANCODE_UP) { ctx->octaveOffset++; continue; }
            if (code == SDL_SCANCODE_DOWN) { ctx->octaveOffset--; continue; }

            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->notes[i].key) {
                    ctx->notes[i].isVisualPressed = true;
                    // Each octave is 12 semitones
                    int midiNote = i + BASE_NOTE + (12 * ctx->octaveOffset);
                    audio_note_on(ctx, midiNote, code);
                }
            }
        }

        else if (ctx->event.type == SDL_KEYUP) {
            SDL_Scancode code = ctx->event.key.keysym.scancode;
            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->notes[i].key) {
                    ctx->notes[i].isVisualPressed = false;
                    audio_note_off(ctx, code);
                }
            }
        }
    }
}