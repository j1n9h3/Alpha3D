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
    Entity(uint32_t id, std::string name, Mesh*  mesh,  Shader* shader);

    uint32_t GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }

    Transform& GetTransform() { return m_Transform; }
    const Transform& GetTransform() const { return m_Transform; }

    bool IsVisible() const { return m_Visible; }
    void SetVisible(bool v) { m_Visible = v; }

    void Draw() const;

private:
    uint32_t m_ID;
    std::string m_Name;
    Transform m_Transform;
    Model* m_Model  = nullptr;
    Mesh* m_Mesh   = nullptr;
    Shader* m_Shader = nullptr;
    bool m_Visible = true;
};
