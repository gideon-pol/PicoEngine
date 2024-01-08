#pragma once

#include "common.h"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/mesh.h"
#include "rendering/shader.h"
#include "rendering/color.h"
#include "rendering/font.h"
#include "ecs/object.h"

class Camera {
    public:
    Camera(fixed fov, fixed near, fixed far, fixed aspect=1);

    FORCE_INLINE void SetPosition(const vec3f position){
        this->position = position;
        orientationUpdated = true;
    };

    FORCE_INLINE void SetRotation(const Quaternion rotation){
        this->rotation = rotation;
        orientationUpdated = true;
    };

    FORCE_INLINE vec3f GetPosition(){
        return position;
    };

    FORCE_INLINE Quaternion GetRotation(){
        return rotation;
    };

    mat4f& GetViewMatrix();
    mat4f GetModelMatrix();

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
        Quaternion rotation;

        mat4f projection;
        mat4f view;

        bool orientationUpdated = false;
};

enum DepthTest {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
};

enum Culling {
    None,
    Front,
    Back,
};

namespace Renderer{
    extern Camera MainCamera;
    extern Color565 FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
    extern uint16_t Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];

    extern Font TextFont;

    extern DepthTest DepthTestMode;
    extern Culling CullingMode;

    extern Color ClearColor;

    namespace {
        BoundingBox2D bounds = BoundingBox2D(vec2f(0), vec2f(0));
        mat4f rasterizationMat = mat4f::identity();
        mat4f VP = mat4f::identity();
        mat4f RVP = mat4f::identity();

        bool testAndSetDepth(vec2i16 pos, uint16_t val){
            if(pos.x() < 0 || pos.x() >= FRAME_WIDTH || pos.y() < 0 || pos.y() >= FRAME_HEIGHT) return false;

            switch(DepthTestMode){
                case DepthTest::Never:
                    return true;
                case DepthTest::Less:
                    return val < Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                case DepthTest::Greater:
                    return val > Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                case DepthTest::Equal:
                    return val == Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                case DepthTest::NotEqual:
                    return val != Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                case DepthTest::LessEqual:
                    return val <= Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                case DepthTest::GreaterEqual:
                    return val >= Zbuffer[pos.y() * FRAME_WIDTH
                     + pos.x()] && (Zbuffer[pos.y() * FRAME_WIDTH + pos.x()] = val);
                default:
                    return false;
            }
        }
    };

    void Init();
    void Clear(Color color);
    void Prepare();

    FORCE_INLINE void PutPixel(vec2i16 pos, Color color){
        if(pos.x() >= 0 && pos.x() < FRAME_WIDTH && pos.y() >= 0 && pos.y() < FRAME_HEIGHT){
            FrameBuffer[pos.y() * FRAME_WIDTH + pos.x()] = color.ToColor565();
        }
    }
    void DrawBox(BoundingBox2D box, Color color);
    void DrawBorder(BoundingBox2D box, uint8_t width, Color color);
    void DrawLine(vec2i32 start, vec2i32 end, Color color, uint8_t lineWidth = 1);
    void DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth = 1);
    void DrawText(const char* text, vec2i16 pos, Color color);
    void DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material);
    void Blit(const Texture2D& tex, vec2i16 pos);

    vec3f WorldToScreen(vec3f worldPos);

    namespace Debug {
        void DrawVolume(BoundingVolume& volume, mat4f& modelMat, Color color);
        void DrawOrientation(mat4f& modelMat);
    }
};