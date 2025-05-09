#pragma once

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "../config.h"

class SceneGraph;
class Prefab;
class Node;
class BoundingBox;
class Model;

using json = nlohmann::json;

void loadPrefabs(std::vector<std::shared_ptr<Prefab>>& prefabs);
std::shared_ptr<Prefab> loadPrefab(const std::string& filename);
Model& getModelById(std::vector<Model>& models, int id);
json vec3_to_json(const glm::vec3& vec);
json quat_to_json(const glm::quat& quat);
glm::quat json_to_quat(const json& j);
glm::vec3 json_to_vec3(const json& j);
json save_node(Node* node);
int saveScene(const std::string& filename, SceneGraph*& scene);
int loadScene(const std::string& filename, SceneGraph*& scene, std::vector<std::shared_ptr<Prefab>>& prefabs, std::vector<BoundingBox*>& colliders);
Node* load_node(json& j, std::vector<BoundingBox*>& colliders, std::vector<std::shared_ptr<Prefab>>& prefabs, SceneGraph*& scene);

std::shared_ptr<Prefab> getPrefab(std::vector<std::shared_ptr<Prefab>>& prefabs, std::string name);
Node* load_prefab_node(json& j, SceneGraph*& scene, std::string& _name);
void savePrefabs(std::vector<std::shared_ptr<Prefab>>& prefabs);
void savePrefab(std::shared_ptr<Prefab>& prefab);
void loadTagLayers();
void saveTagLayers();

#endif // !SERIALIZATION_H
