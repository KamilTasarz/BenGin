#include "Camera.h"

glm::mat4 Camera::GetView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::checkInput(GLfloat deltaTime, int dir, float speed)
{
	GLfloat cameraSpeed = speed * deltaTime;
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    Yaw += xoffset * 0.1;
    Pitch += yoffset * 0.1;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}


