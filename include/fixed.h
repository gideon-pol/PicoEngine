#pragma once
#include <stdint.h>
#include <iostream>

#include "common.h"

#define FIXED_32_SHIFT 10
#define FIXED_32_FRAC_MASK 0x3FF



struct fixed {
    int32_t value;

    FORCE_INLINE constexpr fixed() : value(0) {}
    constexpr fixed(const fixed& other) : value(other.value) {}

    constexpr fixed(int32_t value) : value(value << FIXED_32_SHIFT) {}
    constexpr fixed(int64_t value, int32_t shift) : value(value >> shift) {}
    constexpr fixed(int64_t value) : value(value << FIXED_32_SHIFT) {}
    constexpr fixed(uint32_t value) : value(value << FIXED_32_SHIFT) {}
    constexpr fixed(uint64_t value) : value(value << FIXED_32_SHIFT) {}
    
    constexpr fixed(float value) : value(value * (1 << FIXED_32_SHIFT)) {}
    constexpr fixed(double value) : value(value * (1 << FIXED_32_SHIFT)) {}

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
        return fixed((int64_t)value * (int64_t)other.value, FIXED_32_SHIFT);
    }

    FORCE_INLINE constexpr fixed operator/(const fixed& other) const {
        return fixed(((int64_t)value << FIXED_32_SHIFT) / (int64_t)other.value, 0);
    }

    FORCE_INLINE constexpr fixed operator-() const {
        return fixed(-value, 0);
    }

    // FORCE_INLINE constexpr fixed operator+(const int& other) const {
    //     return fixed(value + (other << FIXED_32_SHIFT));
    // }

    // FORCE_INLINE constexpr fixed operator-(const int& other) const {
    //     return fixed(value - (other << FIXED_32_SHIFT));
    // }

    // FORCE_INLINE constexpr fixed operator*(const int& other) const {
    //     return fixed((int64_t)value * (int64_t)(other << FIXED_32_SHIFT), FIXED_32_SHIFT);
    // }

    // FORCE_INLINE constexpr fixed operator/(const int& other) const {
    //     return fixed(((int64_t)value << FIXED_32_SHIFT) / (int64_t)other, 0);
    // }

    FORCE_INLINE constexpr fixed operator+=(const fixed& other) {
        value += other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator-=(const fixed& other) {
        value -= other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator*=(const fixed& other) {
        value = (int64_t)value * (int64_t)other.value >> FIXED_32_SHIFT;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator/=(const fixed& other) {
        value = (int64_t)value << FIXED_32_SHIFT / (int64_t)other.value;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator++() {
        value += 1 << FIXED_32_SHIFT;
        return *this;
    }

    FORCE_INLINE constexpr fixed operator--() {
        value -= 1 << FIXED_32_SHIFT;
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

    FORCE_INLINE friend std::ostream& operator<<(std::ostream& os, const fixed& s) {
        return os << s.value / (float)(1 << FIXED_32_SHIFT);
    }

    FORCE_INLINE constexpr explicit operator uint8_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator uint16_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator uint32_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator uint64_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator int8_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator int16_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator int32_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator int64_t() const {
        return value >> FIXED_32_SHIFT;
    }

    FORCE_INLINE constexpr explicit operator float() const {
        return value / (float)(1 << FIXED_32_SHIFT);
    }

    FORCE_INLINE constexpr explicit operator double() const {
        return value / (double)(1 << FIXED_32_SHIFT);
    }

private:
    // constexpr fixed(int64_t value, int64_t shift) : value(value >> shift) {}
};

fixed constexpr operator"" fp(long double value) {
    return fixed((float)value);
}

fixed constexpr operator"" fp(unsigned long long value) {
    return fixed((uint64_t)value);
}

FORCE_INLINE constexpr fixed abs(fixed f) {
    return f < 0 ? -f : f;
};

FORCE_INLINE constexpr fixed min(fixed a, fixed b) {
    return a < b ? a : b;
}

FORCE_INLINE constexpr fixed max(fixed a, fixed b) {
    return a > b ? a : b;
}

FORCE_INLINE constexpr fixed sqrt(fixed f) {
    int32_t x = 1 << FIXED_32_SHIFT;
    int64_t n_one = f.value << FIXED_32_SHIFT;

    while(true){
        int32_t old = x;
        x = (x + n_one / x) >> 1;
        if(x == old) return fixed(old, 0);
    }
}

FORCE_INLINE constexpr fixed mod(fixed a, fixed b) {
    return fixed(a.value % b.value, 0);
}

FORCE_INLINE constexpr fixed floor(fixed f) {
    return fixed(f.value & ~FIXED_32_FRAC_MASK, 0);
}

FORCE_INLINE constexpr fixed ceil(fixed f) {
    return fixed((f.value & ~FIXED_32_FRAC_MASK) + (1 << FIXED_32_SHIFT), 0);
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

