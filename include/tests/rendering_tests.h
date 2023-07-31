#pragma once

#include <lodepng.h>

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

    Camera* main = Renderer::MainCamera;

    main->SetPosition(vec3f(1, 1, 0));

    mat4f rot = getRotationalMatrix(vec3f(20, 20, 0));
    mat4f trans = mat4f::translate(vec3f(0, 0, 5));
    mat4f trans2 = mat4f::translate(vec3f(1, 0, 20));
    mat4f scale = mat4f::scale(vec3f(1, 1, 1));

    mat4f M = trans * rot * scale;
    mat4f M2 = trans2 * rot * scale;

    Renderer::Clear(0xFF0000FF);
    Renderer::DrawMesh(cube, M);
    Renderer::DrawMesh(cube, M2);

    BoundingVolume vol = cube.Volume;
    // Renderer::Debug::DrawVolume(vol, M, 0xFF000000);
    // Renderer::Debug::DrawVolume(vol, M2, 0xFF000000);

    vec3f forward = M(2).xyz();
    vec3f up = M(1).xyz();
    vec3f right = M(0).xyz();
    Renderer::DrawLine(vec3f(0, 0, 5), vec3f(0, 0, 5) + up, 0xFFF00FFFF);
    Renderer::DrawLine(vec3f(0, 0, 5), vec3f(0, 0, 5) + right, 0xFFFFFFF00);
    Renderer::DrawLine(vec3f(0, 0, 5), vec3f(0, 0, 5) + forward, 0xFFFFFFFFF);
}