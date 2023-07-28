#pragma once

#include <math.h>
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


        constexpr bool IsEmpty(){
            return abs(Max.x() - Min.x()) < 0.001f || (Max.y() - Min.y()) < 0.001f;
        }

        static BoundingBox2D FromTriangle(const vec2f& a, const vec2f& b, const vec2f& c){
            return BoundingBox2D(
                vec2f(min(a(0), min(b(0), c(0))), min(a(1), min(b(1), c(1)))),
                vec2f(max(a(0), max(b(0), c(0))), max(a(1), max(b(1), c(1))))
            );
        };
};

// typedef BoundingBox2D<int16_t> BoundingBox2Di16;
// typedef BoundingBox2D<float> BoundingBox2Df;

// TODO: AI generated, doesn't necessarily work
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
        constexpr void GetCorners(vec3f (&corners)[8]) const {
            corners[0] = vec3f(Min(0), Min(1), Min(2));
            corners[1] = vec3f(Min(0), Min(1), Max(2));
            corners[2] = vec3f(Min(0), Max(1), Min(2));
            corners[3] = vec3f(Min(0), Max(1), Max(2));
            corners[4] = vec3f(Max(0), Min(1), Min(2));
            corners[5] = vec3f(Max(0), Min(1), Max(2));
            corners[6] = vec3f(Max(0), Max(1), Min(2));
            corners[7] = vec3f(Max(0), Max(1), Max(2));
        };
};

// function that checks if a point is inside a triangle
bool pointInTriangle(vec3f p, vec3f a, vec3f b, vec3f c){
    a.z() = 0;
    b.z() = 0;
    c.z() = 0;
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

vec3f barycentric(vec2f p, vec2f t1, vec2f t2, vec2f t3){
    vec2f v0 = t2 - t1, v1 = t3 - t1, v2 = p - t1;
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return vec3f(u, v, w);
}