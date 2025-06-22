#include "Model.h"
#include "Animation.h"
#include "Mesh.h"
#include "../ResourceManager.h"

void Model::loadModel(string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;

    /*std::cout << "Obsługiwane formaty:\n";
    for (size_t i = 0; i < importer.GetImporterCount(); ++i) {
        const aiImporterDesc* desc = importer.GetImporterInfo(i);
        std::cout << "- " << desc->mName << " (" << desc->mFileExtensions << ")\n";
    }*/

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //| aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    exact_path = path;
    if (scene->mNumAnimations > 0) {
        has_animations = true;
    }
    else {
        has_animations = false;
    }
    // process ASSIMP's root node recursively
    if (!has_animations && move_origin) {
        findAABB(scene->mRootNode, scene);
    }
    processNode(scene->mRootNode, scene);

    if (!has_animations && move_origin) {
        max_points -= min_points;
        min_points = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void Model::findAABB(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        findAABBMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        findAABB(node->mChildren[i], scene);
    }
}

void Model::findAABBMesh(aiMesh* mesh, const aiScene* scene)
{
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;


        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        min_points.x = min(min_points.x, vector.x);
        min_points.y = min(min_points.y, vector.y);
        min_points.z = min(min_points.z, vector.z);

        max_points.x = max(max_points.x, vector.x);
        max_points.y = max(max_points.y, vector.y);
        max_points.z = max(max_points.z, vector.z);
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)

{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }



    /*max_points.x = ceil(max_points.x - min_points.x) + min_points.x;
    max_points.y = ceil(max_points.y - min_points.y) + min_points.y;
    max_points.z = ceil(max_points.z - min_points.z) + min_points.z;*/
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<shared_ptr<Texture>> textures;

    glm::vec3 translate = -min_points;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }

        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        if (!has_animations && move_origin) {
            vector.x = mesh->mVertices[i].x + translate.x;
            vector.y = mesh->mVertices[i].y + translate.y;
            vector.z = mesh->mVertices[i].z + translate.z;
        }
        else {
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
        }
        vertex.Position = vector;

        if (has_animations || !move_origin) {

            min_points.x = min(min_points.x, vector.x);
            min_points.y = min(min_points.y, vector.y);
            min_points.z = min(min_points.z, vector.z);

            max_points.x = max(max_points.x, vector.x);
            max_points.y = max(max_points.y, vector.y);
            max_points.z = max(max_points.z, vector.z);
        }
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }



    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    //aiTexture* _textures =  scene->mTextures[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN



    aiString path;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        std::string texPath = path.C_Str();
        // Załaduj plik tekstury (np. przez stb_image)
    }


    // 1. diffuse maps
    std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    extractBoneWeightForVertices(vertices, mesh, scene);

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, std::move(textures));
}

void Model::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{

    // przejscie po wszystkich kosciach danego meshu i zapisanie tych, ktorych nie ma w mapie informujacej o kosciach
    for (int bone_id = 0; bone_id < mesh->mNumBones; bone_id++) {
        std::string bone_name = mesh->mBones[bone_id]->mName.C_Str();
        int ID = -1;
        if (m_BoneInfoMap.find(bone_name) == m_BoneInfoMap.end()) {
            BoneInfo bone_to_add;
            bone_to_add.id = m_BoneCounter;

            //transformacja macierzy asimpa na glm
            aiMatrix4x4 mat4 = mesh->mBones[bone_id]->mOffsetMatrix;
            bone_to_add.offset = glm::transpose(glm::mat4(glm::vec4(mat4.a1, mat4.a2, mat4.a3, mat4.a4), glm::vec4(mat4.b1, mat4.b2, mat4.b3, mat4.b4),
                glm::vec4(mat4.c1, mat4.c2, mat4.c3, mat4.c4), glm::vec4(mat4.d1, mat4.d2, mat4.d3, mat4.d4)));


            //bone_to_add.offset = glm::translate(bone_to_add.offset, translate);

            m_BoneInfoMap[bone_name] = bone_to_add;



            ID = m_BoneCounter;
            m_BoneCounter++;
        }
        else {
            //jesli juz jest w mapie to znaczy, ze pobieramy ID kosci i przypiszemy do wierzcholka tylko wage
            ID = m_BoneInfoMap[bone_name].id;
        }

        if (ID != -1) {

            //pobranie wag i ich ilosci dla kosci z danego mesha
            auto weights = mesh->mBones[bone_id]->mWeights;
            int numWeights = mesh->mBones[bone_id]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
            {

                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                if (vertexId < vertices.size()) {
                    setVertexBoneData(vertices[vertexId], ID, weight);
                }

            }
        }
    }

    for (auto& ver : vertices) normalizeBoneWeights(ver);
}

void Model::setVertexBoneData(Vertex& vertex, int id, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (vertex.m_BoneIDs[i] < 0 && weight > 0.f)
        {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = id;
            break;
        }
    }

}

