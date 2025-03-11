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

// Default camera values
const float YAW = 45.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
//const float SENSITIVITY
//const float ZOOM

enum CameraMode {
    // There might be multiple camera modes in the future (p.e. free, fixed)S
};

class Camera {

private:

    void updateCameraVectors() {

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        cameraFront = glm::normalize(front);

        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    }

public:

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));

    float Yaw, Pitch;

    float MovementSpeed = 15.0f; // Only temporary value, will be flexible later
    float MouseSensitivity = 0.1f; // Only temporary value, will be flexible later

    Camera(float x = 0.f, float y = 0.f, float z = 0.f) {
        Yaw = 90.f;
        Pitch = 0.f;
        cameraPos = glm::vec3(x, y, z);
        worldUp = cameraUp;

        updateCameraVectors();
        
    }

    glm::mat4 GetView();

    void ProcessKeyboard(GLfloat deltaTime, int dir);

    void ProcessMouseMovement(float xoffset, float yoffset);
    
    // Setters
    void setPosition(glm::vec3 position);
    void setMovementSpeed(float speed);
    void setMouseSensitivity(float sensitivity);

};

#endif

