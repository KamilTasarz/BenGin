#include "Node.h"

#include "../config.h"

#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "Model.h"
#include "Mesh.h"
#include "Animator.h"
#include "../Basic/Shader.h"

#include "../Grid.h"

#include "../ResourceManager.h"
#include "../System/Tag.h"

#include "../System/Rigidbody.h"
#include "../Gameplay/ScriptFactory.h"
#include "../Particle/Particle.h"

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

void SceneGraph::addChild(Node* p, Node* parent) {
    
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
        to_delete_vec.push_back(*it);
        //siblings.erase(it);
    }
}

void SceneGraph::clearDeleteVector()
{
    while (!to_delete_vec.empty()) {
        Node* parent = to_delete_vec.back()->parent;
        if (!parent) return;
        auto& siblings = parent->children;
        auto it = std::find(siblings.begin(), siblings.end(), to_delete_vec.back());
        if (it != siblings.end()) {
            
            siblings.erase(it);
        }
        delete to_delete_vec.back();
        to_delete_vec.pop_back();
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
    if (!p->from_prefab)
        addChild(p);
    point_lights.push_back(p);
    point_light_number++;
    
}

void SceneGraph::addDirectionalLight(DirectionalLight* p) {
    if (!p->from_prefab)
        addChild(p);
        
    directional_lights.push_back(p);
    directional_light_number++;
    
}

void SceneGraph::addPointLight(PointLight* p, std::string name) {
    
    if (!p->from_prefab)
        addChild(p, name);
    point_lights.push_back(p);
    point_light_number++;
}

void SceneGraph::addDirectionalLight(DirectionalLight* p, std::string name) {
    if (!p->from_prefab)
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
        if (dir_light->is_shining) {
            dir_light->render(depthMapFBO, *ResourceManager::Instance().shader_shadow);
            glClear(GL_DEPTH_BUFFER_BIT);
            root->drawShadows(*ResourceManager::Instance().shader_shadow);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
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
    
    if (!is_editing)
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
        if (point_light->in_frustrum) {
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
        if (i >= 100) break;
    }
    i = 0;
    for (auto& dir_light : directional_lights) {
        string index = to_string(i);
        shader->setVec3("directional_lights[" + index + "].direction", dir_light->direction);
        if (dir_light->is_shining == true) {
            shader->setVec3("directional_lights[" + index + "].ambient", dir_light->ambient);
            shader->setVec3("directional_lights[" + index + "].diffuse", dir_light->diffuse);
            shader->setVec3("directional_lights[" + index + "].specular", dir_light->specular);
            shader->setInt("useShadows", 1);
        }
        else {
            shader->setVec3("directional_lights[" + index + "].ambient", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("directional_lights[" + index + "].diffuse", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setVec3("directional_lights[" + index + "].specular", glm::vec3({ 0.f, 0.f, 0.f }));
            shader->setInt("useShadows", 0);
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

void Node::addComponent(std::unique_ptr<Component> component) {
    
    if (component->name == "Rigidbody") has_RB = true;

    component->onAttach(this);
    components.push_back(std::move(component));
    
}

void Node::deleteComponent(std::list<std::unique_ptr<Component>>::iterator& it) {
    (*it)->onDetach();  
	if ((*it)->name == "Rigidbody") has_RB = false;
    
    it = components.erase(it);
}

void Node::increaseCount() {
    scene_graph->size++;
}

std::string Node::getTagName()
{
    auto lockedTag = tag.lock();
    return lockedTag ? lockedTag->name : "";
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

Node* Node::getChildByTag(std::string tag) {
	for (auto child : children) {
		if (child->getTagName() == tag) {
			return child;
		}
		// Checks the children of the first level recursively
		Node* foundChild = child->getChildByTag(tag);
		if (foundChild != nullptr) {
			return foundChild;
		}
	}
	return nullptr;
}

Node* Node::clone(std::string instance_name, SceneGraph* new_scene_graph) {

    Node* copy = nullptr;

    // Tworzymy nowego Node'a na podstawie bieżących danych
    if (!dynamic_cast<PointLight*>(this) && !dynamic_cast<DirectionalLight*>(this)) {
        copy = new Node(this->name + "_" + instance_name, this->id);

        //^ zapewnić kopie instancji prefabów ^

        copy->color = this->color;
        copy->transform = this->transform;
		if (new_scene_graph)
			copy->scene_graph = new_scene_graph;
		else
			copy->scene_graph = this->scene_graph;  // zakładamy, że jest współdzielony (lub ustawiany później)
        //copy->scene_graph = this->scene_graph;  // zakładamy, że jest współdzielony (lub ustawiany później)

        // Model - zakładamy, że można go współdzielić
        copy->pModel = this->pModel;

        // AABB - głęboka kopia
        if (this->AABB) {
            copy->AABB = this->AABB->clone(copy);
        }

        // Animator - głęboka kopia (jeśli nie jest współdzielony)
        if (this->animator)
            copy->animator = new Animator(*this->animator);

        // Przeniesienie ustawień widoczności i stanu
        copy->is_visible = this->is_visible;
        copy->in_frustrum = this->in_frustrum;
        copy->is_marked = this->is_marked;
        copy->no_textures = this->no_textures;
		copy->is_physic_active = this->is_physic_active;
		copy->is_logic_active = this->is_logic_active;
        

        // Layer i Tag — kopiujemy weak_ptr
        copy->layer = this->layer;
        copy->tag = this->tag;

        //copy->parent = this->parent;

        // Komponenty — głęboka kopia
        //std::vector<std::string> scripts = ScriptFactory::instance().getScriptNames();
        for (auto& comp : this->components) {
            if (comp->name == "Rigidbody") {
                Rigidbody* rigidbody = dynamic_cast<Rigidbody*>(comp.get());
                copy->addComponent(std::make_unique<Rigidbody>(rigidbody->mass, rigidbody->gravity, rigidbody->is_static));
            }
            else {
                Script* script = dynamic_cast<Script*>(comp.get());
                std::unique_ptr<Script> new_script = ScriptFactory::instance().create(comp->name);

                for (auto& var : script->getFields()) {
                    for (auto& v : new_script->getFields()) {

                        if (var->name == v->name && var->type == v->type) {
                            void* ptr = reinterpret_cast<char*>(script) + v->offset;
                            void* new_ptr = reinterpret_cast<char*>(new_script.get()) + v->offset;

                            if (var->type == "float") {
                                float* f = reinterpret_cast<float*>(new_ptr);
                                float* _f = reinterpret_cast<float*>(ptr);
                                *f = *_f;
                            } else if (var->type == "int") {
                                int* i = reinterpret_cast<int*>(new_ptr);
                                int* _i = reinterpret_cast<int*>(ptr);
                                *i = *_i;
                            }
                            else if (var->type == "Node*") {
                                Node** n = reinterpret_cast<Node**>(new_ptr);
                                Node** _n = reinterpret_cast<Node**>(ptr);
                                *n = *_n;
                            }
                            
                            

                            break;
                        }

                    }
                }

                copy->addComponent(std::move(new_script));

            }
        }

        // Dzieci — rekurencyjna kopia
        for (Node* child : this->children) {
            Node* child_copy = child->clone(instance_name, new_scene_graph);
            if (child_copy) {
                copy->addChild(child_copy);  // Ustawia parent i dodaje do zbioru
                
            }

        }
    }
    

    

    return copy;
}


void Node::checkIfInFrustrum(std::vector<BoundingBox*>& colliders, std::vector<BoundingBox*>& colliders_RB) {
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        if (in_frustrum) {
			if (is_physic_active) colliders.push_back(AABB);
            if (is_logic_active && AABB_logic) colliders.push_back(AABB_logic);
            if (has_RB) {
                if (is_physic_active) colliders_RB.push_back(AABB);
                if (is_logic_active && AABB_logic) colliders_RB.push_back(AABB_logic);
            }
        }
    }
    else {
        in_frustrum = true;
    }

    for (auto& child : children) {
        child->checkIfInFrustrum(colliders, colliders_RB);
    }
}

void Node::checkIfInFrustrumLogic(std::vector<BoundingBox*>& colliders_logic, std::vector<BoundingBox*>& colliders_RB_logic)
{
    if (AABB_logic) {
        
        if (in_frustrum) {
            colliders_logic.push_back(AABB_logic);
            if (has_RB) {
                colliders_RB_logic.push_back(AABB_logic);
            }
        }
    }

    for (auto& child : children) {
        child->checkIfInFrustrumLogic(colliders_logic, colliders_RB_logic);
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
    if (AABB_logic != nullptr) {
        AABB_logic->transformAABB(transform.getModelMatrix());
    }
}

// This will update if there were changes only (checks the dirty flag)
void  Node::updateSelfAndChild(bool controlDirty) {

    controlDirty |= transform.isDirty();

    if (in_frustrum) {
        if (controlDirty) {
            forceUpdateSelfAndChild();
            //return;
            //transform.computeModelMatrix();

        }
    }

    for (auto& child : children)
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
        dynamic_color = glm::vec3(0.f, 0.8f, 0.f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
        if (scene_graph->is_editing && AABB_logic) {
            AABB_logic->draw(*ResourceManager::Instance().shader_outline);
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
    if (in_frustrum) {
        if (animator != nullptr) {
            animator->updateAnimation(deltaTime);
        }

        for (auto& component : components) {
            component->onUpdate(deltaTime);
        }
    }
    
    for (auto& child : children) {
        child->updateComponents(deltaTime);
    }
}

void Node::createComponents()
{
   
    for (auto& component : components) {
        component->onStart();
    }

    for (auto& child : children) {
        child->createComponents();
    }
}

void Node::drawShadows(Shader& shader) {
    if (pModel && in_frustrum) {

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
    AABB_logic = nullptr;
    no_textures = true;

    layer = TagLayerManager::Instance().getLayer("Default");
    tag = TagLayerManager::Instance().getTag("Default");
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

	AABB_logic = AABB->clone(this);
    AABB_logic->is_logic = true;

    //this->no_textures = no_textures;
    
}

//Node::Node(std::shared_ptr<Model> model, std::string nameOfNode, int _id, glm::vec3 min_point, glm::vec3 max_point) : pModel{ model } {
//    name = nameOfNode;
//    id = _id;
//    no_textures = false;
//
//    if (model && model->mode == "plane") {
//        max_point = glm::vec3(0.5f, 0.0f, 0.5f);
//        min_point = glm::vec3(-0.5f, 0.0f, -0.5f);
//
//    }
//
//    layer = TagLayerManager::Instance().getLayer("Default");
//    tag = TagLayerManager::Instance().getTag("Default");
//
//    if (model && model->min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), this, model->min_points, model->max_points);
//    else AABB = new BoundingBox(transform.getModelMatrix(), this, min_point, max_point);
//
//    AABB_logic = AABB->clone(this);
//    AABB_logic->is_logic = true;
//
//    //this->no_textures = no_textures;
//}

Node::~Node()
{
    delete AABB;
    AABB = nullptr;

    delete AABB_logic;
    AABB_logic = nullptr;

    delete animator;
    animator = nullptr;



    for (Node* child : children) {
        
        delete child;
    }

    children.clear();
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

InstanceManager::~InstanceManager()
{
    for (Node* child : children) {
        delete child;
    }
}

void InstanceManager::drawSelfAndChild() {
    if (in_frustrum && !scene_graph->is_editing) {
        glStencilMask(0xFF);
        ResourceManager::Instance().shader_instanced->use();
        pModel->DrawInstanced(*ResourceManager::Instance().shader_instanced, size);
        glStencilMask(0x00);
    }
}

void InstanceManager::updateSelfAndChild(bool controlDirty) {

    controlDirty |= transform.isDirty();

    if (in_frustrum) {
        if (controlDirty) {
            forceUpdateSelfAndChild();
        }
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

void InstanceManager::checkIfInFrustrum(std::vector<BoundingBox*>& colliders, std::vector<BoundingBox*>& colliders_RB)
{
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        if (in_frustrum) {
            if (is_physic_active) colliders.push_back(AABB);
            if (is_logic_active && AABB_logic) colliders.push_back(AABB_logic);
            if (has_RB) {
                if (is_physic_active) colliders_RB.push_back(AABB);
                if (is_logic_active && AABB_logic) colliders_RB.push_back(AABB_logic);
            }
        }
    }
    else {
        in_frustrum = true;
    }

    for (auto& child : children) {
        child->checkIfInFrustrum(colliders, colliders_RB);
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

PrefabInstance::PrefabInstance(std::shared_ptr<Prefab> prefab, SceneGraph* _scene_graph)
    : Node(prefab->prefab_scene_graph->root->name + "_inst") {
    this->prefab = prefab;
    AABB = new BoundingBox(transform.getModelMatrix(), this);

    scene_graph = _scene_graph;

    prefab->prefab_instances.push_back(this);

    set_prefab_colliders(prefab->prefab_scene_graph->root);

    //if (scene_graph) {
    prefab_root = prefab->clone(this->name, scene_graph);
    prefab_root->parent = this;
    //}
}

PrefabInstance::PrefabInstance(std::shared_ptr<Prefab> prefab, SceneGraph* _scene_graph, glm::vec3 position)
    : Node(prefab->prefab_scene_graph->root->name + "_inst") {
    this->prefab = prefab;
    AABB = new BoundingBox(transform.getModelMatrix(), this);

	scene_graph = _scene_graph;
    
    prefab->prefab_instances.push_back(this);

    set_prefab_colliders(prefab->prefab_scene_graph->root);
    
    //if (scene_graph) {
    prefab_root = prefab->clone(this->name, scene_graph);
    prefab_root->parent = this;
	prefab_root->transform.setLocalPosition(position);
    //}
}

void PrefabInstance::set_prefab_colliders(Node* node)
{
    if (node->AABB) {

        AABB->max_point_local = glm::max(AABB->max_point_local, node->AABB->max_point_world);
        AABB->min_point_local = glm::min(AABB->min_point_local, node->AABB->min_point_world);
    }
    for (Node* child : node->children) {
        set_prefab_colliders(child);
    }
}

void PrefabInstance::updateSelf()
{
    delete prefab_root;
    delete AABB;
	AABB = new BoundingBox(transform.getModelMatrix(), this);
	prefab_root = prefab->clone(this->name, scene_graph);
    prefab_root->parent = this;
	updateSelfAndChild(true);
    set_prefab_colliders(prefab->prefab_scene_graph->root);
	forceUpdateSelfAndChild();
}

void PrefabInstance::updateSelfAndChild(bool controlDirty)
{
    if (scene_graph && !prefab_root) {
        prefab_root = prefab->clone(this->name, scene_graph);
		prefab_root->transform.setLocalPosition(transform.getLocalPosition());
		prefab_root->transform.setLocalRotation(transform.getLocalRotation());
		prefab_root->transform.setLocalScale(transform.getLocalScale());
    }


    controlDirty = controlDirty || transform.isDirty();

    if (controlDirty) {
        /*transform.computeModelMatrix();
        AABB->transformAABB(transform.getModelMatrix());*/

		//AABB->transformAABB(transform.getModelMatrix());
		forceUpdateSelfAndChild();
        //prefab_root->transform.setLocalPosition(transform.getGlobalPosition());
        //prefab_root->transform.setLocalRotation(transform.getLocalRotation());
        //prefab_root->transform.setLocalScale(transform.getLocalScale());
        
    }
    prefab_root->updateSelfAndChild(controlDirty);
    
}
void PrefabInstance::updateComponents(float deltaTime)
{
    if (in_frustrum && prefab_root) {
		prefab_root->updateComponents(deltaTime);
    }
}
void PrefabInstance::createComponents()
{
	if (in_frustrum && prefab_root) {
		prefab_root->createComponents();
	}
}
void PrefabInstance::forceUpdateSelfAndChild()
{
    
   /* prefab_root->transform.setLocalPosition(transform.getLocalPosition());
    prefab_root->transform.setLocalRotation(transform.getLocalRotation());
    prefab_root->transform.setLocalScale(transform.getLocalScale());*/
    
    //prefab_root->transform.computeModelMatrix(transform.getModelMatrix());
    

    if (parent) {
        transform.computeModelMatrix(parent->transform.getModelMatrix());
    }
    else {
        transform.computeModelMatrix();
    }
    if (AABB != nullptr) {
        AABB->transformAABB(transform.getModelMatrix());
    }
    if (AABB_logic != nullptr) {
        AABB_logic->transformAABB(transform.getModelMatrix());
    }
}
void PrefabInstance::drawSelfAndChild()
{
    if (in_frustrum) {
        prefab_root->drawSelfAndChild();

        if (scene_graph->is_editing) {
            ResourceManager::Instance().shader_outline->use();
            glm::vec3 dynamic_color = glm::vec3(0.f, 0.6f, 0.6f);
            ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
            AABB->draw(*ResourceManager::Instance().shader_outline);
        }
    }
    
    
}

void PrefabInstance::drawShadows(Shader& shader)
{
	if (in_frustrum) {
		prefab_root->drawShadows(shader);
	}
}

void PrefabInstance::checkIfInFrustrum(std::vector<BoundingBox*>& colliders, std::vector<BoundingBox*>& colliders_RB)
{
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        if (in_frustrum) {
            for (auto& child : prefab_root->getAllChildren()) {
                child->in_frustrum = true;
                if (child->AABB && child->is_physic_active) {
                    if (child->has_RB) colliders_RB.push_back(child->AABB);
                    colliders.push_back(child->AABB);
                }
                if (child->AABB_logic && child->is_logic_active) {
                    if (child->has_RB) colliders_RB.push_back(child->AABB_logic);
                    colliders.push_back(child->AABB_logic);
                }
            }
        }
        else {
            for (auto& child : prefab_root->getAllChildren()) {
                child->in_frustrum = false;
            }
        }
    }
    else {
        in_frustrum = true;
    }
}

Light::Light(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Node(model, nameOfNode), ambient(ambient), diffuse(diffuse), specular(specular) {

    no_textures = true;

	is_shining = _is_shining;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

DirectionalLight::DirectionalLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, _is_shining, ambient, diffuse, specular), direction(direction) {
    updateMatrix();
}

DirectionalLight::~DirectionalLight()
{
    scene_graph->directional_lights.remove(this);
	scene_graph->directional_light_number--;
    glDeleteTextures(1, &depthMap);
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

PointLight::PointLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, float quadratic, float linear, float constant, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, _is_shining, ambient, diffuse, specular), quadratic(quadratic), linear(linear), constant(constant) {

    glGenTextures(1, &depthMapBack);
    glBindTexture(GL_TEXTURE_2D, depthMapBack);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

PointLight::~PointLight()
{
    scene_graph->point_lights.remove(this);
	scene_graph->point_light_number--;
    glDeleteTextures(1, &depthMap);
    glDeleteTextures(1, &depthMapBack);
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
    prefab_scene_graph->directional_lights.push_back(new DirectionalLight(nullptr, "editor_light", true));
    prefab_scene_graph->directional_light_number++;
    //prefab_scene_graph->addDirectionalLight();
}

Node* Prefab::clone(std::string instance_name, SceneGraph* scene_graph, bool light_copy)
{
	Node* copy = prefab_scene_graph->root->clone(instance_name, scene_graph);
    /*copy->scene_graph = scene_graph;*/
    for (auto& light : prefab_scene_graph->point_lights) {
		PointLight* new_light = new PointLight(light->pModel, light->name + "_" + instance_name, light->is_shining, light->quadratic, light->linear, light->constant,
            light->ambient, light->diffuse, light->specular);
        new_light->from_prefab = !light_copy;
        new_light->parent = copy;
		copy->addChild(new_light);
		new_light->scene_graph = scene_graph;
		new_light->transform.setLocalPosition(light->transform.getGlobalPosition());
		new_light->transform.computeModelMatrix();
		//scene_graph->addPointLight(new_light);
        scene_graph->point_lights.push_back(new_light);
        scene_graph->point_light_number++;
		//new_light->parent = copy;
    }
    return copy;
}

void Prefab::notifyInstances()
{
    for (auto it = prefab_instances.begin(); it != prefab_instances.end();) {
        if (*it) {
			(*it)->updateSelf();
			//(*it)->forceUpdateSelfAndChild();
			++it;
		}
		else {
			it = prefab_instances.erase(it);
        }
    }
}


// ====PARTICLE EMITTER====

ParticleEmitter::ParticleEmitter(unsigned int _texture_id, unsigned int _particle_number) : Node("ParticleEmitter", 99), particle_number(_particle_number)
{
    this->shader = ResourceManager::Instance().shader_particle;
	texture_id = _texture_id;     
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
    glBindTexture(GL_TEXTURE_2D, texture_id);
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
