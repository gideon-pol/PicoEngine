#include <cstring>
#include <iostream>

#include "common.h"
#include "time.hpp"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/color.h"
#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/postprocessing.h"
#include "hardware/input.h"
#include "ecs/object.h"

#include "game/shaders.h"

extern const Vertex sphere_obj_vertices[1080];
extern const uint32_t sphere_obj_indices[1080];

extern const Vertex suzanne_obj_vertices[2904];
extern const uint32_t suzanne_obj_indices[2904];

extern const Color16 mercury_png[80000];
extern const Color16 venus_png[80000];
extern const Color16 earth_png[80000];
extern const Color16 moon_png[80000];
extern const Color16 mars_png[80000];
extern const Color16 jupiter_png[80000];
extern const Color16 saturn_png[80000];
extern const Color16 uranus_png[80000];
extern const Color16 neptune_png[80000];

extern const Color16 flare_png[22800];

#define LINE_SIZE 10
struct Body : public Object {
    vec3<float> Velocity;
    vec3f RotationalVelocity;
    float Mass;
    Material* _Material;
    vec3f LinePoints[LINE_SIZE];
    Body* Parent;
    bool Render = true;
    vec3<float> D;
    char Name[32];
};

Vertex quadVerts[] = {
    (Vertex){vec3f(-1, -1, 0), vec3f(0), vec2f(0, 0)},
    (Vertex){vec3f(1, -1, 0), vec3f(0), vec2f(1, 0)},
    (Vertex){vec3f(-1, 1, 0), vec3f(0), vec2f(0, 1)},
    (Vertex){vec3f(1, 1, 0), vec3f(0), vec2f(1, 1)},
};

uint32_t quadIndices[] = {
    0, 1, 2,
    2, 1, 3,
};

Mesh quad = Mesh((Vertex*)&quadVerts, 4, (uint32_t*)&quadIndices, 2);

Camera& cam = Renderer::MainCamera;
Mesh sphere = Mesh((Vertex*)&sphere_obj_vertices, 1080, (uint32_t*)&sphere_obj_indices, 1080/3);
Mesh suzanne = Mesh((Vertex*)&suzanne_obj_vertices, 2904, (uint32_t*)&suzanne_obj_indices, 2904/3);

Texture2D mercury = Texture2D((Color16*)&mercury_png, 400, 200);
Texture2D venus = Texture2D((Color16*)&venus_png, 400, 200);
Texture2D earth = Texture2D((Color16*)&earth_png, 400, 200);
Texture2D moon = Texture2D((Color16*)&moon_png, 400, 200);
Texture2D mars = Texture2D((Color16*)&mars_png, 400, 200);
Texture2D jupiter = Texture2D((Color16*)&jupiter_png, 400, 200);
Texture2D saturn = Texture2D((Color16*)&saturn_png, 400, 200);
Texture2D uranus = Texture2D((Color16*)&uranus_png, 400, 200);
Texture2D neptune = Texture2D((Color16*)&neptune_png, 400, 200);

FlatShader debug = FlatShader();

Texture2D flare = Texture2D((Color16*)&flare_png, 200, 114);
TextureShader t = TextureShader();
PlanetShader p = PlanetShader();
FastPlanetShader f = FastPlanetShader();
RainbowTestShader r = RainbowTestShader();
FlatLightingShader l = FlatLightingShader();
SmoothLightingShader s = SmoothLightingShader();

Body planets[10];

fixed yaw, pitch;

const float G = 0.1f;

Color lightColor = Color(255, 255, 255, 255);

