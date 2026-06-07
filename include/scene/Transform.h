#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles, degrees
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 matrix = glm::mat4(1.0f); // primary state, written directly by ImGuizmo

    // Sync
    void SyncFromMatrix(); // matrix -> position/rotation/scale
    void SyncToMatrix();   // position/rotation/scale -> matrix

    // Getters
    const glm::mat4& GetModelMatrix() const { return matrix; }

    glm::mat3 GetNormalMatrix() const {
        return glm::mat3(glm::transpose(glm::inverse(matrix)));
    }

    // Setters — 设置后自动同步 matrix
    void SetPosition(const glm::vec3& pos) {
        position = pos;
        SyncToMatrix();
    }

    void SetRotation(const glm::vec3& rot) {
        rotation = rot;
        SyncToMatrix();
    }

    void SetScale(const glm::vec3& s) {
        scale = s;
        SyncToMatrix();
    }

    // Translate/Rotate/Scale 增量操作
    void Translate(const glm::vec3& delta) {
        position += delta;
        SyncToMatrix();
    }

    void Rotate(const glm::vec3& deltaDegrees) {
        rotation += deltaDegrees;
        SyncToMatrix();
    }

    void Scale(const glm::vec3& factor) {
        scale *= factor;
        SyncToMatrix();
    }

    // 重置到默认状态
    void Reset() {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
        matrix = glm::mat4(1.0f);
    }

    // 是否是默认状态
    bool IsIdentity() const {
        return position == glm::vec3(0.0f)
            && rotation == glm::vec3(0.0f)
            && scale == glm::vec3(1.0f);
    }
};