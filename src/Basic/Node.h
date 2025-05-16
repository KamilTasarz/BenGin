#pragma once

#ifndef NODE_H
#define NODE_H

#include "../config.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <list>
#include <array>
#include <memory>
#include <set>

#include "../System/Component.h"
//#include "../System/Tag.h"

class BoundingBox;
class Model;
class ResourceManager;
class Grid;
class Shader;
class Tag;
class Layer;
class ParticleEmitter;
struct Particle;    
struct ParticleInstanceData;


//class Component;

struct Ray {
	glm::vec3 origin;
	glm::vec4 direction;
};

class SceneGraph;

class Transform {

protected:

    // Local position
    glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f }; // Local position
    glm::quat m_quat = glm::quat(1.f, 0.f, 0.f, 0.f); // Local rotation (quat)
    glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f }; // Local scale

    // Global position in a matrix
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    // Flag for optimization (transformation is only applied when something was changed)
    bool m_isDirty = true;
    bool touched = false;

    // Local Matrix getter
    glm::mat4 getLocalModelMatrix()
    {
        // X, Y and Z rotations
        //const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        //const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        //const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

        //// Combining three rotation matrices
        //const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        /*glm::quat qx = glm::angleAxis(glm::radians(m_quat.x), glm::vec3(1, 0, 0));
        glm::quat qy = glm::angleAxis(glm::radians(m_quat.y), glm::vec3(0, 1, 0));
        glm::quat qz = glm::angleAxis(glm::radians(m_quat.z), glm::vec3(0, 0, 1));
        glm::quat rotation = qy * qx * qz;*/
		glm::mat4 rotationMatrix = glm::mat4_cast(m_quat);

        // Combining translation, rotation and scale matrices and returning them as a local model matrix
        return glm::translate(glm::mat4(1.0f), m_pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
    }

public:

    // After applying changes set "isDirty" to false for optimization
    void computeModelMatrix()
    {
        if (!touched) {
            m_modelMatrix = getLocalModelMatrix();
        }
        
        m_isDirty = false;
        touched = false;
    }

    // Calculates global model matrix
    void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        if (!touched) {
            m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        }
        else {
            m_modelMatrix = parentGlobalModelMatrix * m_modelMatrix;
        }
        
        m_isDirty = false;
        touched = false; 
        // nie wykorzystuje isDirty poniewaz isDirty moze byc 
        //ustawione przy podaniu pozycji po prostu, zaś touched tylko jak ustawiamy macierz przez setter

    }

    // Set position and set "isDirty" to true, for the program to know changes have been made
    void setLocalPosition(const glm::vec3& newPosition)
    {
        m_pos = newPosition;
        m_isDirty = true;
    }

    // Set rotation and set "isDirty" to true, for the program to know changes have been made
    void setLocalRotation(const glm::vec3& newRotation)
    {
        glm::vec3 eulerRadians = glm::radians(newRotation);
        m_quat = glm::quat(eulerRadians);
        m_isDirty = true;
    }

    void setLocalRotation(const glm::quat& newRotation)
    {
        m_quat = newRotation;
        m_isDirty = true;
    }

    // Set scale and set "isDirty" to true, for the program to know changes have been made
    void setLocalScale(const glm::vec3& newScale)
    {
        m_scale = newScale;
        m_isDirty = true;
    }

    void setModelMatrix(glm::mat4& model) {
        m_modelMatrix = model;
        touched = true;
    }

    // -- GETTERS --

    const glm::vec3& getGlobalPosition() const
    {
        return m_modelMatrix[3];
    }

    const glm::vec3& getLocalPosition() const
    {
        return m_pos;
    }

    const glm::quat& getLocalRotation() const
    {
        return m_quat;
    }

    const glm::vec3& getLocalScale() const
    {
        return m_scale;
    }

    const glm::mat4& getModelMatrix() const
    {
        return m_modelMatrix;
    }

    glm::vec3 getRight() const
    {
        return m_modelMatrix[0];
    }

    glm::vec3 getUp() const
    {
        return m_modelMatrix[1];
    }

    glm::vec3 getBackward() const
    {
        return m_modelMatrix[2];
    }

    // ??? do direction swiatla

    glm::vec3 getForward() const
    {
        return -m_modelMatrix[2];
    }

    // Check the flag - have there been any changes?
    bool isDirty() const
    {
        return m_isDirty;
    }

};

class Animator;

class Node {

public:

    // ----------- VARIABLES -----------

    // Unique node name
    std::string name;

    int id;

    // Color
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Node model
    std::shared_ptr<Model> pModel = nullptr;

    // Node transformation
    Transform transform;

	SceneGraph* scene_graph;

    // Children list
    std::set<Node*> children;

    int size = 0;

	Node* parent = nullptr; // Pointer to a parent object

    // No textures parameter
    bool no_textures;

