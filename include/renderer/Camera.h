#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glfw/glfw3.h>
#include "core/Window.h"

#include "imgui/imgui.h"
#include <ImGuizmo/ImGuizmo.h>
#include "imgui_internal.h"



class Camera {
public:
    Camera(float fov, Window& window);

    glm::mat4 GetView();
    bool SetView();

    glm::mat4 GetProjection();
    void SetProjection(float fov, float aspectRatio);

    float GetFov()    const { return m_Fov; }
    bool  IsOrtho()   const { return m_IsOrtho; }
    void  SetFov(float fov);
    void  SetOrtho(bool isOrtho);

    glm::vec3 GetPosition();
    void ResetMouseState();
    void ProcessInput(GLFWwindow * window);
    void ProcessMouseMovement(float xpos, float ypos);

private:
    void RebuildProjection();

    // camera direction
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 up;
    glm::vec3 right;

    // MVP properties
    glm::mat4 view;
    glm::mat4 projection;

    float m_Fov         = 90.0f;
    float m_AspectRatio = 1.0f;
    float m_Near        = 0.1f;
    float m_Far         = 100.0f;
    bool  m_IsOrtho     = false;
    float m_OrthoSize   = 5.0f;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX;
    float lastY;
    bool firstMouse = true;
};
