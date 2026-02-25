#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <stdbool.h>
#include <math.h>
#include <SDL_ttf.h>

#define NUM_NOTES 12
#define NUM_VOICES 10

// Audio Settings
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048

// Musical Standards
#define A4_FREQUENCY 440.0f
#define A4_MIDI_NUMBER 69
#define BASE_NOTE 60 // C4

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_SIZE 24
#define FONT_PATH "/System/Library/Fonts/Supplemental/Arial.ttf"

typedef enum {
    OFF,
    ATTACK,
    SUSTAIN, // Currently used when key is pressed
    RELEASE
} EnvelopeState;

typedef enum {
    WAVE_SINE,
    WAVE_SAW,
    WAVE_SQUARE,
    WAVE_TRIANGLE
} WaveformType;

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
    const char* name;
    SDL_Scancode key;
    SDL_Rect rect;
    bool isVisualPressed; // Only for GUI rendering
    SDL_Texture* text;
    int lableWidth, lableHeight;
} Note;

typedef struct {
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_AudioDeviceID audioDevice;

    Note notes[NUM_NOTES];
    Voice voices[NUM_VOICES];

    int octaveOffset;   // 0, 1, -1, ...
    float masterVolume; // 0.0 to 1.0
    WaveformType waveform; // Current selected waveform

} AppContext;

#endif