#pragma once

#include "common.h"
#include "ECS.h"
#include "mathematics.h"
#include "rendering/texture.h"
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

    FORCE_INLINE constexpr mat4f& GetProjectionMatrix(){
        return projection;
    }

    // Frustum intersection test that checks if any of the bounding volume's corners
    // are inside the frustrum. Very naive and stupid but SAT is expensive.
    // This function can break for large bounding volumes or if the bounding volume
    // is too close to the camera.
    bool IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat){
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
    Color16 FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
    uint16_t Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];

    namespace {
        BoundingBox2D bounds = BoundingBox2D(vec2f(0), vec2f(0));
        mat4f rasterizationMat;
    }

    void Init(float fov, float near, float far){
        MainCamera = new Camera(fov, near, far, (float)FRAME_WIDTH / FRAME_HEIGHT);
        bounds = BoundingBox2D(vec2f(0, 0), vec2f(FRAME_WIDTH, FRAME_HEIGHT));
        rasterizationMat = 
            mat4f::scale(vec3f(FRAME_WIDTH, FRAME_HEIGHT, 1)) *
            mat4f::translate(vec3f(0.5, 0.5, 0)) *
            mat4f::scale(vec3f(0.5, 0.5, 1));
    }

    void Clear(Color color){
        for(int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++){
            FrameBuffer[i] = color.ToColor16();
            // Zbuffer[i] = 1;
            Zbuffer[i] = 65535;
        }
    }

    FORCE_INLINE void PutPixel(vec2i16 pos, Color color){
        if(pos.x() >= 0 && pos.x() < FRAME_WIDTH && pos.y() >= 0 && pos.y() < FRAME_HEIGHT){
            FrameBuffer[pos.y() * FRAME_WIDTH + pos.x()] = color.ToColor16();
        }
    }

    void DrawBox(BoundingBox2D box, Color color){
        BoundingBox2D bbi = bounds.Intersect(box);

        for(int y = bbi.Min.y(); y < static_cast<int>(bbi.Max.y()); y++){
            for(int x = bbi.Min.x(); x < static_cast<int>(bbi.Max.x()); x++){
                FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor16();
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
                // FrameBuffer[y * FRAME_WIDTH + x] = color;
                PutPixel(vec2i16(x, y), color);
            }
        }

        for(int y = startY + width; y < endY - width; y++){
            for(int x = startX; x < startX + width; x++){
                // FrameBuffer[y * FRAME_WIDTH + x] = color;
                PutPixel(vec2i16(x, y), color);
            }

            for(int x = endX - width; x < endX; x++){
                // FrameBuffer[y * FRAME_WIDTH + x] = color;
                PutPixel(vec2i16(x, y), color);
            }
        }

        for(int y = endY - width; y < endY; y++){
            for(int x = startX; x < endX; x++){
                // FrameBuffer[y * FRAME_WIDTH + x] = color;
                PutPixel(vec2i16(x, y), color);

            }
        }
    }

    void DrawLine(vec2i16 start, vec2i16 end, Color color, uint8_t lineWidth = 1){
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
            for(int y = y0 - lineWidth; y < y0 + lineWidth; y++){
                for(int x = x0 - lineWidth; x < x0 + lineWidth; x++){
                    PutPixel(vec2i16(x, y), color);
                }
            }

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


    void DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth = 1){
        mat4f rVP = rasterizationMat * MainCamera->GetProjectionMatrix() * MainCamera->GetViewMatrix();

        vec3f v1 = (rVP * vec4f(p1, 1)).homogenize();
        vec3f v2 = (rVP * vec4f(p2, 1)).homogenize();

        DrawLine(v1.xy(), v2.xy(), color, lineWidth);
    }

    void Blit(const Texture2D& tex, vec2i16 pos){
        BoundingBox2D bbi = BoundingBox2D(vec2i16(pos), vec2i16(tex.Width, tex.Height))
                            .Intersect(bounds);

        for(int y = bbi.Min.y(); y < bbi.Max.y(); y++){
            for(int x = bbi.Min.x(); x < bbi.Max.x(); x++){
                FrameBuffer[y * FRAME_WIDTH + x] = tex.GetPixel(vec2i16(x - pos.x(), y - pos.y())).ToColor16();
            }
        }
    }

    void DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material){
        // TODO: do not calculate this for every mesh
        mat4f rMVP = rasterizationMat *
                     MainCamera->GetProjectionMatrix() *
                     MainCamera->GetViewMatrix() * 
                     modelMat;

        if(!MainCamera->IntersectsFrustrum(mesh.Volume, modelMat)){
            return;
        }
        
        for(int i = 0; i < mesh.PolygonCount; i++){
            uint32_t idx = i * 3;

            TriangleShaderData t = {
                mesh.Vertices[mesh.Indices[idx]],
                mesh.Vertices[mesh.Indices[idx+1]],
                mesh.Vertices[mesh.Indices[idx+2]],
                Color::Purple
            };

            if(material._Shader.Type == ShaderType::Flat){
                t.TriangleColor = ((FlatShader::Parameters*)material.Parameters)->_Color;
            } else if(material._Shader.TriangleProgram){
                material._Shader.TriangleProgram(t, material.Parameters);
            }

            vec3f pv1 = (rMVP * vec4f(t.v1.Position, 1)).homogenize();
            vec3f pv2 = (rMVP * vec4f(t.v2.Position, 1)).homogenize();
            vec3f pv3 = (rMVP * vec4f(t.v3.Position, 1)).homogenize();

            BoundingBox2D bb = BoundingBox2D::FromTriangle(pv1.xy(), pv2.xy(), pv3.xy());
            BoundingBox2D bbi = bounds.Intersect(bb);

            if(bbi.IsEmpty()) continue;

            if(material._Shader.Type == ShaderType::WireFrame){
                Color color = ((WireFrameShader::Parameters*)material.Parameters)->_Color;
                vec2i16 p1 = vec2i16(pv1.x(), pv1.y());
                vec2i16 p2 = vec2i16(pv2.x(), pv2.y());
                vec2i16 p3 = vec2i16(pv3.x(), pv3.y());

                DrawLine(p1, p2, color);
                DrawLine(p2, p3, color);
                DrawLine(p3, p1, color);
                continue;
            }

            vec3f windingOrder = (pv2 - pv1).cross(pv3 - pv1);

            if(windingOrder.z() > 0) continue;

            float area = edgeFunction(pv1, pv2, pv3);
            for(int16_t x = bbi.Min.x(); x < bbi.Max.x(); x++){
                for(int16_t y = bbi.Min.y(); y < bbi.Max.y(); y++){
                    vec3f p = vec3f(x, y, 0);
                    float w0 = edgeFunction(pv2, pv3, p);
                    float w1 = edgeFunction(pv3, pv1, p);
                    float w2 = edgeFunction(pv1, pv2, p);

                    if(w0 >= 0 && w1 >= 0 && w2 >= 0){
                        vec3f uvw = vec3f(w0, w1, w2) / area;
                        float z = vec3f(pv1.z(), pv2.z(), pv3.z()) * uvw;
                        if(z > 1.0f || z < 0.0f) continue;

                        uint16_t z16 = static_cast<uint16_t>(z * 65535.0f);

                        if(z16 >= Zbuffer[y * FRAME_WIDTH + x]) continue;
                        Zbuffer[y * FRAME_WIDTH + x] = z16;

                        Color fragmentColor = t.TriangleColor;

                        switch(material._Shader.Type){
                            case ShaderType::Texture: {
                                TextureShader::Parameters* params = (TextureShader::Parameters*)material.Parameters;
                                Texture2D* tex = params->_Texture;
                                vec2f uv = (t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z());
                                uv = vec2f(uv.x() * params->TextureScale.x(), uv.y() * params->TextureScale.y());
                                fragmentColor = tex->Sample(uv).ToColor16();
                                break;
                            }
                            case ShaderType::Custom: {
                                if(material._Shader.FragmentProgram){
                                    vec3f normal = t.v1.Normal * uvw.x() + t.v2.Normal * uvw.y() + t.v3.Normal * uvw.z();
                                    normal = (modelMat * vec4f(normal, 0)).xyz().normalize();
                                    vec3f fragCoord = vec3f(x, y, z);
                                    vec2f uv = t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z();

                                    FragmentShaderData data = {
                                        normal,
                                        fragCoord,
                                        uv,
                                        vec2f(FRAME_WIDTH, FRAME_HEIGHT),
                                        fragmentColor
                                    };

                                    material._Shader.FragmentProgram(data, material.Parameters);
                                    fragmentColor = data.FragmentColor;
                                }
                                break;
                            }
                            default:
                                break;
                        }

                        FrameBuffer[y * FRAME_WIDTH + x] = fragmentColor.ToColor16();
                    }
                }
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