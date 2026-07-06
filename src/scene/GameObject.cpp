#include "scene/GameObject.h"
#include "renderer/Model.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "core/time.h"

GameObject::GameObject(uint32_t id, std::string name, Model* model, Shader* shader)
    : id(id), name(std::move(name)), model(model), shader(shader), type("model") { }

GameObject::GameObject(uint32_t id, std::string name, Mesh* mesh, Shader* shader)
    : id(id), name(std::move(name)), mesh(mesh), shader(shader), type("mesh") { }

void GameObject::Draw() const {
    if (!visible || !shader) return;

    shader->use();
    shader->setMat4("model", transform.GetModelMatrix());

    if (model) {
        shader->setMat3("normalMatrix", transform.GetNormalMatrix());
        model->Draw(*shader);
    } else if (mesh) {
        shader->setMat3("normalMatrix", transform.GetNormalMatrix());
        mesh->Draw();
    }
}


void GameObject::Update()
{
    transform.Rotate(
        glm::vec3(
            rotate_speed_x * Time::DeltaTime(), 
            rotate_speed_y * Time::DeltaTime(), 
            rotate_speed_z * Time::DeltaTime()
        )
    );
    transform.SyncToMatrix();
}