

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/Window.h"
#include "core/Config.h"
#include "core/Log.h"

#include "renderer/Mesh.h"
#include "renderer/Primitive.h"
#include "renderer/Camera.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"

#include <assimp/Importer.hpp>

#include "core/Editor.h"

#include <ImGuizmo/ImGuizmo.h>

#include "scene/Scene.h"


int main()
{
    Log::Init();

    Config::Load("config/engine.toml");
    const auto& config = Config::Get();

    Window mainWindow(config.window.width, config.window.height, config.window.title);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR(Window, "Failed to initialize GLAD");
        return -1;
    }

    Mesh cubeMesh = Primitive::Cube();

    Texture wallTexture("textures/wall.jpg");
    Texture diffuseTexture("textures/diffuse.png");
    Texture specularTexture("textures/specular.png");

    //Model Backpack(model_path.c_str());
    std::string project_path = "C:/Users/17912/Projects/GraphicEngine/A3_GraphicEngine";
    std::string model_path = project_path + "/" + "models/Backpack/backpack.obj";
    Model Backpack(model_path.c_str());

    Shader shader("shaders/light_obj.vert", "shaders/light_obj.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");
    Shader ourShader("shaders/phong.vert", "shaders/phong.frag");

    // camera init
    Camera camera(config.renderer.fov, mainWindow);
    mainWindow.GetWindowContext().camera = &camera;

    mainWindow.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });
    
    Editor editor(mainWindow.GetGLFWWindow());

    ImGuizmo::Style& style = ImGuizmo::GetStyle();
    style.TranslationLineThickness = 10.0f;
    style.TranslationLineArrowSize = 16.0f;
    style.RotationLineThickness = 10.0f;

    style.ScaleLineThickness = 10.0f;

    //std::string fontPath = std::string(std::getenv("WINDIR")) + "/Fonts/" + "JetBrains Mono/JetBrainsMono-Regular.ttf";
    std::string fontPath = "assets/fonts/JetBrainsMono-Regular.ttf";

    glEnable(GL_DEPTH_TEST);

    Scene scene;
    Entity& backpackEntity = scene.AddEntity("Backpack", &Backpack, &shader);
    Entity& lightEntity    = scene.AddEntity("Light",    &cubeMesh, &lightShader);

    lightEntity.GetTransform().position = glm::vec3(0.0f, 1.0f, 2.0f);
    lightEntity.GetTransform().scale    = glm::vec3(0.2f);
    lightEntity.GetTransform().SyncToMatrix();

    scene.SetSelected(backpackEntity.GetID());

    while (!mainWindow.ShouldClose())
    {

        editor.BeginFrame();
        ImGuizmo::BeginFrame();
        // input process

        if (!editor.Hover() && !ImGuizmo::IsUsing()) {
            camera.ProcessInput(mainWindow.GetGLFWWindow());
            mainWindow.ProcessKeyboardInput();
        }
        else {
            camera.ResetMouseState();
        }

        // clear
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering

        glm::vec3 lightPos = lightEntity.GetTransform().position;

        lightShader.use();
        lightShader.setMat4("view",       camera.GetView());
        lightShader.setMat4("projection", camera.GetProjection());

        shader.use();
        shader.setMat4("view",            camera.GetView());
        shader.setMat4("projection",      camera.GetProjection());
        shader.setVec3("light.ambient",   glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setVec3("light.diffuse",   glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("light.specular",  glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("objectColor",     glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("viewPos",  camera.GetPosition());
        shader.setVec3("lightPos", lightPos);

        Entity* selected = scene.GetSelected();
        ImGuizmo::SetRect(0, 0, (float)mainWindow.GetWidth(), (float)mainWindow.GetHeight());
        if (selected) {
            ImGuizmo::Manipulate(
                glm::value_ptr(camera.GetView()),
                glm::value_ptr(camera.GetProjection()),
                ImGuizmo::TRANSLATE,
                ImGuizmo::WORLD,
                glm::value_ptr(selected->GetTransform().matrix)
            );
        }

        scene.Render();
        scene.OnImGuiHierarchy();
        scene.OnImGuiProperties();

        editor.EndFrame();

        // check events
        mainWindow.Update();
    }

    glfwTerminate();

    return 0;
}