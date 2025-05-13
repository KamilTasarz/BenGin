#include "Node.h"
#include "../Component/CameraGlobals.h"

#include "../ResourceManager.h"

void SceneGraph::unmark() {
    marked_object->is_marked = false;
    marked_object = nullptr; //marked object returns as nullptr after draw
}

void SceneGraph::addChild(Node* p) {
    int i = 1;
	string name = p->name;
    while (root->getChildByName(p->name)) {
        p->name = name + "_" + to_string(i);
        i++;
    }
    root->addChild(p);
}

void SceneGraph::addChild(Node* p, std::string name) {
    Node* parent = root->getChildByName(name);
    if (parent != nullptr) {
        int i = 1;
        string name = p->name;
        while (root->getChildByName(p->name)) {
            
            p->name = name + "_" + to_string(i);
            i++;
        }
        parent->addChild(p);
    }
}

void SceneGraph::deleteChild(Node* p)
{
    Node* parent = p->parent;
    if (!parent) return;

    auto& siblings = parent->children;
    auto it = std::find(siblings.begin(), siblings.end(), p);
    if (it != siblings.end()) {
        siblings.erase(it);
        delete p;
    }
}

void SceneGraph::deletePointLight(PointLight* p) {
    auto it = std::find(point_lights.begin(), point_lights.end(), p);
    point_lights.erase(it);
    point_light_number--;
    deleteChild(p);
}

