#pragma once
#include "renderer/Mesh.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "renderer/Primitive.h"
#include "scene/GameObject.h"
#include "scene/Scene.h"
#include "scenes/BaseScene.h"

struct VolumetricCloudParameters
{
    int noiseMode = 6;

    float densityScale = 1.0f;
    float extinction = 1.0f;
    float shapeScale = 0.02f;
    float detailScale = 0.10f;
    float erosionStrength = 0.20f;
    glm::vec3 windDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float cloudSpeed = 10.0f;
    float cloudTopOffset = 0.0f;
    float anvilBias = 0.0f;
    float cloudCoverageBlend = 0.0f;

    float cubeNoiseScale = 1.0f;
    float cubeDetailStrength = 0.5f;
    float cubeDensityThreshold = 0.0f;
    float cubeEdgeSoftness = 0.1f;
    float cubeBottomFade = 0.0f;
    float cubeTopFade = 1.0f;

    glm::vec3 lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.85f);
    glm::vec3 ambientLight = glm::vec3(0.6f);
    float lightIntensity = 10.0f;

    int maxSteps = 64;
    int lightSteps = 8;
    float rayJitterStrength = 1.0f;
    float transmittanceCutoff = 0.01f;

    float phaseG = 0.5f;

    glm::vec3 cloudMapVolumeScale = glm::vec3(1000.0f, 60.0f, 1000.0f);
    glm::vec3 cloudMapVolumeTranslation = glm::vec3(0.0f, 200.0f, 0.0f);
};

class VolumetricCloud : public BaseScene
{
public:
    void Load(Window& window) override;
    void Render(RenderContext& context) override;
    void Unload() override;
    void RenderEditor(Editor& editor) override;

    std::string GetName() const override { return name; }
    int GetNoiseMode() const { return parameters.noiseMode; }
    void SetNoiseMode(int mode);
    void SetCloudMapVolumeTransform(const glm::vec3& scale, const glm::vec3& translation);

    VolumetricCloudParameters parameters;
private:
    static bool UsesNoisePreviewScale(int mode);
    void RemoveModeTransform(int mode);
    void ApplyModeTransform(int mode);

    bool LoadNoiseTexture(unsigned int& texture, int channel);
    bool LoadHighFrequencyNoiseTexture();
    bool LoadCloudMapTexture();
    bool LoadHeightMapTexture(unsigned int& texture, const char* fileName);

    static constexpr int noiseTextureSize = 128;
    static constexpr int noiseTextureChannels = 4;
    static constexpr int highFrequencyNoiseTextureSize = 32;
    static constexpr float noisePreviewScale = 1000.0f;

    std::string name = "volumetric_cloud";

    Shader shader_volumetric_cloud = Shader(
        "shaders/volumetric_cloud/cube.vert",
        "shaders/volumetric_cloud/volumetric_cloud.frag");
    Mesh cubeMesh = Primitive::Cube();
    GameObject* test_object = nullptr;
    unsigned int perlinWorleyNoiseTexture = 0;
    unsigned int worleyNoise1 = 0;
    unsigned int worleyNoise2 = 0;
    unsigned int worleyNoise3 = 0;
    unsigned int highFrequencyNoiseTexture = 0;
    unsigned int cloudMapTexture = 0;
    unsigned int cumulusHeightTexture = 0;
    unsigned int cumulonimbusHeightTexture = 0;
    unsigned int stratusHeightTexture = 0;
    unsigned int baseReduceTexture = 0;
};
