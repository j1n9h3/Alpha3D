#pragma once

class Mesh {
public:
    Mesh(float* vertices, unsigned int count);
    ~Mesh();
    void Bind();
    void Draw();
private:
    unsigned int vao, vbo;
    unsigned int vertex_count;
};