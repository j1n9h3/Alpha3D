#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio);
    glm::mat4 GetView();
    glm::mat4 GetProjection();
    void SetProjection(float fov, float aspectRatio);
private:
    glm::mat4 m_View;
    glm::mat4 m_Projection;
};