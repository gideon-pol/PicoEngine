#pragma once

#include <math.h>
#include "mathematics/vector.h"

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