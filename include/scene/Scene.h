#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
//#include "scene/GameObject.h"

class GameObject;
class Model;
class Mesh;
class Shader;

class Scene {
public:
    ~Scene();
    GameObject& AddGameObject(std::string name, Model* model, Shader* shader);
    GameObject& AddGameObject(std::string name, Mesh* mesh, Shader* shader);

    void SetSelected(uint32_t id) { this->selected_id = id; };
    GameObject* GetSelected();

    void ClearSelection() { this->selected_id = -1; };

    void Render() const;

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return game_objects; }

private:
    std::vector<std::unique_ptr<GameObject>> game_objects;
    uint32_t selected_id = 0;
    uint32_t next_id = 1;
};
