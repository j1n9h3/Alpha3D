

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/Window.h"
#include "core/Viewport.h"
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

#include <imgui_docking/imgui_internal.h>

#include "scene/Scene.h"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "scene/GameObject.h"
#include "renderer/IBL.h"
#include "renderer/Environment.h"

#include "scenes/PBRIBLScene1_Custom.h"
#include "scenes/PBRIBLScene2_Spheres.h"
#include "scenes/PBRIBLScene3_Single.h"


void SwitchScene(BaseScene* next, BaseScene*& current, Window& window)
{
    current->Unload();
    current = next;
    current->Load(window);
}

int main()
{
    Log::Init();

    Config::Load("config/engine.toml");
    const auto& config = Config::Get();

    Window window;
    window.Init(config.window.width, config.window.height, config.window.x, config.window.y, config.window.title);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR(Window, "Failed to initialize GLAD");
        return -1;
    }

    Viewport viewport;
    viewport.Init(config.window.width, config.window.height);

    Editor editor;
    editor.Init(window.GetGLFWWindow());

    PBRIBLScene1_Custom pbribl_scene1_custom;
    PBRIBLScene2_Spheres pbribl_scene2_spheres;
    PBRIBLScene3_Single pbr_horse_statue_4k("/assets/models/horse_statue_01_4k/horse_statue_01_4k.gltf", "pbr_horse_statue_4k");
    PBRIBLScene3_Single lion_head_4k("/assets/models/lion_head_4k/lion_head_4k.gltf", "lion_head_4k");

    std::vector<BaseScene*> scenes = {
        &pbribl_scene1_custom, &pbribl_scene2_spheres, &pbr_horse_statue_4k, &lion_head_4k
    };

    BaseScene* currentScene = &pbribl_scene2_spheres;

    currentScene->Load(window);

    Camera camera(config.renderer.fov, window, glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    window.GetWindowContext().camera = &camera;

    window.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });

    while (!window.ShouldClose())
    {
        editor.BeginFrame(&viewport);
        camera.SetProjection(camera.GetFov(), viewport.GetWidth() / viewport.GetHeight());
        viewport.BeginRender();

        ImGuiWindow* hoveredWindow = ImGui::GetCurrentContext()->HoveredWindow;

        ImGuiIO& io = ImGui::GetIO();

        camera.ProcessEditorInput(window.GetGLFWWindow(), editor.IsViewportHovered());

        currentScene->Render(camera);

        viewport.EndRender();

        editor.BeginMainMenu(currentScene, scenes, window);
        editor.BeginLog();
        editor.BeginCamera(camera);
        currentScene->RenderEditor(editor);
        editor.EndFrame();

        window.Update();
    }

    window.Destroy();
    glfwTerminate();

    return 0;
}