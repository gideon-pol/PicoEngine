#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <lodepng.h>
#include <time.h>
// #include <SDL2/SDL.h>
#include "mathematics.h"
#include "rendering/renderer.h"
#include "tests/rendering_tests.h"
// #include "common.h"

#define TARGET_NATIVE

extern "C" const uint16_t main_font[];

// void setupWindow(vec2i16 resolution){
//     if(SDL_Init(SDL_INIT_VIDEO) == -1)
//     {
//         std::cout << "SDL failed to init: " << SDL_GetError() << "\n";
//         exit(1);
//     }
//     SDL_Window* window = SDL_CreateWindow("Native", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.x(), resolution.y(), SDL_WINDOW_SHOWN);
//     if(window == NULL)
//     {
//         std::cout << "SDL failed to create window: " << SDL_GetError() << "\n";
//         exit(1);
//     }

//     // Make the screen black, do not use surfaces
//     // Use software rendering ONLY
//     // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
//     // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//     // SDL_RenderClear(renderer);
//     // SDL_RenderPresent(renderer);


//     bool is_running = true;
//     SDL_Event event;
//     while (is_running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 is_running = false;
//             }
//         }
//         SDL_Delay(16);
//     }


//     SDL_DestroyWindow(window);
//     SDL_Quit();
// }

int main(int argc, char** argv){
    // setupWindow(vec2i16(1000));
    Renderer::Init(45, 0.1, 100);

    printf("Running rendering unit tests\n");

    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start);

    // printf("%x\n", Color(255, 0, 0, 128));
    // printf("%x\n", Color(0, 255, 0, 128));
    // printf("%x\n", Color(0, 0, 255, 128));

    drawCubeTest();


    fixed t = 1.5fp;
    fixed d = 0.5fp;
    fixed e = sqrt(t);
    fixed f = cos(2);
    fixed g = sin(2);
    fixed h = abs(-t);
    fixed i = mod(3.5fp, 1.2fp);
    fixed j = floor(3.5fp);
    fixed k = ceil(3.5fp);
    std::cout << (t / d) << "\n";
    std::cout << d << "\n";
    std::cout << e << "\n";
    std::cout << f << "\n";    
    std::cout << g << "\n";
    std::cout << h << "\n";
    std::cout << i << "\n";
    std::cout << j << "\n";
    std::cout << k << "\n";

    mat4f m = mat4f::translate(vec3f(1, 2, 3));
    mat4f m2 = mat4f::rotate(90, vec3f(0, 1, 0));
    vec4f p = vec4f(1, 0, 0, 1);

    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            printf("%f ", (float)m2(y)(x));
        }
        printf("\n");
    }
 
    p = m2 * p;
    printf("%f %f %f %f\n", (float)p.x(), (float)p.y(), (float)p.z(), (float)p.w());

    // clock_gettime(CLOCK_REALTIME, &now);
    // // calculate difference in seconds
    // double diff = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
    // printf("Drawing took %lf s\n", diff);

    Color* pixels = new Color[FRAME_WIDTH * FRAME_HEIGHT];
    for(int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++){
        pixels[i] = Color(Renderer::FrameBuffer[i]);
    }

    unsigned error = lodepng::encode("test.png", (unsigned char*)pixels, FRAME_WIDTH, FRAME_HEIGHT);
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    return 0;
}