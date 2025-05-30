#include "LineManager.h"
#include "../ResourceManager.h"
#include "../Component/CameraGlobals.h"

void LineManager::clearLines()
{
	size = 0;
	iterator = 0;
}

void LineManager::drawLines()
{
	if (size > 0) {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * size, &vertices[0]);

		ResourceManager::Instance().shader_line->use();
		ResourceManager::Instance().shader_line->setMat4("view", camera->GetView());
		ResourceManager::Instance().shader_line->setMat4("projection", camera->GetProjection());

		glDrawArrays(GL_LINE_STRIP, 0, size);
		glBindVertexArray(0);
	}
}

void LineManager::addVertices(std::vector<glm::vec3>& verts)
{
	for (int i = 0; i < verts.size(); ++i) {
		vertices[iterator] = verts[i].x;
		iterator++;
		vertices[iterator] = verts[i].y;
		iterator++;
		vertices[iterator] = verts[i].z;
		iterator++;
		size++;
	}
}
