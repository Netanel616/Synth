#include <stdio.h>
#include "common.h"
#include "gui.h"
#include "input.h"
#include "audio.h"
#include "setup.h"
bool init(AppContext* ctx)
{
    setup_init(ctx);
    if (!init_gui(ctx)|| !init_audio(ctx))
    //if (!init_gui(ctx))
    {
        return false;
    }
    return true;
}
void cleanup(AppContext* ctx)
{
    //destroy_audio(ctx);
    destroy_gui(ctx);
    setup_destroy(ctx);
}
int main(void)
{
    AppContext ctx = {0};
    if (!init(&ctx)) return 1;

    while (ctx.running)
    {
        handle_input(&ctx);
        render_frame(&ctx);
        SDL_Delay(16);
    }

    cleanup(&ctx);
    return 0;
}