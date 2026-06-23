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

class PBRIBLScene2_Spheres : public PBRIBLScene0_Base
{
public:
    std::string name = "PBRIBLScene2_Spheres";

    void Load(Window& window);
    void Render(Camera& camera);
    void Unload();
    void RenderEditor(Editor& editor);

    //GameObject* GetMainGameObject() { return sphere; }

    std::string GetName() { return name; }

private:

    glm::vec3 sphere_albedo = glm::vec3(0.5f, 0.5f, 0.5f);
    GameObject* sphere = nullptr;

};