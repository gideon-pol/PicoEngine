#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define FORCE_INLINE __attribute__((always_inline)) inline
#define SCAST static_cast

// #define RENDER_DEBUG_FACE_NORMALS   1
// #define RENDER_DEBUG_WIREFRAME      1

#ifdef PLATFORM_PICO
#define FRAME_WIDTH                 120
#define FRAME_HEIGHT                120

#define ENABLE_OVERCLOCK            1
#endif

#ifdef PLATFORM_NATIVE
#define FRAME_WIDTH                 120
#define FRAME_HEIGHT                120
#endif
