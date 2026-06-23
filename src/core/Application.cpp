

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

    bool wireframe = false;

    Mesh cubeMesh = Primitive::Cube();
    Mesh mesh_sphere = Primitive::Sphere(128, 64);;

    ////path
    std::string project_path = "C:/Users/17912/Projects/GraphicEngine/A3_GraphicEngine";
    std::string light_path = project_path + "/" + "assets/models/lightbulb_led_4k/lightbulb_led_4k.gltf";
    std::string ginger_path = project_path + "/" + "assets/models/food_ginger_01_4k/food_ginger_01_4k.gltf";
    //std::string hdr_path = project_path + "/" + "assets/hdri/blinds_4k.hdr";
    //std::string hdr_path = project_path + "/" + "assets/hdri/subway_entrance_4k.hdr";


    Model model_ginger(ginger_path.c_str());
    Model model_bulb(light_path.c_str());

    // light Shaders
    Shader shader_light("shaders/pbr/pbr_ibl.vert", "shaders/pbr/pbr_ibl.frag");

    Shader shader_pbr("shaders/pbr/pbr_ibl.vert", "shaders/pbr/pbr_ibl.frag");

    // IBL Shaders
    Shader to_cubemap_shader("shaders/pbr/equirect_to_cubemap.vert", "shaders/pbr/equirect_to_cubemap.frag");
    Shader skybox_shader("shaders/pbr/skybox.vert", "shaders/pbr/skybox.frag");
    Shader irradiance_shader("shaders/pbr/irradiance_convolution.vert", "shaders/pbr/irradiance_convolution.frag");
    Shader prefilter_shader("shaders/pbr/prefilter_convolution.vert", "shaders/pbr/prefilter_convolution.frag");
    Shader brdf_integrate_shader("shaders/pbr/brdf_integrate.vert", "shaders/pbr/brdf_integrate.frag");

    // PBR IBL Object Shader
    Shader shader_pbr_ibl_test("shaders/pbr_ibl_test.vert", "shaders/pbr_ibl_test.frag");


    Environment env_map;
    env_map.Scan(project_path + "/assets/hdri");

    IBL ibl;
    ibl.Load(env_map.GetSelectedPath(), cubeMesh, to_cubemap_shader, irradiance_shader, prefilter_shader, brdf_integrate_shader);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


    Scene scene;
    float theScale = 20.0f;


    GameObject& sphere = scene.AddGameObject("PBR_sphere", &mesh_sphere, &shader_pbr_ibl_test);
    sphere.SetPosition(glm::vec3(2.0f, 1.5f, 0.0f));
    sphere.SetScale(2.0f);
    sphere.pbr_test = PBRTestComponent{};

    const int pbr_test_grid = 6;
    const float spacing = 1.4f;
    const glm::vec3 pbr_test_grid_origin = glm::vec3(-5.0f, 4.0f, -5.0f); // 调整到合适位置

    for (int row = 0; row < pbr_test_grid; row++)       // row → roughness
    {
        for (int col = 0; col < pbr_test_grid; col++)   // col → metallic
        {
            std::string name = "sphere_" + std::to_string(row) + "_" + std::to_string(col);
            GameObject& s = scene.AddGameObject(name, &mesh_sphere, &shader_pbr_ibl_test);

            s.SetPosition(pbr_test_grid_origin + glm::vec3(0.0f, -row * spacing, col * spacing));
            s.SetScale(1.2f);

            s.pbr_test = PBRTestComponent{};
            (*s.pbr_test).albedo = glm::vec3(1.0f, 1.0f, 1.0f);
            (*s.pbr_test).roughness = glm::clamp((float)row / (pbr_test_grid - 1), 0.05f, 1.0f);
            (*s.pbr_test).metallic = (float)col / (pbr_test_grid - 1);
        }
    }

    GameObject& ginger = scene.AddGameObject("ginger", &model_ginger, &shader_pbr);
    ginger.SetScale(5.0f);
    ginger.SetPosition(glm::vec3(0.560, -0.440, 1.490));
    ginger.SetRotation(glm::vec3(0.0f, 22.5f, 0.0f));

    GameObject& bulb = scene.AddGameObject("bulb", &model_bulb, &shader_light);

    bulb.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    bulb.SetScale(10.0f);
    bulb.light = LightComponent{};

    scene.SetSelected(bulb.GetID());

    // camera init
    Camera camera(config.renderer.fov, window, glm::vec3(-1.0f, 1.0f, -1.0f), ginger.GetPosition() + glm::vec3(1.0f, 1.0f, 0.0f));
    window.GetWindowContext().camera = &camera;

    window.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });

    // light control
    bool lightOn = true;
    bool lightOn_last = lightOn;
    glm::vec3 localLightPos = glm::vec3(0.0f, 0.084f, 0.0f); // 灯头在模型坐标系的位置
    glm::vec4 worldLightPos;
    glm::vec3 ambient = glm::vec3(0.2f);
    float intensity = 10.0f;
    float intensity_last = intensity;
    glm::vec3 lightColor = glm::vec3(1.0f);
    float lightIntensity = 10.0f;
    glm::vec3 sphere_color = glm::vec3(1.0, 0.71, 0.29);

    glViewport(0, 0, window.GetWidth(), window.GetHeight());


    while (!window.ShouldClose())
    {
        editor.BeginFrame(&viewport);
        camera.SetProjection(camera.GetFov(), viewport.GetWidth() / viewport.GetHeight());
        viewport.BeginRender();

        // input process

        ImGuiWindow* hoveredWindow = ImGui::GetCurrentContext()->HoveredWindow;


        ImGuiIO& io = ImGui::GetIO();

        if (editor.IsViewportHovered() || camera.GetMoving()) {
            double xpos, ypos;
            glfwGetCursorPos(window.GetGLFWWindow(), &xpos, &ypos);
            camera.ProcessMouseMovement((float)xpos, (float)ypos);
            camera.ProcessInput(window.GetGLFWWindow());
            io.ConfigDockingWithShift = true;
        }
        else {
            camera.ResetMouseState();
            io.ConfigDockingWithShift = false;
        }

        // light object rendering
        if (lightOn == false && lightOn_last == true) { // turn off the light
            lightOn_last = lightOn;
            intensity_last = intensity;
            intensity = 0.0f;
        }
        else if (lightOn == true && lightOn_last == false) {
            lightOn_last = lightOn;
            intensity = intensity_last;
        }
        else {
            lightOn_last = lightOn;
        }

        worldLightPos = bulb.GetTransform().matrix * glm::vec4(localLightPos, 1.0f); // w=1 表示点
        glm::vec3 lightPos = glm::vec3(worldLightPos);

        ibl.Bind(shader_pbr);
        shader_pbr.setBool("isLight", false);
        shader_pbr.setMat4("view", camera.GetView());
        shader_pbr.setMat4("projection", camera.GetProjection());
        shader_pbr.setVec3("emissiveIntensity", pow((*bulb.light).intensity * (*bulb.light).color, glm::vec3(2.0f)));
        shader_pbr.setVec3("light.ambient", ambient);
        shader_pbr.setVec3("light.intensity", glm::vec3((*bulb.light).intensity * (*bulb.light).color));
        shader_pbr.setVec3("F0", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_pbr.setVec3("viewPos", camera.GetPosition());
        shader_pbr.setVec3("lightPos", lightPos);

        ibl.Bind(shader_light);
        shader_light.setBool("isLight", true);
        shader_light.setBool("lightOn", true);
        shader_light.setVec3("emissiveIntensity", (*bulb.light).intensity * (*bulb.light).color / 2.0f);
        shader_light.setMat4("view", camera.GetView());
        shader_light.setMat4("projection", camera.GetProjection());
        shader_light.setVec3("light.ambient", ambient);
        shader_light.setVec3("light.intensity", glm::vec3((*bulb.light).intensity * (*bulb.light).color));
        shader_light.setVec3("F0", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_light.setVec3("viewPos", camera.GetPosition());
        shader_light.setVec3("lightPos", lightPos);


        ibl.Bind(shader_pbr_ibl_test);
        shader_pbr_ibl_test.setMat4("view", camera.GetView());
        shader_pbr_ibl_test.setMat4("projection", camera.GetProjection());
        shader_pbr_ibl_test.setVec3("light.ambient", ambient);
        shader_pbr_ibl_test.setVec3("light.intensity", glm::vec3((*bulb.light).intensity * (*bulb.light).color));
        shader_pbr_ibl_test.setVec3("viewPos", camera.GetPosition());
        shader_pbr_ibl_test.setVec3("lightPos", lightPos);
        shader_pbr_ibl_test.setVec3("baseColor", (*sphere.pbr_test).albedo);
        shader_pbr_ibl_test.setFloat("roughness", (*sphere.pbr_test).roughness);
        shader_pbr_ibl_test.setFloat("metallic", (*sphere.pbr_test).metallic);


        GameObject* selected = scene.GetSelected();

        scene.Render();


        // skybox rendering
        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();
        skybox_shader.setMat4("projection", camera.GetProjection());
        skybox_shader.setMat4("view", camera.GetView());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.envCubemap);
        skybox_shader.setInt("environmentMap", 0);

        cubeMesh.Draw();

        glDepthFunc(GL_LESS); // 恢复默认

        // editor
        editor.BeginEnvironment(env_map); // Editor UI

        if (env_map.HasChanged()) {
            ibl.Load(env_map.GetSelectedPath(), cubeMesh, to_cubemap_shader, irradiance_shader, prefilter_shader, brdf_integrate_shader);
            env_map.ClearChanged();
        }

        editor.BeginLog();
        editor.BeginCamera(camera);
        editor.BeginHierarchy(scene);
        editor.BeginDetails(*selected);

        viewport.EndRender();

        editor.EndFrame();
        window.Update();

        // check events


    }
    window.Destroy();
    glfwTerminate();

    return 0;
}