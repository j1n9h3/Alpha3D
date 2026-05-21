// Camera.cpp
#include "renderer/Camera.h"

Camera::Camera(float fov, float aspectRatio) {
    m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    m_Projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

glm::mat4 Camera::GetView() {
    return m_View;
}

glm::mat4 Camera::GetProjection() {
    return m_Projection;
}
