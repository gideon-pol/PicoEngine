#pragma once

#include "common.h"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/mesh.h"
#include "rendering/shader.h"
#include "rendering/color.h"
#include "rendering/font.h"
#include "ecs/object.h"

#include "game/shaders.h"

class Camera : public Object {
    public:
    Camera(fixed fov, fixed near, fixed far, fixed aspect=1);

    mat4f GetViewMatrix();

    FORCE_INLINE mat4f& GetProjectionMatrix(){
        return projection;
    }

    bool IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat);

    private:
        fixed fov;
        fixed near;
        fixed far;

        mat4f projection;
        mat4f view;
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

class DrawCall {
public:
    DrawCall(Mesh& mesh, mat4f& modelMatrix, Material& material, Culling culling = Culling::Back, DepthTest depthTest = DepthTest::Less)
        : _Mesh(mesh), ModelMatrix(modelMatrix), _Material(material), CullingMode(culling), DepthTestMode(depthTest) {}
    Mesh& _Mesh;
    mat4f ModelMatrix;
    Material& _Material;
    Culling CullingMode;
    DepthTest DepthTestMode;
};

namespace Renderer{
    extern Camera MainCamera;
    extern Color565 FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
    extern uint16_t Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];

    extern Font TextFont;

    extern Color ClearColor;

    namespace {
        BoundingBox2D bounds = BoundingBox2D(vec2f(0, 0), vec2f(FRAME_WIDTH, FRAME_HEIGHT));
        mat4f rasterizationMat = mat4f::identity();
        mat4f VP = mat4f::identity();
        mat4f RVP = mat4f::identity();

        bool testAndSetDepth(vec2i16 pos, uint16_t val, DepthTest depthTestMode){
            if(pos.x() < 0 || pos.x() >= FRAME_WIDTH || pos.y() < 0 || pos.y() >= FRAME_HEIGHT) return false;

            switch(depthTestMode){
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

    void Submit(const DrawCall& call);
    bool Render();
    void Finish();

    FORCE_INLINE void PutPixel(vec2i16 pos, Color color){
        if(pos.x() >= 0 && pos.x() < FRAME_WIDTH && pos.y() >= 0 && pos.y() < FRAME_HEIGHT){
            FrameBuffer[pos.y() * FRAME_WIDTH + pos.x()] = color.ToColor565();
        }
    }
    
    void DrawBox(BoundingBox2D box, Color color);
    void DrawBorder(BoundingBox2D box, uint8_t width, Color color);
    void DrawLine(vec2i16 start, vec2i16 end, Color color, uint8_t lineWidth = 1);
    void DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth = 1, DepthTest depthTestMode = DepthTest::Less);
    void DrawText(const char* text, vec2i16 pos, Color color);
    void DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material, Culling cullingMode = Culling::Back, DepthTest depthTestMode = DepthTest::Less);
    void Blit(const Texture2D& tex, vec2i16 pos);

    vec3f WorldToScreen(vec3f worldPos);

    namespace Debug {
        void DrawVolume(BoundingVolume& volume, mat4f& modelMat, Color color);
        void DrawOrientation(mat4f& modelMat);
    }
};