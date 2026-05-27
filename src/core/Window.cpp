#include "core/Window.h"
#include "core/Log.h"

void Window::SetResizeCallback(std::function<void(int, int)> callback) {
    ResizeCallback = callback;
}

void Window::framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height)
{
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
    }
    glViewport(0, 0, width, height);
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
    this->width = width;
    this->height = height;
    this->title = title;
    
    glfwInit();
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

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
    LOG_INFO(Window, "Window created: {}, {}x{}.", title, this->width, this->height);

    // set user pointer
    this->context.window = this;
    this->context.camera = nullptr;
    glfwSetWindowUserPointer(glfw_window, &this->context);
    LOG_INFO(Window, "Window Context created.");
}

Window::~Window() {
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
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

void Window::ProcessInput() {
    if (glfwGetKey(this->glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->glfw_window, true);
}

float Window::GetAspectRatio() {
    return (float)this->width / (float)this->height;
}