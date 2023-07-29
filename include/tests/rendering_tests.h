#pragma once

#include <lodepng.h>

void rendering_test(){

    // output a black png

    unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (int i = 0; i < width * height * 4; i+=4) {
        image[i] = 255;
        image[i+1] = 0;
        image[i+2] = 0;
        image[i+3] = 255;
    }

    unsigned error = lodepng::encode("black.png", image, width, height);
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));
}

void drawCubeTest(){
    Vertex cubeVerts[] = {
        (Vertex){vec3f(-1, -1, -1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, -1, -1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(-1, 1, -1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, 1, -1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(-1, -1, 1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, -1, 1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(-1, 1, 1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, 1, 1), vec3f(0), vec2f(0)},
    };

    // counter clock wise polygons
    uint32_t cubeIndices[] = {
        0, 2, 1,
        1, 2, 3,
        1, 3, 5,
        5, 3, 7,
        5, 7, 4,
        4, 7, 6,
        4, 6, 0,
        0, 6, 2,
        4, 0, 5,
        5, 0, 1,
        2, 6, 3,
        3, 6, 7,
    };

    Mesh cube = Mesh((Vertex*)&cubeVerts, 8, (uint32_t*)&cubeIndices, 12);

    mat4f rot = getRotationalMatrix(vec3f(0, 45, 0));
    mat4f trans = mat4f::translate(vec3f(0, 0, 10));
    mat4f scale = mat4f::scale(vec3f(1, 1, 1));

    mat4f M = trans * rot * scale;

    Renderer::DrawMesh(cube, M);

}