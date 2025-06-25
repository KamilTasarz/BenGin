#include "Node.h"

#include "../config.h"

#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "Model.h"
#include "Mesh.h"
#include "Animator.h"
#include "../Basic/Shader.h"

#include "../System/Grid.h"

#include "../ResourceManager.h"
#include "../System/Tag.h"

#include "../System/Rigidbody.h"
#include "../Gameplay/ScriptFactory.h"
#include "../Particle/Particle.h"
#include "../System/PhysicsSystem.h"
#include "../System/RenderSystem.h"

#include "../Gameplay/GameManager.h"

using namespace std;


SceneGraph::SceneGraph()
{

    if (!point_lights.empty())
        last_index = point_lights.begin();
    else
        last_index = point_lights.end();


    root = new Node("root", 0);
    grid = new Grid();
    grid->gridType = camera->mode == FRONT_ORTO ? GRID_XY : GRID_XZ;
    grid->Update();
    root->scene_graph = this;
    marked_object = nullptr;
    new_marked_object = nullptr;
    size++;

    glGenFramebuffers(1, &depthMapFBO);

    GameManager::instance().constructorsScene++;
	cout << "SceneGraph constructor called" << endl;
}

SceneGraph::~SceneGraph()
{
	cout << "SceneGraph destructor called" << endl;
    GameManager::instance().destructorsScene++;
    delete root;
    root = nullptr;
    delete grid;
    grid = nullptr;

}

void SceneGraph::unmark() {
    marked_object->is_marked = false;
    marked_object = nullptr; //marked object returns as nullptr after draw
}

void SceneGraph::addChild(Node* p) {

    int i = 1;
	string name = p->name;
    root->addChild(p);

    if (dynamic_cast<InstanceManager*>(p)) emitter = dynamic_cast<InstanceManager*>(p);

}

void SceneGraph::addChild(Node* p, std::string name) {

    Node* parent = root->getChildByName(name);
    if (parent != nullptr) {
        int i = 1;
        string name = p->name;

        parent->addChild(p);

    }

}

void SceneGraph::addChild(Node* p, Node* parent) {
    
    if (parent != nullptr) {
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
    }
}

void SceneGraph::clearDeleteVector()
{
    std::unordered_set<Node*> unique_nodes(to_delete_vec.begin(), to_delete_vec.end());

    for (Node* node : unique_nodes) {
        if (!node) continue;
        Node* parent = node->parent;
        if (!parent) continue;

        auto& siblings = parent->children;
        auto it = std::find(siblings.begin(), siblings.end(), node);
        if (it != siblings.end()) {
            siblings.erase(it);
            this->size--;
        }
        bool is_iter = false;
        if (dynamic_cast<PointLight*>(node)) {
			is_iter = true;
        }

        
        delete node;
		last_index = point_lights.begin(); 
    }

    to_delete_vec.clear();
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

    float time = glfwGetTime();

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();

    // --- BASIC SHADER --- //

    auto& basic_shader = *ResourceManager::Instance().shader;

    basic_shader.use();
    setLights(&basic_shader);
    basic_shader.setMat4("projection", projection);
    basic_shader.setMat4("view", view);
    basic_shader.setFloat("time", time);

    if (active_point_lights >= 16) basic_shader.setInt("point_light_number", 16);
    else basic_shader.setInt("point_light_number", active_point_lights);

    basic_shader.setInt("directional_light_number", directional_light_number);
    basic_shader.setFloat("far_plane", 20.f);
    
    // --- TILE SHADER --- //

    auto& tile_shader = *ResourceManager::Instance().shader_tile;

    tile_shader.use();
    setLights(&tile_shader);
    tile_shader.setMat4("projection", projection);
    tile_shader.setMat4("view", view);
    if (active_point_lights >= 16) tile_shader.setInt("point_light_number", 16);
    else  tile_shader.setInt("point_light_number", active_point_lights);
    tile_shader.setInt("directional_light_number", directional_light_number);
    tile_shader.setFloat("far_plane", 20.f);
    tile_shader.setFloat("tile_scale", 1.f);
    tile_shader.setFloat("time", time);
    
    // --- INSTANCED SHADER --- //

    ResourceManager::Instance().shader_instanced->use();
    setLights(ResourceManager::Instance().shader_instanced);
    ResourceManager::Instance().shader_instanced->setMat4("projection", projection);
    ResourceManager::Instance().shader_instanced->setMat4("view", view);
    ResourceManager::Instance().shader_instanced->setFloat("totalTime", glfwGetTime());
    ResourceManager::Instance().shader_instanced->setFloat("u_time", glfwGetTime());
    ResourceManager::Instance().shader_instanced->setFloat("scaleFactor", 0.2f);
    ResourceManager::Instance().shader_instanced->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader_instanced->setInt("directional_light_number", directional_light_number);

    // --- OUTLINE SHADER --- //

    ResourceManager::Instance().shader_outline->use();
    ResourceManager::Instance().shader_outline->setMat4("projection", projection);
    ResourceManager::Instance().shader_outline->setMat4("view", view);
    ResourceManager::Instance().shader_outline->setInt("point_light_number", point_light_number);
    ResourceManager::Instance().shader_outline->setInt("directional_light_number", directional_light_number);
}

