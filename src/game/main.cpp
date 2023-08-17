#include "common.h"
#include "time.hpp"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/color.h"
#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "hardware/input.h"

extern const Vertex sphere_obj_vertices[1080];
extern const uint32_t sphere_obj_indices[1080];

extern const Color16 earth_png[80000];
extern const Color16 mars_png[80000];

struct Body {
    vec3f Position;
    vec3f Rotation;
    vec3f Velocity;
    vec3f RotationalVelocity;
    vec3f Scale;
    fixed Mass;
    Material* _Material;
};

Camera& cam = Renderer::MainCamera;
Mesh planet = Mesh((Vertex*)&sphere_obj_vertices, 1080, (uint32_t*)&sphere_obj_indices, 1080/3);
Texture2D earth = Texture2D(nullptr, 400, 200);
Texture2D mars = Texture2D(nullptr, 400, 200);
TextureShader t = TextureShader();
FlatShader f = FlatShader();

Body planets[2];

fixed yaw, pitch;

void game_init(){
    printf("Initializing game\n");
    Material* earthMat = new Material(f);
    ((FlatShader::Parameters*)earthMat->Parameters)->_Color = Color::Green;
    // ((TextureShader::Parameters*)earthMat->Parameters)->_Texture = &earth;
    // ((TextureShader::Parameters*)earthMat->Parameters)->TextureScale = vec2f(1);

    Material* marsMat = new Material(f);
    ((FlatShader::Parameters*)marsMat->Parameters)->_Color = Color::Blue;
    // ((TextureShader::Parameters*)marsMat->Parameters)->_Texture = &mars;
    // ((TextureShader::Parameters*)marsMat->Parameters)->TextureScale = vec2f(1);

    planets[0] = (Body){
        vec3f(0),
        vec3f(0),
        vec3f(0),
        vec3f(0),
        vec3f(1),
        1fp,
        earthMat
    };

    planets[1] = (Body){
        vec3f(0),
        vec3f(0),
        vec3f(0),
        vec3f(0),
        vec3f(1),
        1fp,
        marsMat
    };

    printf("Finished initializing game\n");
}

void game_update(){
    printf("Delta time: %f\n", Time::GetDeltaTime());
    yaw += Input::GetAxis(Input::Axis::X) * 20fp * Time::GetDeltaTime();
    pitch += Input::GetAxis(Input::Axis::Y) * 20fp * Time::GetDeltaTime();

    yaw = mod(yaw, 360fp);
    pitch = clamp(pitch, -90fp, 90fp);

    // yaw = mod(yaw + fixed(Time::GetDeltaTime()) * 10fp, 360fp);
    // printf("Yaw: %f, Pitch: %f\n", (float)yaw, (float)pitch);

    cam.SetRotation(vec3f(0, yaw, 0));
    // planets[0].Rotation = vec3f(0, yaw, 0);
    vec3f camForward = cam.GetViewMatrix().col(2).xyz();

    // for(int i = 0; i < sizeof(planets)/sizeof(Body); i++){
    //     for(int j = 0; j < sizeof(planets)/sizeof(Body); j++){
    //         if(i == j) continue;

    //         vec3f direction = planets[j].Position - planets[i].Position;
    //         fixed distance = direction.magnitude();
    //         vec3f force = direction.normalize() * (planets[i].Mass * planets[j].Mass) / (distance * distance);
    //         planets[i].Velocity += force * Time::GetDeltaTime();
    //     }
    // }

    // for(int i = 0; i < sizeof(planets)/sizeof(Body); i++){
    //     planets[i].Rotation += planets[i].RotationalVelocity * Time::GetDeltaTime();
    //     planets[i].Position += planets[i].Velocity * Time::GetDeltaTime();
    // }

    cam.SetPosition(planets[0].Position - camForward * 5fp);
    vec3f camPos = cam.GetPosition();
    printf("Camera position: %f, %f, %f\n", (float)camPos(0), (float)camPos(1), (float)camPos(2));
}

void game_render(){
    Renderer::Clear(Color::Red);
    static FlatShader flatShader = FlatShader();
    static Material flatMat = Material(flatShader);
    ((FlatShader::Parameters*)flatMat.Parameters)->_Color = Color::Green;

    for(int i = 0; i < 1 /*sizeof(planets)/sizeof(Body)*/; i++){
        mat4f modelMat = 
            mat4f::translate(planets[i].Position) *
            mat4f::scale(planets[i].Scale) *
            getRotationalMatrix(planets[i].Rotation);
        Renderer::DrawMesh(planet, modelMat, *planets[i]._Material);
    }

    // Renderer::DrawBox(BoundingBox2D(vec2f(yaw,0), vec2f(yaw+10,10)), Color::Green);

    vec3f upV = getRotationalMatrix(planets[0].Rotation).col(1).xyz();
    vec3f rightV = getRotationalMatrix(planets[0].Rotation).col(0).xyz();
    vec3f forwardV = getRotationalMatrix(planets[0].Rotation).col(2).xyz();
    Renderer::DrawLine(planets[0].Position, planets[0].Position + upV, Color::Yellow, 2);
    Renderer::DrawLine(planets[0].Position, planets[0].Position + rightV, Color::Blue, 2);
    Renderer::DrawLine(planets[0].Position, planets[0].Position + forwardV, Color::Cyan, 2);
}