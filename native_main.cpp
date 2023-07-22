#include <stdio.h>
#include <unistd.h>
#include <lodepng.h>
#include "mathematics.h"
#include "rendering/frame.h"
#include "tests/rendering_tests.h"

int main(char** argv, int argc){
    printf("Running rendering unit tests\n");
    rendering_test();

    vec2i16 v1 = vec2i16(1, 0);
    vec2i16 v2 = vec2i16(0, 1);
    vec2i16 v3 = vec2i16(0, 0);

    printf("Vec: %d %d\n", v1.x(), v1.y());
    printf("Vec: %d %d\n", v2.x(), v2.y());
    printf("Vec: %d %d\n", v3.x(), v3.y());

    BoundingBox2D<float> bb2 = BoundingBox2D<float>(vec2f(0, 0), vec2f(1, 1));
    BoundingBox2D<float> bb3 = BoundingBox2D<float>(vec2f(-0.5, -0.5), vec2f(0.5, 0.5));
    BoundingBox2D<float> bb4 = bb2.Intersect(bb3);

    uint32_t* fb = new uint32_t[240 * 240];
    Frame frame = Frame(fb, vec2i16(240, 240));

    frame.Fill(0xFF0000FF);

    frame.DrawLine(vec2i16(0, 0), vec2i16(120, 120), 0xFFFFFFFF);


    vec2test<2> d = vec2test<2>({1.0f, 1.0f});
    d[0] = 0;

    // frame.PutPixel(vec2i16(10, 10), 0xFFFFFFFF);
    printf("Frame size: %d %d\n", frame.Size.x(), frame.Size.y());
    printf("%ld\n", sizeof(vec2test<2>));
    printf("%ld\n", sizeof(vec2f));
    printf("%ld\n", sizeof(vec2i16));
    printf("%ld\n", sizeof(vec3f));
    printf("%ld\n", sizeof(vec4f));
    printf("%ld\n", sizeof(mat2));
    printf("%ld\n", sizeof(mat3));
    printf("%ld\n", sizeof(mat4));


    unsigned error = lodepng::encode("test.png", (unsigned char*)frame.Pixels, frame.Size.x(), frame.Size.y());
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    vec2f v4 = vec2f(0.5, 0.5);

    v4 = vec2f(1.0f, 1.0f);

    printf("Vec: %f %f\n", v4.x(), v4.y());

    printf("%d\n", bb2.Min.size());
    printf("Bounding box: %f %f %f %f\n", bb4.Min[0], bb4.Min[1], bb4.Max[0], bb4.Max[1]);

    return 0;
}