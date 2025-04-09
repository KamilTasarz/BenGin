#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../Basic/Node.h"

// Default camera values
const float YAW = 45.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
//const float SENSITIVITY
//const float ZOOM

enum CameraMode {
    // There might be multiple camera modes in the future (p.e. free, fixed)S
    FREE = 0, FOLLOWING = 1, FIXED = 2
};

class Camera {

private:

    void updateCameraVectors() {
        if (mode != FOLLOWING) {
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            cameraFront = glm::normalize(front);

            cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
            cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
        }
        else {
			cameraFront = (object_to_follow->transform.getGlobalPosition() + origin_point) - cameraPos;
            cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
            cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
        }
        

    }

public:

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 oldCameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    glm::vec3 camera_following_offset = glm::vec3(0.f, 2.f, 10.f);
	glm::vec3 origin_point = glm::vec3(0.f, 0.f, 0.f);
    CameraMode mode;

    Node *object_to_follow;

    float Yaw, Pitch;
    float MovementSpeed = 20.0f; // Only temporary value, will be flexible later
    float MouseSensitivity = 0.1f; // Only temporary value, will be flexible later

    Camera(float x = 0.f, float y = 0.f, float z = 0.f) : mode(FREE) {
        Yaw = 90.f;
        Pitch = 0.f;
        cameraPos = glm::vec3(x, y, z);
        worldUp = cameraUp;
        object_to_follow = nullptr;
        updateCameraVectors();

    }

    glm::mat4 GetView();

    void ProcessKeyboard(GLfloat deltaTime, int dir);

    void ProcessMouseMovement(float xoffset, float yoffset);
    
    // Setters
    void setPosition(glm::vec3 position);
    void setMovementSpeed(float speed);
    void setMouseSensitivity(float sensitivity);
    void setObjectToFollow(Node* object, glm::vec3 origin);

    void changeMode(CameraMode mode);

};

#endif

