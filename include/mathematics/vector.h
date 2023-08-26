#pragma once

#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "common.h"
#include "fixed.h"

#pragma pack(push, 1)
template<typename T, int C>
struct vec {
    T data[C]{};

    constexpr vec() {
        for (int i = 0; i < C; i++) {
            data[i] = SCAST<T>(0);
        }
    }

    constexpr vec(T val){
        for (int i = 0; i < C; i++) {
            data[i] = val;
        }
    };

    constexpr vec(const vec<T, C>& other) {
        for (int i = 0; i < C; i++) {
            data[i] = other(i);
        }
    };

    constexpr vec(const T (&arr)[C]) {
        for (int i = 0; i < C; i++) {
            data[i] = arr[i];
        }
    };

    FORCE_INLINE constexpr T& operator[](int i) {
        return data[i];
    };

    FORCE_INLINE constexpr T operator()(int i) const {
        return data[i];
    };

    constexpr vec<T, C>& operator=(const vec<T, C>& other) {
        for (int i = 0; i < C; i++) {
            data[i] = other(i);
        }
        return *this;
    };

    constexpr T operator*(const vec<T, C>& other) const {
        T result = SCAST<T>(0);
        for (int i = 0; i < C; i++) {
            result += data[i] * other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator*(const T& other) const {
        vec<T, C> result = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] * other;
        }
        return result;
    };

    constexpr vec<T, C> operator/(const vec<T, C>& other) const {
        vec<T, C> result = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] / other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator/(const T& other) const {
        vec<T, C> result = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] / other;
        }
        return result;
    };
    
    constexpr vec<T, C> operator+(const vec<T, C>& other) const {
        vec<T, C> result  = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] + other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator-(const vec<T, C>& other) const {
        vec<T, C> result = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] - other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator-() const {
        vec<T, C> result = vec<T, C>();
        for (int i = 0; i < C; i++) {
            result[i] = -data[i];
        }
        return result;
    };

    constexpr vec<T, C>& operator+=(const vec<T, C>& other) {
        for (int i = 0; i < C; i++) {
            data[i] += other(i);
        }
        return *this;
    };

    constexpr vec<T, C>& operator-=(const vec<T, C>& other) {
        for (int i = 0; i < C; i++) {
            data[i] -= other(i);
        }
        return *this;
    };

    constexpr vec<T, C>& operator*=(const T& other) {
        for (int i = 0; i < C; i++) {
            data[i] *= other;
        }
        return *this;
    };

    constexpr bool operator==(const vec<T, C>& other) const {
        for (int i = 0; i < C; i++) {
            if (data[i] != other(i)) {
                return false;
            }
        }
        return true;
    };

    FORCE_INLINE constexpr uint32_t size() const {
        return C;
    };

    constexpr T magnitude() const {
        // This calculation is done using floats, despite the loss of performance.
        // Results tend to get so big that they overflow any other type.

        float result = 0;
        for (int i = 0; i < C; i++) {
            result += SCAST<float>(data[i]) * SCAST<float>(data[i]);
        }
        return SCAST<T>(sqrt(result));
    };

    constexpr vec<T, C> normalize() const {
        vec<T, C> result = vec<T, C>();
        T mag = magnitude();

        if(mag == SCAST<T>(0)) return vec<T, C>();

        for (int i = 0; i < C; i++) {
            result[i] = data[i] / mag;
        }
        return result;
    };

    constexpr T dot(const vec<T, C>& other) const {
        T result = 0;
        for (int i = 0; i < C; i++) {
            result += data[i] * other(i);
        }
        return result;
    };
};

template<typename T>
struct vec2 : public vec<T, 2> {
    using vec<T, 2>::data;
    using vec<T, 2>::vec;
    constexpr vec2(T x, T y) : vec<T, 2>({x, y}) {};
    constexpr vec2(const vec<T, 2>& other) : vec<T, 2>(other) {};

    template<typename U>
    constexpr vec2(const vec<U, 2>& other) : vec<T, 2>({
        SCAST<T>(other(0)), SCAST<T>(other(1))}) {};

