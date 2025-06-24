#include "SceneManager.h"
#include "../Basic/Node.h"
#include "../System/Serialization.h"
#include "../System/PrefabRegistry.h"
#include "../System/GuiManager.h"
#include "../Gameplay/GameManager.h"
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
	GameManager::instance().onEnd();
	switched = true; // Set switched to true to indicate a scene switch
	if (sceneGraph) {
		delete sceneGraph;
	}
	GuiManager::Instance().getObjects().clear();

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
	GameManager::instance().onEnd();
	switched = true; // Set switched to true to indicate a scene switch
	if (sceneGraph) {
		delete sceneGraph;
	}
	GuiManager::Instance().getObjects().clear();
	
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

void SceneManager::goToScene(unsigned int index)
{
	GameManager::instance().onEnd();
	switched = true; // Set switched to true to indicate a scene switch
	if (sceneGraph) {
		delete sceneGraph;
	}
	GuiManager::Instance().getObjects().clear();
	if (index < scenes.size())
	{
		currentSceneIndex = index;
	}
	std::vector<std::shared_ptr<Prefab>> rooms = PrefabRegistry::GetRooms();
	std::vector<std::shared_ptr<Prefab>> puzzles = PrefabRegistry::GetPuzzles();
	loadScene(scenes[currentSceneIndex].path_name, sceneGraph, rooms, puzzles);
}

void SceneManager::reset()
{
	GameManager::instance().onEnd();
	switched = true; // Set switched to true to indicate a scene switch
	if (sceneGraph) {
		delete sceneGraph;
	}
	GuiManager::Instance().getObjects().clear();
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

bool SceneManager::isSwitched() const
{
	return switched;
}

void SceneManager::resetSwitched()
{
	switched = false;
}

void SceneManager::LateNext()
{
	late_next = true;
}

void SceneManager::resetLateNext()
{
	late_next = false;
}

bool SceneManager::isLateNext() const
{
	return late_next;
}

SceneGraph* SceneManager::getCurrentScene()
{

	return sceneGraph;
}

std::string SceneManager::getCurrentScenePath()
{
	if (currentSceneIndex >= 0)
		return std::string(scenes[currentSceneIndex].path_name);
	else
		return std::string();
}

void SceneManager::Initialize()
{
	std::ifstream file("res/config.json");
	if (!file) {
		std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
	}
	else {

		json sceneData;
		file >> sceneData;
		file.close();

		for (json scene : sceneData["scene_order"]) {
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
