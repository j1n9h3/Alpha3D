#include <glfw/glfw3.h>
#include <string>

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

private:
    GLFWwindow* m_Window;
    int m_Width, m_Height;
};