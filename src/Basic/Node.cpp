#include "Node.h"

void SceneGraph::unmark() {
    marked_object->is_marked = false;
    marked_object = nullptr; //marked object returns as nullptr after draw
}

void SceneGraph::addChild(Node* p) {
    while (root->getChildByName(p->name)) {
        p->name += "_copy";
    }
    root->addChild(p);
}

void SceneGraph::addChild(Node* p, std::string name) {
    Node* parent = root->getChildByName(name);
    if (parent != nullptr) {

        while (root->getChildByName(p->name)) {
            p->name += "_copy";
        }
        parent->addChild(p);
    }
}

void SceneGraph::addPointLight(PointLight* p) {
    addChild(p);
    point_lights.push_back(p);
    point_light_number++;
}

void SceneGraph::addDirectionalLight(DirectionalLight* p) {
    addChild(p);
    directional_lights.push_back(p);
    directional_light_number++;
}

void SceneGraph::setShaders() {
    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();
    shader->use();
    setLights(shader);
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setInt("point_light_number", point_light_number);
    shader->setInt("directional_light_number", directional_light_number);

    shader_instanced->use();
    setLights(shader_instanced);
    shader_instanced->setMat4("projection", projection);
    shader_instanced->setMat4("view", view);
    shader_instanced->setInt("point_light_number", point_light_number);
    shader_instanced->setInt("directional_light_number", directional_light_number);

    shader_outline->use();
    shader_outline->setMat4("projection", projection);
    shader_outline->setMat4("view", view);
    shader_outline->setInt("point_light_number", point_light_number);
    shader_outline->setInt("directional_light_number", directional_light_number);
}

