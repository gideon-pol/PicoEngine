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

    template<typename T2>
    constexpr mat(const mat<T2, C, R>& other) {
        for (int i = 0; i < R; i++) {
            data[i] = vec<T, C>(other(i));
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

    constexpr vec<T, C> col(int i) const {
        vec<T, C> result = vec<T, C>();
        for (int j = 0; j < R; j++) {
            result[j] = data[j](i);
        }
        return result;
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
        mat<T, R, C> result = mat<T, R, C>();
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                result[j][i] = data[i](j);
            }
        }
        return result;
    };

    template<int C2>
    constexpr mat<T, C2, R> operator*(const mat<T, C2, C>& other) const {
        mat<T, C2, R> result = mat<T, C2, R>();
        mat<T, C, C2> trans = ~other;

        for (int i = 0; i < C2; i++) {
            for(int j = 0; j < R; j++){
                result[j][i] = data[j] * trans[i];
            }

        }
        return result;
    };

    constexpr vec<T, R> operator*(const vec<T, C>& other) const {
        vec<T, R> result = vec<T, R>();
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
        mat<T, C, R> result = mat<T, C, R>();
        for (int i = 0; i < R; i++) {
            for(int j = 0; j < C; j++){
                result[i][j] = i == j ? SCAST<T>(1) : SCAST<T>(0);
            }
        }
        return result;
    };


};

typedef mat<fixed, 2, 2> mat2;
typedef mat<fixed, 3, 3> mat3;

struct mat4f : public mat<fixed, 4, 4> {
    using mat<fixed, 4, 4>::mat;
    constexpr mat4f(const mat<fixed, 4, 4>& other) : mat<fixed, 4, 4>(other) { };

    FORCE_INLINE constexpr mat4f operator*(const mat4f& other) const { return mat<fixed, 4, 4>::operator*(other); };
    FORCE_INLINE constexpr vec4f operator*(const vec4f& other) const { return mat<fixed, 4, 4>::operator*(other); };
    FORCE_INLINE constexpr vec4f operator()(int i) const { return mat<fixed, 4, 4>::operator()(i); };
    FORCE_INLINE constexpr fixed operator()(int y, int x) const { return mat<fixed, 4, 4>::operator()(y, x); };
    FORCE_INLINE constexpr vec4f col(int i) const { return mat<fixed, 4, 4>::col(i); };

    constexpr mat4f transpose(){
        mat4f result = mat4f::identity();
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                result[i][j] = data[j](i);
            }
        }
        return result;
    }

    constexpr static mat4f translate(const vec<fixed, 3>& v) {
        mat4f result = mat4f::identity();
        result[0][3] = v(0);
        result[1][3] = v(1);
        result[2][3] = v(2);
        return result;
    };

    constexpr static mat4f scale(const vec<fixed, 3>& v) {
        mat4f result = mat4f::identity();
        result[0][0] = v(0);
        result[1][1] = v(1);
        result[2][2] = v(2);
        return result;
    };

    constexpr static mat4f rotate(fixed angle, const vec<fixed, 3>& axis) {
        mat4f result = mat4f::identity();
        float piAngle = (float)angle * PI / 180.0f;
        fixed c = cos(piAngle);
        fixed s = sin(piAngle);
        fixed t = 1fp - c;

        vec<fixed, 3> ax = axis.normalize();

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

    constexpr static mat4f euler(const vec<fixed, 3>& v){
        return mat4f::rotate(v(2), vec3f::forward) *
               mat4f::rotate(v(0), vec3f::right) *
               mat4f::rotate(v(1), vec3f::up);
    }

    constexpr static mat4f perspective(fixed fov, fixed aspect, fixed near, fixed far) {
        mat4f result = mat4f();
        fixed yScale = 1fp / tan((float)fov * 0.5f * PI / 180.0f);
        fixed xScale = yScale * (1fp / aspect);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = -far / (far - near);
        result[2][3] = (far * near) / (far - near);
        result[3][2] = -1fp;
        
        return result;
    };
};