#pragma once

#include <math.h>

#include "common.h"
#include "mathematics/basic.h"

// Seems to be working, requires further testing however

/*
struct Quaternion {
    float x;
    float y;
    float z;
    float w;

    constexpr Quaternion() : x(0), y(0), z(0), w(1) {};
    constexpr Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
    constexpr Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};

    FORCE_INLINE constexpr Quaternion operator+(const Quaternion& other) const {
        return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
    };

    FORCE_INLINE constexpr Quaternion operator-(const Quaternion& other) const {
        return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
    };

    FORCE_INLINE constexpr Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.x - x * other.w - y * other.z + z * other.y,
            w * other.y - y * other.w - z * other.x + x * other.z,
            w * other.z - z * other.w - x * other.y + y * other.x,
            w * other.w + x * other.x + y * other.y + z * other.z
        );
    };

    FORCE_INLINE constexpr Quaternion operator*=(const Quaternion& other) {
        return *this = *this * other;
    };

    FORCE_INLINE constexpr Quaternion operator*(const float& other) const {
        return Quaternion(x * other, y * other, z * other, w * other);
    };

    FORCE_INLINE constexpr Quaternion operator/(const float& other) const {
        return Quaternion(x / other, y / other, z / other, w / other);
    };

    FORCE_INLINE constexpr Quaternion operator*=(const float& other) {
        return *this = *this * other;
    };

    FORCE_INLINE constexpr Quaternion operator/=(const float& other) {
        return *this = *this / other;
    };

    FORCE_INLINE constexpr static Quaternion RotateAround(const vec3f& axis, float angle) {
        float halfAngle = angle * 0.5f / 180.0f * PI;
        float sinHalfAngle = sin(halfAngle);
        float cosHalfAngle = cos(halfAngle);

        // value 0 is pitch, value 1 is yaw, value 2 is roll
        return Quaternion(
            SCAST<float>(axis(0)) * sinHalfAngle,
            SCAST<float>(axis(1)) * sinHalfAngle,
            SCAST<float>(axis(2)) * sinHalfAngle,
            cosHalfAngle
        );
    };

    // Converts to quaternion from euler angles, with rotation order yxz (yaw, pitch roll)
    constexpr static Quaternion Euler(const vec3f& euler) {
        vec3f e = euler * 0.5f / 180.0f * PI;

        float cy = cos(SCAST<float>(e(1))); // Rotation around Y (up)
        float sy = sin(SCAST<float>(e(1)));
        float cx = cos(SCAST<float>(e(0))); // Rotation around X (right)
        float sx = sin(SCAST<float>(e(0)));
        float cz = cos(SCAST<float>(e(2))); // Rotation around Z (forward)
        float sz = sin(SCAST<float>(e(2)));

        return Quaternion(
            cy * sx * cz + sy * cx * sz,
            sy * cx * cz - cy * sx * sz,
            cy * cx * sz - sy * sx * cz,
            cy * cx * cz + sy * sx * sz
        );
    }

    // converts the quaternion to a matrix, with rotation order yxz
    FORCE_INLINE constexpr mat4f ToMatrix() const {
        return mat4f({
            {1 - 2 * y * y - 2 * z * z, 2 * x * y + 2 * w * z, 2 * x * z - 2 * w * y, 0},
            {2 * x * y - 2 * w * z, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * w * x, 0},
            {2 * x * z + 2 * w * y, 2 * y * z - 2 * w * x, 1 - 2 * x * x - 2 * y * y, 0},
            {0, 0, 0, 1}
        });
    };

    // converts the quaternion back to euler angles, with rotation order yxz
    constexpr vec3f ToEuler() const {
        float sqw = w * w;
        float sqx = x * x;
        float sqy = y * y;
        float sqz = z * z;

        float unit = sqx + sqy + sqz + sqw;
        float test = x * y + z * w;

        if (test > 0.499f * unit) {
            return vec3f(
                0,
                2 * atan2(x, w) * 180.0f / PI,
                90.0f
            );
        } else if (test < -0.499f * unit) {
            return vec3f(
                0,
                -2 * atan2(x, w) * 180.0f / PI,
                -90.0f
            );
        } else {
            return vec3f(
                atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw) * 180.0f / PI,
                atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw) * 180.0f / PI,
                asin(2 * test / unit) * 180.0f / PI
            );
        }
    }
};*/


