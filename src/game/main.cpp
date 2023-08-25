#include <cstring>
#include "common.h"
#include "time.hpp"
#include "mathematics.h"
#include "rendering/texture.h"
#include "rendering/color.h"
#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "hardware/input.h"
#include "ecs/object.h"

extern const Vertex sphere_obj_vertices[1080];
extern const uint32_t sphere_obj_indices[1080];

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
class PlanetShader : Shader {
    public:
    struct Parameters {
        Texture2D* _Texture;
        vec3f LightPosition;
        Color LightColor;
    };

    PlanetShader(){
        Type = ShaderType::Custom;
        TriangleProgram = [](TriangleShaderData& i, void* p){
            // caluculate normal
            Parameters* parameters = (Parameters*)p;
            vec3f normal = (i.v2.Position - i.v1.Position).cross(i.v3.Position - i.v1.Position).normalize();
            // fixed diff = clamp(dot(normal, (parameters->LightPosition - i.v1.Position).normalize()), 0fp, 1fp);

            
            // i.TriangleColor = parameters->LightColor
        };
        FragmentProgram = [](FragmentShaderData& i, void* p){
            // Texture2D* texture = ((Parameters*)p)->_Texture;
            // vec3f lightPosition = ((Parameters*)p)->LightPosition;
            // Color lightColor = ((Parameters*)p)->LightColor;

            // vec3f normal = i.Normal.normalize();
            // vec3f lightDirection = (lightPosition - i.FragCoord).normalize();
            // fixed lightIntensity = clamp(dot(normal, lightDirection), 0fp, 1fp);

            // Color textureColor = texture->Sample(i.UV);
            // i.FragmentColor = textureColor * lightColor * lightIntensity;
        };
    }
    
    Parameters* CreateParameters() override {
        return new Parameters();
    }
};

#define LINE_SIZE 5

