#pragma once

#include "../config.h"

struct ParticleInstanceData {

	glm::vec3 position;
	glm::vec4 color;

};

struct Particle
{

	glm::vec3 position, velocity;

	float life;

	Particle() : position(0.f), velocity(0.f), life(0.f) {}

};

