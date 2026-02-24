#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <stdbool.h>
#include <math.h>
#include <SDL_ttf.h>

#define NUM_NOTES 12
#define NUM_VOICES 10
#define BASE_FREQUENCY 44100
#define BASE_NOTE 60
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_SIZE 24
#define FONT_PATH "/System/Library/Fonts/Supplemental/Arial.ttf"

typedef struct {
    float frequency;
    float amplitude;
    int midiNote;
    bool isActive;
    SDL_Scancode ownerKey; // המקש שתפס את הקול הזה
} Voice;

typedef struct {
    const char* name;
    SDL_Scancode key;
    SDL_Rect rect;
    bool isVisualPressed;// רק לצורך הציור ב-GUI
    SDL_Texture* text;
    int lableWidth, lableHeight;
} Note;

typedef struct {
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    Note notes[NUM_NOTES];
    Voice voices[NUM_VOICES];

    int octaveOffset;   //0, 1, -1, ...
    float masterVolume; // 0.0 to 1.0
} AppContext;

#endif