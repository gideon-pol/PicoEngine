#pragma once

#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "mathematics.h"
#include "rendering/color.h"

enum SampleMode { Nearest, Bilinear };

class Texture2D {
public:
    uint32_t Width;
    uint32_t Height;
    Color16* Data;
    SampleMode Mode;

    Texture2D(Color16* data, uint32_t width, uint32_t height, SampleMode mode = SampleMode::Nearest){
        Width = width;
        Height = height;
        Data = data;
        Mode = mode;
    }

    FORCE_INLINE constexpr Color GetPixel(const vec2i16 pos) const {
        return Color(Data[pos(1) * Width + pos(0)]);
    }

    FORCE_INLINE constexpr Color Sample(const vec2f uv) const {
        if(Mode == SampleMode::Nearest){
            int x = SCAST<int>(mod(abs(uv(0)), 1) * fixed(Width));
            int y = SCAST<int>(mod(abs(uv(1)), 1) * fixed(Height));
            return Data[y * Width + x];

        // This mode is probably mostly useless as colors are reduced to 16 bits anyway
        } else if(Mode == SampleMode::Bilinear){
            fixed x = uv(0) * fixed(Width);
            fixed y = uv(1) * fixed(Height);

            int x0 = SCAST<int>(x);
            int y0 = SCAST<int>(y);
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            fixed xWeight = x - x0;
            fixed yWeight = y - y0;

            Color c00 = Color(Data[y0 * Width + x0]);
            Color c01 = Color(Data[y0 * Width + x1]);
            Color c10 = Color(Data[y1 * Width + x0]);
            Color c11 = Color(Data[y1 * Width + x1]);

            Color c0 = Color::Lerp(c00, c01, xWeight);
            Color c1 = Color::Lerp(c10, c11, xWeight);

            return Color::Lerp(c0, c1, yWeight);
        }
    }
};