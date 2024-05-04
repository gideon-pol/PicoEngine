#pragma once

#include <vector>
#include <iostream>
#include <cstring>
#include "rendering/color.h"

namespace PostProcessing {
    class Gradient {
    public:
        Gradient(Color* colors, int size) : colors(colors), size(size) {}

        Color Sample(float t) {
            int index = t * (size - 1);
            float alpha = t * (size - 1) - index;
            return Color::Lerp(colors[index], colors[index + 1], alpha);
        }

    private:
        Color* colors;
        int size;
    };

    class Layer {
    public:
        virtual void Apply(Color565* buffer, const vec2i16& size) = 0;
    };

    namespace {
        std::vector<Layer*> layers;

        void applyKernel(Color565* buffer, const vec2i16& size, const float* kernel, vec2i16 kernelSize) {
            Color* window = new Color[kernelSize(0) * kernelSize(0)];
            for (int y = 0; y < size(0); y++) {
                for (int x = 0; x < size(0); x++) {
                    // Copy the original values of the pixels in the current window to the window buffer
                    for (int ky = 0; ky < kernelSize(0); ky++) {
                        for (int kx = 0; kx < kernelSize(0); kx++) {
                            vec2i16 bufferPos = vec2i16(x, y) + vec2i16(kx, ky) - kernelSize / 2;
                            if (bufferPos(0) < 0 || bufferPos(0) >= size(0) || bufferPos(1) < 0 || bufferPos(1) >= size(0)) {
                                window[kx + ky * kernelSize(0)] = Color::Black;
                            } else {
                                window[kx + ky * kernelSize(0)] = Color(buffer[bufferPos(0) + bufferPos(1) * size(0)]);
                            }
                        }
                    }
                    // Apply the kernel operation using the original values in the window buffer
                    Color color = Color::Black;
                    for (int ky = 0; ky < kernelSize(0); ky++) {
                        for (int kx = 0; kx < kernelSize(0); kx++) {
                            Color windowColor = window[kx + ky * kernelSize(0)];

                            color = Color(
                                SCAST<uint8_t>(color.r + windowColor.r * kernel[kx + ky * kernelSize(0)]),
                                SCAST<uint8_t>(color.g + windowColor.g * kernel[kx + ky * kernelSize(0)]),
                                SCAST<uint8_t>(color.b + windowColor.b * kernel[kx + ky * kernelSize(0)]),
                                255
                            );
                        }
                    }
                    buffer[x + y * size(0)] = color.ToColor565();
                }
            }
            delete[] window;
        }

        void applyKernelFast(Color565* buffer, const vec2i16& size, const float* kernel, vec2i16 kernelSize) {
            for (int y = 0; y < size(0); y++) {
                for (int x = 0; x < size(0); x++) {
                    Color color = Color::Black;
                    for (int ky = 0; ky < kernelSize(0); ky++) {
                        for (int kx = 0; kx < kernelSize(0); kx++) {
                            vec2i16 bufferPos = vec2i16(x, y) + vec2i16(kx, ky) - kernelSize / 2;
                            if (bufferPos(0) >= 0 && bufferPos(0) < size(0) && bufferPos(1) >= 0 && bufferPos(1) < size(0)) {
                                Color windowColor = Color(buffer[bufferPos(0) + bufferPos(1) * size(0)]);
                                color = Color(
                                    SCAST<uint8_t>(color.r + windowColor.r * kernel[kx + ky * kernelSize(0)]),
                                    SCAST<uint8_t>(color.g + windowColor.g * kernel[kx + ky * kernelSize(0)]),
                                    SCAST<uint8_t>(color.b + windowColor.b * kernel[kx + ky * kernelSize(0)]),
                                    255
                                );
                            }
                        }
                    }
                    buffer[x + y * size(0)] = color.ToColor565();
                }
            }
        }
    };

    void Add(Layer* layer);
    void Remove(Layer* layer);
    void Apply(Color565* buffer, const vec2i16& size);

