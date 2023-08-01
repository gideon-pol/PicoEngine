#pragma once

#include <stdint.h>
#include "mathematics.h"


typedef struct {
    vec3f Position;
    vec3f Normal;
    vec2f UV;
} Vertex;
class Mesh {
    public:
        Vertex* Vertices;
        uint32_t* Indices;
        uint32_t VertexCount;
        uint32_t PolygonCount;
        BoundingVolume Volume;

        Mesh(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t polygonCount){
            Vertices = vertices;
            VertexCount = vertexCount;
            Indices = indices;
            PolygonCount = polygonCount;
            
            RecalculateVolume();
        }

        constexpr inline uint32_t GetPolygonCount(){
            return PolygonCount;
        }

        BoundingVolume& RecalculateVolume(){
            vec3f min = vec3f(0);
            vec3f max = vec3f(0);

            for (int i = 0; i < VertexCount; i++){
                min = vec3f(
                    min(0) < Vertices[i].Position(0) ? min(0) : Vertices[i].Position(0),
                    min(1) < Vertices[i].Position(1) ? min(1) : Vertices[i].Position(1),
                    min(2) < Vertices[i].Position(2) ? min(2) : Vertices[i].Position(2));
                max = vec3f(
                    max(0) > Vertices[i].Position(0) ? max(0) : Vertices[i].Position(0),
                    max(1) > Vertices[i].Position(1) ? max(1) : Vertices[i].Position(1),
                    max(2) > Vertices[i].Position(2) ? max(2) : Vertices[i].Position(2));
            }

            Volume = BoundingVolume(min, max);
            return Volume;
        }

    private:
        uint32_t polygonCount, vertexCount;
};