#pragma once

#ifndef NODE_H
#define NODE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <list>
#include <array>
#include <memory>

#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "Model.h"
#include "Animator.h"

#include <set>
#include "../Grid.h"

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

class Node {

public:

    // ----------- VARIABLES -----------

    // Unique node name
    std::string name;

    int id;

    // Color
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Node model
    shared_ptr<Model> pModel = nullptr;

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

    bool is_marked = false;

    //Hitbox
    BoundingBox *AABB;

	Animator* animator = nullptr; // Animator for the model

    // ----------- CONSTRUCTORS -----------

    // No model
    Node(std::string nameOfNode, int _id = 0) {
        pModel = nullptr;
        name = nameOfNode;
        id = _id;
        AABB = nullptr;
        no_textures = true;
    }

    // Model
    Node(shared_ptr<Model> model, std::string nameOfNode, std::vector<BoundingBox*>& vector_of_colliders, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f)) : pModel{ model } {
        name = nameOfNode;
        id = _id;
        no_textures = false;

        if (model && model->mode._Equal("plane")) {
			max_point = glm::vec3(0.5f, 0.0f, 0.5f);
			min_point = glm::vec3(-0.5f, 0.0f, -0.5f);

        }

        if (model && model->min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), model->min_points, model->max_points);
        else AABB = new BoundingBox(transform.getModelMatrix(), min_point, max_point);

        //this->no_textures = no_textures;
        vector_of_colliders.push_back(AABB);
    }

    Node(shared_ptr<Model> model, std::string nameOfNode, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f)) : pModel{ model } {
        name = nameOfNode;
        id = _id;
        no_textures = false;
        
        if (model && model->mode._Equal("plane")) {
            max_point = glm::vec3(0.5f, 0.0f, 0.5f);
            min_point = glm::vec3(-0.5f, 0.0f, -0.5f);

        }

        if (model && model->min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), model->min_points, model->max_points);
        else AABB = new BoundingBox(transform.getModelMatrix(), min_point, max_point);

        //this->no_textures = no_textures;
    }

    // DESTRUCTOR

    ~Node() {
        for (auto& child : children) {
            delete child;
        }
        delete AABB;
    }

    // Change color
    void changeColor(glm::vec4 newColor) {
        color = newColor;
    }

    // Add child
    void virtual addChild(Node* p);

    void increaseCount();

    // Get child by its name
    Node* getChildByName(const std::string& name);

    Node* getChildById(int id);

    std::set<Node*> getAllChildren() {
        std::set<Node*> result;
        collectAllChildren(result);
        return result;
    }

    void collectAllChildren(std::set<Node*>& out) {
        for (Node* child : children) {
            if (out.insert(child).second) { // If it's the first occurence of the child
                child->collectAllChildren(out);
            }
        }
    }

    void mark(Ray rayWorld, float& marked_depth);

    // Forcing an update of self and children even if there were no changes
    void forceUpdateSelfAndChild();

    // This will update if there were changes only (checks the dirty flag)
    void virtual updateSelfAndChild(bool controlDirty);

    // Draw self and children
    void virtual drawSelfAndChild();

	// Draw self and children for prefabs -> computing model matrix
    void virtual drawSelfAndChild(Transform &parent);

    void updateAnimation(float deltaTime);

    void drawShadows(Shader& shader);

    std::string getName() {
        return name;
    }

    void rename(std::string new_name) {
        this->name = new_name;
    }

    void separate(const BoundingBox* other_AABB);

    const Transform& getTransform();

};

class InstanceManager : public Node {
public:
    int size = 0, current_min_id = 0;
    int max_size = 1000;
    unsigned int buffer;

    InstanceManager(shared_ptr<Model> model, std::string nameOfNode, int id = 0, int max_size = 1000) : Node(nameOfNode, id), max_size(max_size) {
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

    // Debug
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

    Light(shared_ptr<Model> model, std::string nameOfNode, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f)) : Node(model, nameOfNode), ambient(ambient), diffuse(diffuse), specular(specular) {
        
        is_shining = true;
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

    glm::vec3 getAmbient() {
        return ambient;
    }

    glm::vec3 getDiffuse() {
        return diffuse;
    }

    glm::vec3 getSpecular() {
        return specular;
    }

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

    DirectionalLight(shared_ptr<Model> model, std::string nameOfNode, glm::vec3 direction = glm::vec3(1.f, -1.f, 1.f), glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f))
        : Light(model, nameOfNode, ambient, diffuse, specular), direction(direction) {
        updateMatrix();
    }

