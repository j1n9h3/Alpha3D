#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glfw/glfw3.h>
#include "core/Window.h"

class Camera {
public:
    Camera(float fov, float aspectRatio, float window_width, float window_height);
    
    glm::mat4 GetView();
    bool SetView();

    glm::mat4 GetProjection();
    void SetProjection(float fov, float aspectRatio);
    void ProcessInput(GLFWwindow * window);
    void ProcessMouseMovement(GLFWwindow* window, double xpos, double ypos);
private:
    // camera direction
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 up;
    glm::vec3 right;
    
    // MVP properties
    glm::mat4 view;
    glm::mat4 projection;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX;
    float lastY;
    bool first_mouse = true;
};