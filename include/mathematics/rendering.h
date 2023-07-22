#pragma once

#include <math.h>
#include "mathematics/basic.h"
#include "mathematics/vector.h"

template<typename T>
class BoundingBox2D {
    public:
        // vec2i16 Min;
        // vec2i16 Max;

        vec<T, 2> Min;
        vec<T, 2> Max;

        constexpr BoundingBox2D(const vec<T, 2>& min, const vec<T, 2>& max) : Min(min), Max(max) {};

        constexpr BoundingBox2D Intersect(const BoundingBox2D& other) const {
            return BoundingBox2D (
                vec<T, 2>({ max(Min(0), other.Min(0)), max(Min(1), other.Min(1)) }),
                vec<T, 2>({ min(Max(0), other.Max(0)), min(Max(1), other.Max(1)) })
            );
        };

        static BoundingBox2D FromTriangle(const vec<T, 2>& a, const vec<T, 2>& b, const vec<T, 2>& c){
            return BoundingBox2D(
                vec<T, 2>({ min(a(0), min(b(0), c(0))), min(a(1), min(b(1), c(1))) }),
                vec<T, 2>({ max(a(0), max(b(0), c(0))), max(a(1), max(b(1), c(1))) })
            );
        };
};

// function that checks if a point is inside a triangle
bool pointInTriangle(vec3f p, vec3f a, vec3f b, vec3f c){
    vec3f ab = b - a;
    vec3f bc = c - b;
    vec3f ca = a - c;

    vec3f ap = p - a;
    vec3f bp = p - b;
    vec3f cp = p - c;

    vec3f v1 = ab.cross(ap);
    vec3f v2 = bc.cross(bp);
    vec3f v3 = ca.cross(cp);

    if(v1.dot(v2) > 0 && v2.dot(v3) > 0 && v3.dot(v1) > 0){
        return true;
    } else{
        return false;
    }
}