void game_init(){
    printf("Initializing game\n");

{
    Material* mercuryMat = new Material(p);
    SHADER_PARAMS(PlanetShader, mercuryMat)->_Texture = &mercury;
    SHADER_PARAMS(PlanetShader, mercuryMat)->LightColor = lightColor;

    Material* venusMat = new Material(p);
    SHADER_PARAMS(PlanetShader, venusMat)->_Texture = &venus;
    SHADER_PARAMS(PlanetShader, venusMat)->LightColor = lightColor;

    Material* earthMat = new Material(p);
    SHADER_PARAMS(PlanetShader, earthMat)->_Texture = &earth;
    SHADER_PARAMS(PlanetShader, earthMat)->LightColor = lightColor;

    Material* moonMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, moonMat)->_Texture = &moon;
    SHADER_PARAMS(FastPlanetShader, moonMat)->LightColor = lightColor;

    Material* marsMat = new Material(p);
    SHADER_PARAMS(PlanetShader, marsMat)->_Texture = &mars;
    SHADER_PARAMS(PlanetShader, marsMat)->LightColor = lightColor;

    Material* jupiterMat = new Material(p);
    SHADER_PARAMS(PlanetShader, jupiterMat)->_Texture = &jupiter;
    SHADER_PARAMS(PlanetShader, jupiterMat)->LightColor = lightColor;

    Material* saturnMat = new Material(p);
    SHADER_PARAMS(PlanetShader, saturnMat)->_Texture = &saturn;
    SHADER_PARAMS(PlanetShader, saturnMat)->LightColor = lightColor;

    Material* uranusMat = new Material(p);
    SHADER_PARAMS(PlanetShader, uranusMat)->_Texture = &uranus;
    SHADER_PARAMS(PlanetShader, uranusMat)->LightColor = lightColor;

    Material* neptuneMat = new Material(p);
    SHADER_PARAMS(PlanetShader, neptuneMat)->_Texture = &neptune;
    SHADER_PARAMS(PlanetShader, neptuneMat)->LightColor = lightColor;
}
        
    Material* mercuryMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, mercuryMat)->_Texture = &mercury;
    SHADER_PARAMS(FastPlanetShader, mercuryMat)->LightColor = lightColor;

    Material* venusMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, venusMat)->_Texture = &venus;
    SHADER_PARAMS(FastPlanetShader, venusMat)->LightColor = lightColor;

    Material* earthMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, earthMat)->_Texture = &earth;
    SHADER_PARAMS(FastPlanetShader, earthMat)->LightColor = lightColor;

    Material* moonMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, moonMat)->_Texture = &moon;
    SHADER_PARAMS(FastPlanetShader, moonMat)->LightColor = lightColor;

    Material* marsMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, marsMat)->_Texture = &mars;
    SHADER_PARAMS(FastPlanetShader, marsMat)->LightColor = lightColor;

    Material* jupiterMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, jupiterMat)->_Texture = &jupiter;
    SHADER_PARAMS(FastPlanetShader, jupiterMat)->LightColor = lightColor;

    Material* saturnMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, saturnMat)->_Texture = &saturn;
    SHADER_PARAMS(FastPlanetShader, saturnMat)->LightColor = lightColor;

    Material* uranusMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, uranusMat)->_Texture = &uranus;
    SHADER_PARAMS(FastPlanetShader, uranusMat)->LightColor = lightColor;

    Material* neptuneMat = new Material(f);
    SHADER_PARAMS(FastPlanetShader, neptuneMat)->_Texture = &neptune;
    SHADER_PARAMS(FastPlanetShader, neptuneMat)->LightColor = lightColor;

    Material* debugMat = new Material(r);

    planets[0].Mass = 1000;
    planets[0].SetPosition(vec3f(0, 0, 0));
    planets[0].Render = false;
    strcpy(planets[0].Name, "Sun");

    planets[1]._Material = mercuryMat;
    planets[1].Mass = 0.0553;
    planets[1].SetScale(vec3f(0.38));
    planets[1].SetPosition(vec3f(0, 0, 39));
    planets[1].SetRotation(Quaternion::Euler(vec3f(7.01, 0, 0)));
    planets[1].RotationalVelocity = vec3f(0, 0.086, 0);
    planets[1].Parent = nullptr;
    planets[1].Enabled = true;
    strcpy(planets[1].Name, "Mercury");

    planets[2]._Material = venusMat;
    planets[2].Mass = 0.815;
    planets[2].SetScale(vec3f(0.95));
    planets[2].SetPosition(vec3f(0, 0, 72));
    planets[2].SetRotation(Quaternion::Euler(vec3f(17.74, 0, 0)));
    planets[2].RotationalVelocity = vec3f(0, 0.02, 0);
    planets[2].Parent = nullptr;
    planets[2].Enabled = true;
    strcpy(planets[2].Name, "Venus");

    planets[3]._Material = earthMat;
    planets[3].Mass = 1;
    planets[3].SetScale(vec3f(1));
    planets[3].SetPosition(vec3f(0, 0, 100));
    planets[3].SetRotation(Quaternion::Euler(vec3f(23, 0, 0)));
    planets[3].RotationalVelocity = vec3f(0, 5, 0);
    planets[3].Parent = nullptr;
    planets[3].Enabled = true;
    strcpy(planets[3].Name, "Earth");

    planets[4]._Material = moonMat;
    planets[4].Mass = 0.0123;
    planets[4].SetScale(vec3f(0.27));
    planets[4].SetPosition(vec3f(0, 0, 104));
    planets[4].SetRotation(Quaternion::Euler(vec3f(5.14, 0, 0)));
    planets[4].Parent = &planets[3];
    planets[4].Enabled = true;
    strcpy(planets[4].Name, "Moon");

    planets[5]._Material = marsMat;
    planets[5].Mass = 0.107;
    planets[5].SetScale(vec3f(0.53));
    planets[5].SetPosition(vec3f(0, 0, 152));
    planets[5].SetRotation(Quaternion::Euler(vec3f(25.19, 0, 0)));
    planets[5].RotationalVelocity = vec3f(0, 4.9, 0);
    planets[5].Parent = nullptr;
    planets[5].Enabled = true;
    strcpy(planets[5].Name, "Mars");

    planets[6]._Material = jupiterMat;
    planets[6].Mass = 317.8;
    planets[6].SetScale(vec3f(11.2));
    planets[6].SetPosition(vec3f(0, 0, 520));
    planets[6].SetRotation(Quaternion::Euler(vec3f(3.13, 0, 0)));
    planets[6].RotationalVelocity = vec3f(0, 12, 0);
    planets[6].Parent = nullptr;
    planets[6].Enabled = true;
    strcpy(planets[6].Name, "Jupiter");

    planets[7]._Material = saturnMat;
    planets[7].Mass = 95.2;
    planets[7].SetScale(vec3f(9.45));
    planets[7].SetPosition(vec3f(0, 0, 954));
    planets[7].SetRotation(Quaternion::Euler(vec3f(26.73, 0, 0)));
    planets[7].RotationalVelocity = vec3f(0, 11.4, 0);
    planets[7].Parent = nullptr;
    planets[7].Enabled = true;
    strcpy(planets[7].Name, "Saturn");

    planets[8]._Material = uranusMat;
    planets[8].Mass = 14.5;
    planets[8].SetScale(vec3f(4));
    planets[8].SetPosition(vec3f(0, 0, 1920));
    planets[8].SetRotation(Quaternion::Euler(vec3f(97.77, 0, 0)));
    planets[8].RotationalVelocity = vec3f(0, 7.1, 0);
    planets[8].Parent = nullptr;
    planets[8].Enabled = true;
    strcpy(planets[8].Name, "Uranus");

    planets[9]._Material = neptuneMat;
    planets[9].Mass = 17.1;
    planets[9].SetScale(vec3f(3.88));
    planets[9].SetPosition(vec3f(0, 0, 3006));
    planets[9].SetRotation(Quaternion::Euler(vec3f(28.32, 0, 0)));
    planets[9].RotationalVelocity = vec3f(0, 7.5, 0);
    planets[9].Parent = nullptr;
    planets[9].Enabled = true;
    strcpy(planets[9].Name, "Neptune");

    for(int i = 1; i < sizeof(planets)/sizeof(Body); i++){
        if(!planets[i].Enabled) continue;

        Body* target = &planets[0];

        if(planets[i].Parent){
            planets[i].Velocity = planets[i].Parent->Velocity;
            target = planets[i].Parent;
        }

        fixed distance = (planets[i].GetPosition() - target->GetPosition()).magnitude();
        float v = sqrt(G * (target->Mass + planets[i].Mass) / SCAST<float>(distance));
        printf("Orbital velocity for %s: %f\n", planets[i].Name, v);
        planets[i].Velocity += vec3f(v, 0, 0);
    }

    for(Body& planet : planets){
        if(!planet.Enabled) continue;
        for(int i = 0; i < LINE_SIZE; i++){
            planet.LinePoints[i] = planet.GetPosition();
        }
    }

    PostProcessing::Bloom* bloom = new PostProcessing::Bloom();

    PostProcessing::Colorize* colorize = new PostProcessing::Colorize(new Color[6]{
        Color::Black,
        Color::Blue,
        Color::Green,
        Color::Cyan,
        Color::White
    }, 6);

    // PostProcessing::Add(colorize);
    PostProcessing::Add(bloom);

    printf("Finished initializing game\n");
}

