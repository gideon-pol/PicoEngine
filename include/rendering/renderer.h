#pragma once
#include "ECS.h"
#include "mathematics.h"
#include "rendering/mesh.h"
#include "rendering/shader.h"


class Camera {
    public:
        Camera(float fov, float near, float far, float aspect=1){
            this->fov = fov;
            this->near = near;
            this->far = far;

            projection = mat4f::perspective(fov, aspect, near, far);
            updateViewMatrix();
        }

    void SetPosition(const vec3f& position){
        this->position = position;
        orientationUpdated = true;
    }

    void SetRotation(const vec3f& rotation){
        this->rotation = rotation;
        orientationUpdated = true;
    }

    // Some internal caching. TODO: measure performance impact of this
    mat4f& GetViewMatrix(){
        if(orientationUpdated){
            updateViewMatrix();
            orientationUpdated = false;
        }
        return view;
    }

    constexpr inline mat4f& GetProjectionMatrix(){
        return projection;
    }

    bool IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat){
        // Stupid stupid stupid and naive implementation, SAT is expensive however
        vec3f corners[8];
        volume.GetCorners(&corners);

        mat4f MVP = GetProjectionMatrix() * GetViewMatrix() * translationMat;
        for(int i = 0; i < 8; i++){
            vec3f corner = (MVP * vec4f(corners[i], 1)).homogenize();
            if( corner.x() >= -1 && corner.x() <= 1 &&
                corner.y() >= -1 && corner.y() <= 1 &&
                corner.z() >= 0 && corner.z() <= 1){
                return true;
            }
        };

        return false;
    }

    private:
        void updateViewMatrix(){
            view = mat4f::translate(-position) *
                    mat4f::rotate(rotation.y(), vec3f::forward) *
                    mat4f::rotate(rotation.x(), vec3f::right) *
                    mat4f::rotate(rotation.z(), vec3f::up);
        }

        float fov;
        float near;
        float far;

        vec3f position;
        vec3f rotation;

        mat4f projection;
        mat4f view;

        bool orientationUpdated = false;
};

namespace Renderer{
    Camera* MainCamera;
    // Frame* FrameBuffer;

    Color* FrameBuffer;
    float* Zbuffer;
    vec2i16 Resolution;

    namespace {
        BoundingBox2D bounds = BoundingBox2D(vec2f(0), vec2f(0));
        mat4f rasterizationMat;
    }

    void Init(vec2i16 resolution, float fov, float near, float far){
        MainCamera = new Camera(fov, near, far, (float)resolution.x() / resolution.y());
        FrameBuffer = new Color[resolution.x() * resolution.y()];
        Zbuffer = new float[resolution.x() * resolution.y()];
        Resolution = resolution;
        bounds = BoundingBox2D(vec2f(0, 0), vec2f(resolution.x(), resolution.y()));
        // FrameBuffer = new Frame(pixels, resolution);
        rasterizationMat = 
            mat4f::scale(vec3f(resolution.x(), resolution.y(), 1)) *
            mat4f::translate(vec3f(0.5, 0.5, 0)) *
            mat4f::scale(vec3f(0.5, 0.5, 1));
    }

    void Clear(Color color){
        for(int i = 0; i < Resolution.x() * Resolution.y(); i++){
            FrameBuffer[i] = color;
            Zbuffer[i] = 1;
        }
    }

    inline void PutPixel(vec2i16 pos, Color color){
        if(pos.x() >= 0 && pos.x() < Resolution.x() && pos.y() >= 0 && pos.y() < Resolution.y()){
            FrameBuffer[pos.y() * Resolution.x() + pos.x()] = color;
        }
    }

    void DrawBox(BoundingBox2D box, Color color){
        BoundingBox2D bbi = bounds.Intersect(box);

        for(int y = bbi.Min.y(); y < static_cast<int>(bbi.Max.y()); y++){
            for(int x = bbi.Min.x(); x < static_cast<int>(bbi.Max.x()); x++){
                FrameBuffer[y * Resolution.x() + x] = color;
            }
        }
    }

    void DrawBorder(BoundingBox2D box, uint8_t width, Color color){
        BoundingBox2D bbi = box;// bounds.Intersect(box);

        int16_t startX = static_cast<int16_t>(bbi.Min.x());
        int16_t startY = static_cast<int16_t>(bbi.Min.y());
        int16_t endX = std::ceil(bbi.Max.x());
        int16_t endY = std::ceil(bbi.Max.y());

        for(int y = startY; y < startY + width; y++){
            for(int x = startX; x < endX; x++){
                // FrameBuffer[y * Resolution.x() + x] = color;
                PutPixel(vec2i16(x, y), color);
            }
        }

        for(int y = startY + width; y < endY - width; y++){
            for(int x = startX; x < startX + width; x++){
                // FrameBuffer[y * Resolution.x() + x] = color;
                PutPixel(vec2i16(x, y), color);
            }

            for(int x = endX - width; x < endX; x++){
                // FrameBuffer[y * Resolution.x() + x] = color;
                PutPixel(vec2i16(x, y), color);
            }
        }

        for(int y = endY - width; y < endY; y++){
            for(int x = startX; x < endX; x++){
                // FrameBuffer[y * Resolution.x() + x] = color;
                PutPixel(vec2i16(x, y), color);

            }
        }
    }

    void DrawLine(vec2i16 start, vec2i16 end, Color color){
        int16_t x0 = start.x();
        int16_t y0 = start.y();
        int16_t x1 = end.x();
        int16_t y1 = end.y();

        int16_t dx = abs(x1 - x0);
        int16_t dy = abs(y1 - y0);

        int16_t sx = x0 < x1 ? 1 : -1;
        int16_t sy = y0 < y1 ? 1 : -1;

        int16_t err = dx - dy;

        while(true){
            PutPixel(vec2i16(x0, y0), color);

            if(x0 == x1 && y0 == y1) break;

            int16_t e2 = 2 * err;

            if(e2 > -dy){
                err -= dy;
                x0 += sx;
            }

            if(e2 < dx){
                err += dx;
                y0 += sy;
            }
        }
    }

