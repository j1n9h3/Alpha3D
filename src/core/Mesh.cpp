#include <glad/glad.h>
#include "core/Log.h"

#include "renderer/Mesh.h"

Mesh::Mesh(float* vertices, unsigned int size) {
	m_VertexCount = size / (5 * sizeof(float)); // 每个顶点5个float (xyz + uv)

	// VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// VBO
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	// position (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texcoord (location = 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	INFO(Window, "Mesh created, {} vertices", m_VertexCount);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	INFO(Window, "Mesh destroyed");
}

void Mesh::Bind() {
	glBindVertexArray(m_VAO);
}

void Mesh::Draw() {
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
	glBindVertexArray(0);
}

