#pragma once
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

#include "core/Log.h"
#include "imgui_internal.h"
#include "renderer/Camera.h"


class Editor {
public:
    Editor(GLFWwindow* window);
    ~Editor();

    void BeginFrame();
    void EndFrame();
    void OnImGuiCamera(Camera& camera, ImGuizmo::OPERATION& gizmoOp);

    bool Hover() const;
    bool WantCaptureKeyboard() const;
private:
    float mainMenuBarHeight = 10.0f;
    ImFont* font_small;
    ImFont* font_normal;
    ImFont* font_large;
    GLuint m_IconTransform;
    GLuint m_IconRotate;
    GLuint m_IconScale;
};