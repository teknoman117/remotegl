#ifndef REMOTEGL_TYPES_H_
#define REMOTEGL_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum _RGLenum
{
    RGL_WINDOW_HINT_RESIZABLE,
    RGL_WINDOW_HINT_RED_BITS,
    RGL_WINDOW_HINT_GREEN_BITS,
    RGL_WINDOW_HINT_BLUE_BITS,
    RGL_WINDOW_HINT_ALPHA_BITS,
    RGL_WINDOW_HINT_DEPTH_BITS,
    RGL_WINDOW_HINT_STENCIL_BITS,
    RGL_WINDOW_HINT_SAMPLES,
} RGLenum;

#ifdef __cplusplus
}
#endif

#endif /* REMOTEGL_TYPES_H_ */

