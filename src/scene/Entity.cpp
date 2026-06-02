#include "scene/Entity.h"
#include "renderer/Model.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

Entity::Entity(uint32_t id, std::string name, Model* model, Shader* shader)
    : m_ID(id), m_Name(std::move(name)), m_Model(model), m_Shader(shader) {}

Entity::Entity(uint32_t id, std::string name, Mesh* mesh, Shader* shader)
    : m_ID(id), m_Name(std::move(name)), m_Mesh(mesh), m_Shader(shader) {}

void Entity::Draw() const {
    if (!m_Visible || !m_Shader) return;

    m_Shader->use();
    m_Shader->setMat4("model", m_Transform.GetModelMatrix());

    if (m_Model) {
        m_Shader->setMat3("normalMatrix", m_Transform.GetNormalMatrix());
        m_Model->Draw(*m_Shader);
    } else if (m_Mesh) {
        m_Mesh->Draw();
    }
}
