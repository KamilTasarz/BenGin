#pragma once

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "../Basic/Shader.h"
#include <unordered_set>

struct SnapResult {
	bool shouldSnap = false;
	glm::vec3 snapOffset = glm::vec3(0.f);
};

class Node;

class BoundingBox {

public:

	unsigned int VAO = 0, VBO;

	glm::vec3 min_point_local; 
	glm::vec3 max_point_local;
	glm::vec3 min_point_world;
	glm::vec3 max_point_world;
	//unsigned int VAO, VBO;
	glm::mat4 model;

	Node* node = nullptr;

	std::unordered_set<BoundingBox*> current_collisons;

	short collison = 0;

	BoundingBox(const glm::mat4& model, Node* _node, glm::vec3 min_point = glm::vec3(-1.f), glm::vec3 max_point = glm::vec3(1.f)) {
		min_point_local = min_point;
		max_point_local = max_point;
		transformAABB(model);
		this->model = glm::mat4(model);
		node = _node;
		setBuffers();
	}

	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float &t) const; // t - parameter
	bool isBoundingBoxIntersects(const BoundingBox& other_bounding_box) const;
	
	void separate(const BoundingBox* other_AABB, float separation_mulitplier = 1.f);

	SnapResult trySnapToWallsX(const BoundingBox& other, float snapThreshold);
	SnapResult trySnapToWallsY(const BoundingBox& other, float snapThreshold);
	SnapResult trySnapToWallsZ(const BoundingBox& other, float snapThreshold);
	void transformAABB(const glm::mat4& model);
	void transformWithOffsetAABB(const glm::mat4& model);
	void draw(Shader& shader);
	void setBuffers();
	
	BoundingBox* clone(Node* new_owner) const {
		BoundingBox* box_copy = new BoundingBox(this->model, new_owner, this->min_point_local, this->max_point_local);

		// Skopiuj dane z oryginalnej AABB do nowej (nie trzeba kopiować VAO/VBO – setBuffers() już działa w konstruktorze)
		box_copy->min_point_world = this->min_point_world;
		box_copy->max_point_world = this->max_point_world;
		box_copy->collison = this->collison;

		return box_copy;
	}

};

#endif
