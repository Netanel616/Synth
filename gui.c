//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "gui.h"
#include <stdio.h>

#define WHITE_COLOR 255, 255, 255, 255
#define RED_COLOR 255, 0, 0, 255
#define BLACK_COLOR 0, 0, 0, 255
#define BLUE_COLOR 0, 0, 255, 255

// Keyboard Layout Constants
#define KEYBOARD_START_X 50
#define KEYBOARD_START_Y 300
#define WHITE_KEY_WIDTH 40
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 25
#define BLACK_KEY_HEIGHT 120

// Ordered by note
static SDL_Scancode pianoKeys[] = {
    // Octave -1 (C3)
    SDL_SCANCODE_Z, SDL_SCANCODE_S, SDL_SCANCODE_X, SDL_SCANCODE_D,
    SDL_SCANCODE_C, SDL_SCANCODE_V, SDL_SCANCODE_G, SDL_SCANCODE_B,
    SDL_SCANCODE_H, SDL_SCANCODE_N, SDL_SCANCODE_J, SDL_SCANCODE_M,
    // Octave 0 (C4)
    SDL_SCANCODE_Q, SDL_SCANCODE_2, SDL_SCANCODE_W, SDL_SCANCODE_3,
    SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_5, SDL_SCANCODE_T,
    SDL_SCANCODE_6, SDL_SCANCODE_Y, SDL_SCANCODE_7, SDL_SCANCODE_U,
    // Octave 1 (C5)
    SDL_SCANCODE_I, SDL_SCANCODE_9, SDL_SCANCODE_O, SDL_SCANCODE_0,
    SDL_SCANCODE_P, SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_EQUALS, SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN
};

static const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// Global font variable to avoid opening/closing every frame
static TTF_Font* globalFont = NULL;

// Creates a texture from a font and message
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* message) {
    if (!font || !message) return NULL;

    SDL_Color blue = {BLUE_COLOR};

    // 1. Create Surface (using Blended for better quality than Solid)
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, blue);
    if (!surface) {
        printf("Surface Error: %s\n", TTF_GetError());
        return NULL;
    }

    // 2. Convert to Texture (VRAM)
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // 3. Free Surface (no longer needed after upload to GPU)
    SDL_FreeSurface(surface);

    return texture;
}

bool setup_notes(GuiContext* ctx, SDL_Renderer* renderer)
{
    if (!globalFont) {
        globalFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
        if (!globalFont) {
            printf("Font Error: %s\n", TTF_GetError());
            return false;
        }
    }

    int whiteKeyCount = 0;
    for (int i = 0; i < NUM_NOTES; i++) {
        int noteInOctave = i % 12;
        ctx->notes[i].name = noteNames[noteInOctave];
        ctx->notes[i].key = pianoKeys[i];
        ctx->notes[i].isVisualPressed = false;

        // Determine if the key is black or white
        // Indices: 1(C#), 3(D#), 6(F#), 8(G#), 10(A#) are black keys
        bool isBlack = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10);
        ctx->notes[i].isBlack = isBlack;

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

    for (int i = 0; i < NUM_NOTES; i++) {
        // Load text and position relative to rectangle
        if (!ctx->notes[i].text)
        {
            SDL_Texture* text = create_text_texture(renderer, globalFont, ctx->notes[i].name);
            ctx->notes[i].text = text;
            // Center text inside key rectangle
            SDL_QueryTexture(text, NULL, NULL, &ctx->notes[i].lableWidth, &ctx->notes[i].lableHeight);
        }
    }
    return true;
}

void destroy_notes(GuiContext* ctx)
{
    for (int i = 0; i < NUM_NOTES; i++)
    {
        if (ctx->notes[i].text) SDL_DestroyTexture(ctx->notes[i].text);
    }
}

