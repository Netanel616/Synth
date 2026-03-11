//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "gui.h"
#include "effects.h"
#include <stdio.h>

#define WHITE_COLOR 255, 255, 255, 255
#define RED_COLOR 255, 0, 0, 255
#define BLACK_COLOR 0, 0, 0, 255
#define BLUE_COLOR 0, 0, 255, 255
#define GREEN_COLOR 0, 255, 0, 255
#define GRAY_COLOR 100, 100, 100, 255
#define DARK_GRAY_COLOR 50, 50, 50, 255
#define HIGHLIGHT_COLOR 255, 255, 0, 255

// Keyboard Layout Constants
#define KEYBOARD_START_X 50
#define KEYBOARD_START_Y 350
#define WHITE_KEY_WIDTH 40
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 25
#define BLACK_KEY_HEIGHT 120

// UI Layout Constants
#define TOP_BAR_HEIGHT 40
#define OSC_PANEL_Y 50
#define OSC_PANEL_HEIGHT 100
#define FX_PANEL_Y 160
#define FX_PANEL_HEIGHT 120
#define HELP_PANEL_Y 560

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
static TTF_Font* smallFont = NULL;

// Creates a texture from a font and message
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* message, SDL_Color color) {
    if (!font || !message) return NULL;
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, color);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Texture* texture = create_text_texture(renderer, font, text, color);
    if (texture) {
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect dest = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
    }
}

bool setup_notes(GuiContext* ctx, SDL_Renderer* renderer)
{
    if (!globalFont) {
        globalFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
        smallFont = TTF_OpenFont(FONT_PATH, 16);
        if (!globalFont || !smallFont) {
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

        bool isBlack = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10);
        ctx->notes[i].isBlack = isBlack;

        if (!isBlack) {
            ctx->notes[i].rect.w = WHITE_KEY_WIDTH;
            ctx->notes[i].rect.h = WHITE_KEY_HEIGHT;
            ctx->notes[i].rect.x = KEYBOARD_START_X + (whiteKeyCount * WHITE_KEY_WIDTH);
            ctx->notes[i].rect.y = KEYBOARD_START_Y;
            whiteKeyCount++;
        } else {
            ctx->notes[i].rect.w = BLACK_KEY_WIDTH;
            ctx->notes[i].rect.h = BLACK_KEY_HEIGHT;
            ctx->notes[i].rect.x = KEYBOARD_START_X + (whiteKeyCount * WHITE_KEY_WIDTH) - (BLACK_KEY_WIDTH / 2);
            ctx->notes[i].rect.y = KEYBOARD_START_Y;
        }
    }

    for (int i = 0; i < NUM_NOTES; i++) {
        if (!ctx->notes[i].text) {
            SDL_Color blue = {BLUE_COLOR};
            ctx->notes[i].text = create_text_texture(renderer, globalFont, ctx->notes[i].name, blue);
            SDL_QueryTexture(ctx->notes[i].text, NULL, NULL, &ctx->notes[i].lableWidth, &ctx->notes[i].lableHeight);
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

void draw_keyboard(GuiContext* ctx) {
    // Draw White Keys
    for (int i = 0; i < NUM_NOTES; i++) {
        if (ctx->notes[i].isBlack) continue;
        if (ctx->notes[i].isVisualPressed) SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        else SDL_SetRenderDrawColor(ctx->renderer, WHITE_COLOR);

        SDL_RenderFillRect(ctx->renderer, &ctx->notes[i].rect);
        SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[i].rect);

        if (ctx->notes[i].text != NULL) {
            SDL_Rect destRect = {
                ctx->notes[i].rect.x + (ctx->notes[i].rect.w - ctx->notes[i].lableWidth) / 2,
                ctx->notes[i].rect.y + ctx->notes[i].rect.h - ctx->notes[i].lableHeight - 20,
                ctx->notes[i].lableWidth,
                ctx->notes[i].lableHeight
            };
            SDL_RenderCopy(ctx->renderer, ctx->notes[i].text, NULL, &destRect);
        }
    }
    // Draw Black Keys
    for (int i = 0; i < NUM_NOTES; i++) {
        if (!ctx->notes[i].isBlack) continue;
        if (ctx->notes[i].isVisualPressed) SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        else SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);

        SDL_RenderFillRect(ctx->renderer, &ctx->notes[i].rect);
        SDL_SetRenderDrawColor(ctx->renderer, WHITE_COLOR); // White border for visibility
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[i].rect);
    }
}

const char* get_waveform_name(WaveformType type) {
    switch (type) {
        case WAVE_SINE: return "Sine";
        case WAVE_SAW: return "Saw";
        case WAVE_SQUARE: return "Square";
        case WAVE_TRIANGLE: return "Tri";
        default: return "Unk";
    }
}

