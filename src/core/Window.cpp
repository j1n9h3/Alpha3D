#include "core/Window.h"
#include "core/Log.h"

#include <windows.h>
#include <imm.h>

#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "imm32.lib")

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

void Window::SetResizeCallback(std::function<void(int, int)> callback) {
    ResizeCallback = callback;
}

void Window::framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height)
{
    if (width <= 0 || height <= 0) return;

    WindowContext* context = (static_cast<WindowContext*>(glfwGetWindowUserPointer(glfw_window)));
    if (!context || !context->window) {
        LOG_ERROR(Window, "GLFW UserPointer get failed!");
        return;
    }

    Window* window = context->window;
    if (window) {
        window->width = width;
        window->height = height;

        if (window->ResizeCallback) {
            window->ResizeCallback(width, height);
        }
        glViewport(0, 0, window->width, window->height);

    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

WindowContext& Window::GetWindowContext() {
    return this->context;
}

Window::Window(int width, int height, const std::string& title) {

    // set window properties
    
    glfwInit();

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    this->width = mode->width;
    this->height = mode->height;
    this->title = title;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    //glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // GLFW init
    glfw_window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
    if (glfw_window == NULL)
    {
        LOG_ERROR(Window, "Window creation failed: {}, {}x{}!", title, this->width, this->height);
        glfwTerminate();
    }
    glfwMakeContextCurrent(glfw_window);

    glViewport(0, 0, this->width, this->height);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
    LOG_INFO(Window, "Window created: {}, {}x{}.", title, this->width, this->height);

    #ifdef _WIN32
        HWND hwnd = glfwGetWin32Window(glfw_window);
        ImmAssociateContextEx(hwnd, NULL, IACE_IGNORENOCONTEXT);

        // 自定义标题栏背景色（Windows 11 / Win10 较新版本）
        COLORREF color = RGB(28, 28, 28); // 深灰色
        DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

        // 自定义边框颜色
        COLORREF border_color = RGB(28, 28, 28);
        DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border_color, sizeof(border_color));

        // 暗色模式（让标题栏文字变白）
        BOOL dark = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

        HICON icon = (HICON)LoadImage(NULL, L"assets/icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    #endif

    // set user pointer
    this->context.window = this;
    this->context.camera = nullptr;
    glfwSetWindowUserPointer(glfw_window, &this->context);

    glfwMaximizeWindow(this->GetGLFWWindow());

    LOG_INFO(Window, "Window Context created.");
}

Window::~Window() {
    glfwDestroyWindow(glfw_window);
    //glfwTerminate();
    LOG_INFO(Window, "Window destroyed.");
}

GLFWwindow* Window::GetGLFWWindow() {
    return this->glfw_window;
}

float Window::GetWidth() {
    return this->width;
}

float Window::GetHeight() {
    return this->height;
}

void Window::Update() {
    glfwPollEvents();
    glfwSwapBuffers(this->glfw_window);
}


bool Window::ShouldClose() {
    return glfwWindowShouldClose(this->glfw_window);
}

void Window::ProcessKeyboardInput() {
    if (glfwGetKey(this->glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->glfw_window, true);
}

float Window::GetAspectRatio() {
    return (float)this->width / (float)this->height;
}