#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <lodepng.h>
#include "mathematics.h"
#include "rendering/renderer.h"
#include "tests/rendering_tests.h"

extern "C" const uint16_t main_font[];

int main(int argc, char** argv){
    printf("Running rendering unit tests\n");
    
    Renderer::Init(vec2i16(240), 45, 0.1, 100);

    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start); 

    drawCubeTest();

    clock_gettime(CLOCK_REALTIME, &now);
    // calculate difference in seconds
    double diff = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
    printf("Drawing took %lf s\n", diff);

    Color* pixels = new Color[Renderer::Resolution.x() * Renderer::Resolution.y()];
    for(int i = 0; i < Renderer::Resolution.x() * Renderer::Resolution.y(); i++){
        pixels[i] = Color(Renderer::FrameBuffer[i]);
    }

    unsigned error = lodepng::encode("test.png", (unsigned char*)pixels, Renderer::Resolution.x(), Renderer::Resolution.y());
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    return 0;
}