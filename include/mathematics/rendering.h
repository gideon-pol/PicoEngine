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

// bool pointInTriangle(vec2i16 p, vec2i16 a, vec2i16 b, vec2i16 c){
//     vec2i16 ab = b - a;
//     vec2i16 bc = c - b;
//     vec2i16 ca = a - c;

//     vec2i16 ap = p - a;
//     vec2i16 bp = p - b;
//     vec2i16 cp = p - c;

//     vec2i16 v1 = ab.cross(ap);
//     vec2i16 v2 = bc.cross(bp);
//     vec2i16 v3 = ca.cross(cp);

//     if(v1.dot(v2) > 0 && v2.dot(v3) > 0 && v3.dot(v1) > 0){
//         return true;
//     } else{
//         return false;
//     }
// }