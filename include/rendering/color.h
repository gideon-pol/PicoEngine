#pragma once

#include <stdint.h>
#include <math.h>
#include "common.h"
#include "mathematics.h"

// Currently, the entire render pipeline may deal with 32-bit colors.
// The framebuffers however hold 16-bit colors to conserve space.
// In terms of computational speed, there is little difference. Memory requirements may vary though.

typedef uint16_t Color16;

struct Color4444 {
    uint8_t g : 4;
    uint8_t b : 4;
    uint8_t a : 4;
    uint8_t r : 4;
} __attribute__((packed));

// struct Color565 {
//     uint8_t g : 6;
//     uint8_t r : 5;
//     uint8_t b : 5;
// } __attribute__((packed));

// TODO: This is a hacky solution to the problem of the 565 color format.
//       A better way would be to sync the display by sending 3 dummy bits first.
struct Color565 {
    uint8_t g1 : 3;
    uint8_t r : 5;
    uint8_t b : 5;
    uint8_t g2 : 3;
} __attribute__((packed));

struct Color332 {
    uint8_t b : 2;
    uint8_t g : 3;
    uint8_t r : 3;
} __attribute__((packed));

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    constexpr Color() : r(0), g(0), b(0), a(255) {};
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {};

    FORCE_INLINE constexpr Color16 ToColor16() const {
        return (g & 0xf0) << 8 | (b & 0xf0) << 4 | (a & 0xf0) | (r >> 4);
    }

    FORCE_INLINE constexpr Color4444 ToColor4444() const {
        return (Color4444){ .g = g >> 4, .b = b >> 4, .a = a >> 4, .r = r >> 4 };
    }

    // FORCE_INLINE constexpr Color565 ToColor565() const {
    //     return (Color565){ .g = g >> 2, .r = r >> 3, .b = b >> 3 };
    // }

    FORCE_INLINE constexpr Color565 ToColor565() const {
        return (Color565){ .g1 = g >> 5, .r = r >> 3, .b = b >> 3, .g2 = g >> 2 };
    }

    FORCE_INLINE constexpr Color332 ToColor332(){
        return (Color332){ .b = b >> 6, .g = g >> 5, .r = r >> 5};
    }

    FORCE_INLINE constexpr Color(Color16 c) :
        r((c & 0xf) << 4), g((c & 0xf000) >> 8), b((c & 0xf00) >> 4), a(c & 0xf0) {};

    FORCE_INLINE constexpr Color(Color4444 c) :
        r(c.r << 4), g(c.g << 4), b(c.b << 4), a(c.a << 4) {};

    // FORCE_INLINE constexpr Color(Color565 c) :
    //     r(c.r << 3), g(c.g << 2), b(c.b << 3), a(255) {};

    FORCE_INLINE constexpr Color(Color565 c) :
        r(c.r << 3), g((c.g1 << 5) | (c.g2 << 2)), b(c.b << 3), a(255) {};

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

    static constexpr Color FromHSV(fixed h, fixed s, fixed v, fixed a = 1.0f) {
        fixed c = v * s;
        fixed x = c * (1fp - abs((h / 60.0f) % 2 - 1));
        fixed m = v - c;

        fixed r = 0, g = 0, b = 0;

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
            SCAST<uint8_t>((r + m) * 255),
            SCAST<uint8_t>((g + m) * 255),
            SCAST<uint8_t>((b + m) * 255),
            SCAST<uint8_t>(a * 255));
    }

    FORCE_INLINE static constexpr Color Lerp(Color a, Color b, fixed t) {
        return Color(
            SCAST<uint8_t>(fixed(a.r) + fixed(b.r - a.r) * t),
            SCAST<uint8_t>(fixed(a.g) + fixed(b.g - a.g) * t),
            SCAST<uint8_t>(fixed(a.b) + fixed(b.b - a.b) * t),
            SCAST<uint8_t>(fixed(a.a) + fixed(b.a - a.a) * t));
    }
} __attribute__((packed));