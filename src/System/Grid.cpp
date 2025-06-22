#include "Grid.h"
#include "../Component/CameraGlobals.h"

void Grid::Init()
{
	if (gridType == GRID_XZ) {
		// Generate grid vertices
		for (int i = -halfCount; i <= halfCount; ++i) {
			lineVertices.push_back(glm::vec3(-halfCount * gridSize, 0.0f, i * gridSize));
			lineVertices.push_back(glm::vec3(halfCount * gridSize, 0.0f, i * gridSize));
			lineVertices.push_back(glm::vec3(i * gridSize, 0.0f, -halfCount * gridSize));
			lineVertices.push_back(glm::vec3(i * gridSize, 0.0f, halfCount * gridSize));
		}
	}
	else { //GRID XY
		for (int i = -halfCount; i <= halfCount; ++i) {
			lineVertices.push_back(glm::vec3(-halfCount * gridSize, i * gridSize, 0.0f));
			lineVertices.push_back(glm::vec3(halfCount * gridSize, i * gridSize, 0.0f));
			lineVertices.push_back(glm::vec3(i * gridSize, -halfCount * gridSize, 0.0f));
			lineVertices.push_back(glm::vec3(i * gridSize, halfCount * gridSize, 0.0f));
		}
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Grid::Update()
{
	lineVertices.clear();
	float offset = 0.5f;
	if (gridType == GRID_XZ) {
		// Generate grid vertices
		for (int i = -halfCount; i <= halfCount; ++i) {
			
			lineVertices.push_back(glm::vec3(-halfCount * gridSize, 0.0f, i * gridSize + offset));
			lineVertices.push_back(glm::vec3(halfCount * gridSize, 0.0f, i * gridSize + offset));
			lineVertices.push_back(glm::vec3(i * gridSize + offset, 0.0f, -halfCount * gridSize));
			lineVertices.push_back(glm::vec3(i * gridSize + offset, 0.0f, halfCount * gridSize));
		}
	}
	else { //GRID XY
		for (int i = -halfCount; i <= halfCount; ++i) {
			lineVertices.push_back(glm::vec3(-halfCount * gridSize, i * gridSize + offset, 0.0f));
			lineVertices.push_back(glm::vec3(halfCount * gridSize, i * gridSize + offset, 0.0f));
			lineVertices.push_back(glm::vec3(i * gridSize + offset, -halfCount * gridSize, 0.0f));
			lineVertices.push_back(glm::vec3(i * gridSize + offset, halfCount * gridSize, 0.0f));
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Grid::Draw()
{
	gridShader->use();
	glm::mat4 view = camera->GetView();
	glm::mat4 projection = camera->GetProjection();
	glm::mat4 view_projection = projection * view;
	glm::mat4 model = glm::mat4(1.0f);
	gridShader->setMat4("view_projection", view_projection);
	gridShader->setMat4("model", model);
	gridShader->setVec3("color", glm::vec3(0.2f));
	glBindVertexArray(vao);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, lineVertices.size());
	glBindVertexArray(0);
}
