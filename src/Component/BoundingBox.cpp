#include "BoundingBox.h"

#include "../Basic/Node.h"

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

void BoundingBox::separate(const BoundingBox* other_AABB, float separation_mulitplier)
{
    

    float left = (other_AABB->min_point_world.x - max_point_world.x);
    float right = (other_AABB->max_point_world.x - min_point_world.x);
    float up = (other_AABB->min_point_world.y - max_point_world.y);
    float down = (other_AABB->max_point_world.y - min_point_world.y);
    float front = (other_AABB->min_point_world.z - max_point_world.z);
    float back = (other_AABB->max_point_world.z - min_point_world.z);
    glm::vec3 v = glm::vec3(std::abs(left) < std::abs(right) ? left : right, std::abs(up) < std::abs(down) ? up : down, std::abs(front) < std::abs(back) ? front : back);

    if (std::abs(v.x) <= std::abs(v.y) && std::abs(v.x) <= std::abs(v.z)) {
        v.y = 0.f;
        v.z = 0.f;
        collison = 1;
    }
    else if (std::abs(v.y) <= std::abs(v.x) && std::abs(v.y) <= std::abs(v.z)) {
        v.x = 0.f;
        v.z = 0.f;
        collison = 2;
    }
    else {
        v.x = 0.f;
        v.y = 0.f;
        collison = 3;
    }

    

    node->transform.setLocalPosition(node->transform.getLocalPosition() + v * separation_mulitplier);
    //forceUpdateSelfAndChild();
    
}


SnapResult BoundingBox::trySnapToWallsX(const BoundingBox& other, float snapThreshold) {
    SnapResult result;

    
    float dx = std::abs(max_point_world.x - other.min_point_world.x); // prawa krawędź naszego do lewej tamtego
    if (dx < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(other.min_point_world.x - max_point_world.x, 0, 0);
        return result;
    }

    dx = std::abs(min_point_world.x - other.max_point_world.x); // lewa krawędź naszego do prawej tamtego
    if (dx < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(other.max_point_world.x - min_point_world.x, 0, 0);
        return result;
    }
        

    return result;
}

SnapResult BoundingBox::trySnapToWallsY(const BoundingBox& other, float snapThreshold) {
    SnapResult result;

    float dy = std::abs(min_point_world.y - other.max_point_world.y); // dolna krawędź naszego do gornej tamtego
    if (dy < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(0, other.max_point_world.y - min_point_world.y, 0);
        return result;
    }

    dy = std::abs(max_point_world.y - other.min_point_world.y); // gorna krawędź naszego do dolnej tamtego
    if (dy < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(0, other.min_point_world.y - max_point_world.y, 0);
        return result;
    }

    return result;
}

SnapResult BoundingBox::trySnapToWallsZ(const BoundingBox& other, float snapThreshold) {
    SnapResult result;

    float dz = std::abs(min_point_world.z - other.max_point_world.z); // przednia krawędź naszego do tylnej tamtego
    if (dz < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(0, 0, other.max_point_world.z - min_point_world.z);
        return result;
    }

    dz = std::abs(max_point_world.z - other.min_point_world.z); // tylna krawędź naszego do przedniej tamtego
    if (dz < snapThreshold) {
        result.shouldSnap = true;
        result.snapOffset = glm::vec3(0, 0, other.min_point_world.z - max_point_world.z);
        return result;
    }

    return result;
}


void BoundingBox::transformAABB(const glm::mat4& model) {
    this->model = glm::mat4(model);
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
        glm::vec3 worldPos = glm::vec3(model * glm::vec4(corners[i], 1.0f));

        //min_point_local = glm::min(min_point_local, worldPos);
        //max_point_local = glm::max(max_point_local, worldPos);

        min_point_world = glm::min(min_point_world, worldPos);
        max_point_world = glm::max(max_point_world, worldPos);
    }
}

void BoundingBox::transformWithOffsetAABB(const glm::mat4& model)
{
    glm::mat4 _model = model * this->model;
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
        glm::vec3 worldPos = glm::vec3(_model * glm::vec4(corners[i], 1.0f));

        //min_point_local = glm::min(min_point_local, worldPos);
        //max_point_local = glm::max(max_point_local, worldPos);

        min_point_world = glm::min(min_point_world, worldPos);
        max_point_world = glm::max(max_point_world, worldPos);
    }
}


void BoundingBox::setBuffers() {
    float vertices[] = {
            min_point_world.x, min_point_world.y, min_point_world.z, //dolny kwadrat
            min_point_world.x, min_point_world.y, max_point_world.z,
            max_point_world.x, min_point_world.y, max_point_world.z,
            max_point_world.x, min_point_world.y, min_point_world.z,
            min_point_world.x, min_point_world.y, min_point_world.z,

            min_point_world.x, max_point_world.y, min_point_world.z, //gorny kwadrat
            min_point_world.x, max_point_world.y, max_point_world.z,
            max_point_world.x, max_point_world.y, max_point_world.z,
            max_point_world.x, max_point_world.y, min_point_world.z,
            min_point_world.x, max_point_world.y, min_point_world.z,

            min_point_world.x, min_point_world.y, max_point_world.z, //pozostale kreski
            min_point_world.x, max_point_world.y, max_point_world.z,

            max_point_world.x, min_point_world.y, max_point_world.z,
            max_point_world.x, max_point_world.y, max_point_world.z,

            max_point_world.x, min_point_world.y, min_point_world.z,
            max_point_world.x, max_point_world.y, min_point_world.z
    };
    
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void BoundingBox::draw(Shader& shader) {
    if (VAO != 0) {
        setBuffers();

        glBindVertexArray(VAO);

        /*glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);*/


        //}




        shader.use();
        shader.setMat4("model", glm::mat4(1.f));

        glDrawArrays(GL_LINE_STRIP, 0, 10);
        glDrawArrays(GL_LINES, 10, 6);
        glBindVertexArray(0);
    }
}
