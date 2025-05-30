#include "BoundingBox.h"

#include "../Basic/Node.h"

bool BoundingBox::isRayIntersects(glm::vec3 direction, glm::vec3 origin, float& t, glm::vec3& endPoint)
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
    endPoint = origin + t * direction;
    return true;
}

bool BoundingBox::isBoundingBoxIntersects(const BoundingBox& other_bounding_box) const
{
    return other_bounding_box.min_point_world.x < max_point_world.x && other_bounding_box.max_point_world.x > min_point_world.x &&
         other_bounding_box.min_point_world.y < max_point_world.y && other_bounding_box.max_point_world.y > min_point_world.y &&
         other_bounding_box.min_point_world.z < max_point_world.z && other_bounding_box.max_point_world.z > min_point_world.z;
}

void BoundingBox::separate(const BoundingBox* other_AABB, float separation_multiplier)
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

    glm::vec3 pos = node->transform.getLocalPosition();
    node->transform.setLocalPosition(pos + v * separation_multiplier);
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
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
    else {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }
}

void BoundingBox::draw(Shader& shader) {
    if (VAO != 0) {
        setBuffers();

        glBindVertexArray(VAO);

        shader.use();
        shader.setMat4("model", glm::mat4(1.f));

        glDrawArrays(GL_LINE_STRIP, 0, 10);
        glDrawArrays(GL_LINES, 10, 6);
        glBindVertexArray(0);
    }
}

Capsule::Capsule(glm::vec3& min, glm::vec3& max, float radius)
{
}

Capsule::Capsule(glm::vec3& mid, float height, float radius)
{
    this->mid = mid;
    A = mid;
    A.x += height;
    B = mid;
    B.x -= height;
    this->radius;
    model = glm::mat4(1.f);
    setBuffers();
    
}

void Capsule::draw(Shader& shader)
{
    if (VAO != 0) {
        //setBuffers();

        glBindVertexArray(VAO);

        shader.use();
        shader.setMat4("model", glm::mat4(1.f));

        glDrawArrays(GL_LINES, 0, 2 * 28);
        glBindVertexArray(0);
    }
}