void SceneGraph::draw(float render_x, float render_y, float width, float height) {

    

    for (auto& dir_light : directional_lights) {
        dir_light->render(depthMapFBO, *shader_shadow);
        glClear(GL_DEPTH_BUFFER_BIT);
        root->drawShadows(*shader_shadow);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(render_x, render_y, width, height);
    glClearColor(.01f, .01f, .01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    setShaders();
    root->drawSelfAndChild();
}

void SceneGraph::update(float delta_time) {
    root->updateSelfAndChild(false);
    root->updateAnimation(delta_time);
}



void SceneGraph::forcedUpdate() {
    root->updateSelfAndChild(true);
}

void SceneGraph::mark(glm::vec4 ray) {
    float t = FLT_MAX;
    new_marked_object = nullptr;
    root->mark(ray, t);
}

void SceneGraph::setLights(Shader* shader) {

    shader->setVec3("cameraPosition", camera->cameraPos);
    int i = 0;
    for (auto& point_light : point_lights) {
        string index = to_string(i);
        shader->setFloat("point_lights[" + index + "].constant", point_light->constant);
        shader->setFloat("point_lights[" + index + "].linear", point_light->linear);
        shader->setFloat("point_lights[" + index + "].quadratic", point_light->quadratic);
        shader->setVec3("point_lights[" + index + "].position", point_light->transform.getLocalPosition());
        shader->setVec3("point_lights[" + index + "].ambient", point_light->ambient);
        shader->setVec3("point_lights[" + index + "].diffuse", point_light->diffuse);
        shader->setVec3("point_lights[" + index + "].specular", point_light->specular);
        i++;
    }
    i = 0;
    for (auto& dir_light : directional_lights) {
        string index = to_string(i);
        shader->setVec3("directional_lights[" + index + "].direction", dir_light->direction);
        shader->setVec3("directional_lights[" + index + "].ambient", dir_light->ambient);
        shader->setVec3("directional_lights[" + index + "].diffuse", dir_light->diffuse);
        shader->setVec3("directional_lights[" + index + "].specular", dir_light->specular);
        glActiveTexture(GL_TEXTURE3 + i);
        glBindTexture(GL_TEXTURE_2D, dir_light->depthMap);
        
        i++;
        shader->setMat4("light_view_projection" + to_string(i), dir_light->getMatrix());
        shader->setInt("shadow_map" + to_string(i), 2 + i);
    }


}

void SceneGraph::drawMarkedObject() {
    if (marked_object != nullptr) {

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        //glm::vec3 scale_matrix = marked_object->transform.getLocalScale();
        //scale_matrix += glm::vec3(0.05f);
        //Transform transform = marked_object->transform;
        //transform.setLocalScale(scale_matrix);
        //transform.computeModelMatrix();
        shader_outline->use();
        shader_outline->setMat4("model", marked_object->transform.getModelMatrix());

        glm::vec3 dynamic_color = glm::vec3(0.4f, 0.f, 0.f);

        if (marked_object->pModel) {
            shader_outline->setVec3("color", dynamic_color);
            marked_object->pModel->Draw(*shader_outline);
        }


        //unmark();
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}

// ====NODE====

void Node::addChild(Node* p) {
    children.emplace_back(p);
    p->scene_graph = scene_graph; // Set this as the created child's parent
    p->parent = this; // Set this as the created child's parent
    increaseCount();
}

void Node::increaseCount() {
    scene_graph->size++;
}

// Get child by its name
Node* Node::getChildByName(const std::string& name) {
    for (auto& child : children) {
        if (child->name._Equal(name)) {
            return child;
        }

        // Checks the children of the first level recursively
        Node* foundChild = child->getChildByName(name);
        if (foundChild != nullptr) {
            return foundChild;
        }
    }
    return nullptr;
}

Node* Node::getChildById(int id) {
    for (auto child : children) {
        if (child->id == id) {
            return child;
        }

        // Checks the children of the first level recursively
        Node* foundChild = child->getChildById(id);
        if (foundChild != nullptr) {
            return foundChild;
        }
    }
    return nullptr;
}

void Node::mark(glm::vec4 rayWorld, float& marked_depth) {

    for (auto&& child : children) {
        float t;

        if (child->AABB != nullptr && child->AABB->isRayIntersects(rayWorld, camera->cameraPos, t)) {

            if (t < marked_depth) {

                scene_graph->new_marked_object = child;
                marked_depth = t;
            }
        }

        child->mark(rayWorld, marked_depth);
    }

}

// Forcing an update of self and children even if there were no changes
void Node::forceUpdateSelfAndChild() {
    if (parent) {
        transform.computeModelMatrix(parent->transform.getModelMatrix());
    }
    else {
        transform.computeModelMatrix();
    }
    if (AABB != nullptr) {
        AABB->transformAABB(transform.getModelMatrix());
    }

}

// This will update if there were changes only (checks the dirty flag)
void  Node::updateSelfAndChild(bool controlDirty) {

    controlDirty |= transform.isDirty();

    if (controlDirty) {
        forceUpdateSelfAndChild();
        //return;
        //transform.computeModelMatrix();

    }

    for (auto&& child : children)
    {
        child->updateSelfAndChild(controlDirty);
    }
}

// Draw self and children
void Node::drawSelfAndChild() {

    if (pModel && is_visible) {
        //_shader.setVec4("dynamicColor", color);
        scene_graph->shader->use();
        scene_graph->shader->setMat4("model", transform.getModelMatrix());

        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                scene_graph->shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        if (is_marked) {
            glStencilMask(0xFF);
        }
        if (no_textures) {
            scene_graph->shader->setInt("is_light", 1);
        }
        pModel->Draw(*scene_graph->shader);
        glStencilMask(0x00);

        scene_graph->shader->setInt("is_light", 0);

        scene_graph->shader_outline->use();
        scene_graph->shader_outline->setMat4("model", transform.getModelMatrix());
        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                scene_graph->shader_outline->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
        scene_graph->shader_outline->setVec3("color", dynamic_color);

        AABB->draw(*scene_graph->shader_outline);

    }


    for (auto&& child : children) {
        child->drawSelfAndChild();
    }

    //_shader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void Node::updateAnimation(float deltaTime) {
    if (animator != nullptr) {
        animator->updateAnimation(deltaTime);
    }
    for (auto&& child : children) {
        child->updateAnimation(deltaTime);
    }
}

void Node::drawShadows(Shader& shader) {
    if (pModel) {

        shader.use();
        shader.setMat4("model", transform.getModelMatrix());
        if (!no_textures) {
            if (animator != nullptr) {
                auto f = animator->final_bone_matrices;
                for (int i = 0; i < f.size(); ++i)
                    shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
            }
            pModel->Draw(shader); //jak nie ma tekstury to najpewniej swiatlo -> przyjmuje takie zalozenie
        }
    }

    for (auto&& child : children) {
        child->drawShadows(shader);
    }
}



void Node::separate(const BoundingBox* other_AABB) {

    float left = (other_AABB->min_point_world.x - AABB->max_point_world.x);
    float right = (other_AABB->max_point_world.x - AABB->min_point_world.x);
    float up = (other_AABB->min_point_world.y - AABB->max_point_world.y);
    float down = (other_AABB->max_point_world.y - AABB->min_point_world.y);
    float front = (other_AABB->min_point_world.z - AABB->max_point_world.z);
    float back = (other_AABB->max_point_world.z - AABB->min_point_world.z);
    glm::vec3 v = glm::vec3(abs(left) < abs(right) ? left : right, abs(up) < abs(down) ? up : down, abs(front) < abs(back) ? front : back);

    if (abs(v.x) <= abs(v.y) && abs(v.x) <= abs(v.z)) {
        v.y = 0.f;
        v.z = 0.f;
        AABB->collison = v.x > 0.f ? 1 : -1;
    }
    else if (abs(v.y) <= abs(v.x) && abs(v.y) <= abs(v.z)) {
        v.x = 0.f;
        v.z = 0.f;
        AABB->collison = v.y > 0.f ? 2 : -2;
    }
    else {
        v.x = 0.f;
        v.y = 0.f;
        AABB->collison = v.z > 0.f ? 3 : -3;
    }

    cout << "collison: " << AABB->collison << endl;

    transform.setLocalPosition(transform.getLocalPosition() + v);
    forceUpdateSelfAndChild();
}

const Transform& Node::getTransform() {
    return transform;
}

// ====INSTANCE MANAGER====

void InstanceManager::drawSelfAndChild() {
    glStencilMask(0xFF);
    scene_graph->shader_instanced->use();
    pModel->DrawInstanced(*scene_graph->shader_instanced, size);
    glStencilMask(0x00);

}

void InstanceManager::updateSelfAndChild(bool controlDirty) {

    controlDirty |= transform.isDirty();

    if (controlDirty) {
        forceUpdateSelfAndChild();
    }

    for (auto&& child : children)
    {
        bool is_dirty = child->transform.isDirty();
        child->updateSelfAndChild(controlDirty);
        if (is_dirty) {
            updateBuffer(child);
        }
    }
}

void InstanceManager::addChild(Node* p) {
    children.push_back(p);
    children.back()->parent = this; // Set this as the created child's parent
    size++;
    p->id = current_min_id;
    current_min_id++;
    updateBuffer(p);
    increaseCount();
}

Node* InstanceManager::find(int id) {
    for (auto&& child : children) {
        if (child->id == id) {
            return child;
        }
    }
    return nullptr;
}


void InstanceManager::removeChild(int id) {
    //usuniecie ze sceny tworzylo by dziury w tablicy
    //wiec znajdujemy to co usuwamy po indexach
    Node* temp = find(id);
    //zapisujemy index
    int new_id = temp->id;
    //usuwamy
    children.remove(temp);
    //i ostatni element dajemy w miejsce dziury
    temp = children.back();
    temp->id = new_id;
    updateBuffer(temp);
    current_min_id--;
    size--;
}
void InstanceManager::prepareBuffer()
{
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, max_size * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    for (unsigned int i = 0; i < pModel->meshes.size(); i++)
    {
        unsigned int VAO = pModel->meshes[i].VAO;
        glBindVertexArray(VAO);
        // Atrybuty wierzchołków
        GLsizei vec4_size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(vec4_size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(2 * vec4_size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(3 * vec4_size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

void InstanceManager::updateBuffer(Node* p) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, p->id * sizeof(glm::mat4), sizeof(glm::mat4), &p->transform.getModelMatrix());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}