#pragma once

#include <math.h>
#include "common.h"
#include "mathematics/basic.h"
#include "mathematics/vector.h"

template<typename T, int C, int R>
struct mat {
    vec<T, C> data[R]{};

    constexpr mat() {
        for (int i = 0; i < R; i++) {
            data[i] = vec<T, C>();
        }
    };

    constexpr mat(const T val) {
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

    FORCE_INLINE constexpr vec<T, C>& operator[](int i){
        return data[i];
    };

    FORCE_INLINE constexpr T operator()(int y, int x) const {
        return data[y](x);
    };

    FORCE_INLINE constexpr vec<T, C> operator()(int i) const {
        return data[i];
    };

    constexpr mat<T, C, R>& operator=(const mat<T, C, R>& other) {
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                data[i][j] = other(i,j);
            }
        }
        return *this;
    };

    constexpr mat<T, R, C> operator~() const {
        mat<T, R, C> result = mat<T, R, C>(0);
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                result[j][i] = data[i](j);
            }
        }
        return result;
    };

    template<int C2>
    constexpr mat<T, C2, R> operator*(const mat<T, C2, C>& other) const {
        mat<T, C2, R> result = mat<T, C2, R>(0);
        mat<T, C, C2> trans = ~other;

        for (int i = 0; i < C2; i++) {
            for(int j = 0; j < R; j++){
                result[j][i] = data[j] * trans[i];
            }

        }
        return result;
    };

    constexpr vec<T, R> operator*(const vec<T, C>& other) const {
        vec<T, R> result = vec<T, R>(0);
        for (int i = 0; i < R; i++) {
            result[i] = data[i] * other;
        }
        return result;
    };

    FORCE_INLINE constexpr int rows() {
        return R;
    };

    FORCE_INLINE constexpr int columns() {
        return C;
    };

    static constexpr mat<T, C, R> identity() {
        mat<T, C, R> result = mat<T, C, R>(0);
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


struct mat4f : public mat<float, 4, 4> {
    using mat<float, 4, 4>::mat;
    constexpr mat4f(const mat<float, 4, 4>& other) : mat<float, 4, 4>(other) {};

    // TODO: figure out why this doesn't work by default
    constexpr mat4f operator*(const mat4f& other) const {
        mat4f result = mat4f(0);
        mat4f trans = ~other;

        // printf("Current matrix:\n");
        // for (int i = 0; i < 4; i++) {
        //     printf("%f %f %f %f\n", data[i](0), data[i](1), data[i](2), data[i](3));
        // }

        // printf("Other matrix:\n");
        // for (int i = 0; i < 4; i++) {
        //     printf("%f %f %f %f\n", trans[i](0), trans[i](1), trans[i](2), trans[i](3));
        // }

        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++){
                result[j][i] = data[j] * trans[i];
            }
        }

        // printf("Resulting matrix:\n");
        // for (int i = 0; i < 4; i++) {
        //     printf("%f %f %f %f\n", result[i](0), result[i](1), result[i](2), result[i](3));
        // }
        return result;
    };

    constexpr vec4f operator*(const vec4f& other) const {
        vec4f result = vec4f(0);
        for (int i = 0; i < 4; i++) {
            result[i] = data[i] * other;
        }
        return result;
    };

    constexpr static mat4f translate(const vec<float, 3>& v) {
        mat4f result = mat4f::identity();
        result[0][3] = v(0);
        result[1][3] = v(1);
        result[2][3] = v(2);
        return result;
    };

    // constexpr vec4f& operator[](int i){
    //     return data[i];
    // };

    FORCE_INLINE constexpr vec4f operator()(int i) const {
        return data[i];
    };

    constexpr static mat4f scale(const vec<float, 3>& v) {
        mat4f result = mat4f::identity();
        result[0][0] = v(0);
        result[1][1] = v(1);
        result[2][2] = v(2);
        return result;
    };

    constexpr static mat4f rotate(float angle, const vec<float, 3>& axis) {
        mat4f result = mat4f::identity();
        float piAngle = angle * PI * (1 / 180.0);
        float c = cos(piAngle);
        float s = sin(piAngle);
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

    constexpr static mat4f perspective(float fov, float aspect, float near, float far) {
        mat4f result = mat4f(0);
        float yScale = 1 / tan(fov * 0.5 * PI / 180);
        float xScale = yScale * (1 / aspect);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = -far / (far - near);
        result[2][3] = (far * near) / (far - near);
        result[3][2] = -1;
        
        return result;
    };
};