#pragma once

#include "rendering/color.h"

typedef struct VertexShaderInput {
    vec3f Position;
    // This is for now a face normal
    vec3f Normal;
};

typedef struct FragmentShaderInput {
    // This is for now a face normal
    vec3f Normal;
    vec2f UV;
};

enum ShaderType { WireFrame, Flat, Texture, CustomTriangle, CustomFragmented };

class Shader {
public:
    ShaderType Type;
    void (*vertex_shader)(VertexShaderInput& input, void* parameters);
    Color (*fragment_shader)(FragmentShaderInput& input, void* parameters);

    virtual void* CreateParameters(){
        return nullptr;
    }
};

class FlatShader : public Shader {
public:
    typedef struct {
        Color _Color;
    } Parameters;

    FlatShader(){
        Type = ShaderType::CustomTriangle;
        vertex_shader = nullptr;

        fragment_shader = [](FragmentShaderInput& input, void* parameters){
            Parameters* params = (Parameters*)parameters;
            return params->_Color;
        };
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

class LightingShader : public Shader {
public:
    typedef struct {
        vec3f LightPosition;
        Color LightColor;
        Color AmbientColor;
    } Parameters;

    LightingShader(){
        Type = ShaderType::CustomTriangle;
        vertex_shader = nullptr;

        fragment_shader = [](FragmentShaderInput& input, void* parameters){
            Parameters* params = (Parameters*)parameters;
            vec3f lightDir = -params->LightPosition.normalize();
            float diff = input.Normal.dot(lightDir);
            Color diffuse = Color(
                                static_cast<uint8_t>(params->LightColor.r * diff),
                                static_cast<uint8_t>(params->LightColor.g * diff),
                                static_cast<uint8_t>(params->LightColor.b * diff),
                                255
                            );
            return diffuse;
        };
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

typedef struct Material {
    Shader& _Shader;
    void* Parameters;

    Material(Shader& shader) : _Shader(shader){
        Parameters = _Shader.CreateParameters();
    }
};