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

void Scene::OnImGuiHierarchy() {

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float sidebarWidth = viewport->Size.x / 6;
    float menuBarHeight = ImGui::GetFrameHeight();


    ImGui::SetNextWindowPos(ImVec2(
        viewport->Pos.x,
        viewport->Pos.y + menuBarHeight)
    );
    
    ImGui::SetNextWindowSize(ImVec2(
        sidebarWidth,
        viewport->Size.y - menuBarHeight)
    );

    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    if (ImGui::CollapsingHeader("Scene Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (auto& entityPtr : m_Entities) {
            Entity* e = entityPtr.get();
            bool isSelected = (m_SelectedID == e->GetID());

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                (isSelected ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)e->GetID(), flags, "%s", e->GetName().c_str());
            if (ImGui::IsItemClicked())
                SetSelected(e->GetID());
        }
    }
    ImGui::End();
}

void Scene::OnImGuiProperties() {

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    float sidebarWidth = viewport->Size.x / 6;
    float menuBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(
        viewport->Pos.x + viewport->Size.x - sidebarWidth,
        viewport->Pos.y + menuBarHeight)
    );
    ImGui::SetNextWindowSize(ImVec2(
        sidebarWidth, 
        viewport->Size.y - menuBarHeight)
    );


    ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
        Entity* sel = GetSelected();
        if (!sel) {
            ImGui::TextDisabled("No entity selected.");
            ImGui::End();
            return;
        }

        ImGui::Text("%s", sel->GetName().c_str());
        ImGui::Separator();

        Transform& t = sel->GetTransform();
        t.SyncFromMatrix();

        bool changed = false;
        changed |= ImGui::DragFloat3("Position", &t.position.x, 0.01f);
        changed |= ImGui::DragFloat3("Rotation", &t.rotation.x, 0.5f);
        changed |= ImGui::DragFloat3("Scale", &t.scale.x, 0.01f, 0.001f);

        if (changed)
            t.SyncToMatrix();
    }
    ImGui::End();
}
