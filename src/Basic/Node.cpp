#include "Node.h"

#include "../config.h"

#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "Model.h"
#include "Animator.h"
#include "../Basic/Shader.h"

#include "../Grid.h"

#include "../ResourceManager.h"
#include "../System/Tag.h"


using namespace std;

SceneGraph::SceneGraph()
{

    root = new Node("root", 0);
    grid = new Grid();
    grid->gridType = camera->mode == FRONT_ORTO ? GRID_XY : GRID_XZ;
    grid->Update();
    root->scene_graph = this;
    marked_object = nullptr;
    new_marked_object = nullptr;
    size++;

    glGenFramebuffers(1, &depthMapFBO);


}

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
    
    //if (is_editing)
        root->updateComponents(delta_time);
    root->updateSelfAndChild(false);
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

void Node::addComponent(std::unique_ptr<Component> component) {
    
    component->onAttach(this);
    components.push_back(std::move(component));
    
}

void Node::deleteComponent(std::list<std::unique_ptr<Component>>::iterator& it) {
    (*it)->onDetach();  
    it = components.erase(it);
}

void Node::increaseCount() {
    scene_graph->size++;
}

std::string Node::getTagName()
{
	
    return tag.lock()->name;
}

std::string Node::getLayerName()
{
    return layer.lock()->name;
}

