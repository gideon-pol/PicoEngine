#pragma once

#include <lodepng.h>
#include "rendering/shader.h"

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

    Vertex pyramidVerts[] = {
        (Vertex){vec3f(0, 0, 0), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, 0, 0), vec3f(0), vec2f(0)},
        (Vertex){vec3f(0, 0, 1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(1, 0, 1), vec3f(0), vec2f(0)},
        (Vertex){vec3f(0.5, 1, 0.5), vec3f(0), vec2f(0)},
    };

    // clock wise polygons
    uint32_t pyramidIndices[] = {
        0, 1, 4,
        1, 3, 4,
        3, 2, 4,
        2, 0, 4,
        0, 2, 1,
        2, 3, 1,
    };

    Mesh pyramid = Mesh((Vertex*)&pyramidVerts, 5, (uint32_t*)&pyramidIndices, 6);

    Camera* main = Renderer::MainCamera;

    LightingShader s = LightingShader();
    WireFrameShader w = WireFrameShader();
    FlatShader f = FlatShader();
    RainbowTestShader r = RainbowTestShader();

    Material mat = Material(s);
    ((LightingShader::Parameters*)mat.Parameters)->LightPosition = -vec3f::up;
    ((LightingShader::Parameters*)mat.Parameters)->LightColor = Color::Yellow;

    Material mat2 = Material(f);
    ((FlatShader::Parameters*)mat2.Parameters)->_Color = Color::Green;

    Material mat3 = Material(w);
    ((WireFrameShader::Parameters*)mat3.Parameters)->_Color = Color::Cyan;

    Material mat4 = Material(r);

    mat4f rot = getRotationalMatrix(vec3f(-20, 20, 0));
    mat4f trans = mat4f::translate(vec3f(-1, 0, 10));
    mat4f trans2 = mat4f::translate(vec3f(5, 0, 20));
    mat4f scale = mat4f::scale(vec3f(1, 1, 1));

    mat4f M = trans * rot * scale;
    mat4f M2 = trans2 * rot * scale;

    for(int i = 0; i < 10000; i++){
        Renderer::Clear(Color::Red);
        Renderer::DrawMesh(pyramid, M, mat4);
        Renderer::DrawMesh(cube, M2, mat4);
    }
    

    BoundingVolume vol1 = cube.Volume;
    BoundingVolume vol2 = pyramid.Volume;
    // Renderer::Debug::DrawVolume(vol2, M, Color::Black);
    // Renderer::Debug::DrawVolume(vol1, M2, Color::Black);

    vec3f forward = M(2).xyz();
    vec3f up = M(1).xyz();
    vec3f right = M(0).xyz();
    Renderer::DrawLine(vec3f(-1, 0, 10), vec3f(-1, 0, 10) + up, Color::Orange);
    Renderer::DrawLine(vec3f(-1, 0, 10), vec3f(-1, 0, 10) + right, Color::Cyan);
    Renderer::DrawLine(vec3f(-1, 0, 10), vec3f(-1, 0, 10) + forward, Color::White);
}