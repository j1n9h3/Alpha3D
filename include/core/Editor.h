#pragma once
#include "core/Viewport.h"

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

#include "core/Log.h"
#include <imgui_docking/imgui_internal.h>
#include "renderer/Camera.h"
#include "scene/Scene.h"

#include "utils/Transform.h"

class Scene;
class Environment;
class Editor {
public:
    void Init(GLFWwindow* window);
    ~Editor();

    void BeginFrame(Viewport* viewport);
    //void BeginFrame();
    void EndFrame();
    void BeginLog();
    void BeginEnvironment(Environment& env);
    void BeginProperties(GameObject& game_object);
    void BeginTransform(GameObject& game_object);
    void BeginHierarchy(Scene& scene);
    bool Hover() const;
    bool WantCaptureKeyboard() const;
    bool IsViewportHovered() { return isViewportHovered; }
    void BeginCamera(Camera& camera);
private:
    float mainMenuBarHeight = 10.0f;
    ImFont* font_small;
    ImFont* font_normal;
    ImFont* font_large;
    GLuint m_IconView;
    GLuint m_IconTransform;
    GLuint m_IconRotate;
    GLuint m_IconScale;

    ImVec2 viewportSize;
    bool isViewportHovered = false;
    static ImGuiTextBuffer log;
};