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
#include "scenes/PBRIBL_Base.h"

class Window;

class PBRIBL_Custom : public PBRIBL_Base
{
public:

    void Load(Window& window);
    void Render(Camera& camera);
    void Unload();
    void RenderEditor(Editor& editor);

    GameObject* GetMainGameObject() { return bulb; }

    std::string GetName() { return name; }

private:
    std::string name = "PBRIBL_Custom";

    Model model_ginger = Model((project_path + "/assets/models/food_ginger_01_4k/food_ginger_01_4k.gltf").c_str());
    Model model_bulb = Model((project_path + "/assets/models/lightbulb_led_4k/lightbulb_led_4k.gltf").c_str());

    glm::vec3 localLightPos = glm::vec3(0.0f, 0.084f, 0.0f);

    GameObject* bulb = nullptr;
    GameObject* sphere = nullptr;
    GameObject* ginger = nullptr;
};