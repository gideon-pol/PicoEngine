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
    
    Renderer::Init(vec2i16(1000), 45, 0.1, 100);

   
    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start); 

    drawCubeTest();

    clock_gettime(CLOCK_REALTIME, &now);
    // calculate difference in seconds
    double diff = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
    printf("Drawing took %lf s\n", diff);

    unsigned error = lodepng::encode("test.png", (unsigned char*)Renderer::FrameBuffer, Renderer::Resolution.x(), Renderer::Resolution.y());
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    return 0;
}