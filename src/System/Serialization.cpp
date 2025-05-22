#include "Serialization.h"


#include "../Basic/Node.h"
#include "../Gameplay/Player.h"
#include "../Component/CameraGlobals.h"
#include "../Light.h"
#include "../ResourceManager.h"
#include "../Basic/Model.h"
#include "../Gameplay/Script.h"
#include "../Gameplay/ScriptFactory.h"
#include "../System/Component.h"
#include "../System/Rigidbody.h"
#include "../System/Tag.h"

//using namespace std;
std::vector<std::pair<Node*, std::pair<std::string, std::string>>> script_nodes;

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

json vec2_to_json(const glm::vec2& vec) {
	json j;
	j["x"] = vec.x;
	j["y"] = vec.y;
	return j;
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

glm::vec2 json_to_vec2(const json& j) {
	return glm::vec2(j["x"], j["y"]);
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
		if (directional_lights->from_prefab) return json();
		json directionalLightData;
		directionalLightData["ambient"] = vec3_to_json(directional_lights->ambient);
		directionalLightData["specular"] = vec3_to_json(directional_lights->specular);
		directionalLightData["diffuse"] = vec3_to_json(directional_lights->diffuse);
		directionalLightData["direction"] = vec3_to_json(directional_lights->direction);
		directionalLightData["is_shining"] = directional_lights->is_shining;
		j["directional_light"] = directionalLightData;
	}
	else if (dynamic_cast<PointLight*>(node)) {
		j["type"] = "PointLight";
		PointLight* point_lights = dynamic_cast<PointLight*>(node);
		if (point_lights->from_prefab) return json();
		json pointLightData;
		pointLightData["ambient"] = vec3_to_json(point_lights->ambient);
		pointLightData["specular"] = vec3_to_json(point_lights->specular);
		pointLightData["diffuse"] = vec3_to_json(point_lights->diffuse);
		pointLightData["constant"] = point_lights->constant;
		pointLightData["linear"] = point_lights->linear;
		pointLightData["quadratic"] = point_lights->quadratic;
		pointLightData["is_shining"] = point_lights->is_shining;
		j["point_light"] = pointLightData;
	}
	else if (dynamic_cast<PrefabInstance*>(node)) {
		j["type"] = "PrefabInstance";
		PrefabInstance* prefab_inst = dynamic_cast<PrefabInstance*>(node);
		json pointLightData;
		pointLightData["prefab_name"] = prefab_inst->prefab->prefab_scene_graph->root->name;

		j["prefab_instance"] = pointLightData;
	}
	else if (dynamic_cast<InstanceManager*>(node)) {
		j["type"] = "InstanceManager";

	} else {
		j["type"] = "Node";
	}

	j["name"] = node->name;
	
	j["tag"] = node->getTagName();
	j["layer"] = node->getLayerName();
	if (node->AABB) {
		if (node->pModel && (node->pModel->min_points != node->AABB->min_point_local || node->pModel->max_points != node->AABB->max_point_local)) {
			j["AABB_min"] = vec3_to_json(node->AABB->min_point_local);
			j["AABB_max"] = vec3_to_json(node->AABB->max_point_local);
		}
	}

	if (node->AABB_logic) {
		if (node->pModel && (node->pModel->min_points != node->AABB_logic->min_point_local || node->pModel->max_points != node->AABB_logic->max_point_local)) {
			j["AABB_logic_min"] = vec3_to_json(node->AABB_logic->min_point_local);
			j["AABB_logic_max"] = vec3_to_json(node->AABB_logic->max_point_local);
		}
	}
	j["id"] = node->id;
	j["position"] = vec3_to_json(node->transform.getLocalPosition());
	j["rotation"] = quat_to_json(node->transform.getLocalRotation());
	j["scale"] = vec3_to_json(node->transform.getLocalScale());
	j["no_textures"] = node->no_textures;
	j["is_visible"] = node->is_visible;
	j["is_logic_active"] = node->is_logic_active;
	j["is_physic_active"] = node->is_physic_active;
	if (node->pModel) {
		j["model.id"] = node->pModel->id;

	}
	else {
		j["model.id"] = -1;
	}

	json componentsData;
	for (auto& component : node->components) {
		json componentJson;
		componentJson["name"] = component->name;
		if (component->name._Equal("Rigidbody")) {
			json rigidbodyJson;
			Rigidbody* rigidbody = dynamic_cast<Rigidbody*>(component.get());
			rigidbodyJson["mass"] = rigidbody->mass;
			rigidbodyJson["gravity"] = rigidbody->gravity;
			rigidbodyJson["is_static"] = rigidbody->is_static;
			rigidbodyJson["lockPositionX"] = rigidbody->lockPositionX;
			rigidbodyJson["lockPositionY"] = rigidbody->lockPositionY;
			rigidbodyJson["lockPositionZ"] = rigidbody->lockPositionZ;

			componentJson["properties"] = rigidbodyJson;
		} else if (dynamic_cast<Script*>(component.get())) {
			json variablesJson;
			Script* script = dynamic_cast<Script*>(component.get());
			for (const auto& field : script->getFields()) {
				json fieldJson;
				void* ptr = reinterpret_cast<char*>(script) + field->offset;

				if (field->type == "float") {
					float* f = reinterpret_cast<float*>(ptr);
					fieldJson["field_type"] = "float";
					fieldJson["name"] = field->name;
					fieldJson["value"] = *f;
				}
				else if (field->type == "string") {
					std::string* s = reinterpret_cast<std::string*>(ptr);
					fieldJson["field_type"] = "string";
					fieldJson["name"] = field->name;
					fieldJson["value"] = *s;
				}
				else if (field->type == "int") {
					int* i = reinterpret_cast<int*>(ptr);
					fieldJson["field_type"] = "int";
					fieldJson["name"] = field->name;
					fieldJson["value"] = *i;
				}
				else if (field->type == "bool") {
					bool* b = reinterpret_cast<bool*>(ptr);
					fieldJson["field_type"] = "bool";
					fieldJson["name"] = field->name;
					fieldJson["value"] = *b;
				}
				else if (field->type == "Node*") {
					Node** n = reinterpret_cast<Node**>(ptr);
					fieldJson["field_type"] = "Node*";
					fieldJson["name"] = field->name;
					if (*n) {
						fieldJson["value"] = (*n)->name;
					}
					else {
						fieldJson["value"] = "null";
					}
				}

				variablesJson.push_back(fieldJson);
			}
			componentJson["variables"] = variablesJson;
		}
		
		
		componentsData.push_back(componentJson);


	}
	if (!componentsData.empty()) {
		j["components"] = componentsData;
	}
	

	// Rekurencyjnie zapisujemy dzieci
	for (Node* child : node->children) {
		j["children"].push_back(save_node(child));
	}

	return j;
}