    class Bloom : public PostProcessing::Layer {
    public:
        void Apply(Color565* buffer, const vec2i16& size) override {
            float kernel[] = {
                0.0625f, 0.125f, 0.0625f,
                0.125f, 0.25f, 0.125f,
                0.0625f, 0.125f, 0.0625f
            };

            float kernelLarge[] = {
                0.003f, 0.013f, 0.022f, 0.013f, 0.003f,
                0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                0.022f, 0.097f, 0.159f, 0.097f, 0.022f,
                0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                0.003f, 0.013f, 0.022f, 0.013f, 0.003f
            };

            Color565 downsampled[(size(0) / 2) * (size(1) / 2)];
            downsample(buffer, (Color565*)&downsampled, size, vec2i16(size(0) / 2, size(1) / 2));
            applyKernel((Color565*)&downsampled, vec2i16(size(0) / 2, size(1) / 2), kernel, vec2i16(3, 3));

            Color565 downsampled2[(size(0) / 4) * (size(1) / 4)];
            downsample((Color565*)&downsampled, (Color565*)&downsampled2, vec2i16(size(0) / 2, size(1) / 2), vec2i16(size(0) / 4, size(1) / 4));
            applyKernel((Color565*)&downsampled2, vec2i16(size(0) / 4, size(1) / 4), kernelLarge, vec2i16(5, 5));

            combine(buffer, (Color565*)&downsampled, buffer, size, vec2i16(size(0) / 2, size(1) / 2), 3);
            combine(buffer, (Color565*)&downsampled2, buffer, size, vec2i16(size(0) / 4, size(1) / 4), 5);
        }

    private:
        void downsample(Color565* src, Color565* dst, vec2i16 srcSize, vec2i16 dstSize) {
            for (int y = 0; y < dstSize(1); y++) {
                for (int x = 0; x < dstSize(0); x++) {
                    Color color = Color::Black;
                    for (int dy = 0; dy < 2; dy++) {
                        for (int dx = 0; dx < 2; dx++) {
                            color = Color(
                                SCAST<uint8_t>(std::clamp(SCAST<int>(color.r) + Color(src[(x * 2 + dx) + (y * 2 + dy) * srcSize(0)]).r, 0, 255)),
                                SCAST<uint8_t>(std::clamp(SCAST<int>(color.g) + Color(src[(x * 2 + dx) + (y * 2 + dy) * srcSize(0)]).g, 0, 255)),
                                SCAST<uint8_t>(std::clamp(SCAST<int>(color.b) + Color(src[(x * 2 + dx) + (y * 2 + dy) * srcSize(0)]).b, 0, 255)),
                                255
                            );
                        }
                    }
                    dst[x + y * dstSize(0)] = color.ToColor565();
                }
            }
        }

        void combine(Color565* src1, Color565* src2, Color565* dst, vec2i16 srcSize, vec2i16 src2Size, int factor) {
            for (int y = 0; y < srcSize(1); y++) {
                for (int x = 0; x < srcSize(0); x++) {
                    // Scale x and y to fit within the src2Size
                    int scaledX = x * src2Size(0) / srcSize(0);
                    int scaledY = y * src2Size(1) / srcSize(1);

                    Color color1 = Color(src1[x + y * srcSize(0)]);
                    Color color2 = Color(src2[scaledX + scaledY * src2Size(0)]);

                    uint8_t r = std::clamp(SCAST<int>(color1.r) + color2.r / factor, 0, 255);
                    uint8_t g = std::clamp(SCAST<int>(color1.g) + color2.g / factor, 0, 255);
                    uint8_t b = std::clamp(SCAST<int>(color1.b) + color2.b / factor, 0, 255);
                    dst[x + y * srcSize(0)] = Color(r, g, b, 255).ToColor565();
                }
            }
        }
    };

    class Monochrome : public PostProcessing::Layer {
        void Apply(Color565* buffer, const vec2i16& size) override {
            for(int i = 0; i < size(0) * size(1); i++){
                Color color = Color(buffer[i]);
                uint8_t average = (color.r + color.g + color.b) / 3;
                buffer[i] = Color(average, average, average, 255).ToColor565();
            }
        }
    };

    class Colorize : public PostProcessing::Layer {
    public:
        Colorize(Color* colors, int size) : gradient(colors, size) {}

        void Apply(Color565* buffer, const vec2i16& size) override {
            for(int i = 0; i < size(0) * size(1); i++){
                Color color = Color(buffer[i]);
                float brightness = (color.r + color.g + color.b) / 3 / 255.0f;
                buffer[i] = gradient.Sample(brightness).ToColor565();
            }
        }

    private:
        Gradient gradient;
    };
};