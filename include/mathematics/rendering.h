#pragma once

#include <math.h>
#include "common.h"
#include "mathematics/basic.h"
#include "mathematics/vector.h"

class BoundingBox2D {
    public:
        vec2f Min;
        vec2f Max;

        BoundingBox2D(const vec2f& min, const vec2f& max) : Min(min), Max(max) {};

        BoundingBox2D Intersect(const BoundingBox2D& other) {
            vec2f minBound = vec2f(max(Min(0), other.Min(0)), max(Min(1), other.Min(1)));
            vec2f maxBound = vec2f(max(min(Max(0), other.Max(0)), minBound(0)), max(min(Max(1), other.Max(1)), minBound(1)));

            return BoundingBox2D(minBound, maxBound);
        };

        FORCE_INLINE constexpr bool IsEmpty(){
            return abs(Max.x() - Min.x()) < 0.001f || (Max.y() - Min.y()) < 0.001f;
        }

        FORCE_INLINE static BoundingBox2D FromTriangle(const vec2f& a, const vec2f& b, const vec2f& c){
            return BoundingBox2D(
                vec2f(min(a(0), min(b(0), c(0))), min(a(1), min(b(1), c(1)))),
                vec2f(max(a(0), max(b(0), c(0))), max(a(1), max(b(1), c(1))))
            );
        };
};

class BoundingVolume {
    public:
        vec3f Min;
        vec3f Max;

        BoundingVolume() : Min(vec3f(0, 0, 0)), Max(vec3f(0, 0, 0)) {};
        BoundingVolume(const vec3f& min, const vec3f& max) : Min(min), Max(max) {};

        BoundingVolume Intersect(const BoundingVolume& other) {
            vec3f minBound = vec3f(max(Min(0), other.Min(0)), max(Min(1), other.Min(1)), max(Min(2), other.Min(2)));
            vec3f maxBound = vec3f(max(min(Max(0), other.Max(0)), minBound(0)), max(min(Max(1), other.Max(1)), minBound(1)), max(min(Max(2), other.Max(2)), minBound(2)));

            return BoundingVolume(minBound, maxBound);
        };

        // TODO: AI generated, test this
        FORCE_INLINE constexpr void GetCorners(vec3f (*corners)[8]) const {
            (*corners)[0] = Min;
            (*corners)[1] = vec3f(Min(0), Min(1), Max(2));
            (*corners)[2] = vec3f(Min(0), Max(1), Min(2));
            (*corners)[3] = vec3f(Min(0), Max(1), Max(2));
            (*corners)[4] = vec3f(Max(0), Min(1), Min(2));
            (*corners)[5] = vec3f(Max(0), Min(1), Max(2));
            (*corners)[6] = vec3f(Max(0), Max(1), Min(2));
            (*corners)[7] = Max;
        };
};

FORCE_INLINE constexpr float edgeFunction(vec3f a, vec3f b, vec3f c){
    return (c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x());
}

// first applies yaw, then pitch, then roll
FORCE_INLINE mat4f getRotationalMatrix(vec3f rot){
    return mat4f::rotate(rot.y(), vec3f::up) *
           mat4f::rotate(rot.z(), vec3f::forward) *
           mat4f::rotate(rot.x(), vec3f::right);
}