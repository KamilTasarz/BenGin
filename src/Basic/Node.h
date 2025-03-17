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

    // ----------- CONSTRUCTORS -----------

    // No model
    Node(std::string nameOfNode, int _id = 0) {
        pModel = nullptr;
        name = nameOfNode;
        id = _id;
    }

    // Model
    Node(Model& model, std::string nameOfNode, int _id = 0) : pModel{ &model } {
        name = nameOfNode;
        id = _id;
    }

    // DESTRUCTOR

    ~Node() {
        for (auto& child : children) {
            delete child;
        }
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

    // Forcing an update of self and children even if there were no changes
    void forceUpdateSelfAndChild() {
        if (parent) {
            transform.computeModelMatrix(parent->transform.getModelMatrix());
        }
        else {
            transform.computeModelMatrix();
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
    void drawSelfAndChild(Shader& _shader, unsigned int& display, unsigned int& total) {

        if (pModel) {
            //_shader.setVec4("dynamicColor", color);
            _shader.setMat4("model", transform.getModelMatrix());
            pModel->Draw(_shader);
            display++;
        }

        total++;

        for (auto&& child : children) {
            child->drawSelfAndChild(_shader, display, total);
        }

        //_shader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    }

};

#endif // !NODE_H