void SceneGraph::draw(float width, float height, unsigned int framebuffer, bool is_editor) {

    int updated_this_frame = 0;
    int checked = 0;
    int total_lights = point_lights.size();

    // Upewnij się, że last_index nie jest przeterminowany
    if (!point_lights.empty()) {

        auto it = last_index;
        if (it == point_lights.end()) it = point_lights.begin(); // reset przy końcu listy

        while (updated_this_frame < limit_per_frame && checked < total_lights) {
            if (it == point_lights.end()) it = point_lights.begin(); // zawijanie listy

            auto& point_light = *it;

            if (point_light->is_shining && point_light->in_frustrum) {
                point_light->render(depthMapFBO, *ResourceManager::Instance().shader_shadow, 0);

                if (is_editor) {
                    root->drawShadows(*ResourceManager::Instance().shader_shadow);
                }
                else {
                    RenderSystem::Instance().renderShadows();
                }

                updated_this_frame++;
            }

            ++it;
            ++checked;
        }

        // Zaktualizuj last_index
        last_index = it;

    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glViewport(0, 0, width, height);
    glClearColor(.01f, .01f, .01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (is_editing) {
        grid->Draw();
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setShaders();
    
    if (is_editor) {
        root->drawSelfAndChild();
     }
    else {
        RenderSystem::Instance().render();
        if (emitter)
            emitter->drawSelfAndChild();
    }

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

void SceneGraph::activateRewindShader() {
    this->is_rewinidng = true;
}

void SceneGraph::deactivateRewindShader() {
    this->is_rewinidng = false;
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
            shader->setBool("point_lights[" + index + "].is_alarm", point_light->is_alarm);
			
            glActiveTexture(GL_TEXTURE8 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, point_light->depthCubemap);
            i++;
        }
		if (i >= 16) break; // Limit 16 swiatel we frustum
    }
    active_point_lights = i;
    GLint loc = glGetUniformLocation(shader->ID, "shadow_maps");
    GLint samplers[16];
    for (int j = 0; j < 16; ++j) {
        samplers[j] = 8 + j;
    }
    glUniform1iv(loc, 16, samplers);

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
        
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, dir_light->depthMap);
        
        //i++;
        shader->setMat4("light_view_projection", dir_light->getMatrix());
        shader->setInt("shadow_map1", 3);
    }


}

