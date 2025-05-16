#include "Engine.h"
#include "ServiceLocator.h"
#include "Window.h"
#include "PhysicsSystem.h"
#include "../Input/InputManager.h"
#include "../System/Serialization.h"
#include "../ResourceManager.h"
#include "../Component/CameraGlobals.h"





void Engine::init()
{
	window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version 1.1.2");
	ServiceLocator::provide(window);
	
	PhysicsSystem::instance();

	ResourceManager::Instance().init();

	loadTagLayers();
	loadPrefabs(prefabs);

	game = new Game(prefabs);
	//game->init();

	editor = new Editor(prefabs);
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
