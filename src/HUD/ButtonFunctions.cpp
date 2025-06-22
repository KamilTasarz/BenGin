#include "ButtonFunctions.h"
#include "../System/SceneManager.h"

void MenuButton()
{
    SceneManager::Instance().next();
    std::cout << "You clicked button in Menu." << std::endl;
}
