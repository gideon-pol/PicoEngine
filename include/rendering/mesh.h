#pragma once

#include <stdint.h>
#include "maths.h"


typedef struct {
    vec3f position;
    vec3f normal;
    vec2 uv;
} Vertex;

typedef struct {
    Vertex* Vertices;
    uint32_t* Indices;
} Mesh;