#pragma once
#include "ECS.h"
#include "mathematics.h"
#include "rendering/frame.h"
#include "rendering/mesh.h"


class Camera {
    public:
        vec3f Position;
        vec3f Rotation;

        Camera(float fov, float near, float far, float aspect=1){
            this->fov = fov;
            this->near = near;
            this->far = far;

            projection = mat4f::perspective(fov, aspect, near, far);
            UpdateVPMatrix();
        }

    // mat4f UpdateViewMatrix(){
    //     view = mat4f::translate(-Position) *
    //             mat4f::rotate(Rotation.y(), vec3f::forward) *
    //             mat4f::rotate(Rotation.x(), vec3f::right) *
    //             mat4f::rotate(Rotation.z(), vec3f::up);
    // }

    mat4f UpdateVPMatrix(){
        VP = projection * mat4f::translate(-Position) *
                mat4f::rotate(Rotation.y(), vec3f(0, 0, 1)) *
                mat4f::rotate(Rotation.x(), vec3f(1, 0, 0)) *
                mat4f::rotate(Rotation.z(), vec3f(0, 1, 0));
        return VP;
    }
    
    // constexpr inline mat4f GetViewMatrix(){
    //     return view;
    // }

    constexpr inline mat4f GetVPMatrix(){
        return VP;
    }

    constexpr inline mat4f GetProjectionMatrix(){
        return projection;
    }

    bool IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat){
        // Stupid stupid stupid and naive implementation, SAT is expensive however
        vec3f corners[8];
        volume.GetCorners(corners);

        mat4f MVP = projection * view * translationMat;
        for(int i = 0; i < 8; i++){
            vec4f corner = VP * vec4f(corners[i], 1);
            if( corner.x() >= -1 && corner.x() <= 1 &&
                corner.y() >= -1 && corner.y() <= 1 &&
                corner.z() >= 0 && corner.z() <= 1){
                return true;
            }
        };

        return false;
    }

    private:
        float fov;
        float near;
        float far;

        mat4f projection;
        mat4f view;
        mat4f VP;
};

namespace Renderer{
    Camera* MainCamera;
    Frame* FrameBuffer;
    uint32_t ClearColor;

    mat4f rasterizationMat = mat4f::translate(vec3f(0.5, 0.5, 0)) * mat4f::scale(vec3f(0.5f, 0.5f, 1.0f));

    void Init(vec2i16 resolution, uint32_t clearColor, float fov, float near, float far){
        MainCamera = new Camera(fov, near, far, (float)resolution.x() / resolution.y());
        uint32_t* pixels = new uint32_t[resolution.x() * resolution.y()];
        FrameBuffer = new Frame(pixels, resolution);
        rasterizationMat = 
            mat4f::scale(vec3f(resolution.x(), resolution.y(), 1)) *
            mat4f::translate(vec3f(0.5, 0.5, 0)) *
            mat4f::scale(vec3f(0.5, 0.5, 1));
        ClearColor = clearColor;
        FrameBuffer->Clear(ClearColor);
    }

    void DrawMesh(const Mesh& mesh, const mat4f& modelMat){
        mat4f rMVP = rasterizationMat * MainCamera->GetVPMatrix() * modelMat;

        // if(!MainCamera->IntersectsFrustrum(mesh.Volume, modelMat)){
        //     return;
        // }

        for(int i = 0; i < mesh.PolygonCount; i++){
            uint32_t idx = i * 3;
            // vec3f ve = mesh.Vertices[mesh.Indices[idx]].Position;
            // vec3f vf = mesh.Vertices[mesh.Indices[idx+1]].Position;
            // vec3f vg = mesh.Vertices[mesh.Indices[idx+2]].Position;

            // printf("ve: %f %f %f\n", ve.x(), ve.y(), ve.z());
            // printf("vf: %f %f %f\n", vf.x(), vf.y(), vf.z());
            // printf("vg: %f %f %f\n", vg.x(), vg.y(), vg.z());

            vec3f v1 = (rMVP * vec4f(mesh.Vertices[mesh.Indices[idx]].Position, 1)).homogenize();
            vec3f v2 = (rMVP * vec4f(mesh.Vertices[mesh.Indices[idx+1]].Position, 1)).homogenize();
            vec3f v3 = (rMVP * vec4f(mesh.Vertices[mesh.Indices[idx+2]].Position, 1)).homogenize();

            vec4f vtest = rMVP * vec4f(mesh.Vertices[mesh.Indices[idx]].Position, 1);

            // printf("v1: %f %f %f\n", v1.x(), v1.y(), v1.z());
            // printf("v2: %f %f %f\n", v2.x(), v2.y(), v2.z());
            // printf("v3: %f %f %f\n", v3.x(), v3.y(), v3.z());

            FrameBuffer->DrawTriangle(v1, v2, v3, 0xFF00FF00);
        }
    }
};