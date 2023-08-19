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

extern const Color16 earth_png[80000];
extern const Color16 mars_png[80000];
extern const Color16 moon_png[80000];

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

#define LINE_SIZE 20

struct Body : public Object {
    vec3<float> Velocity;
    vec3<float> RotationalVelocity;
    float Mass;
    Material* _Material;
    vec3f LinePoints[LINE_SIZE];
};

Camera& cam = Renderer::MainCamera;
Mesh planetMesh = Mesh((Vertex*)&sphere_obj_vertices, 1080, (uint32_t*)&sphere_obj_indices, 1080/3);
Texture2D earth = Texture2D((Color16*)&earth_png, 400, 200);
Texture2D mars = Texture2D((Color16*)&mars_png, 400, 200);
Texture2D moon = Texture2D((Color16*)&moon_png, 400, 200);
TextureShader t = TextureShader();
FlatShader f = FlatShader();

Body planets[3];

fixed yaw, pitch;

const float G = 1;

void game_init(){
    printf("Initializing game\n");
    Material* earthMat = new Material(t);
    ((TextureShader::Parameters*)earthMat->Parameters)->_Texture = &earth;
    ((TextureShader::Parameters*)earthMat->Parameters)->TextureScale = vec2f(1);

    Material* marsMat = new Material(t);
    ((TextureShader::Parameters*)marsMat->Parameters)->_Texture = &mars;
    ((TextureShader::Parameters*)marsMat->Parameters)->TextureScale = vec2f(1);

    Material* moonMat = new Material(t);
    ((TextureShader::Parameters*)moonMat->Parameters)->_Texture = &moon;
    ((TextureShader::Parameters*)moonMat->Parameters)->TextureScale = vec2f(1);

    planets[0]._Material = earthMat;
    planets[0].Mass = 1;
    planets[0].Velocity = vec3<float>(0);
    planets[1]._Material = marsMat;
    planets[1].SetPosition(vec3f(5, 0, 0));
    planets[1].Enabled = false;
    planets[2]._Material = moonMat;
    planets[2].SetPosition(vec3f(0, 0, 5));
    planets[2].SetScale(vec3f(0.5, 0.5, 0.5));
    planets[2].Mass = 0.1;
    planets[2].Velocity = vec3<float>(0.5, 0, 0);

    // planets[2].Velocity = vec3f(0, 0, sqrt(G * (planets[0].Mass + planets[2].Mass) / (planets[2].GetPosition() - planets[0].GetPosition()).magnitude() / 2));

    printf("Finished initializing game\n");
}

int targetPlanet = 0;

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
        while(!planets[++targetPlanet].Enabled);
    }

    cam.SetPosition(planets[targetPlanet].GetPosition() - camForward * 5fp);
}

void game_render(){
    Renderer::Clear(Color::Black);
    FlatShader f = FlatShader();
    Material flatMat = Material(f);
    ((FlatShader::Parameters*)flatMat.Parameters)->_Color = Color::White;
    
    for(Body& planet : planets){
        if(!planet.Enabled) continue;
        Renderer::DrawMesh(planetMesh, planet.GetModelMatrix(), *planet._Material);

        Renderer::DrawLine(planet.GetPosition(), planet.LinePoints[0], Color::White, 1);
        for(int i = 0; i < LINE_SIZE-1; i++){
            Renderer::DrawLine(planet.LinePoints[i], planet.LinePoints[i+1], Color::White, 1);
        }
    }

    // vec3f upV = mat4f::euler(planets[0].Rotation).col(1).xyz();
    // vec3f rightV = mat4f::euler(planets[0].Rotation).col(0).xyz();
    // vec3f forwardV = mat4f::euler(planets[0].Rotation).col(2).xyz();
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + upV, Color::Yellow, 2);
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + rightV, Color::Blue, 2);
    // Renderer::DrawLine(planets[0].Position, planets[0].Position + forwardV, Color::Cyan, 2);
}