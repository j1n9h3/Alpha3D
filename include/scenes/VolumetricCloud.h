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
    float densityScale = 0.8f;
    float extinction = 0.6f;

    bool useLowFreqNoise = true;
    bool useHighFreqNoise = true;
    float erosionStrength = 0.5f;

    float lowFreqNoiseScale = 68.0f;
    float highFreqNoiseScale = 68.0f;

    float cloudMapScale = 1000.0f;


    glm::vec3 windDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float cloudSpeed = 2.0f;
    float cloudTopOffset = 0.0f;
    float anvilBias = 0.0f;
    float cloudCoverageBlend = 1.0f;

    glm::vec3 lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientLight = glm::vec3(0.65f);
    float lightIntensity = 7.0f;

    int primarySteps = 64;
    int lightSteps = 8;
    float rayJitterStrength = 1.0f;
    float transmittanceCutoff = 0.01f;

    float phaseG = 0.5f;

    glm::vec3 cloudMapVolumeScale = glm::vec3(20000.0f, 60.0f, 20000.0f);
    glm::vec3 cloudMapVolumeTranslation = glm::vec3(0.0f, 260.0f, 0.0f);

};

class VolumetricCloud : public BaseScene
{
public:
    void Load(Window& window) override;
    void Render(RenderContext& context) override;
    void Unload() override;
    void RenderEditor(Editor& editor) override;

    std::string GetName() const override { return name; }
    void updateVolumeTransform(const glm::vec3& scale, const glm::vec3& translation);

    VolumetricCloudParameters parameters;
private:
    static bool UsesNoisePreviewScale(int mode);
    bool LoadcloudMapTex();
    bool LoadHeightMapTexture(unsigned int& texture, const char* fileName);

    static constexpr int noiseTextureSize = 128;
    static constexpr int noiseTextureChannels = 4;
    static constexpr int highFreqNoiseTexSize = 32;
    static constexpr float noisePreviewScale = 1000.0f;

    std::string name = "volumetric_cloud";

    Shader shader_volumetric_cloud = Shader("shaders/volumetric_cloud/cube.vert", "shaders/volumetric_cloud/volumetric_cloud.frag");
    Mesh cubeMesh = Primitive::Cube();
    GameObject* cloudVolume = nullptr;

    unsigned int lowFreqNoiseTex = 0;
    unsigned int highFreqNoiseTex = 0;    
    unsigned int cloudMapTex = 0;
    unsigned int heightTex = 0;
};
