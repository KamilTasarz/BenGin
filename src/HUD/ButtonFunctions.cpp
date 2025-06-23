#include "ButtonFunctions.h"
#include "../System/SceneManager.h"
#include "../Gameplay/MusicManager.h"

void MenuButton()
{
    SceneManager::Instance().next();
    MusicManager::instance().StartGameTransition();
    std::cout << "You clicked button in Menu." << std::endl;
}