    FORCE_INLINE constexpr vec2<T> operator+(const vec<T, 2>& other) const { return vec<T, 2>::operator+(other); }
    FORCE_INLINE constexpr vec2<T> operator-(const vec<T, 2>& other) const { return vec<T, 2>::operator-(other); }
    FORCE_INLINE constexpr vec2<T> operator-() const { return vec<T, 2>::operator-(); }
    FORCE_INLINE constexpr vec2<T> operator*(const T& other) const { return vec<T, 2>::operator*(other); }
    FORCE_INLINE constexpr T operator*(const vec<T, 2>& other) const { return vec<T, 2>::operator*(other); }
    FORCE_INLINE constexpr vec2<T> operator/(const vec<T, 2>& other) const { return vec<T, 2>::operator/(other); }
    FORCE_INLINE constexpr vec2<T> operator/(const T& other) const { return vec<T, 2>::operator/(other); }
    FORCE_INLINE constexpr vec2<T> normalize() const { return vec<T, 2>::normalize(); }

    FORCE_INLINE constexpr T& x() { return data[0]; };
    FORCE_INLINE constexpr T& y() { return data[1]; };
}; 

template<typename T>
struct vec3 : public vec<T, 3> {
    using vec<T, 3>::data;
    using vec<T, 3>::vec;
    constexpr vec3(T x, T y, T z) : vec<T, 3>({x, y, z}) {};
    constexpr vec3(const vec<T, 3>& other) : vec<T, 3>(other) {};
    constexpr vec3(const vec2<T>& other, T z) : vec<T, 3>({other(0), other(1), z}) {};
    
    template <typename U>
    constexpr vec3(const vec<U, 3>& other) : vec<T, 3>({
        SCAST<T>(other(0)), SCAST<T>(other(1)), SCAST<T>(other(2))}) {};

    constexpr vec3 cross(const vec3<T>& other) const {
        vec3<T> result = vec3<T>(0);
        result[0] = data[1] * other(2) - data[2] * other(1);
        result[1] = data[2] * other(0) - data[0] * other(2);
        result[2] = data[0] * other(1) - data[1] * other(0);
        return result;
    };

    FORCE_INLINE constexpr vec3<T> operator+(const vec<T, 3>& other) const { return vec<T, 3>::operator+(other); }
    FORCE_INLINE constexpr vec3<T> operator-(const vec<T, 3>& other) const { return vec<T, 3>::operator-(other); }
    FORCE_INLINE constexpr vec3<T> operator-() const { return vec<T, 3>::operator-(); }
    FORCE_INLINE constexpr vec3<T> operator*(const T& other) const { return vec<T, 3>::operator*(other); }
    FORCE_INLINE constexpr T operator*(const vec<T, 3>& other) const { return vec<T, 3>::operator*(other); }
    FORCE_INLINE constexpr vec3<T> operator/(const vec<T, 3>& other) const { return vec<T, 3>::operator/(other); }
    FORCE_INLINE constexpr vec3<T> operator/(const T& other) const { return vec<T, 3>::operator/(other); }
    FORCE_INLINE constexpr vec3<T> normalize() const { return vec<T, 3>::normalize(); }

    FORCE_INLINE constexpr T& x() { return data[0]; };
    FORCE_INLINE constexpr T& y() { return data[1]; };
    FORCE_INLINE constexpr T& z() { return data[2]; };
    FORCE_INLINE constexpr vec2<T> xy() const { return vec2<T>(data[0], data[1]); };
    FORCE_INLINE constexpr vec2<T> xz() const { return vec2<T>(data[0], data[2]); };
    FORCE_INLINE constexpr vec2<T> yz() const { return vec2<T>(data[1], data[2]); };
};

template<typename T>
struct vec4 : public vec<T, 4> {
    using vec<T, 4>::data;
    using vec<T, 4>::vec;
    constexpr vec4(T x, T y, T z, T w) : vec<T, 4>({x, y, z, w}) {};
    constexpr vec4(const vec<T, 4>& other) : vec<T, 4>(other) { };
    constexpr vec4(const vec3<T>& other, T w) : vec<T, 4>({other(0), other(1), other(2), w}) {};

    template <typename U>
    constexpr vec4(const vec<U, 4>& other) : vec<T, 4>({
        SCAST<T>(other(0)), SCAST<T>(other(1)), SCAST<T>(other(2)), SCAST<T>(other(3))}) {};

