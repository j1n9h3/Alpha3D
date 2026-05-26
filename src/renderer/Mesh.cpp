#include <glad/glad.h>
#include "core/Log.h"

#include "renderer/Mesh.h"

Mesh::Mesh(float* vertices, unsigned int size) {
	vertex_count = size / (5 * sizeof(float)); // 每个顶点5个float (xyz + uv)

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	// position (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texcoord (location = 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	LOG_INFO(Mesh, "Mesh created, {} vertices", vertex_count);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	LOG_INFO(Mesh, "Mesh destroyed");
}

void Mesh::Bind() {
	glBindVertexArray(vao);
}

void Mesh::Draw() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	glBindVertexArray(0);
}