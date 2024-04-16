#pragma once

#include "rendering/shader.h"
#include "rendering/color.h"
#include "rendering/texture.h"
#include "rendering/renderer.h"

// extern Vertex cube_obj_vertices[36];
// extern uint32_t cube_obj_indices[36];

// extern Vertex suzanne_obj_vertices[2904];
// extern uint32_t suzanne_obj_indices[2904];

// extern Color16 test_png[920*900];

extern Color16 dirt_png[400*400];

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

Vertex pyramidVerts[] = {
    (Vertex){vec3f(0, 0, 0), vec3f(0), vec2f(0)},
    (Vertex){vec3f(1, 0, 0), vec3f(0), vec2f(0)},
    (Vertex){vec3f(0, 0, 1), vec3f(0), vec2f(0)},
    (Vertex){vec3f(1, 0, 1), vec3f(0), vec2f(0)},
    (Vertex){vec3f(0.5, 1, 0.5), vec3f(0), vec2f(0)},
};

uint32_t pyramidIndices[] = {
    4, 1, 0,
    4, 3, 1,
    4, 2, 3,
    4, 0, 2,
    1, 2, 0,
    1, 3, 2,
};

extern Vertex quadVerts[];
extern uint32_t quadIndices[];

void drawCubeTest(){
    Mesh cube = Mesh((Vertex*)&cubeVerts, 8, (uint32_t*)&cubeIndices, 12);
    Mesh pyramid = Mesh((Vertex*)&pyramidVerts, 5, (uint32_t*)&pyramidIndices, 6);
    Mesh quad = Mesh((Vertex*)&quadVerts, 4, (uint32_t*)&quadIndices, 2);
    // Mesh objtest = Mesh((Vertex*)&cube_obj_vertices, sizeof(cube_obj_vertices)/sizeof(Vertex), (uint32_t*)&cube_obj_indices, sizeof(cube_obj_indices)/sizeof(uint32_t)/3);
    // Mesh suzanne = Mesh((Vertex*)&suzanne_obj_vertices, sizeof(suzanne_obj_vertices)/sizeof(Vertex), (uint32_t*)&suzanne_obj_indices, sizeof(suzanne_obj_indices)/sizeof(uint32_t)/3);

    // Texture2D tex = Texture2D((Color16*)&test_png, 920, 900);
    // Texture2D dirt = Texture2D((Color16*)&dirt_png, 400, 400);

    Camera& main = Renderer::MainCamera;

    FlatLightingShader s = FlatLightingShader();
    FlatShader f = FlatShader();
    RainbowTestShader r = RainbowTestShader();
    TextureShader t = TextureShader();

    mat4f rot = mat4f::euler(vec3f(20, 160, 0));
    mat4f trans = mat4f::translate(vec3f(-1, 0, 6));
    mat4f trans2 = mat4f::translate(vec3f(1, 0, 6));
    mat4f trans3 = mat4f::translate(vec3f(-1, 0, 3));
    mat4f scale = mat4f::scale(vec3f(1, 1, 1));

    mat4f M = trans * rot * scale;
    mat4f M2 = trans2 * rot * scale;
    mat4f M3 = trans3 * rot * scale;

    Material mat = Material(s);
    ((FlatLightingShader::Parameters*)mat.Parameters)->LightDirection = vec3f::forward;
    ((FlatLightingShader::Parameters*)mat.Parameters)->LightColor = Color::Yellow;

    Material mat2 = Material(f);
    ((FlatShader::Parameters*)mat2.Parameters)->_Color = Color::Green;

    Material mat4 = Material(r);

    // Material mat5 = Material(t);
    // ((TextureShader::Parameters*)mat5.Parameters)->_Texture = &dirt;
    // ((TextureShader::Parameters*)mat5.Parameters)->TextureScale = vec2f(3);

    // get current time
    for(int i = 0; i < 1; i++){
        Renderer::Clear(Color::Red);
        Renderer::DrawMesh(cube, M2, mat);
        Renderer::DrawMesh(cube, M3, mat);
        Renderer::DrawMesh(pyramid, M, mat);
        // Renderer::Blit(tex, vec2i16(0));
    }

    vec3f forward = M2.col(2).xyz();
    vec3f up = M2.col(1).xyz();
    vec3f right = M2.col(0).xyz();
    vec3f pos = M2.col(3).xyz();
    Renderer::DrawLine(pos, pos + up, Color::Orange, 5);
    Renderer::DrawLine(pos, pos + right, Color::Cyan, 5);
    Renderer::DrawLine(pos, pos + forward, Color::White, 5);
}


#ifdef PLATFORM_PICO

#include "hardware/st7789.h"
#include "hardware/input.h"

