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

class PBRIBL_Spheres : public PBRIBL_Base
{
public:

    void Load(Window& window);
    void Render(RenderContext& context);
    void Unload();
    void RenderEditor(Editor& editor);

    //GameObject* GetMainGameObject() { return sphere; }

    std::string GetName() { return name; }

private:
    std::string name = "PBRIBL_Spheres";

    glm::vec3 sphere_albedo = glm::vec3(0.5f, 0.5f, 0.5f);
    GameObject* sphere = nullptr;

};
