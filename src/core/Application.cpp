
#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/Shader.h"
#include "stb/stb_image.h"

#include "core/Window.h"
#include "core/Config.h"
#include "core/Log.h"

#include "renderer/Mesh.h"
#include "renderer/Camera.h"
#include "renderer/Texture.h"

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
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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

    Shader shader("shaders/light_obj.vert", "shaders/light_obj.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");

    // camera init
    Camera camera(config.renderer.fov, mainWindow.GetAspectRatio(), mainWindow.GetWidth(), mainWindow.GetHeight());
    mainWindow.GetWindowContext().camera = &camera;

    mainWindow.SetResizeCallback([&](int width, int height) {
        camera.SetProjection(config.renderer.fov, (float)width / height);
    });
    

    glEnable(GL_DEPTH_TEST);

    while (!mainWindow.ShouldClose())
    {
        // input process
        camera.ProcessInput(mainWindow.GetGLFWWindow());
        mainWindow.ProcessInput();

        // clear
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering

        glm::mat4 trans = glm::mat4(1.0f);

        lightShader.use();
        lightShader.setMat4("view", camera.GetView());
        lightShader.setMat4("projection", camera.GetProjection());
        // light trans
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::mat4 light_trans = glm::translate(trans, lightPos);
        light_trans = glm::scale(light_trans, glm::vec3(0.2f));


        lightShader.setMat4("model", light_trans);
        cubeMesh.Draw();


        shader.use();
        shader.setMat4("view", camera.GetView());
        shader.setMat4("projection", camera.GetProjection());
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("viewPos", camera.GetPosition());

        
        shader.setVec3("lightPos", lightPos);


        // shader.setFloat("someUniform", 1.0f);

        //wallTexture.Bind();


        // cube trans
        for (unsigned int i = 0; i < 10; i++)
        {
            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, cubePositions[i]);
            trans = glm::scale(trans, glm::vec3(1.0, 1.0, 1.0));
            trans = glm::rotate(trans, (float)glfwGetTime() * 1, glm::vec3(0.0f, 1.0f, 1.0f));
            shader.setMat4("model", trans);
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(trans)));
            shader.setMat3("normalMatrix", normalMatrix);

            cubeMesh.Draw();
        }

        // check events
        mainWindow.Update();
    }

    glfwTerminate();
    return 0;
}