#pragma once
#include <glfw/glfw3.h>
#include <string>
#include <functional>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void Update();

    float GetWidth();
    float GetHeight();

    bool ShouldClose();
    void ProcessInput();
    float GetAspectRatio();


    void SetResizeCallback(std::function<void(int, int)> callback);

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    GLFWwindow* m_Window;
    int m_Width, m_Height;

    std::function<void(int, int)> m_ResizeCallback;
};