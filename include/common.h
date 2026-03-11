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
#define MAX_DELAY_SECONDS 2.0f

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

typedef enum {
    FX_DISTORTION,
    FX_FILTER,
    FX_DELAY,
    FX_COUNT // Helper to know how many effects there are
} EffectType;

// --- Data Structures ---

// Oscillator Definition
typedef struct {
    WaveformType type;
    float volume;       // 0.0 to 1.0
    int octaveOffset;   // -2, -1, 0, 1, 2...
    float detune;       // Fine tuning (0.9 to 1.1 approx)
    bool enabled;
} Oscillator;

// Master Effects
typedef struct {
    // Distortion
    bool distEnabled;
    float distAmount;   // 0.0 to 1.0

    // Filter (Low Pass)
    bool filterEnabled;
    float filterCutoff; // 0.0 to 1.0 (Frequency)
    float filterResonance; // 0.0 to 0.95

    // Delay (BPM Synced)
    bool delayEnabled;
    int delayNoteDiv;   // 1=Whole, 2=Half, 4=Quarter, 8=Eighth
    float delayFeedback;// 0.0 to 0.9
    float delayMix;     // 0.0 to 1.0
} MasterEffects;

// 1. Shared State (Logic & Settings)
typedef struct {
    // Global Settings
    int octaveOffset;       // Keyboard octave shift
    float masterVolume;     // Master gain
    float bpm;              // Beats Per Minute
    bool running;

    // Modular Oscillator Engine
    Oscillator* oscillators;    // Dynamic array of oscillators
    int numOscillators;         // Current count
    int oscCapacity;            // Current allocated capacity
    int selectedOscIndex;       // Currently selected oscillator for editing

    // Effects
    MasterEffects effects;
    EffectType selectedEffect; // For UI editing
} SharedState;

// 2. Audio Context (Sound Engine)
typedef struct {
    float frequency;
    float amplitude;
    int midiNote;
    bool isActive;
    SDL_Scancode ownerKey;

    // Per-oscillator phase tracking
    double* phases;
    int phasesCapacity;

    EnvelopeState state;
} Voice;

typedef struct {
    // Filter State (Stereo)
    float lpfLow[2];  // Left/Right
    float lpfBand[2]; // Left/Right

    SDL_AudioDeviceID deviceId;
    Voice voices[NUM_VOICES];
    SharedState* state;

    // Delay Buffer
    float* delayBuffer;
    int delayBufferIndex;
    int delayBufferSize;
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