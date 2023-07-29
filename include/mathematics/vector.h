#pragma once

#include <stdint.h>

#pragma pack(push, 1)
template<typename T, int C>
struct vec {
    T data[C];

    constexpr vec() {
        for (int i = 0; i < C; i++) {
            data[i] = T(0);
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

    constexpr T& operator[](int i) {
        return data[i];
    };

    constexpr T operator()(int i) const {
        return data[i];
    };

    constexpr vec<T, C>& operator=(const vec<T, C>& other) {
        for (int i = 0; i < C; i++) {
            data[i] = other(i);
        }
        return *this;
    };

    constexpr T operator*(const vec<T, C>& other) const {
        T result = 0;
        for (int i = 0; i < C; i++) {
            result += data[i] * other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator*(const T& other) const {
        vec<T, C> result = vec<T, C>(0);
        for (int i = 0; i < C; i++) {
            result[i] = data[i] * other;
        }
        return result;
    };

    constexpr vec<T, C> operator/(const T& other) const {
        vec<T, C> result = vec<T, C>(0);
        for (int i = 0; i < C; i++) {
            result[i] = data[i] / other;
        }
        return result;
    };

    constexpr vec<T, C> operator+(const vec<T, C>& other) const {
        vec<T, C> result  = vec<T, C>(0);
        for (int i = 0; i < C; i++) {
            result[i] = data[i] + other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator-(const vec<T, C>& other) const {
        vec<T, C> result = vec<T, C>(0);
        for (int i = 0; i < C; i++) {
            result[i] = data[i] - other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator-() const {
        vec<T, C> result = vec<T, C>(0);
        for (int i = 0; i < C; i++) {
            result[i] = -data[i];
        }
        return result;
    };

    constexpr bool operator==(const vec<T, C>& other) const {
        for (int i = 0; i < C; i++) {
            if (data[i] != other(i)) {
                return false;
            }
        }
        return true;
    };

    constexpr uint32_t size() const {
        return C;
    };

    constexpr T magnitude() const {
        T result = 0;
        for (int i = 0; i < C; i++) {
            result += data[i] * data[i];
        }
        return sqrt(result);
    };

    constexpr vec<T, C> normalize() const {
        vec<T, C> result  = vec<T, C>(0);
        T mag = magnitude();
        for (int i = 0; i < C; i++) {
            result[i] = data[i] * (1/ mag);
        }
        return result;
    };

    constexpr float dot(const vec<T, C>& other) const {
        float result = 0;
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

    inline constexpr T& x() { return data[0]; };
    inline constexpr T& y() { return data[1]; };
}; 

template<typename T>
struct vec3 : public vec<T, 3> {
    using vec<T, 3>::data;
    using vec<T, 3>::vec;
    constexpr vec3(T x, T y, T z) : vec<T, 3>({x, y, z}) {};
    constexpr vec3(const vec<T, 3>& other) : vec<T, 3>(other) {};
    constexpr vec3(const vec2<T>& other, T z) : vec<T, 3>({other(0), other(1), z}) {};

    constexpr vec3 cross(const vec3<T>& other) const {
        vec3<T> result = vec3<T>(0);
        result[0] = data[1] * other(2) - data[2] * other(1);
        result[1] = data[2] * other(0) - data[0] * other(2);
        result[2] = data[0] * other(1) - data[1] * other(0);
        return result;
    };

    inline constexpr T& x() { return data[0]; };
    inline constexpr T& y() { return data[1]; };
    inline constexpr T& z() { return data[2]; };
    inline constexpr vec2<T> xy() const { return vec2<T>(data[0], data[1]); };
    inline constexpr vec2<T> xz() const { return vec2<T>(data[0], data[2]); };
    inline constexpr vec2<T> yz() const { return vec2<T>(data[1], data[2]); };
};

template<typename T>
struct vec4 : public vec<T, 4> {
    using vec<T, 4>::data;
    using vec<T, 4>::vec;
    constexpr vec4(T x, T y, T z, T w) : vec<T, 4>({x, y, z, w}) {};
    constexpr vec4(const vec<T, 4>& other) : vec<T, 4>(other) {};
    constexpr vec4(const vec3<T>& other, T w) : vec<T, 4>({other(0), other(1), other(2), w}) {};

    inline constexpr T& x() { return data[0]; };
    inline constexpr T& y() { return data[1]; };
    inline constexpr T& z() { return data[2]; };
    inline constexpr T& w() { return data[3]; };
    inline constexpr vec2<T> xy() const { return vec2<T>(data[0], data[1]); };
    inline constexpr vec2<T> xz() const { return vec2<T>(data[0], data[2]); };
    inline constexpr vec2<T> yz() const { return vec2<T>(data[1], data[2]); };
    inline constexpr vec3<T> xyz() const { return vec3<T>(data[0], data[1], data[2]); };

    constexpr vec3<T> homogenize() const {
        if(data[3] == 0) return vec3<T>(0);
        return vec3<T>(data[0] / data[3], data[1] / data[3], data[2] / data[3]);
    };
};

struct vec2i16 : public vec2<int16_t> {
    using vec2<int16_t>::vec;
    constexpr vec2i16(int16_t x, int16_t y) : vec2<int16_t>(x, y) {};
    constexpr vec2i16(const vec2<int16_t>& other) : vec2<int16_t>(other) {};
    
    constexpr vec2i16& operator=(const vec2i16& other) {
        vec<int16_t, 2>::operator=(other);
        return *this;
    };
};

struct vec2f : public vec2<float> {
    using vec2<float>::vec2;
    constexpr vec2f(float x, float y) : vec2<float>(x, y) {};
    constexpr vec2f(const vec<float, 2>& other) : vec2<float>(other) {};
    constexpr vec2f& operator=(const vec2f& other) {
        vec<float, 2>::operator=(other);
        return *this;
    };
};

struct vec3f : public vec3<float> {
    using vec3<float>::vec3;
    constexpr vec3f(float x, float y, float z) : vec3<float>(x, y, z) {};
    constexpr vec3f(const vec<float, 3>& other) : vec3<float>(other) {};
    constexpr vec3f& operator=(const vec3f& other) {
        vec<float, 3>::operator=(other);
        return *this;
    };

    static const vec3f up;
    static const vec3f forward;
    static const vec3f right;
};

struct vec4f : public vec4<float> {
    using vec4<float>::vec4;
    constexpr vec4f(float x, float y, float z, float w) : vec4<float>(x, y, z, w) {};
    constexpr vec4f(const vec4<float>& other) : vec4<float>(other) {};
    constexpr vec4f& operator=(const vec4f& other) {
        vec4<float>::operator=(other);
        return *this;
    };
};
#pragma pack(pop)
