#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "renderer/Shader.h"
#include "renderer/Texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

class Mesh {
public:
    Mesh(const float* vertices, unsigned int count); // for simple cube
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures); // for assimp
    ~Mesh();

    // 禁用拷贝
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // 允许移动
    Mesh(Mesh&& other) noexcept
        : vao(other.vao), vbo(other.vbo), ebo(other.ebo),
        vertices(std::move(other.vertices)),
        indices(std::move(other.indices)),
        textures(std::move(other.textures)),
        vertex_count(other.vertex_count)
    {
        other.vao = 0;  // 置0，析构时不会删除
        other.vbo = 0;
        other.ebo = 0;
    }

    void Bind();
    
    void Draw();
    void Draw(Shader& shader);
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int vao, vbo, ebo;
    unsigned int light_vao, light_vbo;
    unsigned int vertex_count;
};