// Get child by its name
Node* Node::getChildByName(const std::string& name) {
    for (auto& child : children) {
        if (child->name == name) {
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

void Node::checkIfInFrustrum(std::vector<BoundingBox*>& colliders) {
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        if (in_frustrum) {
			colliders.push_back(AABB);
        }
    }
    else {
        in_frustrum = true;
    }

    for (auto& child : children) {
        child->checkIfInFrustrum(colliders);
    }
}

void Node::mark(Ray rayWorld, float& marked_depth) {

    for (auto&& child : children) {
        float t;

        if (child->AABB != nullptr && child->in_frustrum && child->AABB->isRayIntersects(rayWorld.direction, rayWorld.origin, t)) {

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

    if (pModel && is_visible && in_frustrum) {
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

    if (pModel && is_visible && in_frustrum) {
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

void Node::updateComponents(float deltaTime) {
    if (animator != nullptr) {
        animator->updateAnimation(deltaTime);
    }

	for (auto&& component : components) {
		component->onUpdate(deltaTime);
	}

    for (auto&& child : children) {
        child->updateComponents(deltaTime);
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

Node::Node(std::string nameOfNode, int _id) {
    pModel = nullptr;
    name = nameOfNode;
    id = _id;
    AABB = nullptr;
    no_textures = true;

    layer = TagLayerManager::Instance().getLayer("Default");
    tag = TagLayerManager::Instance().getTag("Default");
}

Node::Node(std::shared_ptr<Model> model, std::string nameOfNode, std::vector<BoundingBox*>& vector_of_colliders, int _id, glm::vec3 min_point, glm::vec3 max_point) : pModel{ model } {
    name = nameOfNode;
    id = _id;
    no_textures = false;

    if (model && model->mode == "plane") {
        max_point = glm::vec3(0.5f, 0.0f, 0.5f);
        min_point = glm::vec3(-0.5f, 0.0f, -0.5f);

    }

	layer = TagLayerManager::Instance().getLayer("Default");
	tag = TagLayerManager::Instance().getTag("Default");

    if (model && model->min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), this, model->min_points, model->max_points);
    else AABB = new BoundingBox(transform.getModelMatrix(), this, min_point, max_point);

    //this->no_textures = no_textures;
    vector_of_colliders.push_back(AABB);
}

Node::Node(std::shared_ptr<Model> model, std::string nameOfNode, int _id, glm::vec3 min_point, glm::vec3 max_point) : pModel{ model } {
    name = nameOfNode;
    id = _id;
    no_textures = false;

    if (model && model->mode == "plane") {
        max_point = glm::vec3(0.5f, 0.0f, 0.5f);
        min_point = glm::vec3(-0.5f, 0.0f, -0.5f);

    }

    layer = TagLayerManager::Instance().getLayer("Default");
    tag = TagLayerManager::Instance().getTag("Default");

    if (model && model->min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), this, model->min_points, model->max_points);
    else AABB = new BoundingBox(transform.getModelMatrix(), this, min_point, max_point);

    //this->no_textures = no_textures;
}

/*void Node::separate(const BoundingBox* other_AABB) {

    float left = (other_AABB->min_point_world.x - AABB->max_point_world.x);
    float right = (other_AABB->max_point_world.x - AABB->min_point_world.x);
    float up = (other_AABB->min_point_world.y - AABB->max_point_world.y);
    float down = (other_AABB->max_point_world.y - AABB->min_point_world.y);
    float front = (other_AABB->min_point_world.z - AABB->max_point_world.z);
    float back = (other_AABB->max_point_world.z - AABB->min_point_world.z);
    glm::vec3 v = glm::vec3(std::abs(left) < std::abs(right) ? left : right, std::abs(up) < std::abs(down) ? up : down, std::abs(front) < std::abs(back) ? front : back);

    if (std::abs(v.x) <= std::abs(v.y) && std::abs(v.x) <= std::abs(v.z)) {
        v.y = 0.f;
        v.z = 0.f;
        AABB->collison = v.x > 0.f ? 1 : -1;
    }
    else if (std::abs(v.y) <= std::abs(v.x) && std::abs(v.y) <= std::abs(v.z)) {
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
*/

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

PrefabInstance::PrefabInstance(std::shared_ptr<Prefab> prefab, std::vector<BoundingBox*>& colliders)
    : Node(prefab->prefab_scene_graph->root->name + "_inst") {
    this->prefab = prefab;
    AABB = new BoundingBox(transform.getModelMatrix(), this);
    set_prefab_colliders(prefab->prefab_scene_graph->root);
    colliders.push_back(AABB);
}

void PrefabInstance::set_prefab_colliders(Node* node)
{
    if (node->AABB) {
        BoundingBox* new_collider = new BoundingBox(node->transform.getModelMatrix(), this, node->AABB->min_point_local, node->AABB->max_point_local);
        new_collider->transformWithOffsetAABB(transform.getModelMatrix());
        prefab_colliders.push_back(new_collider);
        AABB->max_point_local = glm::max(AABB->max_point_local, new_collider->max_point_world);
        AABB->min_point_local = glm::min(AABB->min_point_local, new_collider->min_point_world);
    }
    for (Node* child : node->children) {
        set_prefab_colliders(child);
    }
}

void PrefabInstance::updateSelfAndChild(bool controlDirty)
{

    controlDirty = controlDirty || transform.isDirty();

    if (controlDirty) {
        transform.computeModelMatrix();
        AABB->transformAABB(transform.getModelMatrix());
        for (auto& collider : prefab_colliders) {
            collider->transformWithOffsetAABB(transform.getModelMatrix());
        }
    }

}
void PrefabInstance::drawSelfAndChild()
{
    if (in_frustrum) {
        prefab->prefab_scene_graph->root->drawSelfAndChild(transform);

        if (scene_graph->is_editing) {
            ResourceManager::Instance().shader_outline->use();
            glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
            ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
            for (auto& collider : prefab_colliders) {
                collider->draw(*ResourceManager::Instance().shader_outline);
            }
            dynamic_color = glm::vec3(0.f, 0.6f, 0.6f);
            ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
            AABB->draw(*ResourceManager::Instance().shader_outline);
        }
    }
    
    
}

Light::Light(std::shared_ptr<Model> model, std::string nameOfNode, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Node(model, nameOfNode), ambient(ambient), diffuse(diffuse), specular(specular) {

    no_textures = true;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

DirectionalLight::DirectionalLight(std::shared_ptr<Model> model, std::string nameOfNode, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, ambient, diffuse, specular), direction(direction) {
    updateMatrix();
}

void DirectionalLight::render(unsigned int depthMapFBO, Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    shader.use();
    updateMatrix();
    shader.setMat4("view_projection", view_projection);

}

PointLight::PointLight(std::shared_ptr<Model> model, std::string nameOfNode, float quadratic, float linear, float constant, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, ambient, diffuse, specular), quadratic(quadratic), linear(linear), constant(constant) {

    glGenTextures(1, &depthMapBack);
    glBindTexture(GL_TEXTURE_2D, depthMapBack);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void PointLight::render(unsigned int depthMapFBO, Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    shader.use();
    updateMatrix();
    shader.setMat4("view_projection", view_projection);

}

void PointLight::renderBack(unsigned int depthMapFBO, Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapBack, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    shader.use();
    updateMatrix();
    shader.setMat4("view_projection", view_projection_back);

}

void PointLight::updateMatrix()
{
    view = glm::lookAt(transform.getGlobalPosition() + glm::vec3(0.0f, 0.5f, 0.0f), transform.getGlobalPosition() + glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.f, 0.f, 1.f));
    view_back = glm::lookAt(transform.getGlobalPosition() + glm::vec3(0.0f, -0.5f, 0.0f), transform.getGlobalPosition() + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.f, 0.f, -1.f));

    projection = glm::perspective(glm::radians(170.f), 1.f, 0.5f, 40.f);
    view_projection = projection * view;
    view_projection_back = projection * view_back;
}

Prefab::Prefab(std::string name, PrefabType prefab_type)
{
    //this->prefab_scene_graph = make_shared<SceneGraph>();
    this->prefab_scene_graph = new SceneGraph();
    this->prefab_scene_graph->root->name = name;
    this->prefab_type = prefab_type;
    prefab_scene_graph->directional_lights.push_back(new DirectionalLight(nullptr, "editor_light"));
    prefab_scene_graph->directional_light_number++;
    //prefab_scene_graph->addDirectionalLight();
}
