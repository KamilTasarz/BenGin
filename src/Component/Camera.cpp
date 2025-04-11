#include "Camera.h"

glm::mat4 Camera::GetView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::ProcessKeyboard(GLfloat deltaTime, int dir) {
    if (mode == FREE) {
        GLfloat cameraSpeed = MovementSpeed * deltaTime;
        if (1 & dir)
            cameraPos += cameraSpeed * cameraFront;
        if (2 & dir)
            cameraPos -= cameraSpeed * cameraFront;
        if (4 & dir)
            cameraPos -= cameraRight * cameraSpeed;
        if (8 & dir)
            cameraPos += cameraRight * cameraSpeed;
        if (16 & dir)
            cameraPos += cameraUp * cameraSpeed;
        if (32 & dir)
            cameraPos -= cameraUp * cameraSpeed;
        
    }
    else if (mode == FOLLOWING) {
        cameraPos = object_to_follow->transform.getGlobalPosition() + camera_following_offset;
    }
    updateCameraVectors();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    if (mode != FOLLOWING) {
        Yaw += xoffset * MouseSensitivity;
        Pitch += yoffset * MouseSensitivity;

		Yaw = fmodf(Yaw, 360.0f);

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }
    
}

void Camera::setPosition(const glm::vec3& position) {
    cameraPos = position;
    updateCameraVectors();
}

void Camera::setMovementSpeed(float speed) {
    MovementSpeed = speed;
}

void Camera::setMouseSensitivity(float sensitivity) {
    MouseSensitivity = sensitivity;
}

void Camera::setObjectToFollow(Node* object, glm::vec3& origin)
{
    object_to_follow = object;
	origin_point = origin;
}

void Camera::changeMode(CameraMode mode)
{
    if (mode == FOLLOWING && object_to_follow == nullptr) this->mode = FREE;
    else this->mode = mode;

	if (mode != FOLLOWING) {
		cameraPos = oldCameraPos;
    }
    else {
		oldCameraPos = cameraPos;
    }

}


