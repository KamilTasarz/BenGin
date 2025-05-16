#include "Camera.h"
#include "../Basic/Node.h"

glm::mat4 Camera::GetView() {
    if (mode != FRONT_ORTO) {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
    else {
		cameraUp = glm::vec3(0.f, 1.f, 0.f);
        return glm::lookAt(cameraPos, cameraPos + glm::vec3(0.0f, 0.0f, -1.f), cameraUp);
    }
	
}

glm::mat4 Camera::GetProjection() {
    if (mode != FRONT_ORTO) {
        return glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
    }
    else {
		return glm::ortho(-20.f * orto_zoom, 20.f * orto_zoom, -11.25f * orto_zoom, 11.25f * orto_zoom, NearPlane, FarPlane);
    }
    
}

void Camera::setAABB()
{
    float height = 2.0f * tan(glm::radians(Zoom) / 2.0f) * FarPlane;
    float width = height * AspectRatio;
    //glm::vec3 halfExtents = glm::vec3(width / 2, height / 2, FarPlane / 2);

    AABB = new BoundingBox(glm::mat4(1), nullptr, cameraPos - glm::vec3(width / 2, height / 2, cameraPos.z), cameraPos + glm::vec3(width / 2, height / 2, -cameraPos.z - FarPlane));

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
    else if (mode == FRONT_ORTO) {
        GLfloat cameraSpeed = MovementSpeed * deltaTime;
        if (4 & dir)
            cameraPos -= cameraRight * cameraSpeed;
        if (8 & dir)
            cameraPos += cameraRight * cameraSpeed;
        if (1 & dir)
            cameraPos += cameraUp * cameraSpeed;
        if (2 & dir)
            cameraPos -= cameraUp * cameraSpeed;

    }

    updateCameraVectors();

}

void Camera::ProcessGamepad() {



}

void Camera::updateCameraVectors() {
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
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, cameraPos);

    if (AABB) AABB->transformAABB(model);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    if (mode == FREE) {
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

void Camera::setAspectRatio(float aspect_ratio) {
    AspectRatio = aspect_ratio;
}

void Camera::setObjectToFollow(Node* object, glm::vec3& origin)
{
    object_to_follow = object;
	origin_point = origin;
}

void Camera::setOffsetToFollowingObject(glm::vec3& offset) {
    camera_following_offset = offset;
}

void Camera::changeMode(CameraMode mode)
{
    if (this->mode == FRONT_ORTO) cameraPos = oldCameraPos;

    if (mode == FOLLOWING && object_to_follow == nullptr) this->mode = FREE;
    else this->mode = mode;

    if (mode == FRONT_ORTO) {
        oldCameraPos = cameraPos;
        cameraPos = glm::vec3(0.0f, 0.0f, FarPlane / 2);
    }

	/*if (mode != FOLLOWING) {
		cameraPos = oldCameraPos;
    }
    else {
		oldCameraPos = cameraPos;
    }*/

}

bool Camera::isInFrustrum(BoundingBox* AABB)
{
    return this->AABB->isBoundingBoxIntersects(*AABB);
}


void Camera::setNearPlane(float near_plane) {
    NearPlane = near_plane;
}

void Camera::setFarPlane(float far_plane) {
    FarPlane = far_plane;
}
