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

class Collider {
public:
	unsigned int VAO = 0, VBO = 0;

	bool is_logic = false;

	bool active = true;

	Node* node = nullptr;

	Collider(Node* node) : node(node) {};
	virtual ~Collider() = default;

	virtual bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float& t, glm::vec3& endPoint) = 0;
};

class BoundingBox : public Collider {

public:


	glm::vec3 min_point_local; 
	glm::vec3 max_point_local;
	glm::vec3 min_point_world;
	glm::vec3 max_point_world;
	//unsigned int VAO, VBO;
	glm::mat4 model;

	std::unordered_set<BoundingBox*> current_collisons;

	short collison = 0;

	BoundingBox(const glm::mat4& model, Node* _node, glm::vec3 min_point = glm::vec3(-1.f), glm::vec3 max_point = glm::vec3(1.f), bool set_buffer = true) : Collider(_node) {
		min_point_local = min_point;
		max_point_local = max_point;
		transformAABB(model);
		this->model = glm::mat4(model);
		//node = _node;
		if (set_buffer) setBuffers();
	}
	~BoundingBox() {

		if (VAO != 0) {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			VAO = 0;
			VBO = 0;
		}
	}

	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float &t, glm::vec3& endPoint) override; // t - parameter
	bool isBoundingBoxIntersects(const BoundingBox& other_bounding_box) const;
	
	void separate(const BoundingBox* other_AABB, float separation_mulitplier = 1.f);

	SnapResult trySnapToWallsX(const BoundingBox& other, float snapThreshold);
	SnapResult trySnapToWallsY(const BoundingBox& other, float snapThreshold);
	SnapResult trySnapToWallsZ(const BoundingBox& other, float snapThreshold);
	void transformAABB(const glm::mat4& model);
	void transformWithOffsetAABB(const glm::mat4& model);
	void draw(Shader& shader);
	void setBuffers();

	glm::vec3 getCenter() const {
		return (min_point_world + max_point_world) / 2.f;
	}
	
	BoundingBox* clone(Node* new_owner) const {
		BoundingBox* box_copy = new BoundingBox(this->model, new_owner, this->min_point_local, this->max_point_local);

		// Skopiuj dane z oryginalnej AABB do nowej (nie trzeba kopiować VAO/VBO – setBuffers() już działa w konstruktorze)
		box_copy->min_point_world = this->min_point_world;
		box_copy->max_point_world = this->max_point_world;
		box_copy->collison = this->collison;
		box_copy->is_logic = this->is_logic;

		return box_copy;
	}

};

class Capsule {
public:
	glm::vec3 A, B, mid;
	float radius, height;

	unsigned int VAO, VBO;

	glm::mat4 model;

	Node* node = nullptr;

	std::unordered_set<BoundingBox*> current_collisons;

	bool is_logic = false;

	bool active = true;

	short collison = 0;

	Capsule(glm::vec3& min, glm::vec3& max, float radius);
	Capsule(glm::vec3& mid, float height, float radius);

	void draw(Shader& shader);
	void setBuffers();
};

class RectOBB : public Collider {
public:
	unsigned int VAO = 0, VBO = 0;
	glm::mat4 model;
	glm::vec3 normal, normalGlobal;
	glm::vec3 minLocal, maxLocal, minGlobal, maxGlobal;

	RectOBB(glm::mat4 model, Node* owner, glm::vec3 min = glm::vec3(-1.f, 0.f, -1.f), glm::vec3 max = glm::vec3(1.f, 0.f, 1.f)) : Collider(owner) {
		this->minLocal = min;
		this->maxLocal = max;
		normal = glm::normalize(glm::cross(glm::normalize(max - min), glm::vec3(0.f, 0.f, -1.f)));
		transform(model);
	};

	void transform(glm::mat4 _model);
	void setBuffers();
	void draw(Shader& shader);
	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float& t, glm::vec3& endPoint) override;
};

#endif