void SceneGraph::drawMarkedObject() {
    if (marked_object != nullptr && is_editing) {

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void Node::setPhysic(bool active)
{
	is_physic_active = active;
	AABB->active = active;
}

void Node::setLogic(bool active)
{
	is_logic_active = active;
	AABB_logic->active = active;
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

Node* Node::getChildByNamePart(const std::string& namePart) {
    for (auto& child : children) {
        // Jeśli nazwa zawiera poszukiwany fragment
        if (child->name.find(namePart) != std::string::npos) {
            return child;
        }

        // Rekurencyjnie przeszukaj dzieci
        Node* foundChild = child->getChildByName(namePart);
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

        if (dynamic_cast<InstanceManager*>(this)) {
            copy = new InstanceManager(pModel, instance_name + "_" + this->name, this->id);
        }
        else if (dynamic_cast<MirrorNode*>(this)) {
            copy = new MirrorNode(pModel, instance_name + "_" + this->name);
        }
        else {
            copy = new Node(instance_name + "_" + this->name, this->id);
        }
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
            copy->AABB->transformAABB(copy->transform.getModelMatrix());
        }

        

        if (this->AABB_logic) {
            copy->AABB_logic = this->AABB_logic->clone(copy);
            copy->AABB_logic->transformAABB(copy->transform.getModelMatrix());
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
                            else if (var->type == "bool") {
                                bool* b = reinterpret_cast<bool*>(new_ptr);
                                bool* _b = reinterpret_cast<bool*>(ptr);
                                *b = *_b;
                            }
                            else if (var->type == "std::string") {
                                std::string* s = reinterpret_cast<std::string*>(new_ptr);
                                std::string* _s = reinterpret_cast<std::string*>(ptr);
                                *s = *_s;
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

void Node::setVariablesNodes(std::string instance_name, Node* root, SceneGraph* new_scene_graph)
{   
    for (auto& comp : this->components) {
        if (comp->name != "Rigidbody") {
            
            Script* script = dynamic_cast<Script*>(comp.get());

            for (auto& var : script->getFields()) {
                void* ptr = reinterpret_cast<char*>(script) + var->offset;
                if (var->type == "Node*") {
                    Node** n = reinterpret_cast<Node**>(ptr);
                    if (*n) {
                        Node* _n = root->getChildByName(instance_name + "_" + (*n)->name);
                        *n = _n;
                    }
                }
            }
        }
        

    }
    for (Node* child : this->children) {
        child->setVariablesNodes(instance_name, root, new_scene_graph);
    }
}


void Node::checkIfInFrustrum(std::unordered_set<Collider*>& colliders, std::unordered_set<Collider*>& colliders_RB,
    std::unordered_set<Node*>& rooms) {
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        
        //in_frustrum = true;
        if (in_frustrum) {
            /*if (dynamic_cast<PointLight*>(this)) {
                scene_graph->active.insert(dynamic_cast<PointLight*>(this));
            }*/
			addRenderQueue();

			if (is_physic_active) colliders.insert(AABB);
			else colliders.erase(AABB);

            if (is_logic_active && AABB_logic) colliders.insert(AABB_logic);
			else colliders.erase(AABB_logic);

            if (has_RB) {
                if (is_physic_active) colliders_RB.insert(AABB);
				else colliders_RB.erase(AABB);
            }
        }
        else {
            if (is_physic_active) colliders.erase(AABB);
            if (is_logic_active && AABB_logic) colliders.erase(AABB_logic);
            if (has_RB) {
                if (is_physic_active) colliders_RB.erase(AABB);
            }

        }
    }
    else {
        in_frustrum = true;
        if (is_logic_active && AABB_logic) colliders.insert(AABB_logic);
        else colliders.erase(AABB_logic);
    }

    

    for (auto& child : children) {
        child->checkIfInFrustrum(colliders, colliders_RB, rooms);
    }
}

void Node::checkIfInFrustrumLogic(std::unordered_set<Collider*>& colliders_logic, std::unordered_set<Collider*>& colliders_RB_logic)
{
    if (AABB_logic) {
        
        if (in_frustrum) {
            colliders_logic.insert(AABB_logic);
            if (has_RB) {
                colliders_RB_logic.insert(AABB_logic);
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
        glm::vec3 end;
        if (child->AABB != nullptr && child->in_frustrum && child->AABB->isRayIntersects(rayWorld.direction, rayWorld.origin, t, end)) {

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
void Node::updateSelfAndChild(bool controlDirty) {

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
            ResourceManager::Instance().shader->setVec4("color", color);
        }
        else {
			ResourceManager::Instance().shader_tile->use();
			ResourceManager::Instance().shader_tile->setMat4("model", transform.getModelMatrix());
			ResourceManager::Instance().shader_tile->setFloat("tile_scale", pModel->tile_scale);
            ResourceManager::Instance().shader_tile->setVec4("color", color);
        }
        
        if (is_animating) {
            ResourceManager::Instance().shader->setInt("is_animating", 1);
        }
        else {
            ResourceManager::Instance().shader->setInt("is_animating", 0);
        }

        if (animator != nullptr && is_animating) {
            auto &f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i) {
                ResourceManager::Instance().shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
            }
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
        
    }

    if (AABB || AABB_logic) {
        ResourceManager::Instance().shader_outline->use();

        if (is_animating) {
            ResourceManager::Instance().shader_outline->setInt("is_animating", 1);
        }
        else {
            ResourceManager::Instance().shader_outline->setInt("is_animating", 0);
        }


        ResourceManager::Instance().shader_outline->setMat4("model", transform.getModelMatrix());
        if (animator != nullptr && is_animating) {
            auto& f = animator->final_bone_matrices;
            for (int i = 0; i < f.size(); ++i)
                ResourceManager::Instance().shader_outline->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);

        }
        glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);

        if (scene_graph->is_editing && AABB) {
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

void Node::drawSelf()
{
    if (pModel->mode.empty()) {
        ResourceManager::Instance().shader->use();
        ResourceManager::Instance().shader->setMat4("model", transform.getModelMatrix());
    }
    else {
        ResourceManager::Instance().shader_tile->use();
        ResourceManager::Instance().shader_tile->setMat4("model", transform.getModelMatrix());
    }

    if (is_animating) {
        ResourceManager::Instance().shader->setInt("is_animating", 1);
    }
    else {
        ResourceManager::Instance().shader->setInt("is_animating", 0);
    }

    if (animator != nullptr && is_animating) {
        //is_animating = true;
        auto& f = animator->final_bone_matrices;
        for (int i = 0; i < f.size(); ++i)
            ResourceManager::Instance().shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);

        //ResourceManager::Instance().shader->setInt("is_animating", 1);
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

    if (is_animating) {
        ResourceManager::Instance().shader_outline->setInt("is_animating", 1);
    }
    else {
        ResourceManager::Instance().shader_outline->setInt("is_animating", 0);
    }


    ResourceManager::Instance().shader_outline->setMat4("model", transform.getModelMatrix());
    if (animator != nullptr && is_animating) {
        auto& f = animator->final_bone_matrices;
        for (int i = 0; i < f.size(); ++i)
            ResourceManager::Instance().shader_outline->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);

        //ResourceManager::Instance().shader_outline->setInt("is_animating", 1);
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

void Node::addRenderQueue()
{
    if (is_visible && !no_textures && std::find(scene_graph->to_delete_vec.begin(), scene_graph->to_delete_vec.end(), this) == scene_graph->to_delete_vec.end()) {
        if (is_animating && pModel && pModel->has_animations) RenderSystem::Instance().addAnimatedObject(this);
        else if ((pModel && pModel->mode.empty()) || (pModel && pModel->has_animations && !is_animating)) RenderSystem::Instance().addStaticObject(this);
        else if (pModel) RenderSystem::Instance().addTileObject(this);
    }
}

void Node::addRenderQueueAndChild()
{
    addRenderQueue();
	for (auto&& child : children) {
		child->addRenderQueueAndChild();
	}
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
            auto &f = animator->final_bone_matrices;
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
            auto &f = animator->final_bone_matrices;
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
            child->drawSelfAndChild(parent);
        }
    }
}

void Node::updateComponents(float deltaTime) {
    if (in_frustrum) {
        if (animator != nullptr && is_animating) {
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

void Node::endComponents()
{
    for (auto& component : components) {
        component->onEnd();
    }

}

void Node::drawShadows(Shader& shader) {
    if (pModel && in_frustrum) {

        shader.use();
        shader.setMat4("model", transform.getModelMatrix());
        if (!no_textures && is_visible) {
            if (animator != nullptr) {
                auto &f = animator->final_bone_matrices;
                for (int i = 0; i < f.size(); ++i)
                    shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);

                shader.setInt("is_animating", 1);
            }
            pModel->Draw(shader); //jak nie ma tekstury to najpewniej swiatlo -> przyjmuje takie zalozenie
            
            shader.setInt("is_animating", 0);
            
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
    AABB_logic = new BoundingBox(transform.getModelMatrix(), this);
    no_textures = true;

	AABB_logic->is_logic = true;
	AABB_logic->active = true;

    cout << "Node constructor called for: " << name << endl;

    layer = TagLayerManager::Instance().getLayer("Default");
    tag = TagLayerManager::Instance().getTag("Default");
    GameManager::instance().constructors++;
}

Node::Node(std::shared_ptr<Model> model, std::string nameOfNode, int _id, glm::vec3 min_point, glm::vec3 max_point) : pModel{ model } {
    name = nameOfNode;
    id = _id;
    no_textures = false;

	cout << "Node constructor called for: " << name << endl;

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

    if (model && model->has_animations) {
        animator = new Animator(model->animations[0].get());
        is_animating = false;
    }

    color = glm::vec4(1.f);
    GameManager::instance().constructors++;
}

Node::~Node()
{
	cout << "Node destructor called for: " << name << endl;
    GameManager::instance().destructors++;
    if (scene_graph && !scene_graph->is_editing) {
        endComponents();
    }

    if (AABB) {
		
       
		PhysicsSystem::instance().colliders.erase(AABB);
		PhysicsSystem::instance().colliders_RigidBody.erase(AABB);
        delete AABB;
    }
    AABB = nullptr;

    if (AABB_logic) {
        
       
        PhysicsSystem::instance().colliders.erase(AABB_logic);
		PhysicsSystem::instance().colliders_RigidBody.erase(AABB_logic);
        delete AABB_logic;
    }
    AABB_logic = nullptr;
    delete animator;
    animator = nullptr;



    for (Node* child : children) {
        
        delete child;
    }

    children.clear();
}

const Transform& Node::getTransform() {
    return transform;
}

// ====INSTANCE MANAGER====

InstanceManager::~InstanceManager()
{
	cout << "InstanceManager destructor called for: " << name << endl;
    for (Node* child : children) {
        delete child;
    }

    
}

void InstanceManager::drawSelfAndChild() {
    if (in_frustrum && !scene_graph->is_editing) {
        glStencilMask(0xFF);
        ResourceManager::Instance().shader_instanced->use();
        pModel->DrawInstanced(*ResourceManager::Instance().shader_instanced, max_size);
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
    float t = glfwGetTime();
    for (int i = 0; i < max_size; i++) {
        if (particles[head].time < 0.f) break;

        if (t - particles[head].time > life_time) {
            removeChild();
        }
        else {
            break;
        }
    }

}

void InstanceManager::checkIfInFrustrum(std::unordered_set<Collider*>& colliders,
    std::unordered_set<Collider*>& colliders_RB, std::unordered_set<Node*>& rooms)
{
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);
        if (in_frustrum) {
            if (is_physic_active) colliders.insert(AABB);
            else colliders.erase(AABB);

            if (is_logic_active && AABB_logic) colliders.insert(AABB_logic);
            else colliders.erase(AABB_logic);

            if (has_RB) {
                if (is_physic_active) colliders_RB.insert(AABB);
                else colliders_RB.erase(AABB);
            }
        }
        else {
            if (is_physic_active) colliders.erase(AABB);
            if (is_logic_active && AABB_logic) colliders.erase(AABB_logic);
            if (has_RB) {
                if (is_physic_active) colliders_RB.erase(AABB);
                if (is_logic_active && AABB_logic) colliders_RB.erase(AABB_logic);
            }
        }
    }
    else {
        in_frustrum = true;
        if (is_logic_active && AABB_logic) colliders.insert(AABB_logic);
        else colliders.erase(AABB_logic);
    }

    for (auto& child : children) {
        child->checkIfInFrustrum(colliders, colliders_RB, rooms);
    }
}


void InstanceManager::addChild(const ParticleGasStruct& particle)
{
    if (size < max_size) {
        particles[tail] = particle;
        updateBuffer(tail);
        tail++;
        tail %= max_size;
        size++;
        
    }
}

void InstanceManager::updateComponents(float deltaTime)
{
    for (auto& component : components) {
        component->onUpdate(deltaTime);
    }
}

void InstanceManager::removeChild() {

    
    float p = -1.f;
    glBindBuffer(GL_ARRAY_BUFFER, buffer_offset);
    glBufferSubData(GL_ARRAY_BUFFER, head * sizeof(float), sizeof(float), &p);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, head * sizeof(glm::vec4), sizeof(glm::vec4), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    head++;

    head %= max_size;
    
    size--;
}
void InstanceManager::prepareBuffer()
{

    std::vector<float> times;
    times.reserve(1000);

    for (int i = 0; i < 1000; i++) {
        times.push_back(particles[i].time);
    }

    glGenBuffers(1, &buffer_offset);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_offset);
    glBufferData(GL_ARRAY_BUFFER, times.size() * sizeof(float), times.data() , GL_STATIC_DRAW);

    for (unsigned int i = 0; i < pModel->meshes.size(); i++)
    {
        unsigned int VAO = pModel->meshes[i].VAO;
        glBindVertexArray(VAO);

        GLsizei float_size = sizeof(float);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, float_size, (void*)0);

        glVertexAttribDivisor(7, 1);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, max_size * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
    for (unsigned int i = 0; i < pModel->meshes.size(); i++)
    {


        unsigned int VAO = pModel->meshes[i].VAO;
        glBindVertexArray(VAO);

        // Atrybuty wierzchołków
        GLsizei vec4_size = sizeof(glm::vec4);
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, vec4_size, (void*)(0));

        glVertexAttribDivisor(8, 1);
        
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstanceManager::updateBuffer(int i) {

    glBindBuffer(GL_ARRAY_BUFFER, buffer_offset);
    glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(float), sizeof(float), &particles[i].time);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(glm::vec4), sizeof(glm::vec4), &particles[i].position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

PrefabInstance::PrefabInstance(std::shared_ptr<Prefab> prefab, SceneGraph* _scene_graph, std::string name)
    : Node(prefab->prefab_scene_graph->root->name + name) {
    this->prefab = prefab;
    AABB = new BoundingBox(transform.getModelMatrix(), this);

	GameManager::instance().constructors++;

    scene_graph = _scene_graph;

    prefab->prefab_instances.push_back(this);

    prefab_root = prefab->clone(this->name, scene_graph);
    prefab_root->parent = this;
    //set_prefab_colliders(prefab_root);
    set_prefab_colliders(prefab_root);
    updateSelfAndChild(true);

    if (!scene_graph->is_editing) prefab_root->createComponents();

    cout << "PrefabInstance constructor called for: " << name << endl;

    //set_prefab_colliders(prefab_root);
}



PrefabInstance::PrefabInstance(std::shared_ptr<Prefab> prefab, SceneGraph* _scene_graph, std::string name, glm::vec3 position)
    : Node(prefab->prefab_scene_graph->root->name + name) {

    this->prefab = prefab;
    AABB = new BoundingBox(transform.getModelMatrix(), this);

    scene_graph = _scene_graph;

    GameManager::instance().constructors++;

    prefab->prefab_instances.push_back(this);

    prefab_root = prefab->clone(this->name, scene_graph);
    prefab_root->parent = this;
    set_prefab_colliders(prefab_root);
    transform.setLocalPosition(position);
    
    updateSelfAndChild(true);

    if (!scene_graph->is_editing)prefab_root->createComponents();

	cout << "PrefabInstance constructor called for: " << name << endl;
}

PrefabInstance::~PrefabInstance()
{

    cout << "PrefabInstance destructor called for: " << name << endl;

    GameManager::instance().destructors++;
    if (scene_graph && !scene_graph->is_editing) {
        endComponents();
    }
    if (prefab_root)
        delete prefab_root;
    prefab.reset();
    
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
    if (prefab_root == nullptr)
        delete prefab_root;
    if (AABB == nullptr)
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
		forceUpdateSelfAndChild();
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
	if (prefab_root) { // ewentualnie dodać sprawdzanie frustum tutaj
		prefab_root->createComponents();
	}
}
void PrefabInstance::endComponents()
{
	if (prefab_root) {
		prefab_root->endComponents();
	}
}
void PrefabInstance::forceUpdateSelfAndChild() { 

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
    
    ResourceManager::Instance().shader_outline->use();
    glm::vec3 dynamic_color = glm::vec3(0.f, 0.6f, 0.6f);
    ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
    AABB->draw(*ResourceManager::Instance().shader_outline);
}

void PrefabInstance::drawShadows(Shader& shader)
{
	if (in_frustrum) {
		prefab_root->drawShadows(shader);
	}
}

void PrefabInstance::checkIfInFrustrum(std::unordered_set<Collider*>& colliders, std::unordered_set<Collider*>& colliders_RB, std::unordered_set<Node*>& rooms)
{
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);

        

        if (in_frustrum && (rooms.find(this) == rooms.end())) {
            for (auto& child : prefab_root->getAllChildren()) {
                child->in_frustrum = true;
                if (child->AABB && child->is_physic_active) {
                    if (child->has_RB) colliders_RB.insert(child->AABB);
                    colliders.insert(child->AABB);
                }
                else {
					if (child->has_RB) colliders_RB.erase(child->AABB);
					colliders.erase(child->AABB);
                }

                if (child->AABB_logic && child->is_logic_active) {
                    colliders.insert(child->AABB_logic);
				}
                else {
                    colliders.erase(child->AABB_logic);
                }

                MirrorNode* m = dynamic_cast<MirrorNode*>(child);
                if (m) {
                    if (m->mirrorCollider) {
                        colliders.insert(m->mirrorCollider);
                    }
                    else {
                        colliders.erase(m->mirrorCollider);
                    }
                }

            }
            rooms.insert(this);
        }
        else if (!in_frustrum && (rooms.find(this) != rooms.end())) {
            for (auto& child : prefab_root->getAllChildren()) {
                child->in_frustrum = false;
                if (child->AABB && child->is_physic_active) {
                    if (child->has_RB) colliders_RB.erase(child->AABB);
                    colliders.erase(child->AABB);
                }
                if (child->AABB_logic && child->is_logic_active) {
                    colliders.erase(child->AABB_logic);
                }
				MirrorNode* m = dynamic_cast<MirrorNode*>(child);
                if (m) {
                    if (m->mirrorCollider) {
						colliders.erase(m->mirrorCollider);
                    }
                }
            }
			rooms.erase(this);
        }

        if (in_frustrum) {
            prefab_root->addRenderQueueAndChild();
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
	cout << "Light constructor called for: " << nameOfNode << endl;
}

DirectionalLight::DirectionalLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, _is_shining, ambient, diffuse, specular), direction(direction) {
    updateMatrix();
    GameManager::instance().constructors++;
	cout << "DirectionalLight constructor called for: " << nameOfNode << endl;
}

DirectionalLight::~DirectionalLight()
{
	cout << "DirectionalLight destructor called for: " << name << endl;
    GameManager::instance().destructors++;
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

void DirectionalLight::updateSelfAndChild(bool controlDirty)
{
    glm::mat4 model = glm::translate(glm::mat4(1.f), camera->cameraPos);
    if (parent) {
        transform.computeModelMatrix(model * parent->transform.getModelMatrix());
    }
    else {
        transform.computeModelMatrix(model);
    }
    if (AABB != nullptr) {
        AABB->transformAABB(transform.getModelMatrix());
    }
    if (AABB_logic != nullptr) {
        AABB_logic->transformAABB(transform.getModelMatrix());
    }
}

PointLight::PointLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, bool _is_alarm, float quadratic, float linear, float constant, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    : Light(model, nameOfNode, _is_shining, ambient, diffuse, specular), quadratic(quadratic), linear(linear), constant(constant), is_alarm(_is_alarm) {

	cout << "PointLight constructor called for: " << nameOfNode << endl;

    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    lastlightPos = glm::vec3(FLT_MAX);

    GameManager::instance().constructors++;
}

PointLight::~PointLight()
{
	cout << "PointLight destructor called for: " << name << endl;
    GameManager::instance().destructors++;
    scene_graph->point_lights.remove(this);
	scene_graph->point_light_number--;
    glDeleteTextures(1, &depthMap);
    glDeleteTextures(1, &depthCubemap);
}

void PointLight::render(unsigned int depthMapFBO, Shader& shader, int index)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    

    glClear(GL_DEPTH_BUFFER_BIT);

    shader.use();
    updateMatrix();
    uploadMatrices(shader);

	glm::vec3 lightPos = transform.getGlobalPosition();
    shader.setVec3("lightPos", lightPos);
    shader.setFloat("far_plane", 20.f);

}

void PointLight::uploadMatrices(Shader& shader) const
{
    for (int i = 0; i < 6; ++i)
        shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
}

void PointLight::updateMatrix()
{
    
    glm::vec3 lightPos = transform.getGlobalPosition();
    
    if (lightPos == lastlightPos) [[likely]] return;
    lastlightPos = lightPos;

    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 20.f);
    shadowTransforms.clear();
    shadowTransforms.reserve(6);
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
    shadowTransforms.emplace_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));
}

Prefab::Prefab(std::string name, PrefabType prefab_type) {

    GameManager::instance().constructors++;
    this->prefab_scene_graph = new SceneGraph();
    this->prefab_scene_graph->root->name = name;
    this->prefab_type = prefab_type;
    prefab_scene_graph->directional_lights.push_back(new DirectionalLight(nullptr, "editor_light", true));
    prefab_scene_graph->directional_light_number++;

}

Prefab::~Prefab()
{
	cout << "Prefab destructor called for: " << prefab_scene_graph->root->name << endl;
    if (prefab_scene_graph)
        delete prefab_scene_graph;
    prefab_scene_graph = nullptr;
}

Node* Prefab::clone(std::string instance_name, SceneGraph* scene_graph, bool light_copy) {

	Node* copy = prefab_scene_graph->root->clone(instance_name, scene_graph);
    copy->setVariablesNodes(instance_name, copy, scene_graph);
    for (auto& light : prefab_scene_graph->point_lights) {
		PointLight* new_light = new PointLight(light->pModel, light->name + "_" + instance_name, light->is_shining, light->is_alarm, light->quadratic, light->linear, light->constant,
            light->ambient, light->diffuse, light->specular);
        new_light->from_prefab = !light_copy;
        new_light->parent = copy;
		copy->addChild(new_light);
		new_light->scene_graph = scene_graph;
		new_light->transform.setLocalPosition(light->transform.getGlobalPosition());
		new_light->transform.computeModelMatrix();
		new_light->is_physic_active = light->is_physic_active;
		new_light->is_logic_active = light->is_logic_active;
        new_light->is_alarm = light->is_alarm;
        scene_graph->point_lights.push_back(new_light);
        scene_graph->point_light_number++;
    }

    return copy;

}

void Prefab::notifyInstances()
{
    for (auto it = prefab_instances.begin(); it != prefab_instances.end();) {
        if (*it) {
			(*it)->updateSelf();
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
    particle.velocity = glm::vec3(0.0f, 0.5f, 0.0f);
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

MirrorNode::MirrorNode(std::shared_ptr<Model> model, std::string nameOfNode) : Node(model, nameOfNode, 0, {-1.f, 0.f, -1.f}, {1.f, 0.f, 1.f}) {
   mirrorCollider = new RectOBB(transform.getModelMatrix(), this);
   GameManager::instance().constructors++;
}

MirrorNode::~MirrorNode()
{
	cout << "MirrorNode destructor called for " << name << endl;
	GameManager::instance().destructors++;
	if (mirrorCollider) {
		PhysicsSystem::instance().colliders.erase(mirrorCollider);
		PhysicsSystem::instance().colliders_RigidBody.erase(mirrorCollider);
		delete mirrorCollider;
		mirrorCollider = nullptr;
	}
}

glm::vec3 MirrorNode::reflectDirection(Ray ray)
{
    glm::vec3 reflected = ray.direction - 2.0f * glm::dot(ray.direction, mirrorCollider->normalGlobal) * mirrorCollider->normalGlobal;
    return reflected;
}

void MirrorNode::forceUpdateSelfAndChild()
{
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
    if (mirrorCollider) {
        mirrorCollider->transform(transform.getModelMatrix());
    }
}

void MirrorNode::drawSelfAndChild()
{
    if (pModel && is_visible && in_frustrum) {

        ResourceManager::Instance().shader->use();
        ResourceManager::Instance().shader->setMat4("model", transform.getModelMatrix());
        ResourceManager::Instance().shader_tile->setMat4("model", transform.getModelMatrix());
        

        ResourceManager::Instance().shader->setInt("is_animating", 0);
        
        if (is_marked) {
            glStencilMask(0xFF);
        }
        
        ResourceManager::Instance().shader->setInt("is_light", 0);
        
        pModel->Draw(*ResourceManager::Instance().shader);

        glStencilMask(0x00);
    }

    if (AABB || AABB_logic || mirrorCollider) {
        ResourceManager::Instance().shader_outline->use();

        ResourceManager::Instance().shader_outline->setInt("is_animating", 0);

        glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);

        if (scene_graph->is_editing && AABB) {
            AABB->draw(*ResourceManager::Instance().shader_outline);
        }
        dynamic_color = glm::vec3(0.f, 0.8f, 0.f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);
        if (scene_graph->is_editing && AABB_logic) {
            AABB_logic->draw(*ResourceManager::Instance().shader_outline);
        }

        dynamic_color = glm::vec3(0.6f, 0.6f, 0.f);
        ResourceManager::Instance().shader_outline->setVec3("color", dynamic_color);

        if (scene_graph->is_editing && mirrorCollider) {
            mirrorCollider->draw(*ResourceManager::Instance().shader_outline);
        }
    }
}

void MirrorNode::checkIfInFrustrum(std::unordered_set<Collider*>& colliders, std::unordered_set<Collider*>& colliders_RB,
    std::unordered_set<Node*>& rooms) {
    if (AABB) {
        in_frustrum = camera->isInFrustrum(AABB);

        addRenderQueue();
        //in_frustrum = true;
        if (in_frustrum) {
            if (mirrorCollider) {
                colliders.insert(mirrorCollider);
            }

            if (is_physic_active) colliders.insert(AABB);
            else colliders.erase(AABB);

            if (is_logic_active && AABB_logic) colliders.insert(AABB_logic);
            else colliders.erase(AABB_logic);

            if (has_RB) {
                if (is_physic_active) colliders_RB.insert(AABB);
                else colliders_RB.erase(AABB);
            }

            
        }
        else {
            if (is_physic_active) colliders.erase(AABB);
            if (is_logic_active && AABB_logic) colliders.erase(AABB_logic);
            if (mirrorCollider) colliders.erase(mirrorCollider);
            if (has_RB) {
                if (is_physic_active) colliders_RB.erase(AABB);   
            }
        }
    }
    else {
        in_frustrum = true;
    }

}
