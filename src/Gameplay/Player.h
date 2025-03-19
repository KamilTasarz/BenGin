#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../Basic/Node.h"

class Player {
public:
	Node* player_node;
	glm::vec3 vel;
	float h_0;
	glm::vec3 grav;
	float speed;
	float v_0;

	bool on_ground = false;

	float yaw = 90.f;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);

	Player(Node* _player_node, float _h_0, float _range, float _speed) {
		player_node = _player_node;
		h_0 = _h_0;
		speed = _speed;
		v_0 = 2 * h_0 * speed / _range;
		grav = glm::vec3(0.f, -2 * h_0 * speed * speed / (_range * _range), 0.f);
		vel = glm::vec3(0.f);
	}

	void update(float delta_time, short inputs, float camera_yaw);
};

#endif
