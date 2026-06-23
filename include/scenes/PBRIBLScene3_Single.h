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
#include "scenes/PBRIBLScene0_Base.h"

class Window;

class PBRIBLScene3_Single : public PBRIBLScene0_Base
{
public:

    PBRIBLScene3_Single(std::string path, std::string inst_name);
    void Load(Window& window);
    void Render(Camera& camera);
    void Unload();
    void RenderEditor(Editor& editor);

    GameObject* GetMainGameObject() { return game_object; }

    std::string GetName() { return name; }

private:
    std::string name = "PBRIBLScene3";

    std::string model_path;
    Model model;

    GameObject* game_object = nullptr;
};