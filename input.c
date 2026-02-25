//
// Created by נתנאל סרי on 23/02/2026.
//
#include "common.h"

#define BASE_NOTE 60        // C4 (Middle C)
#define BASE_FREQUENCY 440.0f // התדר של A4 (נשתמש בו כנקודת ייחוס בנוסחה)
#define MIDI_A4 69          // המספר של A4 בתקן MIDI

// void activate_voice(AppContext* ctx, int midiNoteNumber, SDL_Scancode sc)
// {
//     static int lowestVoiceInd = 0;
//     float lowestAmplitude = 2;
//     for (int i = 0; i < NUM_VOICES; i++)
//     {
//         if (!ctx->voices[i].isActive)
//         {
//             ctx->voices[i].isActive = true;
//             ctx->voices[i].state = ATTACK;
//             ctx->voices[i].frequency = BASE_FREQUENCY*pow(2.0, (double)(midiNoteNumber-BASE_NOTE)/NUM_NOTES);
//             ctx->voices[i].ownerKey = sc;
//             ctx->voices[i].phase = 0;
//             ctx->voices[i].amplitude = 0;
//             //ctx->voices[i].amplitude = 1;
//             ctx->voices[i].midiNote = midiNoteNumber;
//             lowestVoiceInd = (i+1)%NUM_VOICES;
//
//             return;
//         }
//         if(ctx->voices[i].isActive)
//         {
//             if (ctx->voices[i].amplitude < lowestAmplitude)
//             {
//                 lowestVoiceInd = i;
//                 lowestAmplitude = ctx->voices[i].amplitude;
//             }
//         }
//     }
//     ctx->voices[lowestVoiceInd].frequency = BASE_FREQUENCY*pow(2.0, (double)(midiNoteNumber-BASE_NOTE)/NUM_NOTES);
//     ctx->voices[lowestVoiceInd].ownerKey = sc;
//     ctx->voices[lowestVoiceInd].amplitude = 1;
// }
void activate_voice(AppContext* ctx, int midiNoteNumber, SDL_Scancode sc) {
    int stealCandidate = -1;
    float minAmplitude = 1.1f;

    // 1. חיפוש קול פנוי לחלוטין או מציאת מועמד לגניבה
    for (int i = 0; i < NUM_VOICES; i++) {
        if (ctx->voices[i].state == OFF) {
            stealCandidate = i;
            break; // מצאנו מקום ריק, אין צורך להמשיך
        }

        // מנגנון Stealing: מחפשים את הקול עם האמפליטודה הכי נמוכה (בד"כ ב-Release)
        if (ctx->voices[i].amplitude < minAmplitude) {
            minAmplitude = ctx->voices[i].amplitude;
            stealCandidate = i;
        }
    }

    // 2. אתחול הקול הנבחר (חדש או גנוב)
    if (stealCandidate != -1) {
        Voice* v = &ctx->voices[stealCandidate];

        // ננעל את האודיו לרגע כדי למנוע קפיצות בסאונד בזמן השינוי
        SDL_LockAudioDevice(ctx->audioDevice);

        v->frequency = BASE_FREQUENCY * pow(2.0, (double)(midiNoteNumber - BASE_NOTE) / 12.0);
        v->ownerKey = sc;
        v->midiNote = midiNoteNumber;
        v->phase = 0.0;
        v->state = ATTACK; // מחזיר אותו לאטק (אם נגנב מה-Release)
        v->isActive = true;
        // אם גנבנו קול, נשאיר את ה-amplitude הנוכחית שלו כדי למנוע "קליק" חד
        // או נאפס אם הוא היה ב-OFF
        if (v->state == OFF) v->amplitude = 0.0f;

        SDL_UnlockAudioDevice(ctx->audioDevice);
    }
}


void deactivate_voice(AppContext* ctx, SDL_Scancode sc) {
    // נועלים כדי להבטיח אטומיות מול ה-Audio Thread
    SDL_LockAudioDevice(ctx->audioDevice);

    for (int i = 0; i < NUM_VOICES; i++) {
        // מחפשים קול ששייך למקש הזה ועדיין לא נמצא בדעיכה
        if (ctx->voices[i].ownerKey == sc && ctx->voices[i].state != RELEASE) {
            ctx->voices[i].state = RELEASE;
        }
    }

    SDL_UnlockAudioDevice(ctx->audioDevice);
}
void handle_input(AppContext* ctx)
{
    while (SDL_PollEvent(&ctx->event)) {
        if (ctx->event.type == SDL_QUIT) {
            ctx->running = false;
        }

        else if (ctx->event.type == SDL_KEYDOWN) {
            if (ctx->event.key.repeat) continue; // מונע "מכונת ירייה" בלחיצה ארוכה

            SDL_Scancode code = ctx->event.key.keysym.scancode;

            // שליטה באוקטבות
            if (code == SDL_SCANCODE_UP) { ctx->octaveOffset++; continue; }
            if (code == SDL_SCANCODE_DOWN) { ctx->octaveOffset--; continue; }

            for (int i = 0; i < NUM_NOTES; i++) {
                if (code == ctx->notes[i].key) {
                    ctx->notes[i].isVisualPressed = true;
                    // כל אוקטבה היא 12 חצאי טונים
                    int midiNote = i + BASE_NOTE + (12 * ctx->octaveOffset);
                    activate_voice(ctx, midiNote, code);
                }
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