    // Visibility
	bool is_visible = true;
	bool in_frustrum = true;    
	bool has_RB = false;    
    
    bool is_physic_active = true; //serializacja potrzbna
    bool is_logic_active = false;

    bool is_marked = false;

    //Hitbox
    BoundingBox *AABB;
    BoundingBox *AABB_logic;

	Animator* animator = nullptr; // Animator for the model

	std::list<std::unique_ptr<Component>> components;

	std::weak_ptr<Layer> layer;
	std::weak_ptr<Tag> tag;



    // ----------- CONSTRUCTORS -----------

    // No model
    Node(std::string nameOfNode, int _id = 0);

    // Model
    Node(std::shared_ptr<Model> model, std::string nameOfNode, std::vector<BoundingBox*>& vector_of_colliders, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f));

    Node(std::shared_ptr<Model> model, std::string nameOfNode, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f));

    // DESTRUCTOR

    virtual ~Node();

    // Change color
    void changeColor(glm::vec4 newColor) {
        color = newColor;
    }

    // Add child
    void virtual addChild(Node* p);

    void addComponent(std::unique_ptr<Component> component);
    void deleteComponent(std::list<std::unique_ptr<Component>>::iterator& it);

    void increaseCount();

	void setLayer(std::shared_ptr<Layer> layer) {
		this->layer = layer;
	}
	void setTag(std::shared_ptr<Tag> tag) {
		this->tag = tag;
	}
    std::string getTagName();
    std::string getLayerName();

    // Get child by its name
    Node* getChildByName(const std::string& name);

    Node* getChildById(int id);

	Node* getChildByTag(std::string tag);

    Node* clone(std::string instance_name, SceneGraph* new_scene_graph = nullptr);

    std::set<Node*> getAllChildren() {
        std::set<Node*> result;
        collectAllChildren(result);
        return result;
    }

    template <typename T>
    T* getComponent() {
        for (auto& comp : components) {
            if (T* casted = dynamic_cast<T*>(comp.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    virtual void checkIfInFrustrum(std::vector<BoundingBox*>& colliders, std::vector<BoundingBox*>& colliders_RB);
    virtual void checkIfInFrustrumLogic(std::vector<BoundingBox*>& colliders_logic, std::vector<BoundingBox*>& colliders_RB_logic);

    void collectAllChildren(std::set<Node*>& out) {
        for (Node* child : children) {
            if (out.insert(child).second) { // If it's the first occurence of the child
                child->collectAllChildren(out);
            }
        }
    }

	void setActive(bool active) {
		is_visible = active;
		is_physic_active = active;

		for (auto& child : children) {
			child->setActive(active);
		}
	}

    void mark(Ray rayWorld, float& marked_depth);

    // Forcing an update of self and children even if there were no changes
    void virtual forceUpdateSelfAndChild();

    // This will update if there were changes only (checks the dirty flag)
    void virtual updateSelfAndChild(bool controlDirty);

    // Draw self and children
    void virtual drawSelfAndChild();

	// Draw self and children for prefabs -> computing model matrix
    void virtual drawSelfAndChild(Transform &parent);

    void virtual updateComponents(float deltaTime);

    void virtual drawShadows(Shader& shader);

    std::string getName() {
        return name;
    }

    void rename(std::string new_name) {
        this->name = new_name;
    }

    //void separate(const BoundingBox* other_AABB);

    const Transform& getTransform();

};

class InstanceManager : public Node {
public:
    int size = 0, current_min_id = 0;
    int max_size = 1000;
    unsigned int buffer;

    InstanceManager(std::shared_ptr<Model> model, std::string nameOfNode, int id = 0, int max_size = 1000) : Node(nameOfNode, id), max_size(max_size) {
        pModel = model;
        AABB = nullptr;
        prepareBuffer();
    }

    void drawSelfAndChild() override;

    void updateSelfAndChild(bool controlDirty) override;

    void addChild(Node* p) override;

    Node* find(int id);

    
    void removeChild(int id);
private:
    void prepareBuffer();

    void updateBuffer(Node* p);
};

class Instance : public Node {

};



const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

class Light : public Node {
public:

    bool from_prefab = false;
    bool is_shining;

    // Colors
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::mat4 view_projection;
    glm::mat4 view_projection_back;
    glm::mat4 view;
    glm::mat4 projection;

    unsigned int depthMap = 0;

    Light(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f));

	//virtual ~Light();

    void setShining(bool flag) {
        is_shining = flag;
    }
};

class DirectionalLight : public Light {
public:
    glm::vec3 direction;


    /*DirectionalLight() : Light(nullptr, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f)) {
        this->direction = glm::vec3(1.f, -1.f, 1.f);
        view_projection = glm::mat4(1.f);
    }*/

    DirectionalLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, glm::vec3 direction = glm::vec3(1.f, -1.f, 1.f), glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f));
    ~DirectionalLight() override;

    void render(unsigned int depthMapFBO, Shader& shader);

    void updateMatrix() {
        view = glm::lookAt(transform.getGlobalPosition(), transform.getGlobalPosition() + direction, glm::vec3(0.f, 1.f, 0.f));
        float size = 60.f;
        projection = glm::ortho(-size, size, -size, size, 1.5f, 50.f);
        view_projection = projection * view;
    }

    glm::mat4& getMatrix() {
        updateMatrix();
        return view_projection;
    }
};