int loadScene(const std::string& filename, SceneGraph*& scene, std::vector<std::shared_ptr<Prefab>>& prefabs) {

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

	load_node(sceneData["scene"], prefabs, scene);

	loadComponents(sceneData["scene"], scene->root, scene);

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

std::shared_ptr<Prefab> getPrefab(std::vector<std::shared_ptr<Prefab>>& prefabs, std::string name) {
	for (auto& prefab : prefabs) {
		if (prefab->prefab_scene_graph->root->name._Equal(name)) {
			return prefab;
		}

	}
	return nullptr;
}

Node* load_node(json& j, std::vector<std::shared_ptr<Prefab>>& prefabs, SceneGraph*& scene) {
	Node* node = nullptr;



	int model_id = j["model.id"];
	int id = j["id"];
	bool no_textures = j["no_textures"];
	bool is_visible = j["is_visible"];

	bool is_logic_active = false;
	bool is_phys_active = true;

	if (j.contains("is_logic_active"))
		is_logic_active = j["is_logic_active"];
	if (j.contains("is_physic_active"))
		is_phys_active = j["is_physic_active"];

	std::string name = j["name"];
	std::string type = j["type"];

	if (name._Equal("root")) {
		scene->root->transform.setLocalPosition(json_to_vec3(j["position"]));
		scene->root->transform.setLocalRotation(json_to_quat(j["rotation"]));
		scene->root->transform.setLocalScale(json_to_vec3(j["scale"]));
		scene->root->is_visible = is_visible;
		for (json j : j["children"]) {

			Node* child = load_node(j, prefabs, scene);
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
				std::shared_ptr<Model> model = ResourceManager::Instance().getModel(model_id); //getModelById(models, model_id);
				if (no_textures) {
					node = new Node(model, name, id);
				}
				else {
					node = new Node(model, name, id);
				}
			}
			else {
				node = new Node(name);
			}

			if (j.contains("AABB_min")) {
				node->AABB->min_point_local = json_to_vec3(j["AABB_min"]);
				node->AABB->max_point_local = json_to_vec3(j["AABB_max"]);

			}
			if (j.contains("AABB_logic_min")) {
				node->AABB_logic->min_point_local = json_to_vec3(j["AABB_logic_min"]);
				node->AABB_logic->max_point_local = json_to_vec3(j["AABB_logic_max"]);
			}

		}
		else if (type._Equal("PointLight")) {
			glm::vec3 ambient = json_to_vec3(j["point_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["point_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["point_light"]["diffuse"]);
			float constant = j["point_light"]["constant"];
			float linear = j["point_light"]["linear"];
			float quadratic = j["point_light"]["quadratic"];

			bool is_shining = true;
			if (j["point_light"].contains("is_shining")) {
				is_shining = j["point_light"]["is_shining"];
			}

			

			node = new PointLight(ResourceManager::Instance().getModel(model_id), name, is_shining, quadratic, linear, constant, ambient, diffuse, specular);
		}
		else if (type._Equal("DirectionalLight")) {
			glm::vec3 ambient = json_to_vec3(j["directional_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["directional_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["directional_light"]["diffuse"]);
			glm::vec3 direction = json_to_vec3(j["directional_light"]["direction"]);
			bool is_shining = true;
			if (j["point_light"].contains("is_shining")) {
				is_shining = j["point_light"]["is_shining"];
			}
			node = new DirectionalLight(ResourceManager::Instance().getModel(model_id), name, is_shining, direction, ambient, diffuse, specular);
		}
		else if (type._Equal("PrefabInstance")) {
			std::string prefab_name = j["prefab_instance"]["prefab_name"];
			std::shared_ptr<Prefab> prefab = getPrefab(prefabs, prefab_name);
			if (prefab) {
				node = new PrefabInstance(prefab, scene, "_" + to_string(prefab->prefab_instances.size()));
			}

		}
		else if (type._Equal("InstanceManager")) {
			
			node = new InstanceManager(ResourceManager::Instance().getModel(model_id), name);
			

		}
		else {
			std::cerr << "Unknown node type: " << type << std::endl;
			return nullptr;
		}
		node->is_visible = is_visible;
		node->is_logic_active = is_logic_active;
		node->is_physic_active = is_phys_active;
		node->transform.setLocalPosition(json_to_vec3(j["position"]));
		node->transform.setLocalRotation(json_to_quat(j["rotation"]));
		node->transform.setLocalScale(json_to_vec3(j["scale"]));
		node->setTag(TagLayerManager::Instance().getTag(j["tag"]));
		node->setLayer(TagLayerManager::Instance().getLayer(j["layer"]));



		/*if (j.contains("components")) {
			for (auto& component : j["components"]) {
				std::string component_name = component["name"];
				
				if (component_name._Equal("Rigidbody")) {
					json rigidbodyJson = component["properties"];
					node->addComponent(std::make_unique<Rigidbody>(rigidbodyJson["mass"], rigidbodyJson["gravity"], rigidbodyJson["is_static"]));
				} else {
					std::unique_ptr <Component> _component = ScriptFactory::instance().create(component_name);
					json variablesJson = component["variables"];
					Script* script = dynamic_cast<Script*>(_component.get());
					for (json fieldJson : variablesJson) {
						
						std::string field_name = fieldJson["name"];
						std::string field_type = fieldJson["field_type"];

						for (const auto& field : script->getFields()) {
							if (field.name == field_name && field.type == field_type) {
								void* ptr = reinterpret_cast<char*>(script) + field.offset;
								if (field.type == "float") {
									float* f = reinterpret_cast<float*>(ptr);
									*f = fieldJson["value"];
								}
								else if (field.type == "int") {
									std::string* s = reinterpret_cast<std::string*>(ptr);
									*s = fieldJson["value"];
								}
								else if (field.type == "int") {
									int* i = reinterpret_cast<int*>(ptr);
									*i = fieldJson["value"];
								}
								else if (field.type == "Node*") {
									std::string node_in_script = fieldJson["value"];
									script_nodes.push_back(std::make_pair(node, std::make_pair(field.name, node_in_script)));
									

								}
							}
						}

						

						

						
					}


					node->addComponent(std::move(_component));
				}


				
				
			}
		}*/


		// Rekurencyjnie zapisujemy dzieci
		for (json _j : j["children"]) {
			Node* child = load_node(_j, prefabs, scene);
			if (dynamic_cast<PointLight*>(child)) {
				PointLight* point_light = dynamic_cast<PointLight*>(child);
				//scene->addPointLight(point_light, node->name);
				node->scene_graph = scene;
				node->addChild(point_light);
				scene->point_lights.push_back(point_light);
				scene->point_light_number++;
			}
			else if (dynamic_cast<DirectionalLight*>(child)) {
				DirectionalLight* directional_light = dynamic_cast<DirectionalLight*>(child);
				//scene->addDirectionalLight(directional_light, node->name);
				node->scene_graph = scene;
				node->addChild(directional_light);
				scene->directional_lights.push_back(directional_light);
				scene->directional_light_number++;
			}
			else {
				//scene->addChild(child, node->name);
				node->scene_graph = scene;
				node->addChild(child);
			}


		}
	}



	return node;
}



Node* load_prefab_node(json& j, SceneGraph*& scene, std::string& _name)
{

	Node* node = nullptr;



	int model_id = j["model.id"];
	int id = j["id"];
	bool no_textures = j["no_textures"];
	bool is_visible = j["is_visible"];
	std::string name = j["name"];
	std::string type = j["type"];

	bool is_logic_active = false;
	bool is_phys_active = true;

	if (j.contains("is_logic_active"))
		is_logic_active = j["is_logic_active"];
	if (j.contains("is_physic_active"))
		is_phys_active = j["is_physic_active"];

	if (name._Equal(_name)) {
		scene->root->transform.setLocalPosition(json_to_vec3(j["position"]));
		scene->root->transform.setLocalRotation(json_to_quat(j["rotation"]));
		scene->root->transform.setLocalScale(json_to_vec3(j["scale"]));
		scene->root->is_visible = is_visible;
		for (json j : j["children"]) {

			Node* child = load_prefab_node(j, scene, _name);
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
				std::shared_ptr<Model> model = ResourceManager::Instance().getModel(model_id); //getModelById(models, model_id);

				node = new Node(model, name, id);

			}
			else {
				node = new Node(name);
			}
			if (j.contains("AABB_min")) {
				node->AABB->min_point_local = json_to_vec3(j["AABB_min"]);
				node->AABB->max_point_local = json_to_vec3(j["AABB_max"]);

			}
			if (j.contains("AABB_logic_min")) {
				node->AABB_logic->min_point_local = json_to_vec3(j["AABB_logic_min"]);
				node->AABB_logic->max_point_local = json_to_vec3(j["AABB_logic_max"]);
			}
		}
		else if (type._Equal("PointLight")) {
			glm::vec3 ambient = json_to_vec3(j["point_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["point_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["point_light"]["diffuse"]);
			float constant = j["point_light"]["constant"];
			float linear = j["point_light"]["linear"];
			float quadratic = j["point_light"]["quadratic"];
			bool is_shining = true;
			if (j["point_light"].contains("is_shining")) {
				is_shining = j["point_light"]["is_shining"];
			}
			node = new PointLight(ResourceManager::Instance().getModel(model_id), name, is_shining, quadratic, linear, constant, ambient, diffuse, specular);
		}
		else if (type._Equal("DirectionalLight")) {
			glm::vec3 ambient = json_to_vec3(j["directional_light"]["ambient"]);
			glm::vec3 specular = json_to_vec3(j["directional_light"]["specular"]);
			glm::vec3 diffuse = json_to_vec3(j["directional_light"]["diffuse"]);
			glm::vec3 direction = json_to_vec3(j["directional_light"]["direction"]);
			bool is_shining = true;
			if (j["point_light"].contains("is_shining")) {
				is_shining = j["point_light"]["is_shining"];
			}
			node = new DirectionalLight(ResourceManager::Instance().getModel(model_id), name, is_shining, direction, ambient, diffuse, specular);
		}
		else if (type._Equal("InstanceManager")) {

			node = new InstanceManager(ResourceManager::Instance().getModel(model_id), name);


		}
		else {
			std::cerr << "Unknown node type: " << type << std::endl;
			return nullptr;
		}
		node->is_visible = is_visible;
		node->is_logic_active = is_logic_active;
		node->is_physic_active = is_phys_active;
		node->transform.setLocalPosition(json_to_vec3(j["position"]));
		node->transform.setLocalRotation(json_to_quat(j["rotation"]));
		node->transform.setLocalScale(json_to_vec3(j["scale"]));
		node->setTag(TagLayerManager::Instance().getTag(j["tag"]));
		node->setLayer(TagLayerManager::Instance().getLayer(j["layer"]));

		/*if (j.contains("components")) {
			for (auto& component : j["components"]) {
				std::string component_name = component["name"];

				if (component_name._Equal("Rigidbody")) {
					json rigidbodyJson = component["properties"];
					node->addComponent(std::make_unique<Rigidbody>(rigidbodyJson["mass"], rigidbodyJson["gravity"], rigidbodyJson["is_static"]));
				}
				else {
					node->addComponent(ScriptFactory::instance().create(component_name));
				}
			}
		}*/

		// Rekurencyjnie zapisujemy dzieci
		for (json j : j["children"]) {
			Node* child = load_prefab_node(j, scene, _name);
			if (dynamic_cast<PointLight*>(child)) {
				PointLight* point_light = dynamic_cast<PointLight*>(child);
				//scene->addPointLight(point_light, node->name);
				node->scene_graph = scene;
				node->addChild(point_light);
				scene->point_lights.push_back(point_light);
				scene->point_light_number++;
			}
			else if (dynamic_cast<DirectionalLight*>(child)) {
				DirectionalLight* directional_light = dynamic_cast<DirectionalLight*>(child);
				//scene->addDirectionalLight(directional_light, node->name);
				node->scene_graph = scene;
				node->addChild(directional_light);
				scene->directional_lights.push_back(directional_light);
				scene->directional_light_number++;
			}
			else {
				//scene->addChild(child, node->name);
				node->scene_graph = scene;
				node->addChild(child);
			}


		}
	}



	return node;
}


std::shared_ptr<Prefab> loadPrefab(const std::string& filename)
{
	std::shared_ptr<Prefab> prefab = make_shared<Prefab>();

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";

	}
	else {
		json prefab_data;
		file >> prefab_data;
		file.close();

		std::string name = prefab_data["name"];
		PrefabType type = PrefabType(prefab_data["pref_type"].get<int>());
		prefab->prefab_type = type;
		prefab->prefab_scene_graph->root->name = name;
		load_prefab_node(prefab_data, prefab->prefab_scene_graph, name);

		loadComponents(prefab_data, prefab->prefab_scene_graph->root, prefab->prefab_scene_graph);
	}


	return prefab;
}

void loadComponents(json& j, Node* node, SceneGraph* scene)
{


	if (j.contains("components")) {
		for (auto& component : j["components"]) {
			std::string component_name = component["name"];

			if (component_name._Equal("Rigidbody")) {
				json rigidbodyJson = component["properties"];
				node->addComponent(std::make_unique<Rigidbody>(rigidbodyJson["mass"], rigidbodyJson["gravity"], rigidbodyJson["is_static"], true, rigidbodyJson["lockPositionX"], rigidbodyJson["lockPositionY"], rigidbodyJson["lockPositionZ"]));
			}
			else {
				std::unique_ptr <Component> _component = ScriptFactory::instance().create(component_name);
				json variablesJson = component["variables"];
				Script* script = dynamic_cast<Script*>(_component.get());
				for (json fieldJson : variablesJson) {

					std::string field_name = fieldJson["name"];
					std::string field_type = fieldJson["field_type"];

					for (auto& field : script->getFields()) {
						if (field->name == field_name && field->type == field_type) {
							void* ptr = reinterpret_cast<char*>(script) + field->offset;
							if (field->type == "float") {
								float* f = reinterpret_cast<float*>(ptr);
								*f = fieldJson["value"].get<float>();
								std::cout << field->name << " " << *f << endl;
							}
							else if (field->type == "std::string") {
								std::string* s = reinterpret_cast<std::string*>(ptr);
								*s = fieldJson["value"];
							}
							else if (field->type == "int") {
								int* i = reinterpret_cast<int*>(ptr);
								*i = fieldJson["value"].get<int>();
							}
							else if (field->type == "bool") {
								bool* b = reinterpret_cast<bool*>(ptr);
								*b = fieldJson["value"].get<bool>();
							}
							else if (field->type == "Node*") {
								std::string node_in_script = fieldJson["value"];
								if (node_in_script == "null") {
									continue;
								}
								else {
									Node** n = reinterpret_cast<Node**>(ptr);
									Node* found = scene->root->getChildByName(node_in_script);
									*n = found;
								}
							}
						}
					}
				}
				node->addComponent(std::move(_component));
				for (auto& com : node->components) {
					Script* addedScript = dynamic_cast<Script*>(com.get());
					if (addedScript) {
						for (const auto& field : addedScript->getFields()) {
							void* ptr = reinterpret_cast<char*>(addedScript) + field->offset;
							if (field->type == "float") {
								std::cout << field->name << ": " << *reinterpret_cast<float*>(ptr) << std::endl;
							}
						}
					}
					
				}
				
			}




		}
	}

	for (json _j : j["children"]) {
		std::string _name = _j["name"];
		Node* child = scene->root->getChildByName(_name);
		loadComponents(_j, child, scene);
	}
}

void loadPrefabs(std::vector<std::shared_ptr<Prefab>>& prefabs, std::vector<std::shared_ptr<Prefab>>& prefabs_puzzle) {
	const fs::path prefab_dir = "res/scene/prefabs";

	for (const auto& entry : fs::recursive_directory_iterator(prefab_dir)) {
		if (entry.is_regular_file()) {
			std::shared_ptr<Prefab> prefab = loadPrefab(entry.path().string());
			prefab->prefab_scene_graph->forcedUpdate();
			prefabs.push_back(prefab);
		}
	}
}



void savePrefab(std::shared_ptr<Prefab>& prefab)
{
	std::string filename = "res/scene/prefabs";
	std::string name = prefab->prefab_scene_graph->root->getName();

	json json_prefab = save_node(prefab->prefab_scene_graph->root);

	switch (prefab->prefab_type) {
	case VERTICAL_DOWN:
		filename += "/vertical/down/";
		json_prefab["pref_type"] = 1;
		break;
	case VERTICAL_UP:
		filename += "/vertical/up/";
		json_prefab["pref_type"] = 0;
		break;
	case HORIZONTAL_LEFT:
		filename += "/horizontal/left/";
		json_prefab["pref_type"] = 2;
		break;
	case HORIZONTAL_RIGHT:
		filename += "/horizontal/right/";
		json_prefab["pref_type"] = 3;
		break;
	default:
		break;
	}

	filename += name + ".json";



	json_prefab["name"] = name;

	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file for writing: " << filename << std::endl;

	}
	else {
		file << json_prefab.dump(4);
		file.close();
	}

}

