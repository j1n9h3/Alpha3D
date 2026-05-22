#include "core/Window.h"
#include "core/Log.h"

void Window::SetResizeCallback(std::function<void(int, int)> callback) {
    m_ResizeCallback = callback;
}

void Window::framebuffer_size_callback(GLFWwindow* m_window, int width, int height)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(m_window));
    if (window) {
        window->m_Width = width;
        window->m_Height = height;

        if (window->m_ResizeCallback) {
            window->m_ResizeCallback(width, height);
        }
    }
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

Window::Window(int width, int height, const std::string& title) {
    m_Width = width;
    m_Height = height;
    
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
    m_Window = glfwCreateWindow(width, height, "title", NULL, NULL);
    if (m_Window == NULL)
    {
        ERROR(Window, "Window creation failed: {}, {}x{}", title, width, height);
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_Window);

    glViewport(0, 0, width, height);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    INFO(Window, "Window creation created: {}, {}x{}", title, width, height);
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
    INFO(Window, "Window destroyed");
}

float Window::GetWidth() {
    return m_Width;
}

float Window::GetHeight() {
    return m_Height;
}

void Window::Update() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}


bool Window::ShouldClose() {
    return glfwWindowShouldClose(m_Window);
}

void Window::ProcessInput() {
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);
}

float Window::GetAspectRatio() {
    return (float)m_Width / (float)m_Height;
}