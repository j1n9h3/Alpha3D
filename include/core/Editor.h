#pragma once
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

#include "core/Log.h"
#include "imgui_internal.h"
#include "renderer/Camera.h"
#include "scene/Scene.h"


class Editor {
public:
    Editor(GLFWwindow* window);
    ~Editor();

    void BeginFrame();
    void EndFrame();

    bool Hover() const;
    bool WantCaptureKeyboard() const;

    ImGuizmo::OPERATION GetGizmoNone() { return GIZMO_NONE; };

    void OnImGuiCamera(Camera& camera, ImGuizmo::OPERATION& gizmoOp);
    void OnImGuiScene(Scene& scene);
private:
    const ImGuizmo::OPERATION GIZMO_NONE = (ImGuizmo::OPERATION)0;
    float mainMenuBarHeight = 10.0f;
    ImFont* font_small;
    ImFont* font_normal;
    ImFont* font_large;
    GLuint m_IconView;
    GLuint m_IconTransform;
    GLuint m_IconRotate;
    GLuint m_IconScale;
};