void picoCubeTest(){
    Camera cam = Renderer::MainCamera;

    Mesh cube = Mesh((Vertex*)&cubeVerts, 8, (uint32_t*)&cubeIndices, 12);
    Mesh pyramid = Mesh((Vertex*)&pyramidVerts, 5, (uint32_t*)&pyramidIndices, 6);
    Mesh quad = Mesh((Vertex*)&quadVerts, 4, (uint32_t*)&quadIndices, 2);
    // Mesh suzanne = Mesh((Vertex*)&suzanne_obj_vertices, sizeof(suzanne_obj_vertices)/sizeof(Vertex), (uint32_t*)&suzanne_obj_indices, sizeof(suzanne_obj_indices)/sizeof(uint32_t)/3);

    Texture2D dirt = Texture2D((Color16*)&dirt_png, 400, 400);

    SmoothLightingShader s = SmoothLightingShader();
    Material lightingMat1 = Material(s);
    ((SmoothLightingShader::Parameters*)lightingMat1.Parameters)->LightDirection = vec3f::forward;
    ((SmoothLightingShader::Parameters*)lightingMat1.Parameters)->LightColor = Color::Yellow;

    Material lightingMat2 = Material(s);
    ((SmoothLightingShader::Parameters*)lightingMat2.Parameters)->LightDirection = vec3f::forward;
    ((SmoothLightingShader::Parameters*)lightingMat2.Parameters)->LightColor = Color::Red;

    FlatShader f = FlatShader();
    Material flatMat = Material(f);
    ((FlatShader::Parameters*)flatMat.Parameters)->_Color = Color::Green;

    

    mat4f trans = mat4f::translate(vec3f(0, 0, 4));
    mat4f scale = mat4f::scale(vec3f(1, 1, 1));

    fixed yaw, pitch;

    Color randomColors[] = {
        Color::Red,
        Color::Green,
        Color::Blue,
        Color::Yellow,
        Color::Magenta,
        Color::Cyan,
        Color::Orange,
        Color::Purple,
        Color::Pink,
        Color::Teal,
        Color::Brown,
    };

    Color clearColor = Color::Black;

    while(true){
        absolute_time_t time = get_absolute_time();

        Input::Poll();

        yaw = Input::GetAxis(Input::Axis::X) * 90;
        pitch = Input::GetAxis(Input::Axis::Y) * 90;

        // mat4f rot = mat4f::euler(vec3f(20, to_ms_since_boot(get_absolute_time())/10, 0));
        mat4f rot = mat4f::euler(vec3f(pitch, yaw+180, 0));
        mat4f M = trans * rot * scale;
        
        mat4f trans2 = mat4f::translate(vec3f(0, sin(to_ms_since_boot(get_absolute_time())/1000.0), 6));
        mat4f M2 = trans2 * rot * scale;

        mat4f rot2 = mat4f::euler(vec3f(45, 20, 0));
        mat4f trans3 = mat4f::translate(vec3f(0, -sin(to_ms_since_boot(get_absolute_time())/1000.0)+1, 10));
        mat4f M3 = trans3 * rot2 * scale;

        mat4f trans4 = mat4f::translate(vec3f(sin(to_ms_since_boot(get_absolute_time())/1000.0), 0, 10));
        mat4f M4 = trans4 * rot * scale;

        mat4f trans5 = mat4f::translate(vec3f(2, 0, (sin(to_ms_since_boot(get_absolute_time())/1000.0) + 1) * 5));
        mat4f M5 = trans5 * rot * scale;

        while(ST7789::IsFlipping());

        if(Input::GetButtonDown(Input::Button::Stick)){
            Renderer::ClearColor = randomColors[rand() % 11];
        }
        
        Renderer::Prepare();

        time = get_absolute_time();

        ((SmoothLightingShader::Parameters*)lightingMat1.Parameters)->LightDirection = vec3f::forward;
        // ((LightingShader::Parameters*)lightingMat2.Parameters)->ModelMatrix = &M3;

        // Renderer::DrawMesh(suzanne, M, lightingMat1);
        // Renderer::DrawMesh(cube, M2, lightingMat1);
        // Renderer::DrawMesh(cube, M3, lightingShader);
        // Renderer::DrawMesh(cube, M3, lightingMat2);
        
        // Renderer::DrawMesh(cube, M4, flatMat);

        // Renderer::DrawMesh(cube, M5, flatMat);
        
        // Renderer::DrawMesh(cube, M, lightingShader);

        // Renderer::Blit(dirt, vec2i16(0));

        ST7789::Flip((Color565*)&Renderer::FrameBuffer);

        printf("FPS: %f\n", 1.0 / (to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(time)) * 1000.0);
    }
}
#endif