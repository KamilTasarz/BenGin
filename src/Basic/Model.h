#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/importerdesc.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


using namespace std;

unsigned int textureFromFile(const char* path, const string& directory, bool gamma = false);
unsigned int textureFromFile(const char* full_path, bool gamma = false);

struct BoneInfo
{
    // id wykorzystywane w shaderze do okreslenia z macierzy kosci, ktora to jest macierz
    int id;

    // offset przenoszacy kosc z originu
    glm::mat4 offset;

};

class Animation;

class Model
{

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const& path);

    void findAABB(aiNode* node, const aiScene* scene);

    void findAABBMesh(aiMesh* mesh, const aiScene* scene);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
    

    void setVertexBoneData(Vertex& vertex, int id, float weight);

    void normalizeBoneWeights(Vertex& vertex);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    

    void loadCube(std::vector<shared_ptr<Texture>>&& textures);

    void loadPlane(std::vector<shared_ptr<Texture>>&& textures);

    void loadAnimations();

public:
    int id = 0;
    // model data 
    vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    string exact_path;
    string mode = "";

    float tile_scale = 1.f;

    bool gammaCorrection, has_animations = false, move_origin;
    std::vector<Animation*> animations;

    std::map<string, BoneInfo> m_BoneInfoMap;  // info o kosciach modelu
    int m_BoneCounter = 0;

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

    glm::vec3 min_points = glm::vec3(FLT_MAX);
    glm::vec3 max_points = glm::vec3(-FLT_MAX);

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, int id, bool move_origin = false, bool gamma = false);

    Model(std::vector<shared_ptr<Texture>>&& textures, int id, string mode = "cube", float scale_factor = 1.f);

    ~Model();

    Animation* getAnimationByName(std::string name);



    void DrawInstanced(Shader& shader, int num) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].DrawInstanced(shader, num);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    void Draw(Shader& shader, const std::vector<unsigned int>& texture);
};

inline unsigned int textureFromFile(const char* full_path, bool gamma) {

    string filename = string(full_path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {

        GLenum internalFormat;
        GLenum format;

        if (nrComponents == 1) {
            internalFormat = format = GL_RED;
        }
        else if (nrComponents == 2) {
            internalFormat = GL_RG8;     
            format = GL_RG;
        }
        else if (nrComponents == 3) {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << full_path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

inline unsigned int textureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    return textureFromFile(filename.c_str(), gamma);
}

