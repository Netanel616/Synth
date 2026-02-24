//
// Created by נתנאל סרי on 24/02/2026.
//
#include "common.h"
#define WHITE_COLOR 255, 255, 255, 255
#define RED_COLOR 255, 0, 0, 255
#define BLACK_COLOR 0, 0, 0, 255
#define BLUE_COLOR 0, 0, 255, 255

static SDL_Texture* octaveText;

// הפונקציה מקבלת פונט שכבר נטען לזיכרון
SDL_Texture* create_text_texture(SDL_Renderer* renderer, TTF_Font* font, const char* message) {
    if (!font || !message) return NULL;

    SDL_Color blue = {BLUE_COLOR};

    // 1. יצירת Surface (בעזרת Blended למראה איכותי יותר מ-Solid)
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, blue);
    if (!surface) {
        printf("Surface Error: %s\n", TTF_GetError());
        return NULL;
    }

    // 2. המרה לטקסטורה (VRAM)
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // 3. ניקוי ה-Surface (כבר אין בו צורך אחרי שהעלינו לכרטיס המסך)
    SDL_FreeSurface(surface);

    return texture;
}

bool setup_notes(AppContext* ctx, SDL_Renderer* renderer)
{
    TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font) {return false;}
    for (int i = 0; i < NUM_NOTES; i++) {
        // 2. טעינת הטקסט ומיקומו ביחס למלבן
        if (!ctx->notes[i].text)
        {
            SDL_Texture* text =create_text_texture(renderer, font, ctx->notes[i].name);
            ctx->notes[i].text = text;
            // מרכז את הטקסט בתוך המלבן של הקליד
            SDL_QueryTexture(text, NULL, NULL, &ctx->notes[i].lableWidth, &ctx->notes[i].lableHeight);
        }
    }
    TTF_CloseFont(font);
    return true;
}

void destroy_notes(AppContext* ctx)
{
    for (int i = 0; i < NUM_NOTES; i++)
    {
        if (ctx->notes[i].text) SDL_DestroyTexture(ctx->notes[i].text);
    }
}


bool init_gui(AppContext* ctx)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
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

void draw_white(AppContext* ctx)
{
    int whiteIndex[] = {0,2,4,5,7,9,11};
    for (int i = 0;i<7; i++)
    {
        if (ctx->notes[whiteIndex[i]].isVisualPressed)
        {
            SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        } else
        {
            SDL_SetRenderDrawColor(ctx->renderer, WHITE_COLOR);
        }
        SDL_RenderFillRect(ctx->renderer, &ctx->notes[whiteIndex[i]].rect);
        SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[whiteIndex[i]].rect);

        if (ctx->notes[whiteIndex[i]].text != NULL) {
            SDL_Rect destRect;
            destRect.w = ctx->notes[whiteIndex[i]].lableWidth;
            destRect.h = ctx->notes[whiteIndex[i]].lableHeight;

            destRect.x = ctx->notes[whiteIndex[i]].rect.x + (ctx->notes[whiteIndex[i]].rect.w - destRect.w) / 2;
            destRect.y = ctx->notes[whiteIndex[i]].rect.y + ctx->notes[whiteIndex[i]].rect.h - destRect.h - 20;

            SDL_RenderCopy(ctx->renderer, ctx->notes[whiteIndex[i]].text, NULL, &destRect);
        }
    }
}
void draw_black(AppContext* ctx)
{
    int blackIndex[] = {1,3,6,8,10};
    for (int i = 0;i<5; i++)
    {
        if (ctx->notes[blackIndex[i]].isVisualPressed)
        {
            SDL_SetRenderDrawColor(ctx->renderer, RED_COLOR);
        } else
        {
            SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        }
        SDL_RenderFillRect(ctx->renderer, &ctx->notes[blackIndex[i]].rect);
        SDL_SetRenderDrawColor(ctx->renderer, BLACK_COLOR);
        SDL_RenderDrawRect(ctx->renderer, &ctx->notes[blackIndex[i]].rect);

        if (ctx->notes[blackIndex[i]].text != NULL) {
            SDL_Rect destRect;
            destRect.w = ctx->notes[blackIndex[i]].lableWidth;
            destRect.h = ctx->notes[blackIndex[i]].lableHeight;

            destRect.x = ctx->notes[blackIndex[i]].rect.x + (ctx->notes[blackIndex[i]].rect.w - destRect.w) / 2;
            destRect.y = ctx->notes[blackIndex[i]].rect.y + ctx->notes[blackIndex[i]].rect.h - destRect.h - 20;

            SDL_RenderCopy(ctx->renderer, ctx->notes[blackIndex[i]].text, NULL, &destRect);
        }
    }
}
void draw_octave_shift(AppContext* ctx)
{

    static SDL_Rect destRect;
    static int init;
    if (!init)
    {
        init = 1;
        char* str = "Octave";
        TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
        if (!font) return;
        octaveText = create_text_texture(ctx->renderer, font, str);
        SDL_QueryTexture(octaveText, NULL, NULL, &destRect.w, &destRect.h);
        TTF_CloseFont(font);
    }

    SDL_RenderCopy(ctx->renderer, octaveText, NULL, &destRect);

}

void render_frame(AppContext* ctx)
{
    SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 30, 255);
    SDL_RenderClear(ctx->renderer);
    draw_white(ctx);
    draw_black(ctx);
    draw_octave_shift(ctx);
    SDL_RenderPresent(ctx->renderer);
}



void destroy_gui(AppContext* ctx)
{
    destroy_notes(ctx);
    SDL_DestroyTexture(octaveText);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    SDL_Quit();
}