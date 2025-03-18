#include "BoundingBox.h"

bool BoundingBox::isRayIntersects(glm::vec3 direction, glm::vec3 origin, float& t) const
{
    // p(t) = p0 + v * t  -->  t = (p(t) - p0) / v
    glm::vec3 dividor = 1.0f / direction;
    glm::vec3 parameters_max = (max_point_world - origin) * dividor;
    glm::vec3 parameters_min = (min_point_world - origin) * dividor;

    glm::vec3 t_min = glm::min(parameters_min, parameters_max);
    glm::vec3 t_max = glm::max(parameters_min, parameters_max);

    float t_near = glm::max(glm::max(t_min.x, t_min.y), t_min.z);
    float t_far = glm::min(glm::min(t_max.x, t_max.y), t_max.z);

    if (t_near > t_far || t_far < 0) return false;

    t = t_near;
    return true;
}

bool BoundingBox::isBoundingBoxIntersects(const BoundingBox& other_bounding_box) const
{
    return other_bounding_box.min_point_world.x < max_point_world.x && other_bounding_box.max_point_world.x > min_point_world.x &&
         other_bounding_box.min_point_world.y < max_point_world.y && other_bounding_box.max_point_world.y > min_point_world.y &&
         other_bounding_box.min_point_world.z < max_point_world.z && other_bounding_box.max_point_world.z > min_point_world.z;
}

void BoundingBox::transformAABB(const glm::mat4& model) {
    glm::vec3 corners[8] = {
        {min_point_local.x, min_point_local.y, min_point_local.z},
        {min_point_local.x, min_point_local.y, max_point_local.z},
        {min_point_local.x, max_point_local.y, min_point_local.z},
        {min_point_local.x, max_point_local.y, max_point_local.z},
        {max_point_local.x, min_point_local.y, min_point_local.z},
        {max_point_local.x, min_point_local.y, max_point_local.z},
        {max_point_local.x, max_point_local.y, min_point_local.z},
        {max_point_local.x, max_point_local.y, max_point_local.z}
    };

    min_point_world = glm::vec3(FLT_MAX);
    max_point_world = glm::vec3(-FLT_MAX);

    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = model * glm::vec4(corners[i], 1.0f);
        glm::vec3 worldPos = glm::vec3(transformed);

        min_point_world = glm::min(min_point_world, worldPos);
        max_point_world = glm::max(max_point_world, worldPos);
    }
}


