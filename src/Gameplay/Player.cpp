#include "Player.h"

void Player::update(float delta_time, short inputs, float camera_yaw) {
	yaw = camera_yaw;
    front.x = cos(glm::radians(yaw));
    front.y = 0.f;
    front.z = sin(glm::radians(yaw));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, up));

    glm::vec3 pos = player_node->transform.getGlobalPosition();

    vel.x = 0;
    vel.z = 0;

    
    if (1 & inputs) {
        vel.x += speed * front.x;
        vel.z += speed * front.z;
    }

    if (2 & inputs) {
        vel.x += -speed * front.x;
        vel.z += -speed * front.z;
    }
    if (4 & inputs) {
        vel.x += -right.x * speed;
        vel.z += -right.z * speed;
    }
    if (8 & inputs) {
        vel.x += right.x * speed;
        vel.z += right.z * speed;
    }
    



    if (16 & inputs && on_ground) //jump
        vel.y = up.y * v_0;

    if (delta_time < 0.01f) {
        pos += (vel * delta_time) + 0.5f * grav * delta_time * delta_time;
        vel += grav * delta_time;
    }



    

    if (vel.y < 0.f && player_node->AABB->collison) {
        on_ground = true;
        vel.y = 0.f;
    }
    else {
        on_ground = false;
    }
    player_node->AABB->collison = false;
    player_node->transform.setLocalPosition(pos);
}
