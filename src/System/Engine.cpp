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
	game->init();

	editor = new Editor(prefabs);
	editor->init();

	camera->setAABB();
}

void Engine::run()
{
	editor->run();
}

void Engine::shutdown()
{

	saveTagLayers();
	editor->shutdown();
	game->shutdown();

	ServiceLocator::shutdownServices();

	delete editor;
	delete game;
}