    FORCE_INLINE constexpr vec4<T> operator+(const vec<T, 4>& other) const { return vec<T, 4>::operator+(other); }
    FORCE_INLINE constexpr vec4<T> operator-(const vec<T, 4>& other) const { return vec<T, 4>::operator-(other); }
    FORCE_INLINE constexpr vec4<T> operator-() const { return vec<T, 4>::operator-(); }
    FORCE_INLINE constexpr vec4<T> operator*(const T& other) const { return vec<T, 4>::operator*(other); }
    FORCE_INLINE constexpr T operator*(const vec<T, 4>& other) const { return vec<T, 4>::operator*(other); }
    FORCE_INLINE constexpr vec4<T> operator/(const vec<T, 4>& other) const { return vec<T, 4>::operator/(other); }
    FORCE_INLINE constexpr vec4<T> operator/(const T& other) const { return vec<T, 4>::operator/(other); }
    FORCE_INLINE constexpr vec4<T> normalize() const { return vec<T, 4>::normalize(); }

    FORCE_INLINE constexpr T& x() { return data[0]; };
    FORCE_INLINE constexpr T& y() { return data[1]; };
    FORCE_INLINE constexpr T& z() { return data[2]; };
    FORCE_INLINE constexpr T& w() { return data[3]; };
    FORCE_INLINE constexpr vec2<T> xy() const { return vec2<T>(data[0], data[1]); };
    FORCE_INLINE constexpr vec2<T> xz() const { return vec2<T>(data[0], data[2]); };
    FORCE_INLINE constexpr vec2<T> yz() const { return vec2<T>(data[1], data[2]); };
    FORCE_INLINE constexpr vec3<T> xyz() const { return vec3<T>(data[0], data[1], data[2]); };

    constexpr vec3<T> homogenize() const {
        if(data[3] == SCAST<T>(0)) return vec3<T>(0);
        return vec3<T>(data[0] / data[3], data[1] / data[3], data[2] / data[3]);
    };
};

typedef vec2<int16_t> vec2i16;
typedef vec2<int32_t> vec2i32;
typedef vec2<fixed> vec2f;

// struct vec2i16 : public vec2<int16_t> {
//     using vec2<int16_t>::vec;
//     constexpr vec2i16(int16_t x, int16_t y) : vec2<int16_t>(x, y) {};
//     constexpr vec2i16(const vec2<int16_t>& other) : vec2<int16_t>(other) {};
    
//     constexpr vec2i16& operator=(const vec2i16& other) {
//         vec<int16_t, 2>::operator=(other);
//         return *this;
//     };

//     // constexpr vec2i16(const vec2<fixed>& other){
//     //     data[0] = std::round(other(0));
//     //     data[1] = std::round(other(1));
//     // };
// };

// struct vec2f : public vec2<fixed> {
//     using vec2<fixed>::vec2;
//     constexpr vec2f(fixed x, fixed y) : vec2<fixed>(x, y) {};
//     constexpr vec2f(const vec<fixed, 2>& other) : vec2<fixed>(other) {};
//     constexpr vec2f& operator=(const vec2f& other) {
//         vec<fixed, 2>::operator=(other);
//         return *this;
//     };
// };

struct vec3f : public vec3<fixed> {
    using vec3<fixed>::vec3;
    constexpr vec3f(fixed x, fixed y, fixed z) : vec3<fixed>(x, y, z) {};
    constexpr vec3f(const vec<fixed, 3>& other) : vec3<fixed>(other) {};
    constexpr vec3f& operator=(const vec3f& other) {
        vec<fixed, 3>::operator=(other);
        return *this;
    };

    static const vec3f up;
    static const vec3f forward;
    static const vec3f right;
};

struct vec4f : public vec4<fixed> {
    using vec4<fixed>::vec4;
    constexpr vec4f(fixed x, fixed y, fixed z, fixed w) : vec4<fixed>(x, y, z, w) {};
    constexpr vec4f(const vec4<fixed>& other) : vec4<fixed>(other) {};
    constexpr vec4f& operator=(const vec4f& other) {
        vec4<fixed>::operator=(other);
        return *this;
    };
};
#pragma pack(pop)