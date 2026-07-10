#pragma once
#include "renderer/Mesh.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "renderer/Primitive.h"
#include "renderer/IBL.h"
#include "renderer/Environment.h"
#include "scene/Scene.h"
#include "scenes/BaseScene.h"

class Window;

struct SkyAtmosphereParameters
{
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, 0.55f, -0.84f));
    float lightIntensity = 40.0f;
    float cameraHeight = 100.0f;
    float planetRadius = 6371000.0f;
    float atmosphereRadius = 6471000.0f;
    glm::vec3 rayleighBeta = glm::vec3(5.5e-6f, 13.0e-6f, 22.4e-6f);
    glm::vec3 mieBeta = glm::vec3(21.0e-6f);
    glm::vec3 absorptionBeta = glm::vec3(2.04e-5f, 4.97e-5f, 1.95e-6f);
    float rayleighHeight = 8000.0f;
    float mieHeight = 1200.0f;
    float absorptionHeight = 30000.0f;
    float absorptionFalloff = 4000.0f;
    float mieG = 0.7f;
    float exposure = 1.0f;
    float gamma = 2.2f;
};

class SkyAtmosphere : public BaseScene
{
public:
    void Load(Window& window);
    void Render(Camera& camera);
    void Unload();
    void RenderEditor(Editor& editor);
    std::string GetName() { return this->name; }

    GameObject* GetMainGameObject() { return nullptr; }

    // Shaders
    Shader shader_sky_atmosphere = Shader("shaders/sky_atmosphere.vert", "shaders/sky_atmosphere.frag");
    SkyAtmosphereParameters parameters;

    IBL ibl;
    Environment env_map;
private:
    std::string name = "sky_atmosphere";

};
