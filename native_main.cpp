#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <lodepng.h>
#include "mathematics.h"
#include "rendering/frame.h"
#include "rendering/renderer.h"
#include "rendering/mesh.h"
#include "tests/rendering_tests.h"

extern "C" const uint16_t main_font[];

int main(int argc, char** argv){
    printf("Running rendering unit tests\n");
    
    Font* font = new Font((uint8_t*)&main_font);

    Renderer::Init(vec2i16(1920), 0xFF0000FF, 45, 0.1, 100);

    vec3f p1 = vec3f(0, 1, 0);
    vec3f p2 = vec3f(1, -1, 0);
    vec3f p3 = vec3f(-1, -1, 0);

    Vertex verts[] = {
        (Vertex){p1, vec3f(0), vec2f(0)},
        (Vertex){p2, vec3f(0), vec2f(0)},
        (Vertex){p3, vec3f(0), vec2f(0)},
    };
    uint32_t indices[] = {0, 1, 2};
    Mesh mesh = Mesh((Vertex*)&verts, 3, (uint32_t*)&indices, 3);

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

    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start); 

    drawCubeTest();

    clock_gettime(CLOCK_REALTIME, &now);
    // calculate difference in seconds
    double diff = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
    printf("Drawing took %lf s\n", diff);

    // Renderer::FrameBuffer->FlipY();
    unsigned error = lodepng::encode("test.png", (unsigned char*)Renderer::FrameBuffer->Pixels, Renderer::FrameBuffer->Size.x(), Renderer::FrameBuffer->Size.y());
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    return 0;
}