#pragma once

#include <stdint.h>
#include <math.h>
#include "common.h"

typedef uint16_t Color16;

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    constexpr Color() : r(0), g(0), b(0), a(255) {};
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {};

    FORCE_INLINE constexpr Color16 ToColor16() const {
        return (r >> 4) << 12 | (g >> 4) << 8 | (b >> 4) << 4 | (a >> 4);
    }

    FORCE_INLINE constexpr Color(Color16 c) : r((c >> 12) << 4), g((c >> 8) << 4), b((c >> 4) << 4), a((c >> 0) << 4) {};

    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;
    static const Color Orange;
    static const Color Purple;
    static const Color Pink;
    static const Color Teal;
    static const Color Grey;
    static const Color Brown;

    static constexpr Color FromHex(uint32_t hex) {
        return Color(
            (hex & 0xFF000000) >> 24,
            (hex & 0x00FF0000) >> 16,
            (hex & 0x0000FF00) >> 8,
            (hex & 0x000000FF));
    }

    static constexpr Color FromHSV(float h, float s, float v, float a = 1.0f) {
        float c = v * s;
        float x = c * (1 - abs(fmod(h / 60.0f, 2) - 1));
        float m = v - c;

        float r = 0, g = 0, b = 0;

        if (h >= 0 && h < 60) {
            r = c;
            g = x;
            b = 0;
        } else if (h >= 60 && h < 120) {
            r = x;
            g = c;
            b = 0;
        } else if (h >= 120 && h < 180) {
            r = 0;
            g = c;
            b = x;
        } else if (h >= 180 && h < 240) {
            r = 0;
            g = x;
            b = c;
        } else if (h >= 240 && h < 300) {
            r = x;
            g = 0;
            b = c;
        } else if (h >= 300 && h < 360) {
            r = c;
            g = 0;
            b = x;
        }

        return Color(
            (r + m) * 255,
            (g + m) * 255,
            (b + m) * 255,
            a * 255);
    }
} __attribute__((packed));