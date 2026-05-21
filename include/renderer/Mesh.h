class Mesh {
public:
    Mesh(float* vertices, unsigned int count);
    ~Mesh();
    void Bind();
    void Draw();
private:
    unsigned int m_VAO, m_VBO;
    unsigned int m_VertexCount;
};