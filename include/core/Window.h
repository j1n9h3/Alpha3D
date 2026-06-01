#pragma once
#include <glfw/glfw3.h>
#include <string>
#include <functional>

#include "core/WindowContext.h"

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void Update();

    float GetWidth();
    float GetHeight();

    bool ShouldClose();
    void ProcessKeyboardInput();
    float GetAspectRatio();

    GLFWwindow* GetGLFWWindow();

    WindowContext& GetWindowContext();
    // void SetWindowContext();

    void SetResizeCallback(std::function<void(int, int)> callback);

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    GLFWwindow* glfw_window;
    int width, height;
    std::string title;
    WindowContext context;

    std::function<void(int, int)> ResizeCallback;
};