#pragma once

#include <stdint.h>

#pragma pack(push, 1)
template<typename T, int C>
struct vec {
    T data[C];

    constexpr vec() = default;

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

    constexpr vec<T, C> operator+(const vec<T, C>& other) const {
        vec<T, C> result;
        for (int i = 0; i < C; i++) {
            result[i] = data[i] + other(i);
        }
        return result;
    };

    constexpr vec<T, C> operator-(const vec<T, C>& other) const {
        vec<T, C> result;
        for (int i = 0; i < C; i++) {
            result[i] = data[i] - other(i);
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
        vec<T, C> result;
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

    constexpr vec<T, C> cross(const vec<T, C>& other) const {
        vec<T, C> result;
        result[0] = data[1] * other(2) - data[2] * other(1);
        result[1] = data[2] * other(0) - data[0] * other(2);
        result[2] = data[0] * other(1) - data[1] * other(0);
        return result;
    };
};

#pragma pack(push, 1)
template<int C>
struct vec2test {
    float data[C];


    constexpr vec2test(const float (&arr)[C]) {
        for (int i = 0; i < C; i++) {
            data[i] = arr[i];
        }
    };

    constexpr float& operator[](int i) {
        return data[i];
    };

    union {
        struct {
            float x;
            float y;
        };
    };

    // const float& x = data[0];
    // const float& y = data[1];
};

// union vec2test {
//     // unionize with vec<T, C> so that we can use the same functions
//     struct : vec<float, 2> {
//         using vec<float, 2>::vec;
//     };

//     float x;
//     float y;
// };

struct vec2f : vec<float, 2> {
    using vec<float, 2>::vec;

    constexpr vec2f(float x, float y) : vec<float, 2>({x, y}) {};
    constexpr vec2f(const vec<float, 2>& other) : vec<float, 2>(other) {};
    // this function is neccessary for some reason to assign vec2's after declaration
    constexpr vec2f& operator=(const vec2f& other) {
        vec<float, 2>::operator=(other);
        return *this;
    };

    // const int16_t& x = data[0];
    // const int16_t& y = data[1];

    constexpr float& x() { return data[0]; };
    constexpr float& y() { return data[1]; };
};

struct vec2i16 : vec<int16_t, 2> {
    using vec<int16_t, 2>::vec;

    constexpr vec2i16(int16_t x, int16_t y) : vec<int16_t, 2>({x, y}) {};
    constexpr vec2i16(const vec<int16_t, 2>& other) : vec<int16_t, 2>(other) {};
    constexpr vec2i16& operator=(const vec2i16& other) {
        vec<int16_t, 2>::operator=(other);
        return *this;
    };
    
    // const int16_t& x = data[0];
    // const int16_t& y = data[1];

    constexpr int16_t& x() { return data[0]; };
    constexpr int16_t& y() { return data[1]; };
};

struct vec3f : vec<float, 3> {
    constexpr vec3f(float x, float y, float z) : vec<float, 3>({x, y, z}) {};
    constexpr vec3f(const vec<float, 3>& other) : vec<float, 3>(other) {};
    constexpr vec3f& operator=(const vec3f& other) {
        vec<float, 3>::operator=(other);
        return *this;
    };

    // const float& x = data[0];
    // const float& y = data[1];
    // const float& z = data[2];

    inline constexpr float& x() { return data[0]; };
    inline constexpr float& y() { return data[1]; };
    inline constexpr float& z() { return data[2]; };
    inline constexpr vec2f xy() const { return vec2f(data[0], data[1]); };
    inline constexpr vec2f xz() const { return vec2f(data[0], data[2]); };
    inline constexpr vec2f yz() const { return vec2f(data[1], data[2]); };

    static const vec3f up;
    static const vec3f forward;
    static const vec3f right;

    using vec<float, 3>::vec;
};

struct vec4f : vec<float, 4> {
    constexpr vec4f(float x, float y, float z, float w) : vec<float, 4>({x, y, z, w}) {};
    constexpr vec4f(const vec<float, 4>& other) : vec<float, 4>(other) {};
    constexpr vec4f& operator=(const vec4f& other) {
        vec<float, 4>::operator=(other);
        return *this;
    };

    constexpr vec3f xyz() const {
        return vec3f(data[0], data[1], data[2]);
    };

    inline constexpr float& x() { return data[0]; };
    inline constexpr float& y() { return data[1]; };
    inline constexpr float& z() { return data[2]; };
    inline constexpr float& w() { return data[3]; };

    // const float& x = data[0];
    // const float& y = data[1];
    // const float& z = data[2];
    // const float& w = data[3];

    using vec<float, 4>::vec;
};
#pragma pack(pop)
