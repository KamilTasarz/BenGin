#include "ButtonFunctions.h"
#include "../System/SceneManager.h"
#include "../Gameplay/MusicManager.h"
#include "../Basic/Node.h"
#include "../Gameplay/KeyboardWrite.h"
#include "../Gameplay/AnimatedMenu.h"

void MenuButton()
{
    SceneManager::Instance().next();
    MusicManager::instance().StartGameTransition();
    std::cout << "You clicked button in Menu." << std::endl;
}

void NickButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
			KeyboardWrite* script = keywriter->getComponent<KeyboardWrite>();
			if (script) {
                script->isActive = true;
			}
			
		}

    
    }
}

void LeaderboardButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->isActive = true;
				script->right = true; // Move to the right
                //script->alfa = 0.f; // Reset alfa to start moving back
            }

        }


    }
}

void ReturnButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->isActive = true;
                script->right = false; // Move to the right
				//script->alfa = 0.f; // Reset alfa to start moving back
            }

        }


    }
}

void ExitButton()
{
	auto* window = ServiceLocator::getWindow();
	if (window) {
		glfwSetWindowShouldClose(window->window, true);
	}
}