bool init_gui(GuiContext* ctx, SharedState* state)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || TTF_Init() == -1) {
        printf("Initialization Error: %s\n", SDL_GetError());
        return false;
    }
    ctx->window = SDL_CreateWindow("Fun Synth", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!setup_notes(ctx, ctx->renderer))
    {
        printf("setup_notes failed: %s\n", SDL_GetError());
        return false;
    };
    return true;
}

void draw_white(GuiContext* ctx)
{
    for (int i = 0; i < NUM_NOTES; i++)
    {
        if (ctx->notes[i].isBlack) continue;

        if (ctx->notes[i].isVisualPressed)
        {
            SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        } else
        {
            SDL_SetRenderDrawColor(ctx->renderer, WHITE_COLOR);
        }
        SDL_RenderFillRect(ctx->renderer, &ctx->notes[i].rect);
        SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[i].rect);

        if (ctx->notes[i].text != NULL) {
            SDL_Rect destRect;
            destRect.w = ctx->notes[i].lableWidth;
            destRect.h = ctx->notes[i].lableHeight;

            destRect.x = ctx->notes[i].rect.x + (ctx->notes[i].rect.w - destRect.w) / 2;
            destRect.y = ctx->notes[i].rect.y + ctx->notes[i].rect.h - destRect.h - 20;

            SDL_RenderCopy(ctx->renderer, ctx->notes[i].text, NULL, &destRect);
        }
    }
}

void draw_black(GuiContext* ctx)
{
    for (int i = 0; i < NUM_NOTES; i++)
    {
        if (!ctx->notes[i].isBlack) continue;

        if (ctx->notes[i].isVisualPressed)
        {
            SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        } else
        {
            SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        }
        SDL_RenderFillRect(ctx->renderer, &ctx->notes[i].rect);
        SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[i].rect);

        if (ctx->notes[i].text != NULL) {
            SDL_Rect destRect;
            destRect.w = ctx->notes[i].lableWidth;
            destRect.h = ctx->notes[i].lableHeight;

            destRect.x = ctx->notes[i].rect.x + (ctx->notes[i].rect.w - destRect.w) / 2;
            destRect.y = ctx->notes[i].rect.y + ctx->notes[i].rect.h - destRect.h - 20;

            SDL_RenderCopy(ctx->renderer, ctx->notes[i].text, NULL, &destRect);
        }
    }
}

const char* get_waveform_name(WaveformType type) {
    switch (type) {
        case WAVE_SINE: return "Sine";
        case WAVE_SAW: return "Sawtooth";
        case WAVE_SQUARE: return "Square";
        case WAVE_TRIANGLE: return "Triangle";
        default: return "Unknown";
    }
}

void draw_status_text(GuiContext* ctx, SharedState* state)
{
    static SDL_Texture* statusTexture = NULL;
    static int lastOctave = -999;
    static WaveformType lastWaveform = -1;
    static int w, h;

    // Only recreate texture if something changed
    if (state->octaveOffset != lastOctave || state->waveform != lastWaveform) {
        if (statusTexture) {
            SDL_DestroyTexture(statusTexture);
        }

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Octave: %d | Wave: %s", state->octaveOffset, get_waveform_name(state->waveform));

        statusTexture = create_text_texture(ctx->renderer, globalFont, buffer);
        SDL_QueryTexture(statusTexture, NULL, NULL, &w, &h);

        lastOctave = state->octaveOffset;
        lastWaveform = state->waveform;
    }

    if (statusTexture) {
        SDL_Rect destRect = {20, 20, w, h}; // Top-left corner
        SDL_RenderCopy(ctx->renderer, statusTexture, NULL, &destRect);
    }
}

void render_frame(GuiContext* ctx, SharedState* state)
{
    SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 30, 255);
    SDL_RenderClear(ctx->renderer);
    draw_white(ctx);
    draw_black(ctx);
    draw_status_text(ctx, state);
    SDL_RenderPresent(ctx->renderer);
}

void destroy_gui(GuiContext* ctx)
{
    destroy_notes(ctx);
    if (globalFont) TTF_CloseFont(globalFont);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    SDL_Quit();
}