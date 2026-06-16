#pragma once
#include <string>
#include <cstdint>
#include "scene/Transform.h"

class Model;
class Mesh;
class Shader;

class Entity {
public:
    Entity(uint32_t id, std::string name, Model* model, Shader* shader);
    Entity(uint32_t id, std::string name, Mesh* mesh, Shader* shader);

    // Identity
    uint32_t           GetID()   const { return m_ID; }
    const std::string& GetName() const { return m_Name; }
    void               SetName(const std::string& name) { m_Name = name; }

    // Transform ¡ª Ö±½Ó²Ù×÷
    Transform& GetTransform() { return m_Transform; }
    const Transform& GetTransform() const { return m_Transform; }

    void SetPosition(const glm::vec3& pos) { m_Transform.SetPosition(pos); }
    void SetRotation(const glm::vec3& rot) { m_Transform.SetRotation(rot); }
    void SetScale(const glm::vec3& scale) { m_Transform.SetScale(scale); }

    void Translate(const glm::vec3& delta) { m_Transform.Translate(delta); }
    void Rotate(const glm::vec3& deltaDegrees) { m_Transform.Rotate(deltaDegrees); }
    void Scale(const glm::vec3& factor) { m_Transform.Scale(factor); }

    glm::vec3 GetPosition() const { return m_Transform.position; }
    glm::vec3 GetRotation() const { return m_Transform.rotation; }
    glm::vec3 GetScale()    const { return m_Transform.scale; }

    void ResetTransform() { m_Transform.Reset(); }

    // Shader
    Shader* GetShader() const { return m_Shader; }
    void    SetShader(Shader* shader) { m_Shader = shader; }

    // Visibility
    bool IsVisible()          const { return m_Visible; }
    void SetVisible(bool v) { m_Visible = v; }
    void ToggleVisible() { m_Visible = !m_Visible; }

    // Draw
    void Draw() const;

private:
    uint32_t    m_ID;
    std::string m_Name;
    Transform   m_Transform;
    Model* m_Model = nullptr;
    Mesh* m_Mesh = nullptr;
    Shader* m_Shader = nullptr;
    bool m_Visible = true;
};