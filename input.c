//
// Created by נתנאל סרי on 23/02/2026.
//
#include "common.h"

void activate_voice(AppContext* ctx, int midiNoteNumber, SDL_Scancode sc)
{
    static int lowestVoiceInd = 0;
    float lowestAmplitude = 2;
    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (!ctx->voices[i].isActive)
        {
            ctx->voices[i].isActive = true;
            ctx->voices[i].frequency = BASE_FREQUENCY*pow(2.0, (double)(midiNoteNumber-BASE_NOTE)/NUM_NOTES);
            ctx->voices[i].ownerKey = sc;
            ctx->voices[i].amplitude = 1;
            ctx->voices[i].midiNote = midiNoteNumber;
            lowestVoiceInd = (i+1)%NUM_VOICES;
            return;
        }
        if(ctx->voices[i].isActive)
        {
            if (ctx->voices[i].amplitude < lowestAmplitude)
            {
                lowestVoiceInd = i;
                lowestAmplitude = ctx->voices[i].amplitude;
            }
        }
    }
    ctx->voices[lowestVoiceInd].frequency = BASE_FREQUENCY*pow(2.0, (double)(midiNoteNumber-BASE_NOTE)/NUM_NOTES);
    ctx->voices[lowestVoiceInd].ownerKey = sc;
    ctx->voices[lowestVoiceInd].amplitude = 1;
}


void deactivate_voice(AppContext* ctx, SDL_Scancode sc)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (sc == ctx->voices[i].ownerKey)
        {
            ctx->voices[i].isActive = false;
        }
    }
}
void handle_input(AppContext* ctx)
{
    while (SDL_PollEvent(&ctx->event)) {
        if (ctx->event.type == SDL_QUIT) {
            ctx->running = false;
        }

        else if (ctx->event.type == SDL_KEYDOWN) {
            SDL_Scancode code = ctx->event.key.keysym.scancode;
            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->notes[i].key) {
                    ctx->notes[i].isVisualPressed = true;
                    int midiNote = i+BASE_NOTE+(NUM_NOTES*ctx->octaveOffset);
                    activate_voice(ctx, midiNote, code);
                }
            }
            if (code == SDL_SCANCODE_UP)
            {
                ctx->octaveOffset++;
            }
            if (code == SDL_SCANCODE_DOWN)
            {
                ctx->octaveOffset--;
            }
        }

        else if (ctx->event.type == SDL_KEYUP) {
            SDL_Scancode code = ctx->event.key.keysym.scancode;
            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->notes[i].key) {
                    ctx->notes[i].isVisualPressed = false;
                    deactivate_voice(ctx, code);
                }
            }
        }
    }
}
