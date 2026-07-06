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

class SkyAtmosphere : public BaseScene
{
public:
    void Load(Window& window);
    void Render(Camera& camera);
    void Unload();
    void RenderEditor(Editor& editor);

    GameObject* GetMainGameObject() { return nullptr; }

    // Shaders
    Shader shader_sky_atmosphere = Shader("shaders/pbr/pbr_ibl.vert", "shaders/pbr/pbr_ibl.frag");

    IBL ibl;
    Environment env_map;

};