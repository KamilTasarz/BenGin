#include "config.h"


class BoundingBox {
public:
	glm::vec3 min_point_local; 
	glm::vec3 max_point_local;
	glm::vec3 min_point_world;
	glm::vec3 max_point_world;

	BoundingBox(const glm::mat4& model, glm::vec3 min_point = glm::vec3(-1.f), glm::vec3 max_point = glm::vec3(1.f)) {
		min_point_local = min_point;
		max_point_local = max_point;
		transformAABB(model);
	}

	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float &t) const; //t - parameter
	void transformAABB(const glm::mat4& model);
};

