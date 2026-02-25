//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"

// Ordered by note
SDL_Scancode pianoKeys[] = {
    SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_E,
    SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_T, SDL_SCANCODE_G,
    SDL_SCANCODE_Y, SDL_SCANCODE_H, SDL_SCANCODE_U, SDL_SCANCODE_J
};

const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

void setup_init(AppContext* ctx)
{
    ctx->octaveOffset = 0;
    ctx->masterVolume = 0.5f;
    ctx->running = true;

    // Initialize all voices to OFF
    for (int i = 0; i < NUM_VOICES; i++) {
        ctx->voices[i].isActive = false;
        ctx->voices[i].frequency = 0.0f;
        ctx->voices[i].ownerKey = SDL_SCANCODE_UNKNOWN;
        ctx->notes[i].text = NULL;
    }

    for (int i = 0; i < NUM_NOTES; i++) {
        ctx->notes[i].name = noteNames[i];
        ctx->notes[i].key = pianoKeys[i];
        ctx->notes[i].isVisualPressed = false;

        // Basic graphical positioning (rectangles side by side)
        ctx->notes[i].rect.x = 50 + (i * 55);
        ctx->notes[i].rect.y = 300;
        ctx->notes[i].rect.w = 55;
        ctx->notes[i].rect.h = 200;

        // Adjust position for black keys
        if (i == 1 || i == 3 || i == 6 || i == 8 || i == 10)
        {
            ctx->notes[i].rect.y -= 100;
        }
    }
}

void setup_destroy(AppContext* ctx)
{
    // Cleanup logic if needed
}