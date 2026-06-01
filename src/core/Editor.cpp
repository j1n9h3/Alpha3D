// Editor.cpp
#include "core/Editor.h"

Editor::Editor(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf", 16.0f * xscale);
    ImGui::GetStyle().ScaleAllSizes(xscale);

    LOG_INFO(Editor, "Success creating editor with x{} scale", xscale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open")) {}
            if (ImGui::MenuItem("Save")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Editor::Hover() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool Editor::WantCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}
