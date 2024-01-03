#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <lodepng.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "common.h"
#include "mathematics.h"
#include "rendering/renderer.h"
#include "hardware/input.h"
#include "time.hpp"

extern void game_init();
extern void game_update();
extern void game_render();


SDL_Window* setupWindow(){
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        std::cout << "SDL failed to init: " << SDL_GetError() << "\n";
        exit(1);
    }
    SDL_Window* window = SDL_CreateWindow("Native", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        std::cout << "SDL failed to create window: " << SDL_GetError() << "\n";
        exit(1);
    }

    return window;
}

int main(int argc, char** argv){
    SDL_Window* window = setupWindow();
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetWindowMinimumSize(window, FRAME_WIDTH, FRAME_HEIGHT);
    SDL_RenderSetLogicalSize(renderer, FRAME_WIDTH, FRAME_HEIGHT);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    // 4 bits per pixel
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, FRAME_WIDTH, FRAME_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);

    if(!surface){
        printf("Failed to create surface\n");
        return 1;
    }

    // SDL_SetRelativeMouseMode(SDL_TRUE);

    fixed t = 1.5fp;
    fixed d = 0.5fp;
    fixed e = sqrt(t);
    fixed f = cos(2);
    fixed g = sin(2);
    fixed h = abs(-t);
    fixed i = 3.5fp % 1.2fp;
    fixed j = floor(3.5fp);
    fixed k = ceil(3.5fp);
    fixed l = sqrt(0fp);
    fixed n = sqrt(5184fp);
    printf("%f\n", (float)(t / d));
    printf("%f\n", (float)d);
    printf("%f\n", (float)e);
    printf("%f\n", (float)f);
    printf("%f\n", (float)g);
    printf("%f\n", (float)h);
    printf("%f\n", (float)i);
    printf("%f\n", (float)j);
    printf("%f\n", (float)k);
    printf("%f\n", (float)l);
    printf("%f\n", (float)n);

    mat4f m = mat4f::translate(vec3f(1, 2, 3));
    mat4f m2 = mat4f::rotate(90, vec3f(0, 1, 0));
    vec4f p = vec4f(1, 0, 0, 1);

    Time::Init();
    Input::Init();
    Renderer::Init();
    game_init();

    while(true){
        Time::Update();
        Input::Poll();

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    SDL_DestroyWindow(window);
                    exit(0);
                    break;
                default:
                    Input::HandleEvent(&event);
            }
        }

        game_update();

        Renderer::Prepare();

        game_render();

        SDL_LockSurface(surface);
        static Color pixels[FRAME_WIDTH * FRAME_HEIGHT];
        for(int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++){
            pixels[i] = Color(Renderer::FrameBuffer[i]);
        }
        memcpy(surface->pixels, &pixels, FRAME_WIDTH * FRAME_HEIGHT * sizeof(Color));
        SDL_UnlockSurface(surface);


        SDL_RenderClear(renderer);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
    }

    // Color* pixels = new Color[FRAME_WIDTH * FRAME_HEIGHT];
    // for(int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++){
        // pixels[i] = Color(Renderer::FrameBuffer[i]);
    // }
    // unsigned error = lodepng::encode("test.png", (unsigned char*)pixels, FRAME_WIDTH, FRAME_HEIGHT);
    // if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    SDL_DestroyWindow(window);

    return 0;
}