void Model::normalizeBoneWeights(Vertex& vertex)
{
    float total = 0.0f;
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        total += vertex.m_Weights[i];

    if (total > 0.0f) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
            vertex.m_Weights[i] /= total;
    }
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    vector<shared_ptr<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(std::make_shared<Texture>(textures_loaded[j]));
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(std::make_shared<Texture>(texture));
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}

void Model::loadCube(std::vector<shared_ptr<Texture>>&& textures)
{

    float boxVertices[] = {

        // FRONT
        // @1
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        // @2
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        // BACK
        // @1    
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        //@2
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

        // LEFT
        // @1
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        // @2
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

        // RIGHT
        // @1   
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        // @2
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

        // TOP
        // @1
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        // @2
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,

        // BOTTOM
        // @1
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        // @2
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f

    };


    // data to fill
    vector<Vertex> vertices;
    //vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < 36; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = boxVertices[i * 8];
        vector.y = boxVertices[i * 8 + 1];
        vector.z = boxVertices[i * 8 + 2];
        vertex.Position = vector;

        // normals
        vector.x = boxVertices[i * 8 + 3];
        vector.y = boxVertices[i * 8 + 4];
        vector.z = boxVertices[i * 8 + 5];
        vertex.Normal = vector;

        // texture coordinates
        glm::vec2 vec;
        vec.x = boxVertices[i * 8 + 6];
        vec.y = boxVertices[i * 8 + 7];
        vertex.TexCoords = vec;

        vertices.push_back(vertex);
    }

    /*for (short i = 0; i < texture_number; i++) {
        Texture texture;
        texture.id = textureFromFile(texture_names[i]);
        texture.path = string(texture_names[i]);
        const string specular_name = "spec";
        if (texture.path.find(specular_name, 0) != string::npos) {
            texture.type = "texture_specular";
        }
        else {
            texture.type = "texture_diffuse";
        }
        textures.push_back(texture);
        textures_loaded.push_back(texture);
    }*/

    meshes.push_back(Mesh(vertices, std::move(textures)));
}

void Model::loadPlane(vector<shared_ptr<Texture>>&& textures)
{

    float planeVertices[] = {

        // @1
        -0.5f, -0.0f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.0f,  0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.0f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         // @2
         0.5f, -0.0f,  0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.0f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.0f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f

    };

    // data to fill
    vector<Vertex> vertices;
    //vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < 6; i++) {

        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = planeVertices[i * 8];
        vector.y = planeVertices[i * 8 + 1];
        vector.z = planeVertices[i * 8 + 2];
        vertex.Position = vector;

        // normals
        vector.x = planeVertices[i * 8 + 3];
        vector.y = planeVertices[i * 8 + 4];
        vector.z = planeVertices[i * 8 + 5];
        vertex.Normal = vector;

        // texture coordinates
        glm::vec2 vec;
        vec.x = planeVertices[i * 8 + 6];
        vec.y = planeVertices[i * 8 + 7];
        vertex.TexCoords = vec;

        vertices.push_back(vertex);

    }

    /*for (short i = 0; i < texture_number; i++) {
        Texture texture;
        texture.id = textureFromFile(texture_names[i]);
        texture.path = string(texture_names[i]);
        const string specular_name = "spec";
        if (texture.path.find(specular_name, 0) != string::npos) texture.type = "texture_specular";
        else texture.type = "texture_diffuse";
        textures.push_back(texture);
        textures_loaded.push_back(texture);
    }*/

    meshes.push_back(Mesh(vertices, std::move(textures)));
}

void Model::loadAnimations()
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(exact_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //| aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    for (int i = 0; i < scene->mNumAnimations; i++) {
        Animation* anim = new Animation(scene->mAnimations[i], *this, scene);
        animations.push_back(anim);
    }
}

Model::Model(string const& path, int id, bool move_origin, bool gamma) : gammaCorrection(gamma), id(id), move_origin(move_origin)
{
    loadModel(path);
    if (has_animations) loadAnimations();
}

Model::Model(std::vector<shared_ptr<Texture>>&& textures, int id, string mode, float scale_factor) : id(id) {
    this->mode = mode;
	this->tile_scale = scale_factor;
    if (mode == "cube") {
        loadCube(std::move(textures));
    }
    else {
        loadPlane(std::move(textures));
    }

}

Model::~Model()
{
    for (int i = 0; i < animations.size(); i++) {
        delete animations[i];
    }
    animations.clear();
}

Animation* Model::getAnimationByName(std::string name)
{
    for (auto& a : animations) {
        if (a->name == name) return a;
    }
    return nullptr;
}

void Model::Draw(Shader& shader, std::vector<unsigned int> texture)
{

    std::vector<std::shared_ptr<Texture>> textures;

    for (int i = 0; i < texture.size(); i++) {
	    textures.push_back(ResourceManager::Instance().getTexture(texture[i]));
    }
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader, textures);
}
