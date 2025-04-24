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

class Node;


// Default camera values
const float YAW = 45.0f;
const float PITCH = 0.0f;
const float MOVEMENT_SPEED = 20.0f;
const float MOUSE_SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum CameraMode {

    // There might be multiple camera modes in the future (p.e. free, fixed)
    FREE = 0, FOLLOWING = 1, FIXED = 2, FRONT_ORTO = 3

};

class Camera {

private:

    void updateCameraVectors();

public:

    glm::vec3 cameraPos;
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

    float MovementSpeed = MOVEMENT_SPEED;
    float MouseSensitivity = MOUSE_SENSITIVITY;
    float Zoom = ZOOM;

    float AspectRatio = 16.0f / 9.0f;
    float NearPlane = 0.1f;
    float FarPlane = 100.1f;

    Camera(float x = 0.f, float y = 0.f, float z = 0.f) : mode(FREE) {

        Yaw = 90.0f;
        Pitch = 0.0f;

        cameraPos = glm::vec3(x, y, z);
        worldUp = cameraUp;
        object_to_follow = nullptr;
        updateCameraVectors();
        std::cout << "Camera constructor called! Position: "
            << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;
    }

    glm::mat4 GetView();
    glm::mat4 GetProjection();

    void ProcessKeyboard(GLfloat deltaTime, int dir);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessGamepad(); // TODO
    
    // Setters
    void setPosition(const glm::vec3& position);
    void setMovementSpeed(float speed);
    void setMouseSensitivity(float sensitivity);

    void setAspectRatio(float aspect_ratio);
    void setNearPlane(float near_plane);
    void setFarPlane(float far_plane);

    void setObjectToFollow(Node* object, glm::vec3& origin);

    void changeMode(CameraMode mode);

};

#endif

