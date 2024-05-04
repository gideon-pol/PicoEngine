#pragma once

#include "rendering/shader.h"

class PlanetShader : public Shader {
public:
    struct Parameters {
        Texture2D* _Texture;
        Color LightColor;
        vec3f DirectionToLight;
    };

    SHADER_AUTO_ID(PlanetShader){}

    inline void FragmentProgram(FragmentShaderData& data, void* parameters){
        Parameters* params = (Parameters*)parameters;
        Texture2D* texture = params->_Texture;

        vec3f normal = (data.V1.Normal * data.UVW(0) + data.V2.Normal * data.UVW(1) + data.V3.Normal * data.UVW(2));
        normal = (data.ModelMatrix * vec4f(normal, 0)).xyz().normalize();
        vec2f uv = (data.V1.UV * data.UVW(0) + data.V2.UV * data.UVW(1) + data.V3.UV * data.UVW(2));
        fixed diff = clamp(normal.dot(params->DirectionToLight) + 0.25fp, 0.02fp, 1fp);
        data.FragmentColor = texture->Sample(uv);

        data.FragmentColor = Color(
            SCAST<uint8_t>(diff * ((uint16_t)data.FragmentColor.r * params->LightColor.r >> 8)),
            SCAST<uint8_t>(diff * ((uint16_t)data.FragmentColor.g * params->LightColor.g >> 8)),
            SCAST<uint8_t>(diff * ((uint16_t)data.FragmentColor.b * params->LightColor.b >> 8)),
            255
        );
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

class FastPlanetShader : public Shader {
public:
    struct Parameters {
        Texture2D* _Texture;
        Color LightColor;
        vec3f DirectionToLight;

        struct PassThrough {
            Color triangleColor;
        } PassThrough;
    };

    SHADER_AUTO_ID(FastPlanetShader){}

    inline void TriangleProgram(TriangleShaderData& data, void* parameters){
        Parameters* params = (Parameters*)parameters;

        vec3f normal = data.V1.Normal + data.V2.Normal + data.V3.Normal;
        normal = (data.ModelMatrix * vec4f(normal, 0)).xyz().normalize();

        fixed diff = clamp(normal.dot(params->DirectionToLight) + 0.25fp, 0.02fp, 1fp);
        params->PassThrough.triangleColor = Color(
            SCAST<uint8_t>(diff * (uint16_t)params->LightColor.r),
            SCAST<uint8_t>(diff * (uint16_t)params->LightColor.g),
            SCAST<uint8_t>(diff * (uint16_t)params->LightColor.b),
            255
        );
        data.TriangleColor = params->PassThrough.triangleColor;
    }

    inline void FragmentProgram(FragmentShaderData& data, void* parameters){
        Parameters* params = (Parameters*)parameters;
        Texture2D* texture = params->_Texture;

        data.FragmentColor = texture->Sample(data.V1.UV * data.UVW(0) + data.V2.UV * data.UVW(1) + data.V3.UV * data.UVW(2));
        data.FragmentColor = Color(
            SCAST<uint8_t>(SCAST<uint16_t>(fixed(params->PassThrough.triangleColor.r) * data.FragmentColor.r) >> 8),
            SCAST<uint8_t>(SCAST<uint16_t>(fixed(params->PassThrough.triangleColor.g) * data.FragmentColor.g) >> 8),
            SCAST<uint8_t>(SCAST<uint16_t>(fixed(params->PassThrough.triangleColor.b) * data.FragmentColor.b) >> 8),
            255
        );
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

REGISTER_SHADERS(
    (TextureShader)
    (FlatLightingShader)
    (SmoothLightingShader)
    (RainbowTestShader)
    (FlatShader)
    (PlanetShader)
    (FastPlanetShader)
)
