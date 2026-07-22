// scenes/BaseScene.h
#pragma once
#include "renderer/Camera.h"
#include "core/Editor.h"
#include "renderer/Environment.h"
#include "renderer/Primitive.h"
#include "core/Log.h"

class Editor;
class Window;

class BaseScene
{
public:
    virtual ~BaseScene() = default;

    virtual void Load(Window& window);
    virtual void Render(Camera& camera) = 0;
    virtual void Unload() = 0;

    virtual void RenderEditor(Editor& editor);
    virtual GameObject* GetMainGameObject() { return nullptr; };

    virtual std::string GetName() = 0;
    void SetDeltaTime(float dt) { delta_time = dt; }
    void RenderFullscreenTriangle();
protected:
    float delta_time = 0.0f;
    Scene scene;

    std::string project_path = "C:/Users/17912/Projects/GraphicEngine/A3_GraphicEngine";
    glm::vec3 ambient = glm::vec3(0.2f);

    Mesh cubeMesh = Primitive::Cube();
    Mesh mesh_sphere = Primitive::Sphere(128, 64);
};