class PointLight : public Light {
public:

    float quadratic;
    float linear;
    float constant;

    unsigned int depthMapBack = 0;
    glm::mat4 view_back;

    //PointLight() : Light(shared_ptr<Model> model, std::string nameOfNode, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f)), quadratic(0.032f), linear(0.09f), constant(1.f) {}

    PointLight(std::shared_ptr<Model> model, std::string nameOfNode, bool _is_shining, float quadratic, float linear, float constant = 1.f, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f));
    ~PointLight() override;

    void render(unsigned int depthMapFBO, Shader& shader);

    void renderBack(unsigned int depthMapFBO, Shader& shader);

    void updateMatrix();

    glm::mat4& getMatrix() {
        updateMatrix();
        return view_projection;
    }
};


class SceneGraph {
public:
    Node* root;
    // Root has ptr on marked object
    Node* marked_object, * new_marked_object;

	Node* to_delete = nullptr;

    int size = 0, point_light_number = 0, directional_light_number = 0;
    std::list<DirectionalLight*> directional_lights;
    std::list<PointLight*> point_lights;

	Grid* grid = nullptr;

    unsigned int depthMapFBO;

	bool is_editing = true;

    

    SceneGraph();

    ~SceneGraph() {


    }

    void unmark();
    void addChild(Node* p);
    void addChild(Node* p, std::string name);
    void addChild(Node* p, Node* parent);
	void deleteChild(Node* p);
    void deletePointLight(PointLight* p);
    void deleteDirectionalLight(DirectionalLight* p);
    void addPointLight(PointLight* p);
    void addDirectionalLight(DirectionalLight* p);
    void addPointLight(PointLight* p, std::string name);
    void addDirectionalLight(DirectionalLight* p, std::string name);
    void setShaders();
    void draw(float width, float height, unsigned int framebuffer);
    void drawMarkedObject();
    void addParticleEmitter(ParticleEmitter* p);
    void update(float delta_time);
    void forcedUpdate();
    void setLights(Shader* shader);
    void mark(Ray ray);
};

enum PrefabType {
	VERTICAL_UP,
	VERTICAL_DOWN,
	HORIZONTAL_LEFT,    
	HORIZONTAL_RIGHT    
};

class PrefabInstance;

class Prefab {

public:
    //shared_ptr<SceneGraph> prefab_scene_graph;
    SceneGraph* prefab_scene_graph;
	PrefabType prefab_type;
	std::vector<PrefabInstance*> prefab_instances;

    Prefab(std::string name = "Prefab", PrefabType prefab_type = HORIZONTAL_RIGHT);

    Node* clone(std::string instance_name, SceneGraph *scene_graph, bool light_copy = false);

    void notifyInstances();
};

class PrefabInstance : public Node {
public:
    std::shared_ptr<Prefab> prefab;
	Node* prefab_root;



    PrefabInstance(std::shared_ptr<Prefab> prefab, std::vector<BoundingBox*>& colliders, SceneGraph* _scene_graph);

	~PrefabInstance() {
		delete prefab_root;
	}

    void set_prefab_colliders(Node* node);

    void updateSelf();

    void updateSelfAndChild(bool controlDirty) override;

	void updateComponents(float deltaTime) override;

	void forceUpdateSelfAndChild() override;

    void drawSelfAndChild() override;

	void drawShadows(Shader& shader) override;

    void checkIfInFrustrum(std::vector<BoundingBox*>& colliders, std::vector<BoundingBox*>& colliders_RB) override;
};

struct Texture;

class ParticleEmitter : public Node {

public:

    unsigned int VAO;

    unsigned int quadVBO, instanceVBO;

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> particle_instances_data;
    unsigned int particle_number;
    unsigned int last_used_particle;

	unsigned int texture_id;
    Shader* shader;

    // Constructor
    ParticleEmitter(unsigned int _texture, unsigned int _particle_number);

    void init();

    void update(float dt, Node* node, unsigned int new_particles, glm::vec3 offset = glm::vec3({ 0.f }));
    void draw(const glm::mat4& view, const glm::mat4& projection);

    void updateInstanceBuffer();

    void respawnParticle(Particle& particle, Node* node, glm::vec3 offset = glm::vec3({ 0.f }));

    unsigned int firstUnusedParticle();

};

#endif // !NODE_H
