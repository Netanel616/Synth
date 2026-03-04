//
// Created by Netanel Seri on 24/02/2026.
//
#include "common.h"
#include "audio.h"
#include "effects.h"
#include <stdlib.h>
#include <stdio.h>

void handle_input(AppContext* ctx)
{
    while (SDL_PollEvent(&ctx->event)) {
        if (ctx->event.type == SDL_QUIT) {
            ctx->state.running = false;
        }

        else if (ctx->event.type == SDL_KEYDOWN) {
            if (ctx->event.key.repeat) continue; // Prevent "machine gun" effect on long press

            SDL_Scancode code = ctx->event.key.keysym.scancode;
            SDL_Keymod mod = SDL_GetModState();
            bool shiftPressed = (mod & KMOD_SHIFT);

            // BPM Control (B + Up/Down)
            if (code == SDL_SCANCODE_B) {
                // Just a flag, actual change happens with arrows
                continue;
            }
            const Uint8* state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_B]) {
                if (code == SDL_SCANCODE_UP) { ctx->state.bpm += 1.0f; printf("BPM: %.1f\n", ctx->state.bpm); continue; }
                if (code == SDL_SCANCODE_DOWN) { ctx->state.bpm -= 1.0f; printf("BPM: %.1f\n", ctx->state.bpm); continue; }
            }

            // Effect Selection (TAB)
            if (code == SDL_SCANCODE_TAB) {
                ctx->state.selectedEffect = (ctx->state.selectedEffect + 1) % FX_COUNT;
                printf("Selected Effect: %s\n", get_effect_name(ctx->state.selectedEffect));
                continue;
            }

            // Effect Toggle (RETURN)
            if (code == SDL_SCANCODE_RETURN) {
                switch (ctx->state.selectedEffect) {
                    case FX_DISTORTION: ctx->state.effects.distEnabled = !ctx->state.effects.distEnabled; break;
                    case FX_FILTER: ctx->state.effects.filterEnabled = !ctx->state.effects.filterEnabled; break;
                    case FX_DELAY: ctx->state.effects.delayEnabled = !ctx->state.effects.delayEnabled; break;
                    default: break;
                }
                printf("Toggled %s\n", get_effect_name(ctx->state.selectedEffect));
                continue;
            }

            // Effect Parameter Control
            // Param 1: [ / ]
            // Param 2: ; / '
            if (code == SDL_SCANCODE_LEFTBRACKET) {
                switch (ctx->state.selectedEffect) {
                    case FX_DISTORTION: ctx->state.effects.distAmount -= 0.05f; if(ctx->state.effects.distAmount < 0) ctx->state.effects.distAmount = 0; break;
                    case FX_FILTER: ctx->state.effects.filterCutoff -= 0.05f; if(ctx->state.effects.filterCutoff < 0) ctx->state.effects.filterCutoff = 0; break;
                    case FX_DELAY: ctx->state.effects.delayFeedback -= 0.05f; if(ctx->state.effects.delayFeedback < 0) ctx->state.effects.delayFeedback = 0; break;
                    default: break;
                }
                continue;
            }
            if (code == SDL_SCANCODE_RIGHTBRACKET) {
                switch (ctx->state.selectedEffect) {
                    case FX_DISTORTION: ctx->state.effects.distAmount += 0.05f; if(ctx->state.effects.distAmount > 1) ctx->state.effects.distAmount = 1; break;
                    case FX_FILTER: ctx->state.effects.filterCutoff += 0.05f; if(ctx->state.effects.filterCutoff > 1) ctx->state.effects.filterCutoff = 1; break;
                    case FX_DELAY: ctx->state.effects.delayFeedback += 0.05f; if(ctx->state.effects.delayFeedback > 0.9) ctx->state.effects.delayFeedback = 0.9; break;
                    default: break;
                }
                continue;
            }
            if (code == SDL_SCANCODE_SEMICOLON) {
                switch (ctx->state.selectedEffect) {
                    case FX_FILTER: ctx->state.effects.filterResonance -= 0.05f; if(ctx->state.effects.filterResonance < 0) ctx->state.effects.filterResonance = 0; break;
                    case FX_DELAY: ctx->state.effects.delayMix -= 0.05f; if(ctx->state.effects.delayMix < 0) ctx->state.effects.delayMix = 0; break;
                    default: break;
                }
                continue;
            }
            if (code == SDL_SCANCODE_APOSTROPHE) {
                switch (ctx->state.selectedEffect) {
                    case FX_FILTER: ctx->state.effects.filterResonance += 0.05f; if(ctx->state.effects.filterResonance > 0.95) ctx->state.effects.filterResonance = 0.95; break;
                    case FX_DELAY: ctx->state.effects.delayMix += 0.05f; if(ctx->state.effects.delayMix > 1) ctx->state.effects.delayMix = 1; break;
                    default: break;
                }
                continue;
            }


            // Oscillator Volume Control (Shift + Up/Down)
            if (shiftPressed && !state[SDL_SCANCODE_B]) {
                if (code == SDL_SCANCODE_UP) {
                    if (ctx->state.numOscillators > 0) {
                        ctx->state.oscillators[ctx->state.selectedOscIndex].volume += 0.05f;
                        if (ctx->state.oscillators[ctx->state.selectedOscIndex].volume > 1.0f)
                            ctx->state.oscillators[ctx->state.selectedOscIndex].volume = 1.0f;
                    }
                    continue;
                }
                if (code == SDL_SCANCODE_DOWN) {
                    if (ctx->state.numOscillators > 0) {
                        ctx->state.oscillators[ctx->state.selectedOscIndex].volume -= 0.05f;
                        if (ctx->state.oscillators[ctx->state.selectedOscIndex].volume < 0.0f)
                            ctx->state.oscillators[ctx->state.selectedOscIndex].volume = 0.0f;
                    }
                    continue;
                }
            }

            // Octave control (No Shift, No B)
            if (!shiftPressed && !state[SDL_SCANCODE_B]) {
                if (code == SDL_SCANCODE_UP) { ctx->state.octaveOffset++; continue; }
                if (code == SDL_SCANCODE_DOWN) { ctx->state.octaveOffset--; continue; }
            }

            // Oscillator Management
            if (code == SDL_SCANCODE_EQUALS) { // '+' key
                if (ctx->state.numOscillators < ctx->state.oscCapacity) {
                    int idx = ctx->state.numOscillators++;
                    ctx->state.oscillators[idx].type = WAVE_SINE;
                    ctx->state.oscillators[idx].volume = 0.5f;
                    ctx->state.oscillators[idx].octaveOffset = 0;
                    ctx->state.oscillators[idx].detune = 1.0f;
                    ctx->state.oscillators[idx].enabled = true;
                    printf("Added Oscillator %d\n", idx);
                }
                continue;
            }
            if (code == SDL_SCANCODE_MINUS) { // '-' key
                if (ctx->state.numOscillators > 1) {
                    ctx->state.numOscillators--;
                    if (ctx->state.selectedOscIndex >= ctx->state.numOscillators) {
                        ctx->state.selectedOscIndex = ctx->state.numOscillators - 1;
                    }
                    printf("Removed Oscillator\n");
                }
                continue;
            }

            // Oscillator Selection (Using Left/Right Arrows)
            if (code == SDL_SCANCODE_RIGHT) {
                if (ctx->state.selectedOscIndex < ctx->state.numOscillators - 1) {
                    ctx->state.selectedOscIndex++;
                    printf("Selected Oscillator %d\n", ctx->state.selectedOscIndex);
                }
                continue;
            }
            if (code == SDL_SCANCODE_LEFT) {
                if (ctx->state.selectedOscIndex > 0) {
                    ctx->state.selectedOscIndex--;
                    printf("Selected Oscillator %d\n", ctx->state.selectedOscIndex);
                }
                continue;
            }

            // Waveform control for selected oscillator
            if (code == SDL_SCANCODE_1) { ctx->state.oscillators[ctx->state.selectedOscIndex].type = WAVE_SINE; printf("Osc %d: Sine\n", ctx->state.selectedOscIndex); continue; }
            if (code == SDL_SCANCODE_2) { ctx->state.oscillators[ctx->state.selectedOscIndex].type = WAVE_SAW; printf("Osc %d: Sawtooth\n", ctx->state.selectedOscIndex); continue; }
            if (code == SDL_SCANCODE_3) { ctx->state.oscillators[ctx->state.selectedOscIndex].type = WAVE_SQUARE; printf("Osc %d: Square\n", ctx->state.selectedOscIndex); continue; }
            if (code == SDL_SCANCODE_4) { ctx->state.oscillators[ctx->state.selectedOscIndex].type = WAVE_TRIANGLE; printf("Osc %d: Triangle\n", ctx->state.selectedOscIndex); continue; }

            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->gui.notes[i].key) {
                    ctx->gui.notes[i].isVisualPressed = true;
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