// Prevent SDL from redefining main
#define SDL_MAIN_HANDLED

// Include the implementation of acutest
#define ACUTEST_IMPLEMENTATION_
#include "acutest.h"

#include "../common.h"

// We include the C file directly to test static functions
#include "../effects.c"

void test_distortion(void) {
    TEST_CHECK(apply_distortion(0.0f, 0.5f) == 0.0f);

    float out = apply_distortion(0.5f, 0.0f);
    TEST_CHECK(out > 0.3f && out < 0.35f);

    out = apply_distortion(0.5f, 1.0f);
    TEST_CHECK(out > 0.9f);

    float pos = apply_distortion(0.5f, 0.5f);
    float neg = apply_distortion(-0.5f, 0.5f);
    TEST_CHECK(fabsf(pos + neg) < 0.0001f);
}

void test_filter(void) {
    AppContext ctx;
    // Initialize the state pointer which is usually done in init_audio
    ctx.audio.state = &ctx.state;

    init_effects(&ctx);

    ctx.state.effects.filterEnabled = true;
    ctx.state.effects.filterCutoff = 1.0f;
    ctx.state.effects.filterResonance = 0.0f;

    float left = 0.5f;
    float right = 0.5f;

    for(int i=0; i<100; i++) {
        apply_filter(&ctx.audio, &left, &right);
    }
    TEST_CHECK(left > 0.1f);

    ctx.state.effects.filterCutoff = 0.0f;
    ctx.audio.lpfLow[0] = 0.0f;
    ctx.audio.lpfLow[1] = 0.0f;
    ctx.audio.lpfBand[0] = 0.0f;
    ctx.audio.lpfBand[1] = 0.0f;

    left = 0.5f;
    right = 0.5f;

    for(int i=0; i<100; i++) {
        apply_filter(&ctx.audio, &left, &right);
    }
    TEST_CHECK(left < 0.1f);

    free(ctx.audio.delayBuffer);
}

void test_delay(void) {
    AppContext ctx;
    // Initialize the state pointer which is usually done in init_audio
    ctx.audio.state = &ctx.state;

    init_effects(&ctx);

    ctx.state.bpm = 120.0f;
    ctx.state.effects.delayEnabled = true;
    ctx.state.effects.delayNoteDiv = 4;
    ctx.state.effects.delayFeedback = 0.0f;
    ctx.state.effects.delayMix = 1.0f;

    int expectedOffset = 22050 * 2;

    float left = 1.0f;
    float right = -1.0f;

    apply_delay(&ctx.audio, &left, &right);

    int writeIndex = ctx.audio.delayBufferIndex - 2;
    if (writeIndex < 0) writeIndex += ctx.audio.delayBufferSize;

    TEST_CHECK(ctx.audio.delayBuffer[writeIndex] == 1.0f);
    TEST_CHECK(ctx.audio.delayBuffer[writeIndex+1] == -1.0f);

    ctx.audio.delayBufferIndex = (writeIndex + expectedOffset) % ctx.audio.delayBufferSize;

    left = 0.0f;
    right = 0.0f;

    apply_delay(&ctx.audio, &left, &right);

    TEST_CHECK(left == 1.0f);
    TEST_CHECK(right == -1.0f);

    free(ctx.audio.delayBuffer);
}

TEST_LIST = {
    { test_distortion ,"distortion" },
    { test_filter,"filter" },
    { test_delay, "delay"  },
    { NULL, NULL }
};