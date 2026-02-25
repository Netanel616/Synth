#include <stdio.h>
#include "common.h"
#include "gui.h"
#include "input.h"
#include "audio.h"
#include "setup.h"

bool init(AppContext* ctx)
{
    setup_init(ctx);
    if (!init_gui(&ctx->gui, &ctx->state) || !init_audio(&ctx->audio, &ctx->state))
    {
        return false;
    }
    return true;
}

void cleanup(AppContext* ctx)
{
    destroy_audio(&ctx->audio);
    destroy_gui(&ctx->gui);
    setup_destroy(ctx);
}

int main(void)
{
    AppContext ctx = {0};
    if (!init(&ctx)) return 1;

    while (ctx.state.running)
    {
        handle_input(&ctx);
        render_frame(&ctx.gui, &ctx.state);
        SDL_Delay(16);
    }

    cleanup(&ctx);
    return 0;
}