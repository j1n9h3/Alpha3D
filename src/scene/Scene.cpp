#include "scene/Scene.h"
#include "renderer/Model.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "scene/GameObject.h"

#include <glm/gtc/type_ptr.hpp>

Scene::~Scene() = default;

GameObject& Scene::AddGameObject(std::string name, Model* model, Shader* shader) {
    game_objects.push_back(std::make_unique<GameObject>(next_id++, std::move(name), model, shader));
    return *game_objects.back();
}

GameObject& Scene::AddGameObject(std::string name, Mesh* mesh, Shader* shader) {
    game_objects.push_back(std::make_unique<GameObject>(next_id++, std::move(name), mesh, shader));
    return *game_objects.back();
}

GameObject* Scene::GetSelected() {
    for (auto& e : game_objects) {
        if (e->GetID() == selected_id) return e.get();
    }
    return nullptr;
}

void Scene::Render() const {
    for (const auto& game_object : game_objects) {
        if (game_object->pbr_test.has_value()) {
            Shader* shader = game_object->GetShader(); // ÐèÒª GameObject ±©Â¶ GetShader()
            shader->setVec3("baseColor", (*game_object->pbr_test).albedo);
            shader->setFloat("roughness", (*game_object->pbr_test).roughness);
            shader->setFloat("metallic", (*game_object->pbr_test).metallic);
        }
        game_object->Draw();
    }
}


