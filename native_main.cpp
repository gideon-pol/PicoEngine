#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <barrier>
#include <condition_variable>
#include <lodepng.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "common.h"
#include "mathematics.h"
#include "rendering/renderer.h"
#include "rendering/postprocessing.h"
#include "hardware/input.h"
#include "time.hpp"

extern void game_init();
extern void game_update();
extern void game_mesh_render();
extern void game_ui_render();

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

std::mutex barrierStartMutex;
std::barrier renderStartBarrier{2};
std::mutex barrierDoneMutex;
std::barrier renderDoneBarrier{2};

void core1(){
    while(true){
        renderStartBarrier.arrive_and_wait();
        while(Renderer::Render());
        Renderer::Finish();
    }
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

    SDL_SetRelativeMouseMode(SDL_TRUE);

    Time::Init();
    Input::Init();
    Renderer::Init();

    game_init();

    std::thread secondThread(core1);

    while(true){
        Time::Tick();
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

        renderStartBarrier.arrive_and_wait();

        Time::Profiler::Enter("DrawMesh");
        game_mesh_render();
        while(Renderer::Render());
        Renderer::Finish();
        Time::Profiler::Exit("DrawMesh");

        Time::Profiler::Enter("PostProcessing");
        PostProcessing::Apply((Color565*)&Renderer::FrameBuffer, vec2i16(120, 120));
        Time::Profiler::Exit("PostProcessing");

        game_ui_render();

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

    SDL_DestroyWindow(window);

    return 0;
}