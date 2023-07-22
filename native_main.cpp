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

    BoundingBox2D<float> bb2 = BoundingBox2D<float>(vec2f(0, 0), vec2f(1, 1));
    BoundingBox2D<float> bb3 = BoundingBox2D<float>(vec2f(-0.5, -0.5), vec2f(0.5, 0.5));
    BoundingBox2D<float> bb4 = bb2.Intersect(bb3);

    uint32_t* fb = new uint32_t[240 * 240];
    Frame frame = Frame(fb, vec2i16(240, 240));

    frame.Fill(0x0);
    frame.DrawLine(vec2i16(0, 0), vec2i16(240, 240), 0xFFFFFFFF);

    unsigned error = lodepng::encode("test.png", (unsigned char*)frame.Pixels, frame.Size.x, frame.Size.y);
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));

    vec2f v4 = vec2f(0.5, 0.5);

    v4 = vec2f(1.0f, 1.0f);

    printf("Vec: %f %f\n", v4.x, v4.y);

    printf("%d\n", bb2.Min.size);
    printf("Bounding box: %f %f %f %f\n", bb4.Min[0], bb4.Min[1], bb4.Max[0], bb4.Max[1]);

    return 0;
}