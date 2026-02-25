#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <stdbool.h>
#include <math.h>
#include <SDL_ttf.h>

#define NUM_NOTES 36
#define NUM_VOICES 10

// Audio Settings
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048

// Musical Standards
#define A4_FREQUENCY 440.0f
#define A4_MIDI_NUMBER 69
#define BASE_NOTE 48 // C3

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 600
#define FONT_SIZE 24
#define FONT_PATH "/System/Library/Fonts/Supplemental/Arial.ttf"

typedef enum {
    OFF,
    ATTACK,
    SUSTAIN,
    RELEASE
} EnvelopeState;

typedef enum {
    WAVE_SINE,
    WAVE_SAW,
    WAVE_SQUARE,
    WAVE_TRIANGLE
} WaveformType;

// --- Data Structures ---

// 1. Shared State (Logic & Settings)
typedef struct {
    int octaveOffset;       // 0, 1, -1, ...
    float masterVolume;     // 0.0 to 1.0
    WaveformType waveform;  // Current selected waveform
    bool running;
} SharedState;

// 2. Audio Context (Sound Engine)
typedef struct {
    float frequency;
    float amplitude;
    int midiNote;
    bool isActive;
    SDL_Scancode ownerKey;
    double phase;
    EnvelopeState state;
} Voice;

typedef struct {
    SDL_AudioDeviceID deviceId;
    Voice voices[NUM_VOICES];
    SharedState* state; // Pointer to shared state (for waveform/volume access in callback)
} AudioContext;

// 3. GUI Context (Visuals)
typedef struct {
    const char* name;
    SDL_Scancode key;
    SDL_Rect rect;
    bool isVisualPressed;
    bool isBlack;
    SDL_Texture* text;
    int lableWidth, lableHeight;
} Note;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Note notes[NUM_NOTES];
} GuiContext;

// 4. Main App Context (Container)
typedef struct {
    SharedState state;
    AudioContext audio;
    GuiContext gui;
    SDL_Event event;
} AppContext;

#endif