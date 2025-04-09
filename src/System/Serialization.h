#pragma once

#include "../Basic/Node.h"
#include <nlohmann/json.hpp>
#include <functional>
#include <iostream>
#include <fstream>

using json = nlohmann::json;


json save_node(Node* node);


int saveScene(const std::string& filename, Node* rootNode) {
	json sceneData;
	
	sceneData["scene"] = save_node(rootNode);

	//sceneData["player"] = player->player_node->name;

	//sceneData["camera"] = camera;

	// Save the JSON to a file
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file for writing: " << filename << std::endl;
		return -1;
	}
	file << sceneData.dump(4); // Pretty print with 4 spaces
	file.close();
	return 0;

}

json vec3_to_json(const glm::vec3& vec) {
	json j;
	j["x"] = vec.x;
	j["y"] = vec.y;
	j["z"] = vec.z;
	return j;
}

json save_node(Node* node) {
	json j;
	j["name"] = node->name;
	j["position"] = vec3_to_json(node->transform.getLocalPosition());
	j["rotation"] = vec3_to_json(node->transform.getLocalRotation());
	j["scale"] = vec3_to_json(node->transform.getLocalScale());
	j["no_textures"] = node->no_textures;
	if (node->pModel) {
		if (node->pModel->directory.empty()) {
			j["model.mode"] = node->pModel->mode;
		}
		else {
			j["model.directory"] = node->pModel->directory;
		}

	}
	else {
		j["model"] = nullptr;
	}

	// Rekurencyjnie zapisujemy dzieci
	for (Node* child : node->children) {
		j["children"].push_back(save_node(child));
	}

	return j;
}

int loadScene(const std::string& filename, Node* rootNode) {
	return 0;
}