void draw_oscillators_panel(GuiContext* ctx, SharedState* state) {
    int startX = 20;
    int width = 150;
    int height = 80;
    int gap = 10;

    SDL_Color white = {WHITE_COLOR};
    SDL_Color yellow = {HIGHLIGHT_COLOR};

    draw_text(ctx->renderer, globalFont, "Oscillators:", startX, OSC_PANEL_Y - 30, white);

    for (int i = 0; i < state->numOscillators; i++) {
        SDL_Rect rect = {startX + i * (width + gap), OSC_PANEL_Y, width, height};

        // Background
        if (i == state->selectedOscIndex) SDL_SetRenderDrawColor(ctx->renderer, 60, 60, 60, 255);
        else SDL_SetRenderDrawColor(ctx->renderer, DARK_GRAY_COLOR);
        SDL_RenderFillRect(ctx->renderer, &rect);

        // Border
        if (i == state->selectedOscIndex) SDL_SetRenderDrawColor(ctx->renderer, HIGHLIGHT_COLOR);
        else SDL_SetRenderDrawColor(ctx->renderer, GRAY_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &rect);

        // Text Info
        char buf[64];
        snprintf(buf, sizeof(buf), "Osc %d: %s", i + 1, get_waveform_name(state->oscillators[i].type));
        draw_text(ctx->renderer, smallFont, buf, rect.x + 10, rect.y + 10, white);

        snprintf(buf, sizeof(buf), "Vol: %.0f%%", state->oscillators[i].volume * 100);
        draw_text(ctx->renderer, smallFont, buf, rect.x + 10, rect.y + 35, white);

        snprintf(buf, sizeof(buf), "Oct: %d", state->oscillators[i].octaveOffset);
        draw_text(ctx->renderer, smallFont, buf, rect.x + 10, rect.y + 55, white);
    }
}

void draw_effects_panel(GuiContext* ctx, SharedState* state) {
    int startX = 20;
    int width = 200;
    int height = 100;
    int gap = 20;

    SDL_Color white = {WHITE_COLOR};
    SDL_Color green = {GREEN_COLOR};
    SDL_Color gray = {GRAY_COLOR};

    draw_text(ctx->renderer, globalFont, "Effects Chain:", startX, FX_PANEL_Y - 30, white);

    for (int i = 0; i < FX_COUNT; i++) {
        SDL_Rect rect = {startX + i * (width + gap), FX_PANEL_Y, width, height};
        bool enabled = false;
        char p1[64] = "", p2[64] = "";

        switch (i) {
            case FX_DISTORTION:
                enabled = state->effects.distEnabled;
                snprintf(p1, sizeof(p1), "Amt: %.0f%%", state->effects.distAmount * 100);
                break;
            case FX_FILTER:
                enabled = state->effects.filterEnabled;
                snprintf(p1, sizeof(p1), "Cut: %.0f%%", state->effects.filterCutoff * 100);
                snprintf(p2, sizeof(p2), "Res: %.0f%%", state->effects.filterResonance * 100);
                break;
            case FX_DELAY:
                enabled = state->effects.delayEnabled;
                snprintf(p1, sizeof(p1), "Fbk: %.0f%%", state->effects.delayFeedback * 100);
                snprintf(p2, sizeof(p2), "Mix: %.0f%%", state->effects.delayMix * 100);
                break;
        }

        // Background
        SDL_SetRenderDrawColor(ctx->renderer, DARK_GRAY_COLOR);
        SDL_RenderFillRect(ctx->renderer, &rect);

        // Border (Green if enabled, Yellow if selected)
        if (i == state->selectedEffect) SDL_SetRenderDrawColor(ctx->renderer, HIGHLIGHT_COLOR);
        else if (enabled) SDL_SetRenderDrawColor(ctx->renderer, GREEN_COLOR);
        else SDL_SetRenderDrawColor(ctx->renderer, GRAY_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &rect);

        // Title
        draw_text(ctx->renderer, smallFont, get_effect_name(i), rect.x + 10, rect.y + 10, enabled ? green : gray);

        // Parameters
        draw_text(ctx->renderer, smallFont, p1, rect.x + 10, rect.y + 40, white);
        draw_text(ctx->renderer, smallFont, p2, rect.x + 10, rect.y + 65, white);
    }
}

void draw_top_bar(GuiContext* ctx, SharedState* state) {
    char buf[128];
    SDL_Color white = {WHITE_COLOR};

    snprintf(buf, sizeof(buf), "BPM: %.0f  |  Octave: %d  |  Master Vol: %.0f%%",
             state->bpm, state->octaveOffset, state->masterVolume * 100);

    draw_text(ctx->renderer, globalFont, buf, 20, 10, white);
}

void draw_help_panel(GuiContext* ctx) {
    SDL_Color gray = {GRAY_COLOR};
    const char* help = "Controls: [Z-M, Q-U, I-P] Play Notes | [B + Up/Down] BPM | [Tab] Select FX | [Enter] Toggle FX | [ [ / ] ] FX Param 1 | [ ; / ' ] FX Param 2 | [ Left / Right ] Select Osc | [Shift + Up/Down] Osc Vol";
    draw_text(ctx->renderer, smallFont, help, 20, WINDOW_HEIGHT - 30, gray);
}

void render_frame(GuiContext* ctx, SharedState* state)
{
    SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 30, 255);
    SDL_RenderClear(ctx->renderer);

    draw_top_bar(ctx, state);
    draw_oscillators_panel(ctx, state);
    draw_effects_panel(ctx, state);
    draw_keyboard(ctx);
    draw_help_panel(ctx);

    SDL_RenderPresent(ctx->renderer);
}

void destroy_gui(GuiContext* ctx)
{
    destroy_notes(ctx);
    if (globalFont) TTF_CloseFont(globalFont);
    if (smallFont) TTF_CloseFont(smallFont);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    SDL_Quit();
}