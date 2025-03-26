#pragma once

#ifndef NODE_H
#define NODE_H

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <list>
#include <array>
#include <memory>

#include <../Camera.h>
#include <../BoundingBox.h>
#include <../src/Basic/Model.h>

class Transform {

protected:

    // Local position
    glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f }; // Local position
    glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; // Local rotation (degrees)
    glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f }; // Local scale

    // Global position in a matrix
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    // Flag for optimization (transformation is only applied when something was changed)
    bool m_isDirty = true;

    // Local Matrix getter
    glm::mat4 getLocalModelMatrix()
    {
        // X, Y and Z rotations
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

        // Combining three rotation matrices
        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        // Combining translation, rotation and scale matrices and returning them as a local model matrix
        return glm::translate(glm::mat4(1.0f), m_pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
    }

public:

    // After applying changes set "isDirty" to false for optimization
    void computeModelMatrix()
    {
        m_modelMatrix = getLocalModelMatrix();
        m_isDirty = false;
    }

    // Calculates global model matrix
    void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        m_isDirty = false;
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
        m_eulerRot = newRotation;
        m_isDirty = true;
    }

    // Set scale and set "isDirty" to true, for the program to know changes have been made
    void setLocalScale(const glm::vec3& newScale)
    {
        m_scale = newScale;
        m_isDirty = true;
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

    const glm::vec3& getLocalRotation() const
    {
        return m_eulerRot;
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
    Model* pModel = nullptr;

    // Node transformation
    Transform transform;

    // Children list
    std::list<Node*> children;

    // Pointer to a parent object
    Node* parent = nullptr;

    // Children size
    int size = 0;

    // Root has ptr on marked object
    Node *marked_object, *new_marked_object;
    bool is_marked = false;

    // No textures parameter
    bool no_textures;

    //Hitbox
    BoundingBox *AABB;

    // ----------- CONSTRUCTORS -----------

    // No model
    Node(std::string nameOfNode, int _id = 0) {
        pModel = nullptr;
        name = nameOfNode;
        id = _id;
        AABB = nullptr;
        marked_object = nullptr;
        new_marked_object = nullptr;
        no_textures = true;
    }

    // Model
    Node(Model& model, std::string nameOfNode, std::vector<BoundingBox*>& vector_of_colliders, bool no_textures = false, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f)) : pModel{ &model } {
        name = nameOfNode;
        id = _id;

        if (model.min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), model.min_points, model.max_points);
        else AABB = new BoundingBox(transform.getModelMatrix(), min_point, max_point);

        marked_object = nullptr;
        new_marked_object = nullptr;
        this->no_textures = no_textures;
        vector_of_colliders.push_back(AABB);
    }

    Node(Model& model, std::string nameOfNode, bool no_textures = false, int _id = 0, glm::vec3 min_point = glm::vec3(-0.5f), glm::vec3 max_point = glm::vec3(0.5f)) : pModel{ &model } {
        name = nameOfNode;
        id = _id;

        if (model.min_points.x != FLT_MAX) AABB = new BoundingBox(transform.getModelMatrix(), model.min_points, model.max_points);
        else AABB = new BoundingBox(transform.getModelMatrix(), min_point, max_point);

        marked_object = nullptr;
        new_marked_object = nullptr;
        this->no_textures = no_textures;
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
    //template<typename... TArgs>
    void addChild(Node* p) {
        children.emplace_back(p);
        children.back()->parent = this; // Set this as the created child's parent
        increaseCount();
    }

    void increaseCount() {
        if (parent != nullptr) parent->increaseCount();
        else size++;
    }

    // Get child by its name
    Node* getChildByName(const std::string& name) {
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

    void unmark() {
        marked_object->is_marked = false;
        marked_object = nullptr; //marked object returns as nullptr after draw
    }

    void mark(glm::vec4 rayWorld, Node* &marked_object, float& marked_depth, glm::vec3& cameraPos) {
        
        for (auto&& child : children) {
            float t;
            
            if (child->AABB->isRayIntersects(rayWorld, cameraPos, t)) {
               
                if (t < marked_depth) {
                    
                    marked_object = child;
                    marked_depth = t;
                }
            }

            child->mark(rayWorld, marked_object, marked_depth, cameraPos);
        }
        /*if (marked_object != nullptr) {
            marked_object->is_marked = false;
        }*/
    }

    // Forcing an update of self and children even if there were no changes
    void forceUpdateSelfAndChild() {
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
    void updateSelfAndChild(bool controlDirty) {

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
    void drawSelfAndChild(Shader& _shader, Shader& _shader_outline, unsigned int& display, unsigned int& total) {

        if (pModel) {
            //_shader.setVec4("dynamicColor", color);
            _shader.use();
            _shader.setMat4("model", transform.getModelMatrix());
            if (is_marked) {
                glStencilMask(0xFF);
            }
            if (no_textures) {
                _shader.setInt("is_light", 1);
            }
            pModel->Draw(_shader);
            glStencilMask(0x00);
            
            _shader.setInt("is_light", 0);
            
            _shader_outline.use();

            glm::vec3 dynamic_color = glm::vec3(0.f, 0.f, 0.8f);
            _shader_outline.setVec3("color", dynamic_color);

            AABB->draw(_shader_outline);

            display++;
        }

        total++;

        for (auto&& child : children) {
            child->drawSelfAndChild(_shader, _shader_outline, display, total);
        }

        //_shader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    }

    void drawShadows(Shader& shader) {
        if (pModel) {
            
            shader.use();
            shader.setMat4("model", transform.getModelMatrix());
            if (!no_textures) {
                pModel->Draw(shader); //jak nie ma tekstury to najpewniej swiatlo -> przyjmuje takie zalozenie
            } 
        }

        for (auto&& child : children) {
            child->drawShadows(shader);
        }
    }

    void drawMarkedObject(Shader& _shader_outline) {
        if (marked_object != nullptr) {

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            glm::vec3 scale_matrix = marked_object->transform.getLocalScale();
            scale_matrix += glm::vec3(0.01f);
            Transform transform = marked_object->transform;
            transform.setLocalScale(scale_matrix);
            transform.computeModelMatrix();
            _shader_outline.setMat4("model", transform.getModelMatrix());
            
            glm::vec3 dynamic_color = glm::vec3(0.4f, 0.f, 0.f);

            _shader_outline.setVec3("color", dynamic_color);
            marked_object->pModel->Draw(_shader_outline);

            //unmark();
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
    }

    void separate(const BoundingBox* other_AABB) {

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
        }
        else if (abs(v.y) <= abs(v.x) && abs(v.y) <= abs(v.z)) {
            v.x = 0.f;
            v.z = 0.f;
        }
        else {
            v.x = 0.f;
            v.y = 0.f;
        }

        transform.setLocalPosition(transform.getLocalPosition() + v);
    
    }

};

class InstanceManager : public Node {
public:
    int size = 0;

    InstanceManager(Model& model, std::string nameOfNode, int id = 0) : Node(nameOfNode, id) {
        pModel = &model;
    }

    void drawSelfAndChild(Shader& _shader, Shader& _shader_outline, unsigned int& display, unsigned int& total) {
        //pModel->Draw(_shader, size);
    }
};

class Instance : public Node {

};

#endif // !NODE_H
