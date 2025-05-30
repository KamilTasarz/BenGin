#pragma once
#include "../config.h"

class LineManager
{
private:
	std::vector<float> vertices;
	unsigned int iterator, size, VAO = 0, VBO = 0;
	LineManager() {

		vertices.resize(2500);
		iterator = 0;
		size = 0;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
	}
	~LineManager() = default;

public:

	static LineManager& Instance() {
		static LineManager instance;
		return instance;
	}
	
	void clearLines();
	void drawLines();
	void addVertices(std::vector<glm::vec3>& verts);

};