void Capsule::setBuffers()
{
    
    float vertices[3 * 2 * 28]; // zaokroglenia sa po 3 linie

    glm::vec3 globalA = glm::vec3(model * glm::vec4(A, 1.f));
    glm::vec3 globalB = glm::vec3(model * glm::vec4(B, 1.f));

    glm::vec3 axis = glm::normalize(globalB - globalA);
    glm::vec3 ortho1 = glm::normalize(glm::cross(axis, glm::vec3(0, 1, 0)));
    if (glm::length(ortho1) < 0.01f)
        ortho1 = glm::normalize(glm::cross(axis, glm::vec3(1, 0, 0)));
    glm::vec3 ortho2 = glm::normalize(glm::cross(axis, ortho1));

    int index = 0;
    for (int i = 0; i < 4; i++) {
        float angle = glm::radians(90.0f * i);
        glm::vec3 offset = radius * (cos(angle) * ortho1 + sin(angle) * ortho2);

        glm::vec3 pA = globalA + offset;
        glm::vec3 pB = globalB + offset;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        vertices[index++] = pB.x;
        vertices[index++] = pB.y;
        vertices[index++] = pB.z;
    }
    float angle = 0.f;
    glm::vec3 offset = radius * (cos(angle) * ortho1 + sin(angle) * ortho2);
    glm::vec3 before = globalA + offset;

    
    
    for (int i = 1; i < 12; i++) {
        angle = glm::radians(30.0f * i);
        offset = radius * (cos(angle) * ortho1 + sin(angle) * ortho2);

        glm::vec3 pA = globalA + offset;

        vertices[index++] = before.x;
        vertices[index++] = before.y;
        vertices[index++] = before.z;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        before = pA;
    }

    angle = 0.f;
    offset = radius * (cos(angle) * ortho1 + sin(angle) * ortho2);
    before = globalA + offset;

    vertices[index++] = before.x;
    vertices[index++] = before.y;
    vertices[index++] = before.z;


    for (int i = 0; i < 3; i++) {
        float angle = glm::radians(30.0f * i);
        float angle2 = glm::radians(30.0f * (i + 1));
        offset = radius * (cos(angle) * ortho1 + sin(angle) * (-axis));
        glm::vec3 offset2 = radius * (cos(angle2) * ortho1 + sin(angle2) * (-axis));

        glm::vec3 pA = globalA + offset;
        glm::vec3 pB = globalA + offset2;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        vertices[index++] = pB.x;
        vertices[index++] = pB.y;
        vertices[index++] = pB.z;

    }

    for (int i = 0; i < 3; i++) {
        float angle = glm::radians(30.0f * i);
        float angle2 = glm::radians(30.0f * (i + 1));
        offset = radius * (cos(angle) * ortho1 + sin(angle) * (-axis));
        glm::vec3 offset2 = radius * (cos(angle2) * ortho2 + sin(angle2) * (-axis));

        glm::vec3 pA = globalA + offset;
        glm::vec3 pB = globalA + offset2;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        vertices[index++] = pB.x;
        vertices[index++] = pB.y;
        vertices[index++] = pB.z;

    }

    for (int i = 0; i < 3; i++) {
        float angle = glm::radians(30.0f * i);
        float angle2 = glm::radians(30.0f * (i + 1));
        offset = radius * (cos(angle) * (-ortho1) + sin(angle) * (-axis));
        glm::vec3 offset2 = radius * (cos(angle2) * ortho1 + sin(angle2) * (-axis));

        glm::vec3 pA = globalA + offset;
        glm::vec3 pB = globalA + offset2;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        vertices[index++] = pB.x;
        vertices[index++] = pB.y;
        vertices[index++] = pB.z;

    }

    for (int i = 0; i < 3; i++) {
        float angle = glm::radians(30.0f * i);
        float angle2 = glm::radians(30.0f * (i + 1));
        offset = radius * (cos(angle) * (-ortho2) + sin(angle) * (-axis));
        glm::vec3 offset2 = radius * (cos(angle2) * ortho1 + sin(angle2) * (-axis));

        glm::vec3 pA = globalA + offset;
        glm::vec3 pB = globalA + offset2;

        vertices[index++] = pA.x;
        vertices[index++] = pA.y;
        vertices[index++] = pA.z;

        vertices[index++] = pB.x;
        vertices[index++] = pB.y;
        vertices[index++] = pB.z;

    }


    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 2 * 28 * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void RectOBB::transform(glm::mat4 _model)
{
    this->model = _model;
    maxGlobal = model * glm::vec4(maxLocal, 1.f);
    minGlobal = model * glm::vec4(minLocal, 1.f);
    normalGlobal = glm::normalize(glm::cross(glm::normalize(maxGlobal - minGlobal), glm::vec3(0.f, 0.f, -1.f)));
    //normalGlobal = glm::normalize(model * glm::vec4(normal, 1.f));
    setBuffers();
}

void RectOBB::setBuffers()
{
    glm::vec3 mid = minGlobal + (maxGlobal - minGlobal) / 2.f;

    float vertices[] = {
        minGlobal.x, minGlobal.y, minGlobal.z,
        minGlobal.x, minGlobal.y, maxGlobal.z,
        maxGlobal.x, maxGlobal.y, maxGlobal.z,
        maxGlobal.x, maxGlobal.y, minGlobal.z,
        minGlobal.x, minGlobal.y, minGlobal.z,
        mid.x, mid.y, mid.z,
        mid.x + normalGlobal.x, mid.y + normalGlobal.y, mid.z + normalGlobal.z
    };

    if (VAO != 0 && VBO != 0) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
    else {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }
}

void RectOBB::draw(Shader& shader)
{
    if (VAO != 0) {
        setBuffers();

        glBindVertexArray(VAO);

        shader.use();
        shader.setMat4("model", glm::mat4(1.f));

        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glDrawArrays(GL_LINES, 5, 2);
        glBindVertexArray(0);
    }
}

bool RectOBB::isRayIntersects(glm::vec3 direction, glm::vec3 origin, float& t, glm::vec3& endPoint)
{
    //musimy umieścić, żeby liczcyć przeciecia dla parametru z układu prostokąta
    glm::mat4 invModel = glm::inverse(model);
    glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(origin, 1.0f));
    glm::vec3 localDir = glm::normalize(glm::vec3(invModel * glm::vec4(direction, 0.0f))); 

    glm::vec3 divDir = 1.0f / localDir;
    glm::vec3 t1 = (minLocal - localOrigin) * divDir;
    glm::vec3 t2 = (maxLocal - localOrigin) * divDir;

    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);

    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

    if (tNear > tFar || tFar < 0)
        return false; 

    t = tNear > 0 ? tNear : tFar; 

    endPoint = glm::vec3(model * glm::vec4(localOrigin + t * localDir, 1.f));

    return true;
}