void loadTagLayers()
{
	std::string filename = "res/scene/tags_layers.json";

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
	}
	else {
		json sceneData;
		file >> sceneData;
		file.close();

		json layerData = sceneData["layers"];

		for (json layer : layerData) {
			std::string layerName = layer["name"];
			TagLayerManager::Instance().addLayer(layerName);
		}

		json tagData = sceneData["tags"];

		for (json tag : tagData) {
			std::string tagName = tag["name"];
			TagLayerManager::Instance().addTag(tagName);
		}
	}

	// add default tag
	TagLayerManager::Instance().addTag("Default");

	
	// add default layer
	TagLayerManager::Instance().addLayer("Default");
}

void saveTagLayers()
{
	std::string filename = "res/scene/tags_layers.json";
	json sceneData;
	json tagData;
	for (auto& tag : TagLayerManager::Instance().getTags()) {
		json tagJson;
		tagJson["name"] = tag->name;
		tagData.push_back(tagJson);
	}
	sceneData["tags"] = tagData;
	json layerData;
	for (auto& layer : TagLayerManager::Instance().getLayers()) {
		json layerJson;
		layerJson["name"] = layer->name;
		layerData.push_back(layerJson);
	}
	sceneData["layers"] = layerData;



	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file for writing: " << filename << std::endl;
		return;
	}
	file << sceneData.dump(4); // Pretty print with 4 spaces
	file.close();

}