    glm::vec3 getDirection() {
        return direction;
    }

    void render(unsigned int depthMapFBO, Shader& shader) {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shader.use();
        updateMatrix();
        shader.setMat4("view_projection", view_projection);

    }

    void updateMatrix() {
        view = glm::lookAt(transform.getGlobalPosition(), transform.getGlobalPosition() + direction, glm::vec3(0.f, 1.f, 0.f));
        float size = 15.f;
        projection = glm::ortho(-size, size, -size, size, 1.5f, 25.f);
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

    PointLight(shared_ptr<Model> model, std::string nameOfNode, float quadratic, float linear, float constant = 1.f, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f))
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

    void render(unsigned int depthMapFBO, Shader& shader) {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shader.use();
        updateMatrix();
        shader.setMat4("view_projection", view_projection);

    }

    void renderBack(unsigned int depthMapFBO, Shader& shader) {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapBack, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shader.use();
        updateMatrix();
        shader.setMat4("view_projection", view_projection_back);

    }

    void updateMatrix() {
        view = glm::lookAt(transform.getGlobalPosition() + glm::vec3(0.0f, 0.5f, 0.0f), transform.getGlobalPosition() + glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.f, 0.f, 1.f));
        view_back = glm::lookAt(transform.getGlobalPosition() + glm::vec3(0.0f, -0.5f, 0.0f), transform.getGlobalPosition() + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.f, 0.f, -1.f));

        projection = glm::perspective(glm::radians(170.f), 1.f, 0.5f, 40.f);
        view_projection = projection * view;
        view_projection_back = projection * view_back;
    }

    glm::mat4& getMatrix() {
        updateMatrix();
        return view_projection;
    }
};

class PrefabInstance;

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

    

    SceneGraph() {

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

    ~SceneGraph() {


    }

    void unmark();
    void addChild(Node* p);
    void addChild(Node* p, std::string name);
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

class Prefab {

public:
    //shared_ptr<SceneGraph> prefab_scene_graph;
    SceneGraph* prefab_scene_graph;
	PrefabType prefab_type;

	Prefab(std::string name = "Prefab", PrefabType prefab_type = HORIZONTAL_RIGHT) {
		//this->prefab_scene_graph = make_shared<SceneGraph>();
		this->prefab_scene_graph = new SceneGraph();
		this->prefab_scene_graph->root->name = name;
		this->prefab_type = prefab_type;
        prefab_scene_graph->directional_lights.push_back(new DirectionalLight(nullptr, "editor_light"));
        prefab_scene_graph->directional_light_number++;
        //prefab_scene_graph->addDirectionalLight();
	}


};

class PrefabInstance : public Node {
public:
    shared_ptr<Prefab> prefab;
	std::vector<BoundingBox*> prefab_colliders;

    PrefabInstance(shared_ptr<Prefab> prefab) : Node(prefab->prefab_scene_graph->root->name + "_inst") {
        this->prefab = prefab;
        //set_prefab_colliders(prefab->prefab_scene_graph->root);
        
    }

    /*void set_prefab_colliders(Node* node) {
        if (node->AABB) {
			BoundingBox* new_collider = new BoundingBox(*node->AABB);
            new_collider->transformWithOffsetAABB(transform.getModelMatrix());
			prefab_colliders.push_back(new_collider);
        }
		for (Node* child : node->children) {
			set_prefab_colliders(child);
		}
    }

	void updateSelfAndChild(bool controlDirty) override {

		controlDirty = controlDirty || transform.isDirty();

		if (controlDirty) {
			transform.computeModelMatrix();
            for (auto& collider : prefab_colliders) {
				collider->transformWithOffsetAABB(transform.getModelMatrix());
            }
		}
		
	}*/

    void drawSelfAndChild() override;
};

#endif // !NODE_H
