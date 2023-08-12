#pragma once
#include <stdint.h>

#include "common.h"

#define FIXED_32_FRAC_BITS 16
#define FIXED_32_FRAC_MASK ((1 << FIXED_32_FRAC_BITS) - 1)

struct fixed {
    int32_t value;

    FORCE_INLINE constexpr fixed() : value(0) {}
    constexpr fixed(const fixed& other) : value(other.value) {}

    constexpr fixed(int8_t value) : value((int32_t)value << FIXED_32_FRAC_BITS) {}
    constexpr fixed(uint8_t value) : value((int32_t)value << FIXED_32_FRAC_BITS) {}

    constexpr fixed(int16_t value) : value((int32_t)value << FIXED_32_FRAC_BITS) {}
    constexpr fixed(uint16_t value) : value((int32_t)value << FIXED_32_FRAC_BITS) {}

    constexpr fixed(int32_t value) : value(value << FIXED_32_FRAC_BITS) {}
    constexpr fixed(uint32_t value) : value(value << FIXED_32_FRAC_BITS) {}

    constexpr fixed(int value) : value(value << FIXED_32_FRAC_BITS) {} 
   
    constexpr fixed(int64_t value, int32_t shift) : value(value >> shift) {}
    constexpr fixed(int64_t value) : value(value << FIXED_32_FRAC_BITS) {}
    constexpr fixed(uint64_t value) : value(value << FIXED_32_FRAC_BITS) {}
    
    constexpr fixed(float value) : value(value * (1 << FIXED_32_FRAC_BITS)) {}
    constexpr fixed(double value) : value(value * (1 << FIXED_32_FRAC_BITS)) {}

    FORCE_INLINE constexpr fixed& operator=(const fixed& other) {
        value = other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator+(const fixed& other) const {
        return fixed(value + other.value, 0);
    }

    FORCE_INLINE constexpr fixed operator-(const fixed& other) const {
        return fixed(value - other.value, 0);
    }

    FORCE_INLINE constexpr fixed operator*(const fixed& other) const {
        return fixed((int64_t)value * (int64_t)other.value, FIXED_32_FRAC_BITS);
    }

    FORCE_INLINE constexpr fixed operator/(const fixed& other) const {
        return fixed(((int64_t)value << FIXED_32_FRAC_BITS) / (int64_t)other.value, 0);
    }

    FORCE_INLINE constexpr fixed operator-() const {
        return fixed(-value, 0);
    }

    FORCE_INLINE constexpr fixed operator+=(const fixed& other) {
        value += other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator-=(const fixed& other) {
        value -= other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator*=(const fixed& other) {
        value = (int64_t)value * (int64_t)other.value >> FIXED_32_FRAC_BITS;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator/=(const fixed& other) {
        value = (int64_t)value << FIXED_32_FRAC_BITS / (int64_t)other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator++() {
        value += 1 << FIXED_32_FRAC_BITS;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator--() {
        value -= 1 << FIXED_32_FRAC_BITS;
        return *this;
    }

    FORCE_INLINE constexpr bool operator==(const fixed& other) const {
        return value == other.value;
    }

    FORCE_INLINE constexpr bool operator!=(const fixed& other) const {
        return value != other.value;
    }

    FORCE_INLINE constexpr bool operator<(const fixed& other) const {
        return value < other.value;
    }

    FORCE_INLINE constexpr bool operator>(const fixed& other) const {
        return value > other.value;
    }

    FORCE_INLINE constexpr bool operator<=(const fixed& other) const {
        return value <= other.value;
    }

    FORCE_INLINE constexpr bool operator>=(const fixed& other) const {
        return value >= other.value;
    }

    // FORCE_INLINE friend std::ostream& operator<<(std::ostream& os, const fixed& s) {
    //     return os << s.value / (float)(1 << FIXED_32_FRAC_BITS);
    // }

    FORCE_INLINE explicit constexpr operator uint8_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator uint16_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator uint32_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE constexpr explicit operator uint64_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator int8_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator int16_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator int32_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE explicit constexpr operator int() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE constexpr explicit operator int64_t() const {
        return value >> FIXED_32_FRAC_BITS;
    }

    FORCE_INLINE constexpr explicit operator float() const {
        return value / (float)(1 << FIXED_32_FRAC_BITS);
    }

    FORCE_INLINE constexpr explicit operator double() const {
        return value / (double)(1 << FIXED_32_FRAC_BITS);
    }

private:
    // constexpr fixed(int64_t value, int64_t shift) : value(value >> shift) {}
};

constexpr fixed operator"" fp(long double value) {
    return fixed((float)value);
}

constexpr fixed operator"" fp(unsigned long long value) {
    return fixed((int32_t)value);
}

FORCE_INLINE constexpr fixed abs(fixed f) {
    return f < 0fp ? -f : f;
};

FORCE_INLINE constexpr fixed min(fixed a, fixed b) {
    return a < b ? a : b;
}

FORCE_INLINE constexpr fixed max(fixed a, fixed b) {
    return a > b ? a : b;
}

// FORCE_INLINE constexpr fixed sqrt(fixed f) {
//     if(f == 0fp) return 0fp;
//     // if(f == 1fp) return 1fp;

//     int32_t x = 1 << FIXED_32_FRAC_BITS;
//     int64_t n_one = f.value << FIXED_32_FRAC_BITS;

//     while(true){
//         int32_t old = x;
//         x = (x + n_one / x) >> 1;
//         printf("%d\n", abs(old - x));
//         if(abs(old - x) <= 1) return fixed(old, 0);
//     }
// }

FORCE_INLINE constexpr fixed sqrt(fixed f) {
    if(f == 0fp) return 0fp;
    fixed x = f;
    fixed x2 = x * x;
    fixed dx = (x2 - f) / (x * 2fp);
    while(abs(dx) > 0.01fp) {
        x -= dx;
        x2 = x * x;
        dx = (x2 - f) / (x * 2fp);
    }
    return x;
}

FORCE_INLINE constexpr fixed mod(fixed a, fixed b) {
    return fixed(a.value % b.value, 0);
}

FORCE_INLINE constexpr fixed floor(fixed f) {
    return fixed(f.value & ~FIXED_32_FRAC_MASK, 0);
}

FORCE_INLINE constexpr fixed ceil(fixed f) {
    return fixed((f.value & ~FIXED_32_FRAC_MASK) + (1 << FIXED_32_FRAC_BITS), 0);
}

// FORCE_INLINE constexpr fixed sin(fixed f) {
//     fixed x = f;
//     fixed x2 = x * x;
//     fixed dx = -x * x2 / 6;
//     while(abs(dx) > 0.0001fp) {
//         x += dx;
//         x2 = x * x;
//         dx = -x * x2 / 6;
//     }
//     return x;
// }

// FORCE_INLINE constexpr fixed cos(fixed f) {
//     fixed x = f;
//     fixed x2 = x * x;
//     fixed dx = -x2 * x2 / 24;
//     while(abs(dx) > 0.0001fp) {
//         x += dx;
//         x2 = x * x;
//         dx = -x2 * x2 / 24;
//     }
//     return x;
// }

// FORCE_INLINE constexpr fixed tan(fixed f) {
//     return sin(f) / cos(f);
// }

