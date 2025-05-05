#pragma once

#include "../Basic/Node.h"
#include "../Gameplay/Player.h"
#include "../Component/CameraGlobals.h"
#include "../Light.h"
#include "../ResourceManager.h"
#include <nlohmann/json.hpp>
#include <functional>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

json vec3_to_json(const glm::vec3& vec);
json quat_to_json(const glm::quat& quat);
json save_node(Node* node);
Node* load_node(json& j, std::vector<BoundingBox*>& colliders, SceneGraph*& scene);

Model& getModelById(std::vector<Model>& models, int id) {
	for (auto& model : models) {
		if (model.id == id) {
			return model;
		}
	}
	throw std::runtime_error("Model not found");
}

int saveScene(const std::string& filename, SceneGraph*& scene) {

	json sceneData;
	
	sceneData["scene"] = save_node(scene->root);
	
	//json modelData;

	//for (auto model = models.begin(); model != models.end(); model++) {
	//	json modelJson;
	//	modelJson["id"] = model->id;
	//	if (model->directory.empty()) {
	//		//kostki i plane'y
	//		modelJson["mode"] = model->mode;
	//		for (int i = 0; i < model->textures_loaded.size(); i++) {
	//			/*json textureJson;
	//			textureJson["path"] = model->textures_loaded[i].path;
	//			modelJson["textures"].push_back(textureJson);*/
	//			modelJson["textures"].push_back(model->textures_loaded[i].path);
	//		}
	//	}
	//	else {
	//		//kutasiarz i cos
	//		modelJson["directory"] = model->exact_path;
	//	}
	//	modelData.push_back(modelJson);
	//}

	//sceneData["models"] = modelData;

	json cameraData;
	cameraData["position"] = vec3_to_json(camera->cameraPos);
	cameraData["Yaw"] = camera->Yaw;
	cameraData["Pitch"] = camera->Pitch;
	//cameraData["object_to_follow"] = camera->object_to_follow->name;
	//cameraData["object_origin"] = vec3_to_json(camera->origin_point);
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

json quat_to_json(const glm::quat& quat) {
	json j;
	j["w"] = quat.w;
	j["x"] = quat.x;
	j["y"] = quat.y;
	j["z"] = quat.z;
	return j;
}


glm::quat json_to_quat(const json& j) {

	return glm::quat(j["w"], j["x"], j["y"], j["z"]);
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
	if (dynamic_cast<DirectionalLight*>(node)) {
		j["type"] = "DirectionalLight";
		DirectionalLight* directional_lights = dynamic_cast<DirectionalLight*>(node);
		json directionalLightData;
		directionalLightData["ambient"] = vec3_to_json(directional_lights->ambient);
		directionalLightData["specular"] = vec3_to_json(directional_lights->specular);
		directionalLightData["diffuse"] = vec3_to_json(directional_lights->diffuse);
		directionalLightData["direction"] = vec3_to_json(directional_lights->direction);
		j["directional_light"] = directionalLightData;
	}
	else if (dynamic_cast<PointLight*>(node)) {
		j["type"] = "PointLight";
		PointLight* point_lights = dynamic_cast<PointLight*>(node);
		json pointLightData;
		pointLightData["ambient"] = vec3_to_json(point_lights->ambient);
		pointLightData["specular"] = vec3_to_json(point_lights->specular);
		pointLightData["diffuse"] = vec3_to_json(point_lights->diffuse);
		pointLightData["constant"] = point_lights->constant;
		pointLightData["linear"] = point_lights->linear;
		pointLightData["quadratic"] = point_lights->quadratic;
		j["point_light"] = pointLightData;
	}
	else {
		j["type"] = "Node";
	}

	j["name"] = node->name;
	j["id"] = node->id;
	j["position"] = vec3_to_json(node->transform.getLocalPosition());
	j["rotation"] = quat_to_json(node->transform.getLocalRotation());
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

int loadScene(const std::string& filename, SceneGraph*& scene, std::vector<BoundingBox*>& colliders) {

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
		return -1;
	}

	
	json sceneData;
	file >> sceneData;
	file.close();

	/*json modelData;

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

		
	}*/

	scene = new SceneGraph();

	load_node(sceneData["scene"],  colliders, scene);

	//json playerData = sceneData["player"];

	//player = new Player(rootNode->getChildByName(playerData["node"].get<string>()), playerData["range"].get<float>(), playerData["h_0"].get<float>(), playerData["speed"].get<float>());

	json cameraData = sceneData["camera"];
	glm::vec3 pos = json_to_vec3(cameraData["position"]);
	//glm::vec3 origin = json_to_vec3(cameraData["object_origin"]);
	//delete camera;
	//camera = new Camera(pos.x, pos.y, pos.z);

	camera->Yaw = cameraData["Yaw"];
	camera->Pitch = cameraData["Pitch"];
	camera->setPosition(pos);
	//camera->setObjectToFollow(rootNode->getChildByName(cameraData["object_to_follow"]), origin);
	camera->changeMode((CameraMode)cameraData["mode"]);

	return 0;
}



Node* load_node(json& j, std::vector<BoundingBox*>& colliders, SceneGraph *&scene) {
	Node* node = nullptr;

	std::cout << "Bleh" << std::endl;

	int model_id = j["model.id"];
	int id = j["id"];
	bool no_textures = j["no_textures"];
	bool is_visible = j["is_visible"];
	std::string name = j["name"];
	std::string type = j["type"];

	if (name._Equal("root")) {
		scene->root->transform.setLocalPosition(json_to_vec3(j["position"]));
		scene->root->transform.setLocalRotation(json_to_quat(j["rotation"]));
		scene->root->transform.setLocalScale(json_to_vec3(j["scale"]));
		scene->root->is_visible = is_visible;
		for (json j : j["children"]) {

			Node* child = load_node(j, colliders, scene);
			if (dynamic_cast<PointLight*>(child)) {
				PointLight* point_light = dynamic_cast<PointLight*>(child);
				scene->addPointLight(point_light);
			}
			else if (dynamic_cast<DirectionalLight*>(child)) {
				DirectionalLight* directional_light = dynamic_cast<DirectionalLight*>(child);
				scene->addDirectionalLight(directional_light);
			}
			else {
				scene->addChild(child);
			}



		}
	}
	else {
		if (type._Equal("Node")) {

			if (model_id != -1) {
				shared_ptr<Model> model = ResourceManager::Instance().getModel(model_id); //getModelById(models, model_id);
				if (no_textures) {
					node = new Node(model, name, id);
				}
				else {
					node = new Node(model, name, colliders, id);
				}
			}
			else {
				node = new Node(name);
			}
		}
		else if (type._Equal("PointLight")) {
			glm::vec3 ambient = json_to_vec3(j["point_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["point_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["point_light"]["diffuse"]);
			float constant = j["point_light"]["constant"];
			float linear = j["point_light"]["linear"];
			float quadratic = j["point_light"]["quadratic"];
			node = new PointLight(ResourceManager::Instance().getModel(model_id), name, quadratic, linear, constant, ambient, diffuse, specular);
		}
		else if (type._Equal("DirectionalLight")) {
			glm::vec3 ambient = json_to_vec3(j["directional_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["directional_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["directional_light"]["diffuse"]);
			glm::vec3 direction = json_to_vec3(j["directional_light"]["direction"]);
			node = new DirectionalLight(ResourceManager::Instance().getModel(model_id), name, direction, ambient, diffuse, specular);
		}
		else {
			std::cerr << "Unknown node type: " << type << std::endl;
			return nullptr;
		}
		node->is_visible = is_visible;

		node->transform.setLocalPosition(json_to_vec3(j["position"]));
		node->transform.setLocalRotation(json_to_quat(j["rotation"]));
		node->transform.setLocalScale(json_to_vec3(j["scale"]));


		// Rekurencyjnie zapisujemy dzieci
		for (json j : j["children"]) {
			Node* child = load_node(j, colliders, scene);
			if (dynamic_cast<PointLight*>(child)) {
				PointLight* point_light = dynamic_cast<PointLight*>(child);
				scene->addPointLight(point_light, node->name);
			}
			else if (dynamic_cast<DirectionalLight*>(child)) {
				DirectionalLight* directional_light = dynamic_cast<DirectionalLight*>(child);
				scene->addDirectionalLight(directional_light, node->name);
			}
			else {
				scene->addChild(child, node->name);
			}
			
			
		}
	}

	

	return node;
}