#include "SceneManager.h"
#include "../Basic/Node.h"
#include "../System/Serialization.h"
#include "../System/PrefabRegistry.h"
#include <fstream>

SceneManager::~SceneManager()
{
	if (sceneGraph != nullptr)
	{
		delete sceneGraph;
		sceneGraph = nullptr;
	}
}

void SceneManager::next()
{
	if (currentSceneIndex + 1 < scenes.size())
	{
		currentSceneIndex++;
		
	}
	else
	{
		currentSceneIndex = 0; 
		
	}
	std::vector<std::shared_ptr<Prefab>> rooms = PrefabRegistry::GetRooms();
	std::vector<std::shared_ptr<Prefab>> puzzles = PrefabRegistry::GetPuzzles();
	
	loadScene(scenes[currentSceneIndex].path_name, sceneGraph, rooms, puzzles);
}

void SceneManager::previous()
{
	if (currentSceneIndex - 1 >= 0)
	{
		currentSceneIndex--;
	}
	else
	{
		currentSceneIndex = scenes.size() - 1;
	}
	std::vector<std::shared_ptr<Prefab>> rooms = PrefabRegistry::GetRooms();
	std::vector<std::shared_ptr<Prefab>> puzzles = PrefabRegistry::GetPuzzles();

	loadScene(scenes[currentSceneIndex].path_name, sceneGraph, rooms, puzzles);
}

void SceneManager::reset()
{
	if (currentSceneIndex >= 0 && currentSceneIndex < scenes.size())
	{
		std::vector<std::shared_ptr<Prefab>> rooms = PrefabRegistry::GetRooms();
		std::vector<std::shared_ptr<Prefab>> puzzles = PrefabRegistry::GetPuzzles();
		loadScene(scenes[currentSceneIndex].path_name, sceneGraph, rooms, puzzles);
	}
	else
	{
		currentSceneIndex = -1;
	}
}

SceneGraph* SceneManager::getCurrentScene()
{

	return sceneGraph;
}

void SceneManager::Initialize()
{
	std::ifstream file("res/scene/scene_order.json");
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
	}
	else {

		json sceneData;
		file >> sceneData;
		file.close();

		for (json scene : sceneData) {
			OrderedScene order;
			order.order = scene["order"];
			order.path_name = scene["path_name"];
			scenes.push_back(order);
		}

		std::sort(scenes.begin(), scenes.end(), [&](OrderedScene a, OrderedScene b) {
			return a.order < b.order;
			});

		next();
	}
}
