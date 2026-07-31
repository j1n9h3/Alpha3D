#pragma once
#include "renderer/Mesh.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "renderer/Primitive.h"
#include "scene/GameObject.h"
#include "scene/Scene.h"
#include "scenes/BaseScene.h"

class VolumetricCloud : public BaseScene
{
public:
    void Load(Window& window) override;
    void Render(RenderContext& context) override;
    void Unload() override;
    void RenderEditor(Editor& editor) override;

    std::string GetName() const override { return name; }
private:
    std::string name = "volumetric_cloud";

    Shader shader_volumetric_cloud = Shader(
        "shaders/volumetric_cloud/cube.vert",
        "shaders/volumetric_cloud/cube.frag");
    Mesh cubeMesh = Primitive::Cube();
    GameObject* test_object;
    unsigned int volumeTexture = 0;
};