    void DrawLine(vec3f p1, vec3f p2, Color color){
        mat4f rVP = rasterizationMat * MainCamera->GetProjectionMatrix() * MainCamera->GetViewMatrix();

        vec3f v1 = (rVP * vec4f(p1, 1)).homogenize();
        vec3f v2 = (rVP * vec4f(p2, 1)).homogenize();

        DrawLine(vec2i16(v1.x(), v1.y()), vec2i16(v2.x(), v2.y()), color);
    }

    void DrawTriangle(vec3f t1, vec3f t2, vec3f t3, Color color){
        BoundingBox2D bb = BoundingBox2D::FromTriangle(t1.xy(), t2.xy(), t3.xy());
        BoundingBox2D bbi = bounds.Intersect(bb);

        // DrawBorder(bbi, 2, 0xFFFF0000);

        if(bbi.IsEmpty()) {
            return;
        }

        // vec2i16 ti1 = vec2i16(t1.x(), t1.y());
        // vec2i16 ti2 = vec2i16(t2.x(), t2.y());
        // vec2i16 ti3 = vec2i16(t3.x(), t3.y());

        // DrawLine(ti1, ti2, 0xFFFFFFFF);
        // DrawLine(ti2, ti3, 0xFFFFFFFF);
        // DrawLine(ti3, ti1, 0xFFFFFFFF);

        float area = edgeFunction(t1, t2, t3);
        for(int16_t x = bbi.Min.x(); x < bbi.Max.x(); x++){
            for(int16_t y = bbi.Min.y(); y < bbi.Max.y(); y++){
                vec3f p = vec3f(x, y, 0);
                float w0 = edgeFunction(t2, t3, p);
                float w1 = edgeFunction(t3, t1, p);
                float w2 = edgeFunction(t1, t2, p);

                if(w0 >= 0 && w1 >= 0 && w2 >= 0){
                    vec3f uvw = vec3f(w0, w1, w2) / area;
                    float z = vec3f(t1.z(), t2.z(), t3.z()) * uvw;
                    
                    if(z < Zbuffer[y * Resolution.x() + x]){
                        FrameBuffer[y * Resolution.x() + x] = color;
                            // 0xFF << 24 |
                            // static_cast<uint8_t>(uvw.x() * 255) << 16 |
                            // static_cast<uint8_t>(uvw.y() * 255) << 8 |
                            // static_cast<uint8_t>(uvw.z() * 255);
                        Zbuffer[y * Resolution.x() + x] = z;
                        // FrameBuffer[y * Resolution.x() + x] = 0xFF << 24 | static_cast<uint8_t>(z/0.1 * 255) << 16;
                    }
                }
            }
        }
    }

    void DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material){
        mat4f rMVP = rasterizationMat * 
                     MainCamera->GetProjectionMatrix() *
                     MainCamera->GetViewMatrix() * 
                     modelMat;

        if(!MainCamera->IntersectsFrustrum(mesh.Volume, modelMat)){
            printf("Mesh is offscreen\n");
            return;
        }

        for(int i = 0; i < mesh.PolygonCount; i++){
            uint32_t idx = i * 3;

            vec3f v1 = mesh.Vertices[mesh.Indices[idx]].Position;
            vec3f v2 = mesh.Vertices[mesh.Indices[idx+1]].Position;
            vec3f v3 = mesh.Vertices[mesh.Indices[idx+2]].Position;

            vec3f pv1 = (rMVP * vec4f(v1, 1)).homogenize();
            vec3f pv2 = (rMVP * vec4f(v2, 1)).homogenize();
            vec3f pv3 = (rMVP * vec4f(v3, 1)).homogenize();

            vec3f windingOrder = (pv2 - pv1).cross(pv3 - pv1);

            if(windingOrder.z() <= 0){
                vec3f normal = (v2 - v1).cross(v3 - v1).normalize();
                float intensity = (-normal).dot(vec3f::forward) * 0.5 + 0.5;
                Color color = Color(0, static_cast<uint8_t>(intensity * 255), 0, 255);
                
                // This is just for testing, properly implement later
                if (material._Shader.Type == ShaderType::CustomTriangle)
                {
                    FragmentShaderInput input = { normal, vec2f(0) };
                    color = material._Shader.fragment_shader(input, material.Parameters);
                }
                
                
                DrawTriangle(pv1, pv2, pv3, color);
            }
        }
    }

    namespace Debug {
        void DrawVolume(BoundingVolume& volume, mat4f& modelMat, Color color){
            vec3f corners[8];
            volume.GetCorners(&corners);

            for(int i = 0; i < 8; i++){
                corners[i] = (modelMat * vec4f(corners[i], 1)).xyz();
            }

            Renderer::DrawLine(corners[0], corners[1], color);
            Renderer::DrawLine(corners[0], corners[2], color);
            Renderer::DrawLine(corners[1], corners[3], color);
            Renderer::DrawLine(corners[2], corners[3], color);

            Renderer::DrawLine(corners[4], corners[5], color);
            Renderer::DrawLine(corners[4], corners[6], color);
            Renderer::DrawLine(corners[5], corners[7], color);
            Renderer::DrawLine(corners[6], corners[7], color);

            Renderer::DrawLine(corners[0], corners[4], color);
            Renderer::DrawLine(corners[1], corners[5], color);
            Renderer::DrawLine(corners[2], corners[6], color);
            Renderer::DrawLine(corners[3], corners[7], color);
        }
    }
};