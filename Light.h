#pragma once

#include "src/Basic/Node.h"
#include "config.h"

const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

class Light {
public:
    Node* object;

    glm::vec3 position;
    // Colors
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::mat4 view_projection;
    glm::mat4 view_projection_back;
    glm::mat4 view;
    glm::mat4 projection;

    unsigned int depthMap = 0;

    Light(Node* node, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f)) : ambient(ambient), diffuse(diffuse), specular(specular) {
        object = node;
        if (object != nullptr) {
            position = object->transform.getGlobalPosition();
        }
        else {
            position = glm::vec3(0.f);
        }
        
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void updatePosition() {
        position = object->transform.getGlobalPosition();
    }

    void setModelPosition() {
        object->transform.setLocalPosition(position);
    }


};

class DirectionalLight : public Light {
public:
    glm::vec3 direction, origin_direction;
    

    DirectionalLight() : Light(nullptr, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f)) {
        this->origin_direction = glm::vec3(1.f, -1.f, 1.f);
        view_projection = glm::mat4(1.f);
        
    }

    DirectionalLight(Node* node, glm::vec3 direction = glm::vec3(1.f, -1.f, 1.f), glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f))
        : Light(node, ambient, diffuse, specular) {
        updateMatrix();
        origin_direction = object->transform.getForward();
        update();
    }

    void render(unsigned int depthMapFBO, Shader &shader) {
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
        view = glm::lookAt(object->transform.getGlobalPosition(), object->transform.getGlobalPosition() + direction, glm::vec3(0.f, 1.f, 0.f));
        float size = 15.f;
        projection = glm::ortho(-size, size, -size, size, 1.5f, 25.f);
        view_projection = projection * view;
    }

    glm::mat4& getMatrix() {
        updateMatrix();
        return view_projection;
    }

    void update() {

        position = object->transform.getGlobalPosition();
        direction = object->transform.getForward();

    }
};

class PointLight : public Light {
public:
    
        float quadratic;
        float linear;
        float constant;

        unsigned int depthMapBack = 0;
        glm::mat4 view_back;

        PointLight() : Light(nullptr, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f)), quadratic(0.032f), linear(0.09f), constant(1.f) {}

        PointLight(Node* node, float quadratic, float linear, float constant = 1.f, glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.8f))            
            : Light(node, ambient, diffuse, specular), quadratic(quadratic), linear(linear), constant(constant) {

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
            view = glm::lookAt(object->transform.getGlobalPosition() + glm::vec3(0.0f, 0.5f, 0.0f), object->transform.getGlobalPosition() + glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.f, 0.f, 1.f));
            view_back = glm::lookAt(object->transform.getGlobalPosition() + glm::vec3(0.0f, -0.5f, 0.0f), object->transform.getGlobalPosition() + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.f, 0.f, -1.f));

            projection = glm::perspective(glm::radians(170.f), 1.f, 0.5f, 40.f);
            view_projection = projection * view;
            view_projection_back = projection * view_back;
        }

        glm::mat4& getMatrix() {
            updateMatrix();
            return view_projection;
        }
};

class SpotLight : public PointLight {

public:

    float cut_off, outer_cut_off; // inner, outer
    glm::vec3 direction = glm::vec3(1.f, 0.f, 0.f);

    SpotLight() : PointLight(nullptr, 0.09f, 0.32f, 1.0f, glm::vec3(0.7f, 0.3f, 0.1f), glm::vec3(0.7f, 0.3f, 0.1f), glm::vec3(0.7f, 0.3f, 0.1f)),
         cut_off(glm::cos(glm::radians(12.5f))), outer_cut_off(glm::cos(glm::radians(17.5f))) {}

    SpotLight(Node* node, float quadratic, float linear, float constant, float cut_off, float outer_cut_off,
        glm::vec3 ambient = glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3 diffuse = glm::vec3(0.8f, 0.1f, 0.1f), glm::vec3 specular = glm::vec3(0.8f, 0.1f, 0.1f))
        : PointLight(node, quadratic, linear, constant, ambient, diffuse, specular), cut_off(glm::cos(glm::radians(cut_off))), outer_cut_off(glm::cos(glm::radians(outer_cut_off))) {
        updateMatrix();
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
        view = glm::lookAt(object->transform.getGlobalPosition(),
            object->transform.getGlobalPosition(),// + direction,
            glm::vec3(0.f, 1.f, 0.f));
        projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100.f);
        view_projection = projection * view;
    }

    glm::mat4& getMatrix() {
        updateMatrix();
        return view_projection;
    }

    void setCutOff(float innerAngle, float outerAngle) {
        cut_off = glm::cos(glm::radians(innerAngle));
        outer_cut_off = glm::cos(glm::radians(outerAngle));
    }

    void update() {

		position = object->transform.getGlobalPosition();
        direction = object->transform.getForward();
        
    }
};
