// scenes/BaseScene.h
#pragma once
#include <functional>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "renderer/Camera.h"
#include "core/Editor.h"
#include "renderer/Environment.h"
#include "renderer/Primitive.h"
#include "core/Log.h"

class Editor;
class Window;
struct RenderContext;

class BaseScene
{
public:
    virtual ~BaseScene() = default;

    virtual void Load(Window& window);
    virtual void Render(RenderContext& context) = 0;
    virtual void Unload() = 0;

    virtual void RenderEditor(Editor& editor);
    virtual GameObject* GetMainGameObject() { return nullptr; };

    virtual std::string GetName() const = 0;
    void SetDeltaTime(float dt) { delta_time = dt; }
    void MarkParametersDirty() { parametersDirty = true; }
    
    void RenderFullscreenTriangle();

    bool LoadVolumeTex(unsigned int& texture, unsigned int texSize, std::string pathTemplate);
    bool LoadTex(unsigned int& texture, std::string path);
    
    void RenderLUT(GLuint lutTexture, int width, int height, GLuint framebuffer, Shader& shader, std::function<void(Shader&)> setUniforms);
    bool SaveTextureLUT(GLuint texture, int width, int height, const std::string& fileName) const;


protected:
    float delta_time = 0.0f;
    bool parametersDirty = true;
    Scene scene;

    std::string project_path = "C:/Users/17912/Projects/GraphicEngine/A3_GraphicEngine";
    std::string generated_path = project_path + "/generated";
    std::string lut_path = generated_path + "/LUT";
    glm::vec3 ambient = glm::vec3(0.2f);

    Mesh cubeMesh = Primitive::Cube();
    Mesh mesh_sphere = Primitive::Sphere(128, 64);
};
