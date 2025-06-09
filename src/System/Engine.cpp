#include "Engine.h"
#include "ServiceLocator.h"
#include "Window.h"
#include "PhysicsSystem.h"
#include "../Input/InputManager.h"
#include "../System/Serialization.h"
#include "../ResourceManager.h"
#include "../Component/CameraGlobals.h"
#include "PrefabRegistry.h"
#include "../System/GuiManager.h"
#include "../System/RenderSystem.h"




void Engine::init()
{
	window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version 1.1.2");
	ServiceLocator::provide(window);
	
	PhysicsSystem::instance();

	ResourceManager::Instance().init();

	RenderSystem::Instance();

	loadTagLayers();
	loadPrefabs(prefabs, prefabs_puzzle);

	PrefabRegistry::Init(&prefabs, &prefabs_puzzle);

	game = new Game(prefabs, prefabs_puzzle);
	//game->init();

	editor = new Editor(prefabs, prefabs_puzzle);
	//editor->init();

	camera->setAABB();
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
		game->init();
		game->run();
		game->shutdown();
	} while (engine_work);
}

void Engine::shutdown()
{

	saveTagLayers();
	//editor->shutdown();
	//game->shutdown();
	//glfwTerminate();

	ServiceLocator::shutdownServices();

	delete editor;
	delete game;
}