// Same quaternion implementation, but with fixed point math
struct Quaternion {
    fixed x;
    fixed y;
    fixed z;
    fixed w;

    constexpr Quaternion() : x(0), y(0), z(0), w(1) {};
    constexpr Quaternion(fixed x, fixed y, fixed z, fixed w) : x(x), y(y), z(z), w(w) {};
    constexpr Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};

    FORCE_INLINE constexpr Quaternion operator+(const Quaternion& other) const {
        return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
    };

    FORCE_INLINE constexpr Quaternion operator-(const Quaternion& other) const {
        return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
    };

    FORCE_INLINE constexpr Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.x - x * other.w - y * other.z + z * other.y,
            w * other.y - y * other.w - z * other.x + x * other.z,
            w * other.z - z * other.w - x * other.y + y * other.x,
            w * other.w + x * other.x + y * other.y + z * other.z
        );
    };

    FORCE_INLINE constexpr Quaternion operator*=(const Quaternion& other) {
        return *this = *this * other;
    };

    FORCE_INLINE constexpr Quaternion operator*(const fixed& other) const {
        return Quaternion(x * other, y * other, z * other, w * other);
    };

    FORCE_INLINE constexpr Quaternion operator/(const fixed& other) const {
        return Quaternion(x / other, y / other, z / other, w / other);
    };

    FORCE_INLINE constexpr Quaternion operator*=(const fixed& other) {
        return *this = *this * other;
    };

    FORCE_INLINE constexpr Quaternion operator/=(const fixed& other) {
        return *this = *this / other;
    };

    FORCE_INLINE constexpr static Quaternion RotateAround(const vec3f& axis, fixed angle) {
        float halfAngle = SCAST<float>(angle) * 0.5f / 180.0f * PI;
        fixed sinHalfAngle = sin(halfAngle);
        fixed cosHalfAngle = cos(halfAngle);

        // value 0 is pitch, value 1 is yaw, value 2 is roll
        return Quaternion(
            SCAST<fixed>(axis(0)) * sinHalfAngle,
            SCAST<fixed>(axis(1)) * sinHalfAngle,
            SCAST<fixed>(axis(2)) * sinHalfAngle,
            cosHalfAngle
        );
    };

    // Converts to quaternion from euler angles, with rotation order yxz (yaw, pitch roll)
    constexpr static Quaternion Euler(const vec3f& euler) {
        vec3f e = euler * 0.5f / 180.0f * PI;

        fixed cy = cos(SCAST<float>(e(1))); // Rotation around Y (up)
        fixed sy = sin(SCAST<float>(e(1)));
        fixed cx = cos(SCAST<float>(e(0))); // Rotation around X (right)
        fixed sx = sin(SCAST<float>(e(0)));
        fixed cz = cos(SCAST<float>(e(2))); // Rotation around Z (forward)
        fixed sz = sin(SCAST<float>(e(2)));

        return Quaternion(
            cy * sx * cz + sy * cx * sz,
            sy * cx * cz - cy * sx * sz,
            cy * cx * sz - sy * sx * cz,
            cy * cx * cz + sy * sx * sz
        );
    }

    // converts the quaternion to a matrix, with rotation order yxz
    FORCE_INLINE constexpr mat4f ToMatrix() const {
        return mat4f({
            {1fp - 2fp * y * y - 2fp * z * z, 2fp * x * y + 2fp * w * z, 2fp * x * z - 2fp * w * y, 0},
            {2fp * x * y - 2fp * w * z, 1fp - 2fp * x * x - 2fp * z * z, 2fp * y * z + 2fp * w * x, 0},
            {2fp * x * z + 2fp * w * y, 2fp * y * z - 2fp * w * x, 1fp - 2fp * x * x - 2fp * y * y, 0},
            {0, 0, 0, 1fp}
        });
    };

};
