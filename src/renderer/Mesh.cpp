#include <glad/glad.h>
#include "core/Log.h"

#include "renderer/Mesh.h"

Mesh::Mesh(const float* vertices, unsigned int size) {
	vertex_count = size / (8 * sizeof(float)); // 每个顶点5个float (xyz + uv)

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	// position (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texcoord (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	LOG_INFO(Mesh, "Mesh created, {} vertices.", vertex_count);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// texcoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (ebo) glDeleteBuffers(1, &ebo);
}

void Mesh::Bind() {
	glBindVertexArray(vao);
}

void Mesh::Draw() {
	glBindVertexArray(vao);

	if (indices.empty()) {
		glDrawArrays(GL_TRIANGLES, 0, vertex_count);        // Cube 走这里
	}
	else {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Sphere 走这里
	}

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int albedoNr = 1;
	unsigned int roughnessNr = 1;
	unsigned int normalNr = 1;
	unsigned int emissiveNr = 1;
	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); 
		std::string number;
		std::string type = this->textures[i].GetType();
		if (type == "albedo")
			number = std::to_string(albedoNr++);
		else if (type == "roughness")
			number = std::to_string(roughnessNr++);
		else if (type == "normal")
			number = std::to_string(normalNr++);
		else if (type == "emissive")
			number = std::to_string(emissiveNr++);

		shader.setInt(("material." + type + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].GetId());
	}
	glActiveTexture(GL_TEXTURE0);

	// 绘制网格
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}