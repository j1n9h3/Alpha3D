#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "scene/Entity.h"

class Model;
class Mesh;
class Shader;

class Scene {
public:
    Entity& AddEntity(std::string name, Model* model, Shader* shader);
    Entity& AddEntity(std::string name, Mesh*  mesh,  Shader* shader);

    void    SetSelected(uint32_t id);
    void    ClearSelection();
    Entity* GetSelected();

    void Render()            const;
    void OnImGuiHierarchy();
    void OnImGuiProperties();

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
    uint32_t m_SelectedID = 0;
    uint32_t m_NextID     = 1;
};
