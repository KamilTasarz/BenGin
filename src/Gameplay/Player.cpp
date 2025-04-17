#include "Player.h"

void Player::update(float delta_time, short inputs, float camera_yaw) {

	yaw = camera_yaw;
    /*front.x = cos(glm::radians(yaw));
    front.y = 0.f;
    front.z = sin(glm::radians(yaw));
    front = glm::normalize(front);*/

    front = glm::vec3(0.f, 0.f, -1.f);

    right = glm::normalize(glm::cross(front, up));

    glm::vec3 pos = player_node->transform.getLocalPosition();

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

    if (16 & inputs && on_ground) { //jump
        vel.y = up.y * v_0;
    }


	glm::vec3 cur_grav = grav;
    //if (delta_time < 0.01f) {
    pos += (vel * delta_time) + 0.5f * grav * delta_time * delta_time;
    vel += grav * delta_time;
    //}

	//if (vel.y < -2.f) {
		//vel.y = -2.f;
	//}   

    float angY = player_node->transform.getLocalRotation().y;
    if (inputs & 64) {
        angY += speed * delta_time;
    }
    if (inputs & 128) {
        angY -= speed * delta_time;
    }

    if (vel.y < 0.f && player_node->AABB->collison == 2) {
        on_ground = true;
        vel.y = 0.f;
		cur_grav = glm::vec3(0.f);
    }
    else {
        on_ground = false;
    }
	
    pos += (vel * delta_time) + 0.5f * cur_grav * delta_time * delta_time;
    vel += cur_grav * delta_time;

    if (pos.y < -10.f) pos.y = 20.f;
	if (vel.y < -20.f) vel.y = -20.f;

    player_node->transform.setLocalRotation({ player_node->transform.getLocalRotation().x, angY, player_node->transform.getLocalRotation().z});
	player_node->transform.setLocalPosition(pos);

    player_node->AABB->collison = 0;
    
    player_node->forceUpdateSelfAndChild();
}
