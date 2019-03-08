#include <remotegl/remotegl-client.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <GL/gl.h>

#include "renderer.h"

static bool running = true;

int main(int argc, char **argv)
{
    // initialize the remotegl api
    rglInitialize(atoi(argv[1]));

    // setup window
    rglWindowHint(RGL_WINDOW_HINT_RESIZABLE, 0);
    rglWindowHint(RGL_WINDOW_HINT_RED_BITS, 8);
    rglWindowHint(RGL_WINDOW_HINT_GREEN_BITS, 8);
    rglWindowHint(RGL_WINDOW_HINT_BLUE_BITS, 8);
    rglWindowHint(RGL_WINDOW_HINT_ALPHA_BITS, 8);
    rglWindowHint(RGL_WINDOW_HINT_DEPTH_BITS, 8);
    rglWindowHint(RGL_WINDOW_HINT_STENCIL_BITS, 0);
    rglWindowHint(RGL_WINDOW_HINT_SAMPLES, 1);

    int windowId = rglWindowCreate("remotegl cube test", 1024, 768);

    struct renderer* renderer = renderer_create(windowId, 1024, 768);

    int counter = 0;
    while (running)
    {
        renderer_draw(renderer);
        renderer_swap(renderer);
    }

    renderer_release(renderer);
}
