#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles, degrees
    glm::vec3 scale    = glm::vec3(1.0f);
    glm::mat4 matrix   = glm::mat4(1.0f); // primary state, written directly by ImGuizmo

    // Implemented in Scene.cpp (requires ImGuizmo)
    void SyncFromMatrix(); // matrix -> position/rotation/scale
    void SyncToMatrix();   // position/rotation/scale -> matrix

    const glm::mat4& GetModelMatrix() const { return matrix; }

    glm::mat3 GetNormalMatrix() const {
        return glm::mat3(glm::transpose(glm::inverse(matrix)));
    }
};
