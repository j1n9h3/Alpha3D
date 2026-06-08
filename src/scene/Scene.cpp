#include "scene/Scene.h"
#include "renderer/Model.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

#include <imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

// Transform sync — implemented here because they require ImGuizmo
void Transform::SyncFromMatrix() {
    float t[3], r[3], s[3];
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix), t, r, s);
    position = { t[0], t[1], t[2] };
    rotation = { r[0], r[1], r[2] };
    scale    = { s[0], s[1], s[2] };
}

void Transform::SyncToMatrix() {
    float t[3] = { position.x, position.y, position.z };
    float r[3] = { rotation.x, rotation.y, rotation.z };
    float s[3] = { scale.x,    scale.y,    scale.z    };
    ImGuizmo::RecomposeMatrixFromComponents(t, r, s, glm::value_ptr(matrix));
}

// Scene

Entity& Scene::AddEntity(std::string name, Model* model, Shader* shader) {
    m_Entities.push_back(std::make_unique<Entity>(m_NextID++, std::move(name), model, shader));
    return *m_Entities.back();
}

Entity& Scene::AddEntity(std::string name, Mesh* mesh, Shader* shader) {
    m_Entities.push_back(std::make_unique<Entity>(m_NextID++, std::move(name), mesh, shader));
    return *m_Entities.back();
}

void Scene::SetSelected(uint32_t id) {
    m_SelectedID = id;
}

void Scene::ClearSelection() {
    m_SelectedID = 0;
}

Entity* Scene::GetSelected() {
    for (auto& e : m_Entities) {
        if (e->GetID() == m_SelectedID) return e.get();
    }
    return nullptr;
}

void Scene::Render() const {
    for (const auto& e : m_Entities) {
        e->Draw();
    }
}


