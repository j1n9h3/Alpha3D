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

    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
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
        glfwSetCursorPosCallback(glfw_window, mouse_callback);

        view = glm::lookAt(position, position + direction, up);
    }
    else {
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        this->first_mouse = true;
    }
}

Camera::Camera(float fov, float aspectRatio, float window_width, float window_height) {

    // init camera properties
    view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);

    position = glm::vec3(0.0f, 0.0f, 3.0f);
    //target = glm::vec3(0.0f, 0.0f, 0.0f);
    //direction = glm::normalize(position - target);
    direction = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::normalize(glm::cross(world_up, direction));

    this->lastX = window_width / 2;
    this->lastY = window_height / 2;

    up = glm::cross(direction, right);

    glm::mat4 view;
    view = glm::lookAt(position, position + direction, up);
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

void Camera::SetProjection(float fov, float aspectRatio) {
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}