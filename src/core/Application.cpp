

#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/Window.h"
#include "core/Config.h"
#include "core/Log.h"

#include "renderer/Mesh.h"
#include "renderer/Camera.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"

#include <assimp/Importer.hpp>

#include "core/Editor.h"

#include <ImGuizmo/ImGuizmo.h>


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

float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    Mesh cubeMesh(vertices, sizeof(vertices));

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
    style.TranslationLineThickness = 10.0f;   // 轴线粗细
    style.TranslationLineArrowSize = 16.0f;   // 箭头大小
    style.RotationLineThickness = 10.0f;      // 旋转圆环粗细

    style.ScaleLineThickness = 10.0f;         // 缩放线粗细

    //std::string fontPath = std::string(std::getenv("WINDIR")) + "/Fonts/" + "JetBrains Mono/JetBrainsMono-Regular.ttf";
    std::string fontPath = "assets/fonts/JetBrainsMono-Regular.ttf";

    glEnable(GL_DEPTH_TEST);

    glm::mat4 trans2 = glm::mat4(1.0f);
    glm::mat4 trans3 = glm::mat4(1.0f);

    while (!mainWindow.ShouldClose())
    {

        editor.BeginFrame();
        ImGuizmo::BeginFrame();
        // input process

        if (!editor.Hover()) {
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


        glm::mat4 trans = glm::mat4(1.0f);

        lightShader.use();
        lightShader.setMat4("view", camera.GetView());
        lightShader.setMat4("projection", camera.GetProjection());
        // light trans
        glm::vec3 lightPos(0.0f, 1.0f, 2.0f);
        glm::mat4 light_trans = glm::translate(trans, lightPos);
        light_trans = glm::scale(light_trans, glm::vec3(0.2f));


        lightShader.setMat4("model", light_trans);
        cubeMesh.Draw();


        shader.use();
        shader.setMat4("view", camera.GetView());
        shader.setMat4("projection", camera.GetProjection());
        shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setVec3("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 32.0f);

        shader.setVec3("viewPos", camera.GetPosition());

        ImGuizmo::SetRect(0, 0, (float)mainWindow.GetWidth(), (float)mainWindow.GetHeight());

        //ImGuizmo::SetPlaneLimit(0.0f);

        ImGuizmo::Manipulate(
            glm::value_ptr(camera.GetView()),
            glm::value_ptr(camera.GetProjection()),
            ImGuizmo::TRANSLATE,
            ImGuizmo::WORLD,
            glm::value_ptr(trans2)
        );

        shader.setMat4("model", trans2);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(trans2)));
        shader.setMat3("normalMatrix", normalMatrix);
        
        shader.setVec3("lightPos", lightPos);
        Backpack.Draw(shader);

        editor.EndFrame();

        // check events
        mainWindow.Update();
    }

    glfwTerminate();

    return 0;
}