void SceneGraph::deleteDirectionalLight(DirectionalLight* p) {
    auto it = std::find(directional_lights.begin(), directional_lights.end(), p);
    directional_lights.erase(it);
    directional_light_number--;
    deleteChild(p);
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

void SceneGraph::addPointLight(PointLight* p, std::string name) {
    addChild(p, name);
    point_lights.push_back(p);
    point_light_number++;
}

void SceneGraph::addDirectionalLight(DirectionalLight* p, std::string name) {
    addChild(p, name);
    directional_lights.push_back(p);
    directional_light_number++;
}

void SceneGraph::addParticleEmitter(ParticleEmitter* p) {
    addChild(p);
}


void SceneGraph::setShaders() {

    

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();
    ResourceManager::Instance().shader->use();
    setLights(ResourceManager::Instance().shader);
    ResourceManager::Instance().shader->setMat4("projection", projection);
    ResourceManager::Instance().shader->setMat4("view", view);
    ResourceManager::Instance().shader->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader->setInt("directional_light_number", directional_light_number);
    
    ResourceManager::Instance().shader_tile->use();
    setLights(ResourceManager::Instance().shader_tile);
    ResourceManager::Instance().shader_tile->setMat4("projection", projection);
    ResourceManager::Instance().shader_tile->setMat4("view", view);
    ResourceManager::Instance().shader_tile->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader_tile->setInt("directional_light_number", directional_light_number);

    ResourceManager::Instance().shader_instanced->use();
    setLights(ResourceManager::Instance().shader_instanced);
    ResourceManager::Instance().shader_instanced->setMat4("projection", projection);
    ResourceManager::Instance().shader_instanced->setMat4("view", view);
    ResourceManager::Instance().shader_instanced->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader_instanced->setInt("directional_light_number", directional_light_number);

    ResourceManager::Instance().shader_outline->use();
    ResourceManager::Instance().shader_outline->setMat4("projection", projection);
    ResourceManager::Instance().shader_outline->setMat4("view", view);
    ResourceManager::Instance().shader_outline->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader_outline->setInt("directional_light_number", directional_light_number);
}

void SceneGraph::draw(float width, float height, unsigned int framebuffer) {

    for (auto& dir_light : directional_lights) {
        dir_light->render(depthMapFBO, *ResourceManager::Instance().shader_shadow);
        glClear(GL_DEPTH_BUFFER_BIT);
        root->drawShadows(*ResourceManager::Instance().shader_shadow);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClearColor(.01f, .01f, .01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (is_editing) {
        grid->Draw();
    }

	

    setShaders();
    root->drawSelfAndChild();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneGraph::update(float delta_time) {
    root->updateSelfAndChild(false);
    root->updateAnimation(delta_time);
}



void SceneGraph::forcedUpdate() {
    root->updateSelfAndChild(true);
}

void SceneGraph::mark(Ray ray) {
    float t = FLT_MAX;
    new_marked_object = nullptr;
    root->mark(ray, t);
}

void SceneGraph::setLights(Shader* shader) {

    shader->setVec3("cameraPosition", camera->cameraPos);
    int i = 0;
    for (auto& point_light : point_lights) {

        string index = to_string(i);
		glm::vec3 light_pos = point_light->transform.getGlobalPosition();
        shader->setFloat("point_lights[" + index + "].constant", point_light->constant);
        shader->setFloat("point_lights[" + index + "].linear", point_light->linear);
        shader->setFloat("point_lights[" + index + "].quadratic", point_light->quadratic);
        shader->setVec3("point_lights[" + index + "].position", light_pos);
        if (point_light->is_shining == true) {
            shader->setVec3("point_lights[" + index + "].ambient", point_light->ambient);
            shader->setVec3("point_lights[" + index + "].diffuse", point_light->diffuse);
            shader->setVec3("point_lights[" + index + "].specular", point_light->specular);
        }
        else {
            shader->setVec3("point_lights[" + index + "].ambient", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("point_lights[" + index + "].diffuse", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("point_lights[" + index + "].specular", glm::vec3({ 0.f, 0.f, 0.f }));
        }
        i++;
        
    }
    i = 0;

    for (auto& dir_light : directional_lights) {
        string index = to_string(i);
        shader->setVec3("directional_lights[" + index + "].direction", dir_light->direction);
        if (dir_light->is_shining == true) {
            shader->setVec3("directional_lights[" + index + "].ambient", dir_light->ambient);
            shader->setVec3("directional_lights[" + index + "].diffuse", dir_light->diffuse);
            shader->setVec3("directional_lights[" + index + "].specular", dir_light->specular);
        }
        else {
            shader->setVec3("directional_lights[" + index + "].ambient", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("directional_lights[" + index + "].diffuse", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("directional_lights[" + index + "].specular", glm::vec3({ 0.f, 0.f, 0.f }));
        }
        glActiveTexture(GL_TEXTURE3 + i);
        glBindTexture(GL_TEXTURE_2D, dir_light->depthMap);
        
        i++;
        shader->setMat4("light_view_projection" + to_string(i), dir_light->getMatrix());
        shader->setInt("shadow_map" + to_string(i), 2 + i);
    }


}

void SceneGraph::drawMarkedObject() {
    if (marked_object != nullptr && is_editing) {

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glm::vec3 scale_matrix = marked_object->transform.getLocalScale();
        //scale_matrix += glm::vec3(0.05f);
        //Transform transform = marked_object->transform;
        //transform.setLocalScale(scale_matrix);
        //transform.computeModelMatrix();
        ResourceManager::Instance().shader_outline->use();
        ResourceManager::Instance().shader_outline->setMat4("model", marked_object->transform.getModelMatrix());

        glm::vec3 dynamic_color = glm::vec3(0.4f, 0.f, 0.f);

        if (marked_object->pModel) {
            ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
            marked_object->pModel->Draw(*ResourceManager::Instance().shader_outline);
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
    children.insert(p);
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

void Node::mark(Ray rayWorld, float& marked_depth) {

    for (auto&& child : children) {
        float t;

        if (child->AABB != nullptr && child->AABB->isRayIntersects(rayWorld.direction, rayWorld.origin, t)) {

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

        if (pModel->mode.empty()) {
            ResourceManager::Instance().shader->use();
            ResourceManager::Instance().shader->setMat4("model", transform.getModelMatrix());
        }
        else {
			ResourceManager::Instance().shader_tile->use();
			ResourceManager::Instance().shader_tile->setMat4("model", transform.getModelMatrix());
        }
        

        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                ResourceManager::Instance().shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        if (is_marked) {
            glStencilMask(0xFF);
        }
        if (no_textures) {
            ResourceManager::Instance().shader->setInt("is_light", 1);
            ResourceManager::Instance().shader_tile->setInt("is_light", 1);
        }

        if (pModel->mode.empty()) {
            pModel->Draw(*ResourceManager::Instance().shader);

            glStencilMask(0x00);

            ResourceManager::Instance().shader->setInt("is_light", 0);
        }
        else {
            pModel->Draw(*ResourceManager::Instance().shader_tile);

            glStencilMask(0x00);

            ResourceManager::Instance().shader_tile->setInt("is_light", 0);
        }
        
        

        ResourceManager::Instance().shader_outline->use();
        ResourceManager::Instance().shader_outline->setMat4("model", transform.getModelMatrix());
        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                ResourceManager::Instance().shader_outline->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);

        if (scene_graph->is_editing) {
            AABB->draw(*ResourceManager::Instance().shader_outline);
        }
        

    }

    if (is_visible) {
        for (auto&& child : children) {
            child->drawSelfAndChild();
        }
    }
    

    //_shader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void Node::drawSelfAndChild(Transform& parent)
{
	glm::mat4 globalTransform = parent.getModelMatrix() * transform.getModelMatrix();

    if (pModel && is_visible) {
        //_shader.setVec4("dynamicColor", color);

        if (pModel->mode.empty()) {
            ResourceManager::Instance().shader->use();
            ResourceManager::Instance().shader->setMat4("model", globalTransform);
        }
        else {
            ResourceManager::Instance().shader_tile->use();
            ResourceManager::Instance().shader_tile->setMat4("model", globalTransform);
        }


        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                ResourceManager::Instance().shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        if (is_marked) {
            glStencilMask(0xFF);
        }
        if (no_textures) {
            ResourceManager::Instance().shader->setInt("is_light", 1);
            ResourceManager::Instance().shader_tile->setInt("is_light", 1);
        }

        if (pModel->mode.empty()) {
            pModel->Draw(*ResourceManager::Instance().shader);

            glStencilMask(0x00);

            ResourceManager::Instance().shader->setInt("is_light", 0);
        }
        else {
            pModel->Draw(*ResourceManager::Instance().shader_tile);

            glStencilMask(0x00);

            ResourceManager::Instance().shader_tile->setInt("is_light", 0);
        }



        ResourceManager::Instance().shader_outline->use();
        ResourceManager::Instance().shader_outline->setMat4("model", globalTransform);
        if (animator != nullptr) {
            auto f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                ResourceManager::Instance().shader_outline->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
        }
        glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);

        if (scene_graph->is_editing) {

            //AABB->draw(*ResourceManager::Instance().shader_outline);
        }


    }

    if (is_visible) {
        for (auto&& child : children) {
            child->drawSelfAndChild(parent);
        }
    }
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
        if (!no_textures && is_visible) {
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
    ResourceManager::Instance().shader_instanced->use();
    pModel->DrawInstanced(*ResourceManager::Instance().shader_instanced, size);
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
    children.insert(p);
    p->parent = this; // Set this as the created child's parent
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
    if (!temp) return;
    //zapisujemy index
    int new_id = temp->id;
    //usuwamy
    children.erase(temp);
    //i ostatni element dajemy w miejsce dziury
    if (!children.empty()) {
        temp = *children.rbegin();
        temp->id = new_id;
        updateBuffer(temp);
    }
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

void PrefabInstance::drawSelfAndChild()
{
    prefab->prefab_scene_graph->root->drawSelfAndChild(transform);
    /*ResourceManager::Instance().shader_outline->use();
    glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
    ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
    for (auto& collider : prefab_colliders) {
        collider->draw(*ResourceManager::Instance().shader_outline);
    }*/
}

// ====PARTICLE EMITTER====

ParticleEmitter::ParticleEmitter(Texture _texture, unsigned int _particle_number) : Node("ParticleEmitter", 99), texture(_texture), particle_number(_particle_number)
{
    this->shader = ResourceManager::Instance().shader_particle;
    this->init();
}

void ParticleEmitter::init() {

    float quadVertices[] = {
        // positions        // texCoords
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,

        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &this->VAO);
    //glGenBuffers(1, &VBO);
    glGenBuffers(1, &this->quadVBO);
    glGenBuffers(1, &this->instanceVBO);

    glBindVertexArray(VAO);

    // quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // texCoords (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // instancing attributes
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, particle_number * sizeof(ParticleInstanceData), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2); // instance position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (void*)0);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3); // instance color
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (void*)(sizeof(glm::vec3)));
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);

    particles.resize(particle_number);
    particle_instances_data.resize(particle_number);

    // create this->particle_number default particle instances
    for (unsigned int i = 0; i < this->particle_number; ++i) {
        this->particles[i].life = 0.f;
        this->particle_instances_data[i].color = glm::vec4(0.0f);
        this->particle_instances_data[i].position = glm::vec4(0.0f);
    }
    //last_used_particle = 0;

}

void ParticleEmitter::update(float dt, Node* node, unsigned int new_particles, glm::vec3 offset) {
    
    // Add new particles
    for (unsigned int i = 0; i < new_particles; ++i) {
        int unused_particle = firstUnusedParticle();
        this->respawnParticle(this->particles[unused_particle], node, offset);
    }

    // Update all particles
    for (unsigned int i = 0; i < this->particle_number; ++i) {
        Particle& p = this->particles[i];
        p.life -= dt;

        if (p.life > 0.f) {
            p.position -= p.velocity * dt;
        }
    }

}

void ParticleEmitter::draw(const glm::mat4& view, const glm::mat4& projection) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    updateInstanceBuffer();

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture.id);
    shader->setInt("sprite", 0);

    // TO-DO: texture logic //

    unsigned int aliveCount = 0;
    for (auto& p : particles)
        if (p.life > 0.0f) ++aliveCount;

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, aliveCount);
    glBindVertexArray(0);

    

}

void ParticleEmitter::updateInstanceBuffer() {

    for (unsigned int i = 0; i < particle_number; ++i) {
        Particle& p = particles[i];
        ParticleInstanceData& inst = particle_instances_data[i];

        if (p.life > 0.0f) {
            inst.position = p.position;
            float alpha = std::clamp(p.life, 0.0f, 1.0f);
            inst.color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);
        }
        else {
            inst.color = glm::vec4(0.0f);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particle_number * sizeof(ParticleInstanceData), particle_instances_data.data());

}

void ParticleEmitter::respawnParticle(Particle& particle, Node* node, glm::vec3 offset) {

    // float spread = 3.5f;
    float spread = 1.5f;


    glm::vec3 random(
        ((rand() % 100) - 50) / 100.0f * spread,
        ((rand() % 100) - 50) / 100.0f * spread,
        ((rand() % 100) - 50) / 100.0f * spread
    );

    particle.position = node->getTransform().getGlobalPosition() + random + offset;
    //particle.velocity = glm::vec3(0.0f, 0.1f, 0.0f);
    particle.velocity = glm::vec3(0.0f, 0.5f, 0.0f);
    //particle.life = 20.0f;
    particle.life = 3.0f;

}

unsigned int ParticleEmitter::firstUnusedParticle() {
    for (unsigned int i = last_used_particle; i < particle_number; ++i) {
        if (particles[i].life <= 0.0f) {
            last_used_particle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < last_used_particle; ++i) {
        if (particles[i].life <= 0.0f) {
            last_used_particle = i;
            return i;
        }
    }
    last_used_particle = 0;
    return 0;
}
