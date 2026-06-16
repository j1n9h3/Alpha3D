

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
#include "imgui_internal.h"

#include "scene/Scene.h"

#include "stb_image.h"



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

    bool wireframe = false;

    Mesh cubeMesh = Primitive::Cube();
    Mesh sphereMesh = Primitive::Sphere(128, 64);;

    //path
    std::string project_path = "C:/Users/17912/Projects/GraphicEngine/A3_GraphicEngine";
    std::string light_path = project_path + "/" + "assets/models/lightbulb_led_1k/lightbulb_led_1k.gltf";
    std::string ginger_path = project_path + "/" + "assets/models/food_ginger_01_4k/food_ginger_01_4k.gltf";
    std::string hdr_path = project_path + "/" + "assets/hdri/blinds_4k.hdr";
    //std::string hdr_path = project_path + "/" + "assets/hdri/subway_entrance_4k.hdr";


    Model ginger_model(ginger_path.c_str());
    Model light_model(light_path.c_str());

    Shader pbr_shader("shaders/pbr.vert", "shaders/pbr.frag");
    Shader light_shader("shaders/light.vert", "shaders/light.frag");
    Shader pbr_test_shader("shaders/pbr_ibl_test.vert", "shaders/pbr_ibl_test.frag");
    Shader to_cubemap_shader("shaders/cubemap.vert", "shaders/cubemap.frag");
    Shader skybox_shader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader irradiance_shader("shaders/irradiance.vert", "shaders/irradiance.frag");
    
    ImGuizmo::Style& style = ImGuizmo::GetStyle();
    style.TranslationLineThickness = 10.0f;
    style.TranslationLineArrowSize = 16.0f;
    style.RotationLineThickness = 10.0f;

    style.ScaleLineThickness = 10.0f;

    glEnable(GL_DEPTH_TEST);

    Scene scene;
    float theScale = 20.0f;

    Entity& ginger = scene.AddEntity("ginger", &ginger_model, &pbr_shader);
    ginger.SetScale(glm::vec3(5.0f));
    ginger.SetPosition(glm::vec3(0.560, -0.440, 1.490));
    ginger.SetRotation(glm::vec3(0.0f, 22.5f, 0.0f));
    ginger.GetTransform().SyncToMatrix();


    Entity& sphere = scene.AddEntity("pbr_test", &sphereMesh, &pbr_test_shader);
    sphere.SetPosition(glm::vec3(2.0f, 1.5f, 0.0f));
    sphere.SetScale(glm::vec3(0.8f));
    sphere.GetTransform().SyncToMatrix();

    Entity& light = scene.AddEntity("Light", &light_model, &light_shader);

    light.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    light.SetScale(glm::vec3(10.0f));
    light.GetTransform().SyncToMatrix();

    scene.SetSelected(light.GetID());

    // camera init
    Camera camera(config.renderer.fov, mainWindow, glm::vec3(-1.0f, 1.0f, -1.0f), ginger.GetPosition() + glm::vec3(1.0f, 1.0f, 0.0f));
    mainWindow.GetWindowContext().camera = &camera;

    mainWindow.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });

    Editor editor(mainWindow.GetGLFWWindow());

    ImGuizmo::OPERATION gizmoOp = editor.GetGizmoNone();


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
    float pbr_roughness = 0.4f;
    float pbr_metallic = 0.4f;
    glm::vec3 sphere_color = glm::vec3(0.05, 0.05, 0.05);




    /////////////////////////////////////////////////////


    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(hdr_path.c_str(), &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(false);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    to_cubemap_shader.use();
    to_cubemap_shader.setInt("equirectangularMap", 0);
    to_cubemap_shader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        to_cubemap_shader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeMesh.Draw();

    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    /////////////////////////////////////////////////////


    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradiance_shader.use();
    irradiance_shader.setInt("environmentMap", 0);
    irradiance_shader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradiance_shader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeMesh.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /////////////////////////////////////////////////////


    glViewport(0, 0, mainWindow.GetWidth(), mainWindow.GetHeight());


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
        glClearColor(0.235f, 0.235f, 0.235f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering

        //glm::vec3 lightPos = light.GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);

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

        worldLightPos = light.GetTransform().matrix * glm::vec4(localLightPos, 1.0f); // w=1 表示点
        glm::vec3 lightPos = glm::vec3(worldLightPos);

        light_shader.use();
        light_shader.setBool("lightOn", lightOn);
        light_shader.setMat4("view", camera.GetView());
        light_shader.setMat4("projection", camera.GetProjection());
        light_shader.setVec3("light.ambient", ambient);
        light_shader.setVec3("light.intensity", glm::vec3(intensity*lightColor));
        light_shader.setVec3("F0", glm::vec3(0.3f, 0.3f, 0.3f));
        light_shader.setVec3("viewPos", camera.GetPosition());
        light_shader.setVec3("lightPos", lightPos);

        pbr_test_shader.use();
        pbr_test_shader.setMat4("view", camera.GetView());
        pbr_test_shader.setMat4("projection", camera.GetProjection());
        pbr_test_shader.setVec3("light.ambient", ambient);
        pbr_test_shader.setVec3("light.intensity", glm::vec3(intensity * lightColor));
        //pbr_test_shader.setVec3("F0", glm::vec3(0.3f, 0.3f, 0.3f));
        pbr_test_shader.setVec3("viewPos", camera.GetPosition());
        pbr_test_shader.setVec3("lightPos", lightPos);
        pbr_test_shader.setVec3("baseColor", sphere_color);
        pbr_test_shader.setFloat("roughness", pbr_roughness);
        pbr_test_shader.setFloat("metallic", pbr_metallic);
        pbr_test_shader.setInt("irradianceMap", 0);
        glActiveTexture(GL_TEXTURE0);                 // 激活 0 号单元
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);  // 绑定(注意是 CUBE_MAP 目标)


        pbr_shader.use();
        pbr_shader.setMat4("view", camera.GetView());
        pbr_shader.setMat4("projection", camera.GetProjection());
        pbr_shader.setVec3("light.ambient", ambient);
        pbr_shader.setVec3("light.intensity", glm::vec3(intensity * lightColor));
        pbr_shader.setVec3("F0", glm::vec3(0.3f, 0.3f, 0.3f));
        pbr_shader.setVec3("viewPos",  camera.GetPosition());
        pbr_shader.setVec3("lightPos", lightPos);

        Entity* selected = scene.GetSelected();
        ImGuizmo::SetOrthographic(camera.IsOrtho());
        ImGuizmo::SetRect(0, 0, (float)mainWindow.GetWidth(), (float)mainWindow.GetHeight());
        if (selected) {
            float distance = glm::length(camera.GetPosition() - selected->GetTransform().position);
            if (distance > 10.0f)
                ImGuizmo::SetGizmoSizeClipSpace(1.0f / distance); // 距离越远越小

            if (gizmoOp != editor.GetGizmoNone()) {
                ImGuizmo::Manipulate(
                    glm::value_ptr(camera.GetView()),
                    glm::value_ptr(camera.GetProjection()),
                    gizmoOp,
                    ImGuizmo::WORLD,
                    glm::value_ptr(selected->GetTransform().matrix)
                );
            }
        }


        scene.Render();


        // skybox
        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();
        skybox_shader.setMat4("projection", camera.GetProjection());
        skybox_shader.setMat4("view", camera.GetView());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        skybox_shader.setInt("environmentMap", 0);

        cubeMesh.Draw();

        glDepthFunc(GL_LESS); // 恢复默认



        // editor
        editor.OnImGuiCamera(camera, gizmoOp);
        editor.OnImGuiScene(scene, wireframe);
        editor.OnImGuiLight(ambient, intensity, lightColor, lightOn);
        editor.OnImGuiPBR(pbr_roughness, pbr_metallic);
        editor.EndFrame();

        // check events
        mainWindow.Update();
    }

    glfwTerminate();

    return 0;
}