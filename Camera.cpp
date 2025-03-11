#include "Camera.h"

glm::mat4 Camera::GetView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::ProcessKeyboard(GLfloat deltaTime, int dir) {
	GLfloat cameraSpeed = MovementSpeed * deltaTime;
    if (1 & dir)
        cameraPos += cameraSpeed * cameraFront;
    if (2 & dir)
        cameraPos -= cameraSpeed * cameraFront;
    if (4 & dir)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (8 & dir)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (16 & dir)
        cameraPos += cameraUp * cameraSpeed;
    if (32 & dir)
        cameraPos -= cameraUp * cameraSpeed;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    Yaw += xoffset * MouseSensitivity;
    Pitch += yoffset * MouseSensitivity;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::setPosition(glm::vec3 position) {
    cameraPos = position;
    updateCameraVectors();
}

void Camera::setMovementSpeed(float speed) {
    MovementSpeed = speed;
}

void Camera::setMouseSensitivity(float sensitivity) {
    MouseSensitivity = sensitivity;
}


