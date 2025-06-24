#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"

#define MAX_BONE_INFLUENCE 4 // How many bones can influence one vertex

using namespace std;

struct Vertex {

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];

};

struct Texture {

    unsigned int id;
    string type;
    string path;

};

class Mesh final {

private:

    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh() noexcept {
  
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (is_EBO) glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        if (is_EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }

public:

    // mesh Data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<shared_ptr<Texture>> textures;
    unsigned int VAO;
    bool is_EBO;

    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<shared_ptr<Texture>>&& textures)
        : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)), is_EBO(true)
    {
        setupMesh();
    }

    Mesh(vector<Vertex>&& vertices, vector<shared_ptr<Texture>>&& textures)
        : vertices(std::move(vertices)), textures(std::move(textures)), is_EBO(false)
    {
       
        setupMesh();
    }

    // render the mesh
    void Draw(Shader& shader) {

        bool setFallback = textures.size() < 2;

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i]->type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
            }

            std::string uniformName = name + number;
            glUniform1i(shader.getUniformLocation(uniformName), i);
            glBindTexture(GL_TEXTURE_2D, textures[i]->id);

            if (setFallback && name == "texture_diffuse") {
                glUniform1i(shader.getUniformLocation("texture_specular1"), i);
                glBindTexture(GL_TEXTURE_2D, textures[i]->id);
            }
        }

        // draw mesh
        glBindVertexArray(VAO);
        if (is_EBO) {
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
        }
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);

    }

    void Draw(Shader& shader, const std::vector<shared_ptr<Texture>>& texture) const
    {
        bool setFallback = texture.size() < 2;

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < texture.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = texture[i]->type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
            }

            std::string uniformName = name + number;
            glUniform1i(shader.getUniformLocation(uniformName), i);
            glBindTexture(GL_TEXTURE_2D, texture[i]->id);

            if (setFallback && name == "texture_diffuse") {
                glUniform1i(shader.getUniformLocation("texture_specular1"), i);
                glBindTexture(GL_TEXTURE_2D, texture[i]->id);
            }
        }

        // draw mesh
        glBindVertexArray(VAO);
        if (is_EBO) {
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
        }
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);

    }

    void DrawInstanced(Shader& shader, int num)
    {
        bool setSpecularFallback = textures.size() < 2;

        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i]->type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
            }

            std::string uniformName = name + number;
            glUniform1i(shader.getUniformLocation(uniformName), i);
            glBindTexture(GL_TEXTURE_2D, textures[i]->id);

            if (setSpecularFallback && name == "texture_diffuse") {
                glUniform1i(shader.getUniformLocation("texture_specular1"), i);
                glBindTexture(GL_TEXTURE_2D, textures[i]->id);
            }
        }

        // draw mesh
        glBindVertexArray(VAO);
        if (is_EBO) {
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, (void*)0, num);
        }
        else {
            glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()), num);
        }
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);

    }

};
