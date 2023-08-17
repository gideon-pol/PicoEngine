#pragma once

#include "common.h"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/mesh.h"
#include "rendering/shader.h"
#include "rendering/color.h"

class Camera {
    public:
    Camera(fixed fov, fixed near, fixed far, fixed aspect=1);

    FORCE_INLINE void SetPosition(const vec3f position){
        this->position = position;
        orientationUpdated = true;
    };

    FORCE_INLINE void SetRotation(const vec3f rotation){
        this->rotation = rotation;
        orientationUpdated = true;
    };

    FORCE_INLINE vec3f GetPosition(){
        return position;
    };

    FORCE_INLINE vec3f GetRotation(){
        return rotation;
    };

    mat4f& GetViewMatrix();

    FORCE_INLINE mat4f& GetProjectionMatrix(){
        return projection;
    }

    bool IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat);

    private:
        void updateViewMatrix();
        
        fixed fov;
        fixed near;
        fixed far;

        vec3f position;
        vec3f rotation;

        mat4f projection;
        mat4f view;

        bool orientationUpdated = false;
};

namespace Renderer{
    extern Camera MainCamera;
    extern Color16 FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
    extern uint16_t Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];

    namespace {
        BoundingBox2D bounds = BoundingBox2D(vec2f(0), vec2f(0));
        mat4f rasterizationMat;
    }

    void Init();
    void Clear(Color color);
    FORCE_INLINE void PutPixel(vec2i16 pos, Color color);
    void DrawBox(BoundingBox2D box, Color color);
    void DrawBorder(BoundingBox2D box, uint8_t width, Color color);
    void DrawLine(vec2i32 start, vec2i32 end, Color color, uint8_t lineWidth = 1);
    void DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth = 1);
    void Blit(const Texture2D& tex, vec2i16 pos);
    void DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material);

    namespace Debug {
        void DrawVolume(BoundingVolume& volume, mat4f& modelMat, Color color);
    }
};