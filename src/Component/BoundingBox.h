#pragma once

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "../Basic/Shader.h"


class BoundingBox {

private:

	unsigned int VAO = 0, VBO;

public:

	glm::vec3 min_point_local; 
	glm::vec3 max_point_local;
	glm::vec3 min_point_world;
	glm::vec3 max_point_world;
	//unsigned int VAO, VBO;
	glm::mat4 model;

	bool collison = false;

	BoundingBox(const glm::mat4& model, glm::vec3 min_point = glm::vec3(-1.f), glm::vec3 max_point = glm::vec3(1.f)) {
		min_point_local = min_point;
		max_point_local = max_point;
		transformAABB(model);
		this->model = glm::mat4(model);
		//this->model = glm::mat4(1.f);
	}

	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float &t) const; // t - parameter
	bool isBoundingBoxIntersects(const BoundingBox& other_bounding_box) const; // t - parameter
	void transformAABB(const glm::mat4& model);
	void draw(Shader& shader);
	
};

#endif
