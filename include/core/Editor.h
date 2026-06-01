#pragma once
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

#include "core/Log.h"

class Editor {
public:
    Editor(GLFWwindow* window);
    ~Editor();

    void BeginFrame();
    void EndFrame();

    bool Hover() const;
    bool WantCaptureKeyboard() const;
};