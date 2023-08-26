#pragma once

#include "rendering/color.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"

struct TriangleShaderData {
    const mat4f& ModelMatrix;
    const Vertex v1;
    const Vertex v2;
    const Vertex v3;
    // This value is not set, but it can be generated by the vertex program
    // It is used to interpolate the triangle color and therefore does not
    // need the invocation of a program for every pixel.
    Color TriangleColor;
};

struct FragmentShaderData {
    const vec3f Position;
    // This is for now a face normal
    const vec3f Normal;
    const vec3f FragCoord;
    const vec2f UV;
    const vec2i16 ScreenSize;
    Color FragmentColor;
};

enum ShaderType { NoShader, Flat, Texture, Custom };

class Shader {
public:
    ShaderType Type;
    // The triangle program runs for every triangle, regardless of visibility!
    void (*TriangleProgram)(TriangleShaderData& input, void* parameters);
    void (*FragmentProgram)(FragmentShaderData& input, void* parameters);

    virtual void* CreateParameters(){
        return nullptr;
    }
};

class FlatShader : public Shader {
public:
    struct Parameters {
        Color _Color;
    };

    FlatShader(){
        // TODO: Test whether this is (significantly) slower than an engine implemented shader
        // Type = ShaderType::Custom;
        // TriangleProgram = [](TriangleShaderData& i, void* p){
        //     i.TriangleColor = ((Parameters*)p)->_Color;
        // };
        Type = ShaderType::Flat;
        TriangleProgram = nullptr;
        FragmentProgram = nullptr;
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

// TODO: this shader is probably so common that it should be engine implemented
class LightingShader : public Shader {
public:
    enum ShadingType { Flat, Smooth };
    struct Parameters {
        vec3f LightDirection;
        Color LightColor;
        Color AmbientColor;
    };

    LightingShader(ShadingType type){
        Type = ShaderType::Custom;
        TriangleProgram = nullptr;

        if(type == ShadingType::Flat){
            FragmentProgram = nullptr;
            TriangleProgram = [](TriangleShaderData& data, void* parameters){
                Parameters* params = (Parameters*)parameters;
                vec3f normal = (data.v2.Position - data.v1.Position).cross(data.v3.Position - data.v1.Position).normalize();
                normal = (data.ModelMatrix * vec4f(normal, 0)).xyz().normalize();
                fixed diff = max(normal.dot(-params->LightDirection), 0fp);
                data.TriangleColor = Color(
                                        SCAST<uint8_t>(fixed(params->LightColor.r) * diff),
                                        SCAST<uint8_t>(fixed(params->LightColor.g) * diff),
                                        SCAST<uint8_t>(fixed(params->LightColor.b) * diff),
                                        255
                                    );
            };
        } else if(type == ShadingType::Smooth){
            TriangleProgram = nullptr;
            FragmentProgram = [](FragmentShaderData& data, void* parameters){
                Parameters* params = (Parameters*)parameters;
                fixed diff = max(data.Normal.dot(-params->LightDirection), 0fp);
                data.FragmentColor = Color(
                                        SCAST<uint8_t>(fixed(params->LightColor.r) * diff),
                                        SCAST<uint8_t>(fixed(params->LightColor.g) * diff),
                                        SCAST<uint8_t>(fixed(params->LightColor.b) * diff),
                                        255
                                    );
            };
        } 
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

class RainbowTestShader : public Shader {
public:
    RainbowTestShader(){
        Type = ShaderType::Custom;
        TriangleProgram = nullptr;

        FragmentProgram = [](FragmentShaderData& data, void* parameters){
            vec2f uv = data.FragCoord.xy() / vec2f(data.ScreenSize);
            fixed h = uv(0) * 360fp;
            data.FragmentColor = Color::FromHSV((float)h, 1, 1);
        };
    }

    void* CreateParameters(){
        return nullptr;
    }
};

class TextureShader : public Shader {
public:
    struct Parameters {
        Texture2D* _Texture;
        vec2f TextureScale;
    };

    TextureShader(){
        Type = ShaderType::Texture;
        TriangleProgram = nullptr;
        // Engine implemented version is 20% to 30% faster 
        FragmentProgram = nullptr;
        // FragmentProgram = [](FragmentShaderData& data, void* parameters){
        //     Parameters* params = (Parameters*)parameters;
        //     data.FragmentColor = params->_Texture->Sample(data.UV);
        // };
    }

    void* CreateParameters(){
        return new Parameters();
    }
};

struct Material {
    Shader& _Shader;
    void* Parameters;

    Material(Shader& shader) : _Shader(shader){
        Parameters = _Shader.CreateParameters();
    }
};