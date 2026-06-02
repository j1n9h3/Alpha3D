// Camera.cpp
#include "renderer/Camera.h"
#include "core/WindowContext.h"
#include "core/Log.h"

void mouse_callback(GLFWwindow* glfw_window, double xpos, double ypos)
{
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(glfw_window));
    if (!context) {
        LOG_ERROR(Camera, "No Window Context Get from GLFW UserPointer!");
        return;
    }

    Camera* camera = context->camera;
    if (!context->camera) {
        LOG_ERROR(Camera, "No Camera Pointer in Window Context!");
        return;
    }

    camera->ProcessMouseMovement(xpos, ypos);
}

void Camera::ProcessMouseMovement(float xpos, float ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    this->yaw += xoffset;
    this->pitch += yoffset;

    if (this->pitch > 89.0f)
        this->pitch = 89.0f;
    if (this->pitch < -89.0f)
        this->pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    this->direction = glm::normalize(front);
}

void Camera::ProcessInput(GLFWwindow* glfw_window)
{
    if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        float cameraSpeed = 0.05f; // adjust accordingly
        if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS)
            position += cameraSpeed * direction;
        if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS)
            position -= cameraSpeed * direction;
        if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(direction, up)) * cameraSpeed;
        if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(direction, up)) * cameraSpeed;

        if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS)
            glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        view = glm::lookAt(position, position + direction, up);
    }
    else {
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        this->firstMouse = true;
    }
}

Camera::Camera(float fov, Window& window) {

    float aspectRatio = window.GetAspectRatio();
    float window_width = window.GetWidth(), window_height = window.GetHeight();

    glfwSetCursorPosCallback(window.GetGLFWWindow(), mouse_callback);

    m_Fov         = fov;
    m_AspectRatio = aspectRatio;

    position  = glm::vec3(0.0f, 0.0f, 3.0f);
    direction = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::normalize(glm::cross(world_up, direction));
    up    = glm::cross(direction, right);

    this->lastX = window_width / 2;
    this->lastY = window_height / 2;

    view = glm::lookAt(position, position + direction, up);
    RebuildProjection();
}

glm::vec3 Camera::GetPosition() {
    return position;
}

glm::mat4 Camera::GetView() {
    return view;
}

glm::mat4 Camera::GetProjection() {
    return projection;
}

void Camera::RebuildProjection() {
    if (m_IsOrtho)
        projection = glm::ortho(
            -m_OrthoSize * m_AspectRatio,  m_OrthoSize * m_AspectRatio,
            -m_OrthoSize,                   m_OrthoSize,
            m_Near, m_Far);
    else
        projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
}

void Camera::SetProjection(float fov, float aspectRatio) {
    m_Fov = fov;
    m_AspectRatio = aspectRatio;
    RebuildProjection();
}

void Camera::SetFov(float fov) {
    m_Fov = fov;
    RebuildProjection();
}

void Camera::SetOrtho(bool isOrtho) {
    m_IsOrtho = isOrtho;
    RebuildProjection();
}

void Camera::ResetMouseState() {
    firstMouse = true;
}


void Camera::OnImGuiCamera(ImGuizmo::OPERATION& gizmoOp) {

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);


    float headbarWidth = viewport->Size.x / 3 * 2;
    float menuBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(
        viewport->Pos.x + 2.0f,
        viewport->Pos.y + menuBarHeight * 2 + 2.0f)
    );
    //ImGui::SetNextWindowSize(ImVec2(
    //    headbarWidth,
    //    menuBarHeight * 2)
    //);

    float menuBarH = ImGui::GetFrameHeight();
    ImGui::SetNextWindowBgAlpha(0.0f);
    constexpr ImGuiWindowFlags kToolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
    if (ImGui::Begin("##toolbar", nullptr, kToolbarFlags)) {

        auto modeBtn = [&](const char* label, ImGuizmo::OPERATION op) {
            bool active = (gizmoOp == op);
            if (active) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            }
            else {
                ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
                color.x -= 0.2f;
                color.y -= 0.2f;
                color.z -= 0.2f;
                color.w = 1.0f;
                ImGui::PushStyleColor(ImGuiCol_Button, color);
            }
            if (ImGui::Button(label)) gizmoOp = op;
            
            ImGui::PopStyleColor();
            
        };
        modeBtn("Translate##t", ImGuizmo::TRANSLATE);
        modeBtn("Rotate##r", ImGuizmo::ROTATE);
        modeBtn("Scale##s", ImGuizmo::SCALE);


        const char* projLabel = this->IsOrtho() ? "Ortho" : "Persp";
        if (ImGui::Button(projLabel)) this->SetOrtho(!this->IsOrtho());


        if (!this->IsOrtho()) {
            ImGui::SameLine();
            float fov = this->GetFov();
            ImGui::SetNextItemWidth(140.0f);
            if (ImGui::SliderFloat("FOV", &fov, 10.0f, 170.0f))
                this->SetFov(fov);
        }

    }
    ImGui::PopStyleVar(3);
    ImGui::End();
}