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
};

class Mesh {
public:
    Mesh(float* vertices, unsigned int count); // for simple cube
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures); // for assimp
    ~Mesh();

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