void savePostProcessData(const std::string& filename, PostProcessData& data) {

	json j;

	j["is_pp"] = data.is_post_process;

	j["is_crt"] = data.is_crt_curved;

	// Save as an array of 2 values
	j["crt_curvature"] = vec2_to_json(data.crt_curvature);

	j["crt_outline_color"] = vec3_to_json(data.crt_outline_color);

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4); // Pretty print with 4 spaces
		file.close();
	}
	else {
		std::cerr << "Nie mozna zapisac pliku: " << filename << std::endl;
	}
	
}

void loadPostProcessData(const std::string& filename, PostProcessData& data) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		try {

			file >> j;

			data.is_post_process = j.at("is_pp");

			data.is_crt_curved = j.at("is_crt");

			data.crt_curvature = json_to_vec2(j.at("crt_curvature"));

			data.crt_outline_color = json_to_vec3(j.at("crt_outline_color"));
		
		}
		catch (const std::exception& e) {
			std::cerr << "Blad przy wczytywaniu pliku " << filename << ": " << e.what() << std::endl;
		}
		file.close();
	}
	else {
		std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
	}
}


void savePrefabs(std::vector<std::shared_ptr<Prefab>>& prefabs) {
	for (auto& prefab : prefabs) {
		savePrefab(prefab);
	}
}
