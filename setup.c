//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"

// Keyboard Layout Constants
#define KEYBOARD_START_X 50
#define KEYBOARD_START_Y 300
#define WHITE_KEY_WIDTH 60
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 40
#define BLACK_KEY_HEIGHT 120

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
    ctx->waveform = WAVE_SINE; // Default waveform

    // Initialize all voices to OFF
    for (int i = 0; i < NUM_VOICES; i++) {
        ctx->voices[i].isActive = false;
        ctx->voices[i].frequency = 0.0f;
        ctx->voices[i].ownerKey = SDL_SCANCODE_UNKNOWN;
        ctx->notes[i].text = NULL;
    }

    int whiteKeyCount = 0;
    for (int i = 0; i < NUM_NOTES; i++) {
        ctx->notes[i].name = noteNames[i];
        ctx->notes[i].key = pianoKeys[i];
        ctx->notes[i].isVisualPressed = false;

        // Determine if the key is black or white
        // Indices: 1(C#), 3(D#), 6(F#), 8(G#), 10(A#) are black keys
        bool isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10);

        if (!isBlack) {
            // White Key Configuration
            ctx->notes[i].rect.w = WHITE_KEY_WIDTH;
            ctx->notes[i].rect.h = WHITE_KEY_HEIGHT;
            ctx->notes[i].rect.x = KEYBOARD_START_X + (whiteKeyCount * WHITE_KEY_WIDTH);
            ctx->notes[i].rect.y = KEYBOARD_START_Y;

            whiteKeyCount++;
        } else {
            // Black Key Configuration
            // Placed centered on the line between the previous white key and the next one.
            // Since 'whiteKeyCount' has already been incremented for the previous white key,
            // it currently represents the index of the *next* white key.
            // So we position it at the start of the next white key, shifted left by half the black key width.
            ctx->notes[i].rect.w = BLACK_KEY_WIDTH;
            ctx->notes[i].rect.h = BLACK_KEY_HEIGHT;
            ctx->notes[i].rect.x = KEYBOARD_START_X + (whiteKeyCount * WHITE_KEY_WIDTH) - (BLACK_KEY_WIDTH / 2);
            ctx->notes[i].rect.y = KEYBOARD_START_Y;
        }
    }
}

void setup_destroy(AppContext* ctx)
{
    // Cleanup logic if needed
}