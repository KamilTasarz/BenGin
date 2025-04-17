#pragma once

#include "../Basic/Node.h"
#include "../Gameplay/Player.h"
#include "../Component/Camera.h"
#include "../Light.h"
#include <nlohmann/json.hpp>
#include <functional>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

json vec3_to_json(const glm::vec3& vec);
json save_node(Node* node);
Node* load_node(json &j, std::vector<Model>& models, std::vector<BoundingBox*>& colliders);


int saveScene(const std::string& filename, Node*& rootNode, Player*& player, 
	PointLight *point_lights, unsigned int point_num, DirectionalLight *directional_lights, unsigned int directional_num, std::vector<Model> &models) {

	json sceneData;
	
	sceneData["scene"] = save_node(rootNode);
	
	json modelData;

	for (auto model = models.begin(); model != models.end(); model++) {
		json modelJson;
		modelJson["id"] = model->id;
		if (model->directory.empty()) {
			//kostki i plane'y
			modelJson["mode"] = model->mode;
			for (int i = 0; i < model->textures_loaded.size(); i++) {
				/*json textureJson;
				textureJson["path"] = model->textures_loaded[i].path;
				modelJson["textures"].push_back(textureJson);*/
				modelJson["textures"].push_back(model->textures_loaded[i].path);
			}
		}
		else {
			//kutasiarz i cos
			modelJson["directory"] = model->exact_path;
		}
		modelData.push_back(modelJson);
	}

	sceneData["models"] = modelData;

	json pointLightsData;

	for (unsigned int i = 0; i < point_num; i++) {
		json pointLightData;
		pointLightData["ambient"] = vec3_to_json(point_lights[i].ambient);
		pointLightData["specular"] = vec3_to_json(point_lights[i].specular);
		pointLightData["diffuse"] = vec3_to_json(point_lights[i].diffuse);
		pointLightData["constant"] = point_lights[i].constant;
		pointLightData["linear"] = point_lights[i].linear;
		pointLightData["quadratic"] = point_lights[i].quadratic;
		pointLightData["node"] = point_lights[i].object->name;
		
		pointLightsData.push_back(pointLightData);
	}

	sceneData["point_lights"] = pointLightsData;

	json directionalLightsData;
	for (unsigned int i = 0; i < directional_num; i++) {
		json directionalLightData;
		directionalLightData["ambient"] = vec3_to_json(directional_lights[i].ambient);
		directionalLightData["specular"] = vec3_to_json(directional_lights[i].specular);
		directionalLightData["diffuse"] = vec3_to_json(directional_lights[i].diffuse);
		directionalLightData["direction"] = vec3_to_json(directional_lights[i].direction);
		directionalLightData["node"] = directional_lights[i].object->name;
		directionalLightsData.push_back(directionalLightData);
	}
	
	sceneData["directional_lights"] = directionalLightsData;

	json playerData;

	playerData["node"] = player->player_node->name;
	playerData["speed"] = player->speed;
	playerData["range"] = player->range;
	playerData["h_0"] = player->h_0;

	sceneData["player"] = playerData;

	json cameraData;
	cameraData["position"] = vec3_to_json(camera->cameraPos);
	cameraData["Yaw"] =camera->Yaw;
	cameraData["Pitch"] =camera->Pitch;
	cameraData["object_to_follow"] =camera->object_to_follow->name;
	cameraData["object_origin"] = vec3_to_json(camera->origin_point);
	cameraData["mode"] = camera->mode;

	sceneData["camera"] = cameraData;

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

glm::vec3 json_to_vec3(const json& j) {
	
	return glm::vec3(j["x"], j["y"], j["z"]);
}

json save_node(Node* node) {
	json j;

	//dorobić instance manager
	/*if (dynamic_cast<InstanceManager*>(node)) {
		j["type"] = "InstanceManager";
	}
	else {
		j["type"] = "Node";
	}*/
	
	j["name"] = node->name;
	j["id"] = node->id;
	j["position"] = vec3_to_json(node->transform.getLocalPosition());
	j["rotation"] = vec3_to_json(node->transform.getLocalRotation());
	j["scale"] = vec3_to_json(node->transform.getLocalScale());
	j["no_textures"] = node->no_textures;
	j["is_visible"] = node->is_visible;
	if (node->pModel) {
		j["model.id"] = node->pModel->id;

	}
	else {
		j["model.id"] = -1;
	}

	// Rekurencyjnie zapisujemy dzieci
	for (Node* child : node->children) {
		j["children"].push_back(save_node(child));
	}

	return j;
}

int loadScene(const std::string& filename, Node*& rootNode, Player*& player, Camera*& camera_w,
	PointLight* point_lights, unsigned int& point_num, DirectionalLight* directional_lights, unsigned int& directional_num, std::vector<Model>& models, std::vector<BoundingBox*>& colliders) {

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
		return -1;
	}

	
	json sceneData;
	file >> sceneData;
	file.close();

	json modelData;

	modelData = sceneData["models"];

	for (json& modelJson : modelData) {
		if (modelJson.contains("directory")) {
			//kutasiarz i cos
			std::string directory = modelJson["directory"].get<string>();
			Model model(directory, modelJson["id"].get<int>());
			models.push_back(model);
		}
		else {
			//kostki i plane'y
			int texture_count = modelJson["textures"].size();
			
			const char** texture_names;
			if (texture_count == 0) {
				texture_names = nullptr;
			}
			else {
				texture_names = new const char* [texture_count];
			}
			std::vector<std::string> texture_storage; 

			for (int i = 0; i < texture_count; i++) {
				texture_storage.push_back(modelJson["textures"][i]);
				texture_names[i] = texture_storage.back().c_str(); // Pobieramy const char*
			}

			std::string mode = modelJson["mode"];
			Model model(texture_names, texture_count, modelJson["id"].get<int>(), mode);
			models.push_back(model);
		}

		
	}

	rootNode = load_node(sceneData["scene"], models, colliders);
	
	json pointLightsData = sceneData["point_lights"];

	point_num = pointLightsData.size();

	for (unsigned int i = 0; i < point_num; i++) {


		float constant = pointLightsData[i]["constant"];
		float linear = pointLightsData[i]["linear"];
		float quadratic = pointLightsData[i]["quadratic"];
		glm::vec3 ambient = json_to_vec3(pointLightsData[i]["ambient"]);
		glm::vec3 specular = json_to_vec3(pointLightsData[i]["specular"]);
		glm::vec3 diffuse = json_to_vec3(pointLightsData[i]["diffuse"]);

		point_lights[i] = PointLight(rootNode->getChildByName(pointLightsData[i]["node"]), quadratic, linear, constant, ambient, diffuse, specular);

	}


	json directionalLightsData = sceneData["directional_lights"];

	directional_num = directionalLightsData.size();

	for (unsigned int i = 0; i < directional_num; i++) {

		glm::vec3 ambient = json_to_vec3(directionalLightsData[i]["ambient"]);
		glm::vec3 specular = json_to_vec3(directionalLightsData[i]["specular"]);
		glm::vec3 diffuse = json_to_vec3(directionalLightsData[i]["diffuse"]);
		glm::vec3 direction = json_to_vec3(directionalLightsData[i]["direction"]);

		directional_lights[i] = DirectionalLight(rootNode->getChildByName(directionalLightsData[i]["node"]), direction, ambient, diffuse, specular);
	}

	json playerData = sceneData["player"];

	player = new Player(rootNode->getChildByName(playerData["node"].get<string>()), playerData["range"].get<float>(), playerData["h_0"].get<float>(), playerData["speed"].get<float>());

	json cameraData = sceneData["camera"];
	glm::vec3 pos = json_to_vec3(cameraData["position"]);
	glm::vec3 origin = json_to_vec3(cameraData["object_origin"]);
	//delete camera;
	//camera = new Camera(pos.x, pos.y, pos.z);

	camera_w->Yaw = cameraData["Yaw"];
	camera_w->Pitch = cameraData["Pitch"];
	camera_w->setPosition(pos);
	camera_w->setObjectToFollow(rootNode->getChildByName(cameraData["object_to_follow"]), origin);
	camera_w->changeMode((CameraMode)cameraData["mode"]);

	return 0;
}

Model& getModelById(std::vector<Model>& models, int id) {
	for (auto& model : models) {
		if (model.id == id) {
			return model;
		}
	}
	throw std::runtime_error("Model not found");
}

Node* load_node(json &j, std::vector<Model>& models, std::vector<BoundingBox*>& colliders) {
	Node* node = nullptr;

	int model_id = j["model.id"];
	int id = j["id"];
	bool no_textures = j["no_textures"];
	bool is_visible = j["is_visible"];
	std::string name = j["name"];
	if (model_id != -1) {
		Model &model = getModelById(models, model_id);
		if (no_textures) {
			node = new Node(model, name, true, id);
		}
		else {
			node = new Node(model, name, colliders, false, id);
		}
	}
	else {
		node = new Node(name);
	}
	node->is_visible = is_visible;
	
	node->transform.setLocalPosition(json_to_vec3(j["position"]));
	node->transform.setLocalRotation(json_to_vec3(j["rotation"]));
	node->transform.setLocalScale(json_to_vec3(j["scale"]));

	
	// Rekurencyjnie zapisujemy dzieci
	for (json j : j["children"]) {
		node->addChild(load_node(j, models, colliders));
	}

	return node;
}
