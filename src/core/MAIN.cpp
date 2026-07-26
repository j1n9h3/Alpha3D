

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

#include "scenes/PBRIBL_Custom.h"
#include "scenes/PBRIBL_Spheres.h"
#include "scenes/PBRIBL_Single.h"
#include "scenes/PBRIBL_SingleTex.h"
#include "scenes/SkyAtmosphere.h"
#include "core/Time.h"
#include "renderer/RenderProfiler.h"
#include "renderer/RenderContext.h"

#include "utils/Recorder.h"

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

    PBRIBL_Custom pbribl_scene1_custom;
    PBRIBL_Spheres pbribl_test_scene2_spheres;
    PBRIBL_SingleTex pbribl_tex_horse_statue_4k("/assets/models/horse_statue_01_4k/horse_statue_01_4k.gltf", "pbr_horse_statue_4k");
    PBRIBL_SingleTex pbribl_tex_food_ginger_4k("/assets/models/food_ginger_01_4k/food_ginger_01_4k.gltf", "pbr_food_ginger_4k");
    PBRIBL_SingleTex pbribl_tex_lion_head_4k("/assets/models/lion_head_4k/lion_head_4k.gltf", "lion_head_4k");
    PBRIBL_Single pbribl_test_bunney("/assets/models/stanford_bunny/scene.gltf", "stanford_bunny");
    SkyAtmosphere sky_atmosphere;

    std::vector<BaseScene*> scenes = {
        // pbr ibl scenes
        &pbribl_test_scene2_spheres, &pbribl_test_bunney,
        //  pbr ibl models
        &pbribl_scene1_custom, &pbribl_tex_horse_statue_4k, &pbribl_tex_lion_head_4k, &pbribl_tex_food_ginger_4k,
        // sky atmosphere scene
        &sky_atmosphere
    };

    BaseScene* currentScene = &sky_atmosphere;

    RenderProfiler profiler;
    {
        A3_PROFILE_PASS(profiler, "One-time/Initial Scene Load");
        currentScene->Load(window);
    }

    Camera camera(config.renderer.fov, window, glm::vec3(-0.0f, 0.0f, -0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    window.GetWindowContext().camera = &camera;

    window.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });

    Recorder recorder;
    recorder.Init(1920, 1080);
    RenderContext renderContext{ camera, profiler };

    while (!window.ShouldClose())
    {
        profiler.BeginFrame();
        Time::Update();
        editor.BeginFrame(&viewport, &recorder);
        camera.SetProjection(camera.GetFov(), viewport.GetWidth() / viewport.GetHeight());

        // Viewport
        {
            A3_PROFILE_PASS(profiler, "Viewport");
            viewport.BeginRender();
            camera.ProcessEditorInput(window.GetGLFWWindow(), editor.IsViewportHovered());
            currentScene->Render(renderContext);
            viewport.EndRender();
        }

        // Photographer
        if (recorder.ConsumePhotoRequest()) {
            A3_PROFILE_PASS(profiler, "Photo");
            camera.SetProjection(camera.GetFov(), (float)recorder.GetWidth() / recorder.GetHeight());
            recorder.BeginRender();
            currentScene->Render(renderContext);
            recorder.SaveImage();
            recorder.EndRender();
        }

        // Recorder
        if (recorder.IsRecording()) {
            A3_PROFILE_PASS(profiler, "Recorder");
            camera.SetProjection(camera.GetFov(), (float)recorder.GetWidth() / recorder.GetHeight());
            recorder.BeginRender();
            currentScene->Render(renderContext);
            recorder.CaptureFrame();
            recorder.EndRender();
        }

        // Editor
        {
            A3_PROFILE_PASS(profiler, "Editor UI");
            editor.BeginSceneSelect(currentScene, scenes, window);
            editor.BeginLog();
            editor.BeginCamera(camera);
            editor.BeginRecorder(recorder);
            currentScene->RenderEditor(editor);
            if (editor.HasPerformanceAffectingEdit())
                profiler.ResetFrameHistory();
            editor.BeginPerformance(profiler);
            editor.EndFrame();
        }

        {
            A3_PROFILE_PASS(profiler, "Present");
            window.Update();
        }
        profiler.EndFrame();
    }

    recorder.Destroy();
    profiler.Shutdown();
    window.Destroy();
    glfwTerminate();

    return 0;
}
