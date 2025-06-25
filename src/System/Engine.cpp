#include "Engine.h"
#include "ServiceLocator.h"
#include "Window.h"
#include "../AudioEngine.h"
#include "PhysicsSystem.h"
#include "../Input/InputManager.h"
#include "../System/Serialization.h"
#include "../ResourceManager.h"
#include "../Component/CameraGlobals.h"
#include "PrefabRegistry.h"
#include "../System/GuiManager.h"
#include "../System/RenderSystem.h"
#include "../System/SceneManager.h"
#include <cstdlib>
#include <ctime>

void Engine::init()
{
	srand(static_cast<unsigned int>(time(nullptr)));

	window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Beta Version 1.3.0");
	ServiceLocator::provide(window);
	
	audio = new CAudioEngine();
	ServiceLocator::provide(audio);

	PhysicsSystem::instance();

	ResourceManager::Instance().init();

	RenderSystem::Instance();

	loadTagLayers();
	loadPrefabs(prefabs, prefabs_puzzle);

	PrefabRegistry::Init(&prefabs, &prefabs_puzzle);

	SceneManager::Instance().Initialize();

	game = new Game(prefabs, prefabs_puzzle);

	editor = new Editor(prefabs, prefabs_puzzle);

	camera->setAABB();

	ResourceManager::Instance().shader_tile->use();
	ResourceManager::Instance().shader_tile->setFloat("start_time", glfwGetTime());
}

void Engine::run()
{
	do {
		editor->play = false;
		editor->init();
		editor->run();
		editor->shutdown();

		game->play = true;
		if (!engine_work) break;
		SceneManager::Instance().goToScene(0);
		SceneManager::Instance().resetSwitched();
		game->init();
		game->run();
		game->shutdown();
	} while (engine_work);
}

void Engine::shutdown()
{
	saveTagLayers();

	ServiceLocator::shutdownServices();

	delete editor;
	delete game;

	prefabs.clear();
	prefabs_puzzle.clear();
}
