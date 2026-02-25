//
// Created by נתנאל סרי on 23/02/2026.
//

#ifndef SYNTH1_GUI_H

#define SYNTH1_GUI_H
#include "common.h"

bool init_gui(GuiContext* ctx, SharedState* state);
void render_frame(GuiContext* ctx, SharedState* state);
void destroy_gui(GuiContext* ctx);

#endif //SYNTH1_GUI_H