#pragma once
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

#include "core/Log.h"
#include "imgui_internal.h"


class Editor {
public:
    Editor(GLFWwindow* window);
    ~Editor();

    void BeginFrame();
    void EndFrame();

    bool Hover() const;
    bool WantCaptureKeyboard() const;
private:
    float mainMenuBarHeight = 10.0f;
    ImFont* font_small;
    ImFont* font_normal;
    ImFont* font_large;
};