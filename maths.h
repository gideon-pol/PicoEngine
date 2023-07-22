#pragma once

#include <stdio.h>
#include <math.h>

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

    constexpr int size() const {
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

template<typename T, int C, int R>
struct mat {
    vec<T, C> data[R];

    constexpr mat() = default;

    constexpr mat(const T val){
        for (int i = 0; i < R; i++) {
            data[i] = vec<T, C>(val);
        }
    };

    constexpr mat(const mat<T, C, R>& other) {
        for (int i = 0; i < R; i++) {
            data[i] = other(i);
        }
    };

    constexpr mat(const T (&arr)[R][C]) {
        for (int i = 0; i < R; i++) {
            data[i] = vec<T, C>(arr[i]);
        }
    };

    constexpr vec<T, C>& operator[](int i){
        return data[i];
    };

    constexpr vec<T, C> operator()(int y, int x) const {
        return data[y](x);
    };

    constexpr vec<T, C> operator()(int i) const {
        return data[i];
    };

    constexpr mat<T, C, R>& operator=(const mat<T, C, R>& other) const {
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                data[i][j] = other(i,j);
            }
        }
        return *this;
    };

    constexpr mat<T, R, C> operator~() const {
        mat<T, R, C> result;
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                result[j][i] = data[i](j);
            }
        }
        return result;
    };

    template<int C2>
    constexpr mat<T, C2, R> operator*(const mat<T, C2, C>& other) const {
        mat<T, C2, R> result;
        mat<T, C, C2> trans = ~other;

        for (int i = 0; i < C2; i++) {
            for(int j = 0; j < R; j++){
                result[j][i] = data[j] * trans[i];
            }

        }
        return result;
    };

    constexpr vec<T, R> operator*(const vec<T, C>& other) const {
        vec<T, R> result;
        for (int i = 0; i < R; i++) {
            result[i] = data[i] * other;
        }
        return result;
    };

    constexpr int rows() {
        return R;
    };

    constexpr int columns() {
        return C;
    };

    static constexpr mat<T, C, R> identity() {
        mat<T, C, R> result;
        for (int i = 0; i < R; i++) {
            for(int j = 0; j < C; j++){
                result[i][j] = i == j ? 1 : 0;
            }
        }
        return result;
    };
};

struct vec2 : vec<float, 2> {
    constexpr vec2(float x, float y) : vec<float, 2>({x, y}) {};
    constexpr vec2(const vec<float, 2>& other) : vec<float, 2>(other) {};

    const float& x = data[0];
    const float& y = data[1];

    using vec<float, 2>::vec;
};

struct vec3 : vec<float, 3> {
    constexpr vec3(float x, float y, float z) : vec<float, 3>({x, y, z}) {};
    constexpr vec3(const vec<float, 3>& other) : vec<float, 3>(other) {};

    const float& x = data[0];
    const float& y = data[1];
    const float& z = data[2];

    static const vec3 up;
    static const vec3 forward;
    static const vec3 right;

    using vec<float, 3>::vec;
};

struct vec4 : vec<float, 4> {
    constexpr vec4(float x, float y, float z, float w) : vec<float, 4>({x, y, z, w}) {};
    constexpr vec4(const vec<float, 4>& other) : vec<float, 4>(other) {};

    const float& x = data[0];
    const float& y = data[1];
    const float& z = data[2];
    const float& w = data[3];

    using vec<float, 4>::vec;
};

typedef mat<float, 2, 2> mat2;
typedef mat<float, 3, 3> mat3;


struct mat4 : mat<float, 4, 4> {
    constexpr mat4(const mat<float, 4, 4>& other) : mat<float, 4, 4>(other) {};
    using mat<float, 4, 4>::mat;

    constexpr mat4 translate(const vec<float, 3>& v) const {
        mat4 result = mat4::identity();
        result[3][0] = v(0);
        result[3][1] = v(1);
        result[3][2] = v(2);
        return result;
    };

    constexpr mat4 scale(const vec<float, 3>& v) const {
        mat4 result = mat4::identity();
        result[0][0] = v(0);
        result[1][1] = v(1);
        result[2][2] = v(2);
        return result;
    };

    constexpr static mat4 rotate(float angle, const vec<float, 3>& axis) {
        mat4 result = mat4(0);
        float c = cos(angle);
        float s = sin(angle);
        float t = 1 - c;

        vec<float, 3> ax = axis.normalize();

        result[0][0] = t * ax[0] * ax[0] + c;
        result[0][1] = t * ax[0] * ax[1] - s * ax[2];
        result[0][2] = t * ax[0] * ax[2] + s * ax[1];

        result[1][0] = t * ax[0] * ax[1] + s * ax[2];
        result[1][1] = t * ax[1] * ax[1] + c;
        result[1][2] = t * ax[1] * ax[2] - s * ax[0];

        result[2][0] = t * ax[0] * ax[2] - s * ax[1];
        result[2][1] = t * ax[1] * ax[2] + s * ax[0];
        result[2][2] = t * ax[2] * ax[2] + c;

        return result;
    };
};