#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <lodepng.h>
#include "mathematics.h"
#include "rendering/frame.h"
#include "tests/rendering_tests.h"

extern "C" const uint16_t main_font[];

int main(int argc, char** argv){
    printf("Running rendering unit tests\n");
    
    Font* font = new Font((uint8_t*)&main_font);
    uint32_t* fb = new uint32_t[240 * 240];
    Frame frame = Frame(fb, vec2i16(240));

    vec3f p1 = vec3f(00, -60, 0);
    vec3f p2 = vec3f(60, 60, 0);
    vec3f p3 = vec3f(-60, 60, 0);


    frame.Clear(0xFF0000FF);
    // frame.DrawLine(vec2i16(0), vec2i16(120), 0xFFFFFFFF);
    // frame.DrawText("Hello world!", vec2i16(0), font, 0xFFFFFFFF);

    mat4f m = mat4f::rotate(0.5f, vec3f(0,0,1));
    mat4f m2 = mat4f::translate(vec3f(100, 100, 0));
    mat4f m3 = mat4f::scale(vec3f(1, 1, 0));
    mat4f mm = m2 * m * m3;

    p1 = (mm * vec4f(p1, 1)).xyz();
    p2 = (mm * vec4f(p2, 1)).xyz();
    p3 = (mm * vec4f(p3, 1)).xyz();
    p3.z() = 0.1;

    frame.DrawTriangle(p1, p2, p3, 0xFFFFFFFF);

    p3.z() = 0;

    p1.z() = 0.1;

    p1 = (m * vec4f(p1, 1)).xyz();
    p2 = (m * vec4f(p2, 1)).xyz();
    p3 = (m * vec4f(p3, 1)).xyz();

    frame.DrawTriangle(p1, p2, p3, 0xFFFFFFFF);


    BoundingBox2D b1 = BoundingBox2D(vec2f(10, 10), vec2f(130, 130));
    BoundingBox2D b2 = BoundingBox2D(vec2f(110, 110), vec2f(230, 230));
    BoundingBox2D bbi = b2.Intersect(b1);

    frame.DrawBorder(b1, 1, 0xFF00FF00);
    frame.DrawBorder(b2, 1, 0xFF00FFFF);
    frame.DrawBox(bbi, 0xFF000000);

    printf("%ld\n", sizeof(vec2f));
    printf("%ld\n", sizeof(vec2i16));
    printf("%ld\n", sizeof(vec3f));
    printf("%ld\n", sizeof(vec4f));
    printf("%ld\n", sizeof(mat2));
    printf("%ld\n", sizeof(mat3));
    printf("%ld\n", sizeof(mat4f));

    // get current time
    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start);

    // while(true){
    //     frame.Fill(0xFF0000FF);

    //     clock_gettime(CLOCK_REALTIME, &now);

    //     // calculate difference in seconds
    //     double diff = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e12;
        

        // mat4f m = mat4f::rotate(diff, vec3f(0,0,1));
        // p1 = (m * vec4f(p1, 0)).xyz();
        // p2 = (m * vec4f(p2, 0.0f)).xyz();
        // p3 = (m * vec4f(p3, 0.0f)).xyz();

    //     // frame.DrawTriangle(p1, p2, p3, 0xFFFFFFFF);

    //     unsigned error = lodepng::encode("test.png", (unsigned char*)frame.Pixels, frame.Size.x(), frame.Size.y());
    //     if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    //     std::this_thread::sleep_for(std::chrono::milliseconds(2));
    // }

    unsigned error = lodepng::encode("test.png", (unsigned char*)frame.Pixels, frame.Size.x(), frame.Size.y());
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    return 0;
}