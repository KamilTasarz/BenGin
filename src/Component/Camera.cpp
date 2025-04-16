#include "Camera.h"

glm::mat4 Camera::GetView() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::GetProjection() {
    return glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
}

void Camera::ProcessKeyboard(GLfloat deltaTime, int dir) {

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

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {

    Yaw += xoffset * MouseSensitivity;
    Pitch += yoffset * MouseSensitivity;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();

}

void Camera::ProcessGamepad() {



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

void Camera::setAspectRatio(float aspect_ratio) {
    AspectRatio = aspect_ratio;
}

void Camera::setNearPlane(float near_plane) {
    NearPlane = near_plane;
}

void Camera::setFarPlane(float far_plane) {
    FarPlane = far_plane;
}
