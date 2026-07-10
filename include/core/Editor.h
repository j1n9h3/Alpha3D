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

class BaseScene;
class Scene;
class Environment;
class Recorder;
class Editor {
public:
    void Init(GLFWwindow* window);
    ~Editor();

    void BeginFrame(Viewport* viewport, Recorder* recorder);
    void EndFrame();
    void BeginLog();
    void BeginEnvironment(Environment& env);
    void BeginDetails(GameObject& game_object);
    void BeginHierarchy(Scene& scene);
    void BeginSceneSwitcher(BaseScene*& currentScene, std::vector<std::pair<std::string, BaseScene*>>& scenes, Window& window);
    std::string GetFont() { return font_name; }
    bool Hover() const;
    bool WantCaptureKeyboard() const;
    bool IsViewportHovered() { return isViewportHovered; }
    void BeginCamera(Camera& camera);
    void BeginSceneSelect(BaseScene*& currentScene, std::vector<BaseScene*>& scenes, Window& window);
    void BeginRecorder(Recorder& recorder);
    void ShowToast(const std::string& message, float duration = 1.5f);

private:
    float mainMenuBarHeight = 10.0f;
    std::string font_name;
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
    std::string toast_message;
    float toast_timer = 0.0f;
    float toast_duration = 0.0f;
    float toast_fade_duration = 0.25f;
};
