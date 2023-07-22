#pragma once

#include <lodepng.h>

void rendering_test(){

    // output a black png

    unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (int i = 0; i < width * height * 4; i+=4) {
        image[i] = 255;
        image[i+1] = 0;
        image[i+2] = 0;
        image[i+3] = 255;
    }

    unsigned error = lodepng::encode("black.png", image, width, height);
    if(error) printf("encoder error %d: %s", error, lodepng_error_text(error));
}