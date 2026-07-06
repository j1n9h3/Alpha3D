#pragma once
#include <glfw/glfw3.h>
#include <string>
#include <functional>

#include "core/WindowContext.h"

class Window {
public:
    void Init(int width, int height, int x, int y, const std::string& title);
    ~Window();

    void Update();
    void Destroy();

    float GetWidth();
    float GetHeight();

    bool ShouldClose();
    void ProcessKeyboardInput();
    float GetAspectRatio();

    GLFWwindow* GetGLFWWindow();

    WindowContext& GetWindowContext();
    // void SetWindowContext();

    void SetResizeCallback(std::function<void(int, int)> callback);
    void SetScaleCallback(std::function<void(float)> callback);

    float GetDeltaTime() { return delta_time; }
private:
    float delta_time = 0.0f;
    double last_time = 0.0f;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    GLFWwindow* glfw_window;
    int width, height;
    std::string title;
    WindowContext context;
    float currentScale = 1.0f;

    std::function<void(int, int)> ResizeCallback;
    std::function<void(float)> ScaleCallback;

};