int targetPlanet = 3;

enum CameraMode {
    Close,
    Medium,
    Far,
};

CameraMode camMode = CameraMode::Medium;
fixed camDistance = 2fp;
fixed targetCamDistance = 2fp;

void game_update(){
    printf("Delta time: %f\n", Time::GetDeltaTime());

    yaw -= Input::GetAxis(Input::Axis::X) * 50fp * Time::GetDeltaTime();
    pitch += Input::GetAxis(Input::Axis::Y) * 50fp * Time::GetDeltaTime();

    yaw %= 360fp;
    pitch = clamp(pitch, -90fp, 90fp);

    for(int i = 0; i < sizeof(planets)/sizeof(Body); i++){
        planets[i].D = vec3f(0);
    }

    for(int i = 0; i < sizeof(planets)/sizeof(Body); i++){
        if(!planets[i].Enabled) continue;

        for(int j = i+1; j < sizeof(planets)/sizeof(Body); j++){
            if(!planets[j].Enabled) continue;

            vec3<float> direction = planets[j].GetPosition() - planets[i].GetPosition();
            float distance = direction.magnitude();
            if(distance == 0) continue;

            float g = (planets[i].Mass * planets[j].Mass) / (distance * distance) * G;
            vec3<float> force = direction.normalize() * g * Time::GetDeltaTime();
            planets[i].Velocity += force / planets[i].Mass;
            planets[j].Velocity -= force / planets[j].Mass;
            planets[i].D += force / planets[i].Mass;
            planets[j].D -= force / planets[j].Mass;
        }
    }

    for(Body& planet : planets){
        if(!planet.Enabled) continue;

        planet.Translate(planet.Velocity * Time::GetDeltaTime());
        planet.Rotate(planet.RotationalVelocity * Time::GetDeltaTime());
    }

    for(int i = 1; i < sizeof(planets)/sizeof(Body); i++){
        if(!planets[i].Enabled || !planets[i].Render) continue;
        if(planets[i]._Material->_Shader._ID == PlanetShader::ID){
            SHADER_PARAMS(PlanetShader, planets[i]._Material)->DirectionToLight = (planets[0].GetPosition() - planets[i].GetPosition()).normalize();
        } else if(planets[i]._Material->_Shader._ID == FastPlanetShader::ID){
            SHADER_PARAMS(FastPlanetShader, planets[i]._Material)->DirectionToLight = (planets[0].GetPosition() - planets[i].GetPosition()).normalize();
        }   
    }

    if(Time::GetFrameCount() % 50 == 0){
        for(Body& planet : planets){
            if(!planet.Enabled) continue;
            for(int i = LINE_SIZE-1; i > 0 ; i--){
                planet.LinePoints[i] = planet.LinePoints[i-1];
            }

            planet.LinePoints[0] = planet.GetPosition();
        }
    }

    cam.SetRotation(Quaternion::Euler(vec3f(pitch, yaw, 0)));

    vec3f camForward = cam.GetModelMatrix()(2).xyz();

    if(Input::GetButtonPress(Input::Button::Stick)){
        while(!planets[(++targetPlanet%=10)].Enabled);
    }

    if(Input::GetButtonDown(Input::Button::A)){
        targetCamDistance = clamp(targetCamDistance - 0.05fp, 1.1fp, 20fp);
    }

    if(Input::GetButtonDown(Input::Button::B)){
        targetCamDistance = clamp(targetCamDistance + 0.05fp, 1.1fp, 20fp);
    }

    camDistance = lerp(camDistance, targetCamDistance * targetCamDistance, Time::GetDeltaTime());

    cam.SetPosition(planets[targetPlanet].GetPosition() - camForward * camDistance * planets[targetPlanet].GetScale().x());
}

