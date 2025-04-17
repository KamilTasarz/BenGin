#pragma once
#include "config.h"

class SimpleObject {
public:
	glm::mat4 model;
	glm::vec3 pos;
	int numberOfTextures;
	unsigned int* textures;

	SimpleObject(int _numberOfTextures = 1) {
		numberOfTextures = _numberOfTextures;
		textures = new unsigned int[numberOfTextures];
	}

	~SimpleObject() {

		delete[] textures;
	}
};