struct Body : public Object {
    vec3<float> Velocity;
    vec3<float> RotationalVelocity;
    float Mass;
    Material* _Material;
    vec3f LinePoints[LINE_SIZE];
    bool Render = true;
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

Texture2D mercury = Texture2D((Color16*)&mercury_png, 400, 200);
Texture2D venus = Texture2D((Color16*)&venus_png, 400, 200);
Texture2D earth = Texture2D((Color16*)&earth_png, 400, 200);
Texture2D moon = Texture2D((Color16*)&moon_png, 400, 200);
Texture2D mars = Texture2D((Color16*)&mars_png, 400, 200);
// Texture2D jupiter = Texture2D((Color16*)&jupiter_png, 400, 200);
// Texture2D saturn = Texture2D((Color16*)&saturn_png, 400, 200);
// Texture2D uranus = Texture2D((Color16*)&uranus_png, 400, 200);
// Texture2D neptune = Texture2D((Color16*)&neptune_png, 400, 200);


Texture2D flare = Texture2D((Color16*)&flare_png, 200, 114);
TextureShader t = TextureShader();
FlatShader f = FlatShader();

Body planets[10];

fixed yaw, pitch;

const float G = 1;

void game_init(){
    printf("Initializing game\n");

    Material* mercuryMat = new Material(t);
    ((TextureShader::Parameters*)mercuryMat->Parameters)->_Texture = &mercury;
    ((TextureShader::Parameters*)mercuryMat->Parameters)->TextureScale = vec2f(1);

    Material* venusMat = new Material(t);
    ((TextureShader::Parameters*)venusMat->Parameters)->_Texture = &venus;
    ((TextureShader::Parameters*)venusMat->Parameters)->TextureScale = vec2f(1);

    Material* earthMat = new Material(t);
    ((TextureShader::Parameters*)earthMat->Parameters)->_Texture = &earth;
    ((TextureShader::Parameters*)earthMat->Parameters)->TextureScale = vec2f(1);

    Material* moonMat = new Material(t);
    ((TextureShader::Parameters*)moonMat->Parameters)->_Texture = &moon;
    ((TextureShader::Parameters*)moonMat->Parameters)->TextureScale = vec2f(1);

    Material* marsMat = new Material(t);
    ((TextureShader::Parameters*)marsMat->Parameters)->_Texture = &mars;
    ((TextureShader::Parameters*)marsMat->Parameters)->TextureScale = vec2f(1);

    // Material* jupiterMat = new Material(t);
    // ((TextureShader::Parameters*)jupiterMat->Parameters)->_Texture = &jupiter;
    // ((TextureShader::Parameters*)jupiterMat->Parameters)->TextureScale = vec2f(1);

    // Material* saturnMat = new Material(t);
    // ((TextureShader::Parameters*)saturnMat->Parameters)->_Texture = &saturn;
    // ((TextureShader::Parameters*)saturnMat->Parameters)->TextureScale = vec2f(1);

    // Material* uranusMat = new Material(t);
    // ((TextureShader::Parameters*)uranusMat->Parameters)->_Texture = &uranus;
    // ((TextureShader::Parameters*)uranusMat->Parameters)->TextureScale = vec2f(1);

    // Material* neptuneMat = new Material(t);
    // ((TextureShader::Parameters*)neptuneMat->Parameters)->_Texture = &neptune;
    // ((TextureShader::Parameters*)neptuneMat->Parameters)->TextureScale = vec2f(1);

    planets[0].Mass = 10000;
    planets[0].SetPosition(vec3f(0, 0, 0));
    planets[0].Render = false;
    strcpy(planets[0].Name, "Sun");

    planets[1]._Material = mercuryMat;
    planets[1].Mass = 0.0553;
    planets[1].SetScale(vec3f(0.38));
    planets[1].SetPosition(vec3f(0, 0, 39));
    strcpy(planets[1].Name, "Mercury");

    planets[2]._Material = venusMat;
    planets[2].Mass = 0.815;
    planets[2].SetScale(vec3f(0.95));
    planets[2].SetPosition(vec3f(0, 0, 72));
    strcpy(planets[2].Name, "Venus");

    planets[3]._Material = earthMat;
    planets[3].Mass = 1;
    planets[3].SetScale(vec3f(1));
    planets[3].SetPosition(vec3f(0, 0, 100));
    planets[3].Velocity = vec3<float>(0);
    strcpy(planets[3].Name, "Earth");

    planets[4]._Material = moonMat;
    planets[4].Mass = 0.0123;
    planets[4].SetScale(vec3f(0.27));
    planets[4].SetPosition(vec3f(0, 0, 104));
    planets[4].Velocity = vec3<float>(0.5, 0, 0);
    strcpy(planets[4].Name, "Moon");

    planets[5]._Material = marsMat;
    planets[5].Mass = 0.107;
    planets[5].SetScale(vec3f(0.53));
    planets[5].SetPosition(vec3f(0, 0, 152));
    strcpy(planets[5].Name, "Mars");

    for(int i = 6; i < 10; i++){
        planets[i].Enabled = false;
    }

    for(int i = 1; i < sizeof(planets)/sizeof(Body); i++){
        // calculate orbital velocity for circular orbit
        fixed distance = (planets[i].GetPosition() - planets[0].GetPosition()).magnitude();
        float v = sqrt(G * (planets[0].Mass + planets[i].Mass) / SCAST<float>(distance));
        printf("Orbital velocity for %s: %f\n", planets[i].Name, v);
        planets[i].Velocity = vec3f(v, 0, 0);
    }

    // planets[5]._Material = jupiterMat;
    // planets[5].Mass = 317.8;
    // planets[5].SetScale(vec3f(11.2));
    // planets[5].SetPosition(vec3f(0, 0, 520));
    // planets[5].Enabled = false;
    
    // strcpy(planets[5].Name, "Jupiter");

    // planets[6]._Material = saturnMat;
    // planets[6].Mass = 95.2;
    // planets[6].SetScale(vec3f(9.45));
    // planets[6].SetPosition(vec3f(0, 0, 954));
    // planets[6].Enabled = false;
    // strcpy(planets[6].Name, "Saturn");

    // planets[7]._Material = uranusMat;
    // planets[7].Mass = 14.5;
    // planets[7].SetScale(vec3f(4));
    // planets[7].SetPosition(vec3f(0, 0, 1920));
    // planets[7].Enabled = false;
    // strcpy(planets[7].Name, "Uranus");

    // planets[8]._Material = neptuneMat;
    // planets[8].Mass = 17.1;
    // planets[8].SetScale(vec3f(3.88));
    // planets[8].SetPosition(vec3f(0, 0, 3006));
    // planets[8].Enabled = false;
    // strcpy(planets[8].Name, "Neptune");

    

    


    // planets[2].Velocity = vec3f(0, 0, sqrt(G * (planets[0].Mass + planets[2].Mass) / (planets[2].GetPosition() - planets[0].GetPosition()).magnitude() / 2));

    printf("Finished initializing game\n");
}

int targetPlanet = 2;

void game_update(){
    printf("Delta time: %f\n", Time::GetDeltaTime());
    yaw += Input::GetAxis(Input::Axis::X) * 50fp * Time::GetDeltaTime();
    pitch += Input::GetAxis(Input::Axis::Y) * 50fp * Time::GetDeltaTime();

    yaw = mod(yaw, 360fp);
    pitch = clamp(pitch, -90fp, 90fp);

    for(int i = 0; i < sizeof(planets)/sizeof(Body); i++){
        if(!planets[i].Enabled) continue;
        for(int j = i+1; j < sizeof(planets)/sizeof(Body); j++){
            if(!planets[j].Enabled) continue;
            if(i == j) continue;
            vec3<float> direction = planets[j].GetPosition() - planets[i].GetPosition();
            float distance = direction.magnitude();
            float g = ((planets[i].Mass * planets[j].Mass) / (distance * distance) * G);
            vec3<float> force = direction.normalize() * g * Time::GetDeltaTime();
            planets[i].Velocity += force / planets[i].Mass;
            planets[j].Velocity -= force / planets[j].Mass;
        }
    }

    for(Body& planet : planets){
        // planets[i].Rotation += planets[i].RotationalVelocity * Time::GetDeltaTime();
        if(!planet.Enabled) continue;
        planet.Translate(planet.Velocity * Time::GetDeltaTime());
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
        while(!planets[(++targetPlanet%=6)].Enabled);
    }

    cam.SetPosition(planets[targetPlanet].GetPosition() - camForward * 5fp);
}

void game_render(){
    Renderer::Clear(Color::Black);

    {
        // Renderer::CullingMode = Culling::None;
        // Renderer::DepthTestMode = DepthTest::Never;

        // static TextureShader t = TextureShader();
        // static Material starsMat = Material(t);
        // ((TextureShader::Parameters*)starsMat.Parameters)->_Texture = &earth;
        // ((TextureShader::Parameters*)starsMat.Parameters)->TextureScale = vec2f(1);

        // Renderer::DrawMesh(sphere, mat4f::translate(cam.GetPosition()), starsMat);

        // Renderer::CullingMode = Culling::Back;
        // Renderer::DepthTestMode = DepthTest::Less;
    }

    vec3f sunPos = Renderer::WorldToScreen(planets[0].GetPosition());

    if(sunPos.z() <= 1 && sunPos.z() >= 0){
        Renderer::Blit(flare, sunPos.xy() - vec2f(flare.Width, flare.Height) / 2);
    }

    for(Body& planet : planets){
        if(!planet.Enabled || !planet.Render) continue;
        Renderer::DrawMesh(sphere, planet.GetModelMatrix(), *planet._Material);

        // Renderer::DrawLine(planet.GetPosition(), planet.LinePoints[0], Color::White, 1);
        // for(int i = 0; i < LINE_SIZE-1; i++){
        //     Renderer::DrawLine(planet.LinePoints[i], planet.LinePoints[i+1], Color::White, 1);
        // }
    }

    Renderer::DrawText(planets[targetPlanet].Name, vec2i16(0), Color::White);
    // vec3f upV = mat4f::euler(planets[0].Rotation).col(1).xyz();
    // vec3f rightV = mat4f::euler(planets[0].Rotation).col(0).xyz();
    // vec3f forwardV = mat4f::euler(planets[0].Rotation).col(2).xyz();
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + upV, Color::Yellow, 2);
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + rightV, Color::Blue, 2);
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + forwardV, Color::Cyan, 2);
}