#include "config.h"


class BoundingBox {
	glm::vec3 min_point_local; 
	glm::vec3 max_point_local;
	glm::vec3 min_point_world;
	glm::vec3 max_point_world;

	BoundingBox(glm::mat4 model, glm::vec3 min_point = glm::vec3(-1.f), glm::vec3 max_point = glm::vec3(1.f)) {
		min_point_local = min_point;
		max_point_local = max_point;
		min_point_world = glm::vec3(model * glm::vec4(min_point_local, 1.0f));
		max_point_world = glm::vec3(model * glm::vec4(max_point_local, 1.0f));
	}

	bool isRayIntersects(glm::vec3 direction, glm::vec3 origin, float &t) const; //t - parameter
};