void game_mesh_render(){
    // static TextureShader t = TextureShader();
    // static Material starsMat = Material(t);
    // ((TextureShader::Parameters*)starsMat.Parameters)->_Texture = &mars;
    // ((TextureShader::Parameters*)starsMat.Parameters)->TextureScale = vec2f(1);

    // Renderer::DrawMesh(sphere, mat4f::translate(cam.GetPosition()) * mat4f::scale(vec3f(100)), starsMat, Culling::Front, DepthTest::Less);
    // Renderer::Finish();
    // return;

    Time::Profiler::Enter("Draw");

    vec3f sunPos = Renderer::WorldToScreen(planets[0].GetPosition());

    if(sunPos.z() < 1 && sunPos.z() >= 0){
        Renderer::Blit(flare, vec2i16(sunPos.xy()) - vec2i16(flare.Width, flare.Height) / 2);
    }
    
    for(Body& planet : planets){
        if(!planet.Enabled || !planet.Render) continue;

        fixed dst = (planet.GetPosition() - cam.GetPosition()).magnitude();

        // if(dst > 200) continue;

        Renderer::Submit(DrawCall(sphere, planet.GetModelMatrix(), *planet._Material));
    }
}

void game_ui_render(){
    for(Body& planet : planets){
        if(!planet.Enabled || !planet.Render) continue;
        fixed dst = (planet.GetPosition() - cam.GetPosition()).magnitude();

        if(dst > 200) continue;

        // Renderer::Debug::DrawOrientation(planet.GetModelMatrix());
        Renderer::DrawLine(planet.GetPosition(), planet.GetPosition() + planet.D * 5000, Color::Cyan, 1, DepthTest::Never);

        Renderer::DrawLine(planet.GetPosition(), planet.LinePoints[0], Color::White, 1, DepthTest::Never);
        for(int i = 0; i < LINE_SIZE-1; i++){
            Renderer::DrawLine(planet.LinePoints[i], planet.LinePoints[i+1], Color::White, 1, DepthTest::Never);
        }
    }

    Renderer::DrawText(planets[targetPlanet].Name, vec2i16(0), Color::White);

    int fps = 1 / Time::GetDeltaTime();

    char str[32];
    snprintf(str, 32, "FPS: %d", fps);
    Renderer::DrawText(str, vec2i16(0, 10), Color::White);

    snprintf(str, 32, "Draw time: %.4f", Time::Profiler::GetSectionMicroseconds("DrawMesh") / 1000000.0f);
    Renderer::DrawText(str, vec2i16(0, 20), Color::Cyan);

    snprintf(str, 32, "Post proc...: %.4f", Time::Profiler::GetSectionMicroseconds("PostProcessing") / 1000000.0f);
    Renderer::DrawText(str, vec2i16(0, 30), Color::Yellow);

    snprintf(str, 32, "Cam dst: %.0fk km", SCAST<float>(camDistance * planets[targetPlanet].GetScale().x() * 6.371f));
    Renderer::DrawText(str, vec2i16(0, 115), Color::White);

    Time::Profiler::Exit("Draw");

    Time::Profiler::Print();
    Time::Profiler::Reset();
}